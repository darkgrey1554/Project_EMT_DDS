#include "AdapterSharedMemory.hpp"

#ifdef _WIN32

namespace scada_ate::gate::adapter::sem
{
	////////////////////////////////////////
	/// --- ������ ��� shared memory --- ///
	///////////////////////////////////////
	/// <param>
	/// source - ���������� ����� ����� ������
	/// </param>
	/// <result> 
	/// - ��� shared memory
	/// 
	std::string AdapterSharedMemory::CreateSMName()
	{
		std::string str;
		str += "Global\\";
		str += config.NameChannel;
		return str;
	}

	std::string AdapterSharedMemory::CreateSMMutexName()
	{
		std::string str;
		str += "Global\\Mutex_";
		str += config.NameChannel;
		return str;
	}
	/////////////////////////////////////////////////
	/// --- ����������� �������� SharedMemory --- ///
	/////////////////////////////////////////////////

	/////////////////////////////////////////////////
	/// --- ���������� �������� SharedMemory --- ///
	////////////////////////////////////////////////

	ResultReqest AdapterSharedMemory::ReadData(std::deque<SetTags>** _data)
	{	
		DWORD mutex_win32 = 0;
		DWORD err_win32 = 0;
		ResultReqest res = ResultReqest::OK;

		try
		{
			if (current_status.load(std::memory_order_relaxed) != atech::common::Status::OK)
			{
				log->Warning("AdapterSharedMemory {}: ReadData IGNOR: error {}", config.NameChannel, 1);
				return ResultReqest::IGNOR;
			}

			log->Debug("AdapterSharedMemory {}: ReadData START", config.NameChannel);	

			HeaderSharedMemory* head = reinterpret_cast<HeaderSharedMemory*>(buf_data);
			char* _buf = buf_data;

			size_t offset=0;
			long long time_data;

			mutex_win32 = WaitForSingleObject(Mutex_SM, 5000);
			if (mutex_win32 != WAIT_OBJECT_0)
			{
				err_win32 = GetLastError();
				throw 1;
			}

			SetTags& set_data = *data.begin();

			time_data = head->TimeLastWrite;

			for (auto it = set_data.map_data.begin(); it != set_data.map_data.end(); it++)
			{
				offset = TakeOffset(it->first.type, it->first.offset);
				if (offset == 0) continue;
				_buf = buf_data + offset;
				
				if (it->first.type == TypeValue::INT)
				{
					it->second.value = *(int*)_buf;
					_buf += sizeof(int);
				}
				else if (it->first.type == TypeValue::FLOAT)
				{
					it->second.value = *(float*)_buf;
					_buf += sizeof(float);
				}
				else if (it->first.type == TypeValue::DOUBLE)
				{
					it->second.value = *(double*)_buf;
					_buf += sizeof(double);
				}
				else if (it->first.type == TypeValue::CHAR)
				{
					it->second.value = *(char*)_buf;
					_buf += sizeof(char);
				}
				else if (it->first.type == TypeValue::STRING)
				{
					it->second.value = *(char*)_buf;
					_buf = _buf + config.size_str+1;
				}

				it->second.quality = *_buf;
				it->second.time = time_data;
			} 			

			/// --- write time in header --- /// 
			set_data.time_source = head->TimeLastWrite;
			head->TimeLastRead = TimeConverter::GetTime_LLmcs();
			head->count_read++;

			*_data = &data;
		}
		catch (int& e)
		{
			
			log->Critical("AdapterSharedMemory {}: ERROR ReadData: error: {}, syserror: {}", config.NameChannel, e, err_win32);
			res = ResultReqest::ERR;

		}
		catch (...)
		{
			log->Critical("AdapterSharedMemory {}: ERROR ReadData: error: {}, syserror: {}", config.NameChannel, 0, 0);
			res = ResultReqest::ERR;
		}

		if (mutex_win32 == WAIT_OBJECT_0) ReleaseMutex(Mutex_SM);
		return res;
	}

	ResultReqest AdapterSharedMemory::WriteData(const std::deque<SetTags>& _data)
	{	
		DWORD mutex_win32 = 0;
		DWORD err_win32 = 0;
		ResultReqest res = ResultReqest::OK;

		try
		{
			if (current_status.load(std::memory_order_relaxed) != atech::common::Status::OK)
			{
				log->Debug("AdapterSharedMemory {}: WriteData IGNOR", config.NameChannel);
				return ResultReqest::IGNOR;
			}

			log->Debug("AdapterSharedMemory {}: WriteData START", config.NameChannel);

			mutex_win32 = WaitForSingleObject(Mutex_SM, 5000);
			if (mutex_win32 != WAIT_OBJECT_0)
			{
				err_win32 = GetLastError();
				throw 1;
			}		

			
			if (data.empty()) throw 2;
			for (const auto& data_in : _data)
			{
				for (LinkTags& link_tag : this->config.vec_link_tags)
				{
					auto it = data_in.map_data.find(link_tag.source);
					if (it == data_in.map_data.end()) continue;
					set_data(link_tag.source.type, it->second, link_tag);
				}
			}

			HeaderSharedMemory* head = (HeaderSharedMemory*)buf_data;
			head->count_write++;
			head->TimeLastWrite = TimeConverter::GetTime_LLmcs();

		}
		catch (int& e)
		{
			
			log->Critical("AdapterSharedMemory {}: ERROR WriteData: error: {}, syserror: {}", config.NameChannel, e, err_win32);
			res = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterSharedMemory {}: ERROR WriteData: error: {}, syserror: {}", config.NameChannel, 0, 0);
			res = ResultReqest::ERR;
		}		

		if (mutex_win32 == WAIT_OBJECT_0) ReleaseMutex(Mutex_SM);
		return res;
	}

	void AdapterSharedMemory::destroy()
	{
		UnmapViewOfFile(buf_data);
		CloseHandle(SM_Handle);
		CloseHandle(Mutex_SM);
		current_status.store(atech::common::Status::Null);
	}

	ResultReqest AdapterSharedMemory::init_adapter()
	{
		ResultReqest res = ResultReqest::OK;
		unsigned int result = 0;
		unsigned long sys_error = 0;
		size_t size_memory;

		/// --- guard from repeated usage --- ///	

		atech::common::Status status = current_status.load(std::memory_order::memory_order_relaxed);
		if (status == atech::common::Status::INIT || status == atech::common::Status::OK)
		{
			ResultReqest res = ResultReqest::IGNOR;
			return res;
		}
		current_status.store(atech::common::Status::INIT, std::memory_order_relaxed);

		security_attr = std::make_shared<SecurityHandle>();
		std::string namememory;
		std::string namemutex;

		try
		{
			if (config.type_adapter != TypeAdapter::SharedMemory) throw 1;

			/// --- coping of configuration --- ///
			//config.vec_link_tasg;
			//config.vec_tags_source;

			log->Info("AdapterSharedMemory {}: Init START", this->config.NameChannel);

			/// --- initialization of security attributes --- ///

			if (security_attr == nullptr)
			{
				sys_error = 0;
				current_status.store(atech::common::Status::ERROR_INIT, std::memory_order_relaxed);
				throw 2;
			}

			result = security_attr->InitSecurityAttrubuts();
			if (result != 0)
			{
				sys_error = result;
				current_status.store(atech::common::Status::ERROR_INIT, std::memory_order_relaxed);
				throw 2;
			}

			log->Debug("AdapterSharedMemory {} : InitSecurityAttributs DONE", this->config.NameChannel);

			/// --- defining size units of data --- ///
			size_memory = GetSizeMemory();

			/// --- initialization handle of shared memory --- ///
			if (SM_Handle != NULL)
			{
				current_status.store(atech::common::Status::ERROR_INIT, std::memory_order_relaxed);
				throw 3;
			}

			namememory = CreateSMName();
			namemutex = CreateSMMutexName();

			Mutex_SM = CreateMutexA(&security_attr->getsecurityattrebut(), TRUE, namemutex.c_str());
			if (Mutex_SM == NULL)
			{
				sys_error = GetLastError();
				current_status.store(atech::common::Status::ERROR_INIT, std::memory_order_relaxed);
				throw 4;
			}

			log->Debug("AdapterSharedMemory {} : CreateMutex DONE", this->config.NameChannel);

			SM_Handle = CreateFileMappingA(INVALID_HANDLE_VALUE, &security_attr->getsecurityattrebut(), PAGE_READWRITE, 0, size_memory, namememory.c_str());
			if (SM_Handle == NULL)
			{
				sys_error = GetLastError();
				current_status.store(atech::common::Status::ERROR_INIT, std::memory_order_relaxed);
				res = ResultReqest::ERR;
				throw 5;
			}

			log->Debug("AdapterSharedMemory {} : CreateFileMapping DONE", this->config.NameChannel);

			buf_data = (char*)MapViewOfFile(SM_Handle, FILE_MAP_ALL_ACCESS, 0, 0, size_memory);
			if (buf_data == NULL)
			{
				current_status.store(atech::common::Status::ERROR_INIT, std::memory_order_relaxed);
				res = ResultReqest::ERR;
				throw 6;
			}

			log->Debug("AdapterSharedMemory {} : MapViewOfFileg DONE", this->config.NameChannel);

			/// --- clear shared memory --- ///
			for (unsigned int i = 0; i < size_memory; i++)
			{
				*(buf_data + i) = 0;
			}

			/// --- initilization header shared memory --- ///
			HeaderSharedMemory* head = reinterpret_cast<HeaderSharedMemory*>(buf_data);
			head->TimeLastRead = 0;
			head->TimeLastWrite = 0;
			head->count_read = 0;
			head->count_write = 0;
			head->size_data_int = config.size_int_data;
			head->size_data_float = config.size_float_data;
			head->size_data_double = config.size_double_data;
			head->size_data_char = config.size_char_data;
			head->size_str_data = config.size_str_data;
			head->size_str = config.size_str;
			ReleaseMutex(Mutex_SM);

			/// --- init vector GetTags --- /// 

			init_deque();

			current_status.store(atech::common::Status::OK, std::memory_order_relaxed);
			log->Info("AdapterSharedMemory {} : Init DONE", this->config.NameChannel);
		}
		catch (int& e)
		{
			log->Critical("AdapterSharedMemory {}: Error Init : error: {}, syserror: {}", this->config.NameChannel, e, sys_error);
			if (Mutex_SM != NULL) ReleaseMutex(Mutex_SM);
			CloseHandle(Mutex_SM);
			UnmapViewOfFile(buf_data);
			CloseHandle(SM_Handle);
			res = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterSharedMemory {}: Error Init : error: {}, syserror: {}", this->config.NameChannel, 0, 0);
			if (Mutex_SM != NULL) ReleaseMutex(Mutex_SM);
			CloseHandle(Mutex_SM);
			UnmapViewOfFile(buf_data);
			CloseHandle(SM_Handle);
			res = ResultReqest::ERR;
		}

		return res;
	}

}

#endif // _WIN32

#ifdef __linux__
namespace scada_ate::gate::adapter::sem
{
	////////////////////////////////////////
	/// --- ������ ��� shared memory --- ///
	///////////////////////////////////////
	
	std::string AdapterSharedMemory::CreateSMName()
	{
		std::string str;
		str += "sm_linux_";
		str += config.NameChannel;
		return str;
	}

	std::string AdapterSharedMemory::CreateSMMutexName()
	{
		std::string str;
		str += "sem_linux_";
		str += config.NameChannel;
		return str;
	}

	/////////////////////////////////////////////////
	/// --- ���������� �������� SharedMemory --- ///
	////////////////////////////////////////////////

	ResultReqest AdapterSharedMemory::init_adapter()
	{
		ResultReqest res = ResultReqest::OK;
		unsigned int result = 0;
		int sys_error = 0;

		StatusAdapter status = current_status.load(std::memory_order::memory_order_relaxed);
		if (status == StatusAdapter::INITIALIZATION || status == StatusAdapter::OK)
		{
			ResultReqest res = ResultReqest::IGNOR;
			return res;
		}
		current_status.store(StatusAdapter::INITIALIZATION, std::memory_order_relaxed);

		std::string namememory;
		std::string namemutex;

		try
		{
			if (config.type_adapter != TypeAdapter::SharedMemory) throw 1;

			/// --- coping of configuration --- ///
			//config.vec_link_tasg;
			//config.vec_tags_source;

			log->Info("AdapterSharedMemory {}: Init START", this->config.NameChannel);

			/// --- defining size units of data --- ///
			size_memory = GetSizeMemory();


			/// --- initialization handle of shared memory --- ///

			if (init_mutex() != ResultReqest::OK) throw 1;
			log->Debug("AdapterSharedMemory {} : Create semaphore done", this->config.NameChannel);

			if (init_shared_memory() != ResultReqest::OK) throw 2;
			log->Debug("AdapterSharedMemory {} : Create shared memory done", this->config.NameChannel);

			if (allocate_memory(size_memory) != ResultReqest::OK) throw 3;
			log->Debug("AdapterSharedMemory {} : Allocate memory done", this->config.NameChannel);

			if (mapping_memory(size_memory) != ResultReqest::OK) throw 4;
			log->Debug("AdapterSharedMemory {} : Mapping memory done ", this->config.NameChannel);

			/// --- initilization header shared memory --- ///

			lock_semaphore();

			HeaderSharedMemory* head = reinterpret_cast<HeaderSharedMemory*>(buf_data);
			head->TimeLastRead = 0;
			head->TimeLastWrite = 0;
			head->count_read = 0;
			head->count_write = 0;
			head->size_data_int = config.size_int_data;
			head->size_data_float = config.size_float_data;
			head->size_data_double = config.size_double_data;
			head->size_data_char = config.size_char_data;
			head->size_str = config.size_str;

			unlock_semaphore();
			/// --- init vector GetTags --- /// 
			init_deque();

			current_status.store(StatusAdapter::OK, std::memory_order_relaxed);
			log->Info("AdapterSharedMemory {} : Init DONE", this->config.NameChannel);

		}
		catch (int& e)
		{
			current_status.store(StatusAdapter::ERROR_INIT, std::memory_order_relaxed);
			log->Critical("AdapterSharedMemory {}: Error Init : error: {}, syserror: {}", this->config.NameChannel, e, sys_error);
			destroy();
			res = ResultReqest::ERR;
		}
		catch (...)
		{
			current_status.store(StatusAdapter::ERROR_INIT, std::memory_order_relaxed);
			log->Critical("AdapterSharedMemory {}: Error Init : error: {}, syserror: {}", this->config.NameChannel, 0, 0);
			destroy();
			res = ResultReqest::ERR;
		}

		return res;
	}

	/////////////////////////////////////////////////
	/// --- ���������� �������� SharedMemory --- ///
	////////////////////////////////////////////////

	ResultReqest AdapterSharedMemory::init_mutex()
	{
		ResultReqest res = ResultReqest::OK;
		int sys_error = 0;
		try
		{
			_semaphor = sem_open(CreateSMMutexName().c_str(), O_CREAT, 0777, 1);
			if (_semaphor == SEM_FAILED)
			{
				sys_error = errno;
				throw 1;
			}
		}
		catch (int& e)
		{
			log->Critical("AdapterSharedMemory {}: Error Init semaphore : error: {}, syserror: {}", this->config.NameChannel, e, sys_error);
			res = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterSharedMemory {}: Error Init semaphore : error: {}, syserror: {}", this->config.NameChannel, 0, 0);
			res = ResultReqest::ERR;
		}

		return res;
	}

	ResultReqest AdapterSharedMemory::init_shared_memory()
	{
		ResultReqest res = ResultReqest::OK;
		int sys_error = 0;

		try
		{
			hnd_sm = shm_open(CreateSMName().c_str(), O_CREAT | O_RDWR, 0777);
			if (hnd_sm == -1)
			{
				sys_error = errno;
				throw 1;
			}
		}
		catch (int& e)
		{
			log->Critical("AdapterSharedMemory {}: Error Init memory: error: {}, syserror: {}", this->config.NameChannel, e, sys_error);
			res = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterSharedMemory {}: Error Init memory: error: {}, syserror: {}", this->config.NameChannel, 0, 0);
			res = ResultReqest::ERR;
		}

		return res;
	}

	ResultReqest AdapterSharedMemory::allocate_memory(size_t size_memory)
	{
		ResultReqest res = ResultReqest::OK;
		int sys_error = 0;

		try
		{
			if (ftruncate(hnd_sm, size_memory) == -1)
			{
				sys_error = errno;
				throw 1;
			}
		}
		catch (int& e)
		{
			log->Critical("AdapterSharedMemory {}: Error allocate memory : error: {}, syserror: {}", this->config.NameChannel, e, sys_error);
			res = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterSharedMemory {}: Error allocate memory : error: {}, syserror: {}", this->config.NameChannel, 0, 0);
			res = ResultReqest::ERR;
		}

		return res;
	}

	ResultReqest AdapterSharedMemory::mapping_memory(size_t size_memory)
	{
		ResultReqest res = ResultReqest::OK;
		int sys_error = 0;

		try
		{
			buf_data = (char*)mmap(NULL, size_memory, PROT_READ | PROT_WRITE, MAP_SHARED, hnd_sm, 0);
			if (buf_data == MAP_FAILED)
			{
				sys_error = errno;
				throw 1;
			}
		}
		catch (int& e)
		{
			log->Critical("AdapterSharedMemory {}: Error mapping memory : error: {}, syserror: {}", this->config.NameChannel, e, sys_error);
			res = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterSharedMemory {}: Error mapping memory : error: {}, syserror: {}", this->config.NameChannel, 0, 0);
			res = ResultReqest::ERR;
		}

		return res;
	}

	ResultReqest  AdapterSharedMemory::lock_semaphore()
	{
		ResultReqest res = ResultReqest::OK;
		timespec tm;
		clock_gettime(CLOCK_REALTIME, &tm);
		tm.tv_sec += 5;
		int res_fun;
		while ((res_fun = sem_timedwait(_semaphor, &tm)) == -1 && errno == EINTR);
		if (res_fun == -1)
		{
			log->Warning("AdapterSharedMemory {}: Error lock semaphor: error: {}, syserror: {}", this->config.NameChannel, 0, errno);
			res = ResultReqest::ERR;
		}

		return res;
	}

	ResultReqest  AdapterSharedMemory::unlock_semaphore()
	{
		ResultReqest res = ResultReqest::OK;
		if (sem_post(_semaphor) == -1)
		{
			log->Warning("AdapterSharedMemory {}: Error unlock semaphor: error: {}, syserror: {}", this->config.NameChannel, 0, errno);
			res = ResultReqest::ERR;
		}

		return res;
	}

	void AdapterSharedMemory::destroy()
	{
		if (buf_data != nullptr && buf_data != (void*)-1)
		{
			if (munmap(buf_data, size_memory) == -1)
				log->Critical("AdapterSharedMemory {}: Error unmapping memory: error: {}, syserror: {}", this->config.NameChannel, 0, errno);
			buf_data == nullptr;
		}

		if (hnd_sm > 0)
		{
			close(hnd_sm);
			if (shm_unlink(CreateSMName().c_str()) == -1)
				log->Critical("AdapterSharedMemory {}: Error unlink memory: error: {}, syserror: {}", this->config.NameChannel, 0, errno);
			hnd_sm = 0;
		}

		if (_semaphor != nullptr && _semaphor != SEM_FAILED)
		{
			sem_close(_semaphor);
			if (sem_unlink(CreateSMMutexName().c_str()) == -1)
				log->Critical("AdapterSharedMemory {}: Error unlink semaphor: error: {}, syserror: {}", this->config.NameChannel, 0, errno);
			_semaphor = nullptr;
		}

		current_status.store(atech::common::Status::Null);
	}

	///////////////////////////////////////////////////////////////////
	/// --- ������ ������������ ��������� �������� SharedMemory --- ///
	//////////////////////////////////////////////////////////////////
	/// <param>
	/// param - ������������� �������� (enum ParamInfoAdapter) 
	/// </param>
	/// <result> 
	/// - ���������� ���������� � ���� std::shared_ptr<BaseAnswer>, ������� �������� ������������ ��� ���� std::shared_ptr<HeaderDataAnswerSM>

	

	//////////////////////////////////////////////////////
	/// --- ������� ������ ������ �� SharedMemory --- ///
	/////////////////////////////////////////////////////

	ResultReqest AdapterSharedMemory::ReadData(std::deque<SetTags>** _data)
	{
		ResultReqest res = ResultReqest::OK;
		bool status_lock = false;
		try
		{
			if (current_status.load(std::memory_order_relaxed) != StatusAdapter::OK)
			{
				log->Warning("AdapterSharedMemory {}: ReadData IGNOR: error {}", config.NameChannel, 1);
				return ResultReqest::IGNOR;
			}

			log->Debug("AdapterSharedMemory {}: ReadData START", config.NameChannel);

			HeaderSharedMemory* head = reinterpret_cast<HeaderSharedMemory*>(buf_data);
			char* _buf = buf_data;

			int int_value = 0;
			float float_value = 0;
			double double_value = 0;
			char char_value = 0;
			std::string str_value;
			char quality = 0;
			size_t offset = 0;
			long long time_data;

			if (lock_semaphore() != ResultReqest::OK) throw 1;
			status_lock = true;

			SetTags& set_data = *data.begin();

			time_data = head->TimeLastWrite;

			for (auto it = set_data.map_data.begin(); it != set_data.map_data.end(); it++)
			{
				offset = TakeOffset(it->first.type, it->first.offset);
				if (offset == 0) continue;
				_buf = buf_data + offset;

				if (it->first.type == TypeValue::INT)
				{
					it->second.value = *(int*)_buf;
					_buf += sizeof(int);
				}
				else if (it->first.type == TypeValue::FLOAT)
				{
					it->second.value = *(float*)_buf;
					_buf += sizeof(float);
				}
				else if (it->first.type == TypeValue::DOUBLE)
				{
					it->second.value = *(double*)_buf;
					_buf += sizeof(double);
				}
				else if (it->first.type == TypeValue::CHAR)
				{
					it->second.value = *(char*)_buf;
					_buf += sizeof(char);
				}
				else if (it->first.type == TypeValue::STRING)
				{
					it->second.value = *(char*)_buf;
					_buf = _buf + config.size_str;
				}

				it->second.quality = *_buf;
				it->second.time = time_data;
			}

			/// --- write time in header --- /// 
			set_data.time_source = head->TimeLastWrite;
			head->TimeLastRead = TimeConverter::GetTime_LLmcs();
			head->count_read++;

			*_data = &data;
		}
		catch (int& e)
		{

			log->Critical("AdapterSharedMemory {}: ERROR ReadData: error: {}, syserror: {}", config.NameChannel, e, 0);
			res = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterSharedMemory {}: ERROR ReadData: error: {}, syserror: {}", config.NameChannel, 0, 0);
			res = ResultReqest::ERR;
		}

		if (status_lock == true) unlock_semaphore();
		return res;
	}

	//////////////////////////////////////////////////////
	/// --- ������� ������ ������ � SharedMemory --- ///
	/////////////////////////////////////////////////////

	ResultReqest AdapterSharedMemory::WriteData(const std::deque<SetTags>& _data)
	{
		ResultReqest res = ResultReqest::OK;
		bool status_lock = false;

		try
		{
			if (current_status.load(std::memory_order_relaxed) != StatusAdapter::OK)
			{
				log->Debug("AdapterSharedMemory {}: WriteData IGNOR", config.NameChannel);
				return ResultReqest::IGNOR;
			}

			log->Debug("AdapterSharedMemory {}: WriteData START", config.NameChannel);

			if (lock_semaphore() != ResultReqest::OK) throw 1;
			status_lock = true;


			if (data.empty()) throw 2;
			for (const auto& data_in : _data)
			{
				for (LinkTags& link_tag : this->config.vec_link_tags)
				{
					auto it = data_in.map_data.find(link_tag.source);
					if (it == data_in.map_data.end()) continue;
					set_data(link_tag.source.type, it->second, link_tag);
				}
			}
			HeaderSharedMemory* head = (HeaderSharedMemory*)buf_data;
			head->count_write++;
			head->TimeLastWrite = TimeConverter::GetTime_LLmcs();

		}
		catch (int& e)
		{

			log->Critical("AdapterSharedMemory {}: ERROR WriteData: error: {}, syserror: {}", config.NameChannel, e, 0);
			res = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterSharedMemory {}: ERROR WriteData: error: {}, syserror: {}", config.NameChannel, 0, 0);
			res = ResultReqest::ERR;
		}

		if (status_lock) unlock_semaphore();

		return res;
	}

}
#endif


namespace scada_ate::gate::adapter::sem
{
	AdapterSharedMemory::AdapterSharedMemory(std::shared_ptr<IConfigAdapter> config)
	{
		std::shared_ptr<ConfigAdapterSharedMemory> config_point = std::reinterpret_pointer_cast<ConfigAdapterSharedMemory>(config);

		if (config_point != nullptr && config_point->type_adapter == TypeAdapter::SharedMemory)
		{
			this->config = *config_point;
		}

		log = LoggerSpaceScada::GetLoggerScada(LoggerSpaceScada::TypeLogger::SPDLOG);
	};

	AdapterSharedMemory::~AdapterSharedMemory()
	{
		const std::lock_guard<std::mutex> lock_init(mutex_init);
		destroy();
	}

	ResultReqest AdapterSharedMemory::InitAdapter()
	{
		const std::lock_guard<std::mutex> lock_init(mutex_init);
		return init_adapter();
	}

	void AdapterSharedMemory::set_data(TypeValue& type, const Value& value, const LinkTags& link)
	{
		if (type == TypeValue::INT)
		{
			set_data_int(value, link);
			return;
		}
		if (type == TypeValue::FLOAT)
		{
			set_data_float(value, link);
			return;
		}
		if (type == TypeValue::DOUBLE)
		{
			set_data_double(value, link);
			return;
		}
		if (type == TypeValue::CHAR)
		{
			set_data_char(value, link);
			return;
		}
		if (type == TypeValue::STRING)
		{
			set_data_string(value, link);
			return;
		}
		return;
	}

	void AdapterSharedMemory::set_data_int(const Value& value, const LinkTags& link)
	{
		char* buf;
		int val_last;
		int val_current = 0;

		buf = buf_data + TakeOffset(link.target.type, link.target.offset);
		val_last = *((int*)buf);

		if (link.source.mask != 0)
		{
			val_current = demask(std::get<int>(value.value), link.source.mask, val_last, link.target.mask);
		}
		else
		{
			val_current = std::get<int>(value.value);
		}

		if (link.type_registration == TypeRegistration::RECIVE)
		{

			*((int*)buf) = val_current;
			buf += sizeof(int);
			*buf = value.quality;
			return;
		}

		if (link.type_registration == TypeRegistration::UPDATE)
		{
			if (val_last == val_current) return;
			*((int*)buf) = val_current;
			buf += sizeof(int);
			*buf = value.quality;
			return;
		}

		if (link.type_registration == TypeRegistration::DELTA)
		{
			if (abs(val_last - val_current) > (int)link.delta)
			{
				*((int*)buf) = val_current;
				buf += sizeof(int);
				*buf = value.quality;
				return;
			}
		}
	}

	void AdapterSharedMemory::set_data_float(const Value& value, const LinkTags& link)
	{
		char* buf;
		float val_last;

		buf = buf_data + TakeOffset(link.target.type, link.target.offset);
		val_last = *((float*)buf);

		if (link.type_registration == TypeRegistration::RECIVE)
		{

			*((float*)buf) = std::get<float>(value.value);
			buf += sizeof(float);
			*buf = value.quality;
			return;
		}

		if (link.type_registration == TypeRegistration::UPDATE)
		{
			if (val_last == std::get<float>(value.value)) return;
			*((float*)buf) = std::get<float>(value.value);
			buf += sizeof(float);
			*buf = value.quality;
			return;
		}

		if (link.type_registration == TypeRegistration::DELTA)
		{
			if (fabs(val_last - std::get<float>(value.value)) > link.delta)
			{
				*((float*)buf) = std::get<float>(value.value);
				buf += sizeof(float);
				*buf = value.quality;
				return;
			}
		}
	}

	void AdapterSharedMemory::set_data_double(const Value& value, const LinkTags& link)
	{
		char* buf;
		double val_last;

		buf = buf_data + TakeOffset(link.target.type, link.target.offset);
		val_last = *((double*)buf);

		if (link.type_registration == TypeRegistration::RECIVE)
		{

			*((double*)buf) = std::get<double>(value.value);
			buf += sizeof(double);
			*buf = value.quality;
			return;
		}

		if (link.type_registration == TypeRegistration::UPDATE)
		{
			if (val_last == std::get<double>(value.value)) return;
			*((double*)buf) = std::get<double>(value.value);
			buf += sizeof(double);
			*buf = value.quality;
			return;
		}

		if (link.type_registration == TypeRegistration::DELTA)
		{
			if (fabs(val_last - std::get<double>(value.value)) > link.delta)
			{
				*((double*)buf) = std::get<double>(value.value);
				buf += sizeof(double);
				*buf = value.quality;
				return;
			}
		}
	}

	void AdapterSharedMemory::set_data_char(const Value& value, const LinkTags& link)
	{
		char* buf;
		char val_last;

		buf = buf_data + TakeOffset(link.target.type, link.target.offset);
		val_last = *((char*)buf);

		if (link.type_registration == TypeRegistration::RECIVE)
		{

			*((char*)buf) = std::get<char>(value.value);
			buf += sizeof(char);
			*buf = value.quality;
			return;
		}

		if (link.type_registration == TypeRegistration::UPDATE)
		{
			if (val_last == std::get<char>(value.value)) return;
			*((char*)buf) = std::get<char>(value.value);
			buf += sizeof(char);
			*buf = value.quality;
			return;
		}

		if (link.type_registration == TypeRegistration::DELTA)
		{
			if (fabs(val_last - std::get<char>(value.value)) > link.delta)
			{
				*((char*)buf) = std::get<char>(value.value);
				buf += sizeof(char);
				*buf = value.quality;
				return;
			}
		}
	}

	void AdapterSharedMemory::set_data_string(const Value& value, const LinkTags& link)
	{
		char* buf;
		std::string val_last;

		buf = buf_data + TakeOffset(link.target.type, link.target.offset);
		val_last = *((char*)buf);

		if (link.type_registration == TypeRegistration::RECIVE)
		{
			size_t i = 0;
			const char* str = std::get<std::string>(value.value).c_str();
			while (i < config.size_str && i < std::get<std::string>(value.value).size())
			{
				*((char*)buf) = *(str + i);
				buf += sizeof(char);
				i++;
			}
			*buf = value.quality;
			return;
		}

		if (link.type_registration == TypeRegistration::UPDATE)
		{
			if (val_last.compare(std::get<std::string>(value.value)) == 0) return;
			size_t i = 0;
			const char* str = std::get<std::string>(value.value).c_str();
			while (i < config.size_str && i < std::get<std::string>(value.value).size())
			{
				*((char*)buf) = *(str + i);
				buf += sizeof(char);
				i++;
			}
			*buf = value.quality;
			return;
		}
	}

	std::shared_ptr<IAnswer> AdapterSharedMemory::GetInfo(ParamInfoAdapter param)
	{
		std::shared_ptr<IAnswer> answer = nullptr;
		std::shared_ptr<AnswerSharedMemoryHeaderData> answerHeaderData = nullptr;

		switch (param)
		{

			/// --- ����� �� ������ Header DATA --- ///
		case ParamInfoAdapter::HeaderData:


			answerHeaderData = AnswerRequestHeaderData();
			answer = std::reinterpret_pointer_cast<IAnswer>(answerHeaderData);
			break;

			/// --- ����� �� ������������ ������ ����� �������� (ResultReqest::IGNOR) --- ///
		default:

			answer = std::make_shared<IAnswer>();
			answer->param = param;
			answer->result = ResultReqest::IGNOR;
			answer->type_adapter = TypeAdapter::SharedMemory;

			break;
		}

		return answer;
	}

	TypeAdapter AdapterSharedMemory::GetTypeAdapter()
	{
		return TypeAdapter::SharedMemory;
	}

	uint32_t AdapterSharedMemory::GetId()
	{
		return config.id_adapter;
	}

	ResultReqest AdapterSharedMemory::GetStatus(std::deque<std::pair<uint32_t, atech::common::Status>>& st, uint32_t id)
	{
		st.push_back({ this->config.id_adapter, current_status.load() });
		return ResultReqest::OK;
	}

	ResultReqest AdapterSharedMemory::Start(std::deque<std::pair<uint32_t, atech::common::Status>>& st, uint32_t id)
	{
		ResultReqest result{ ResultReqest::OK };

		if (current_status.load() == atech::common::Status::STOP ||
			current_status.load() == atech::common::Status::OK)
		{
			current_status.store(atech::common::Status::OK);
			result = ResultReqest::OK;
		}
		else
		{
			result = ResultReqest::ERR;
		}

		st.push_back({ config.id_adapter, current_status.load() });
		return result;
	}

	ResultReqest AdapterSharedMemory::Stop(std::deque<std::pair<uint32_t, atech::common::Status>>& st, uint32_t id)
	{
		ResultReqest result{ ResultReqest::OK };

		if (current_status.load() == atech::common::Status::STOP ||
			current_status.load() == atech::common::Status::OK)
		{
			current_status.store(atech::common::Status::STOP);
			result = ResultReqest::OK;
		}
		else
		{
			result = ResultReqest::ERR;
		}

		st.push_back({ config.id_adapter, current_status.load() });
		return result;
	}

	ResultReqest AdapterSharedMemory::ReInit(std::deque<std::pair<uint32_t, atech::common::Status>>& st, uint32_t id)
	{
		ResultReqest result {ResultReqest::OK};
		const std::lock_guard<std::mutex> lock_init(mutex_init);
		this->destroy();
		result = this->init_adapter();
		st.push_back({ config.id_adapter, current_status.load() });
		return result;
	}

	int AdapterSharedMemory::demask(const int& value, int mask_source, const int& value_target, const int& mask_target)
	{
		int val_out = value & mask_source;
		if (mask_target > mask_source)
		{
			while (mask_target != mask_source)
			{
				mask_source <<= 1;
				val_out <<= 1;
			}
		}
		else
		{
			while (mask_target != mask_source)
			{
				mask_source >>= 1;
				val_out >>= 1;
			}
		}

		val_out = value_target & (~mask_target) | val_out;
		return val_out;
	}

	size_t AdapterSharedMemory::GetSizeMemory()
	{
		size_t result = 0;
		result += sizeof(HeaderSharedMemory); // size header

		offset_int = result; // save offset to int (for fast ac�ess)
		result += (sizeof(int) + sizeof(char)) * config.size_int_data; // size DataCollectionInt

		offset_float = result;
		result += (sizeof(float) + sizeof(char)) * config.size_float_data; // size DataCollectionFloat

		offset_double = result;
		result += (sizeof(double) + sizeof(char)) * config.size_double_data; // size DataCollectionDouble

		offset_char = result;
		result += (sizeof(char) + sizeof(char)) * config.size_char_data; // size DataCollectionChar

		offset_str = result;
		result += (sizeof(char) * config.size_str + sizeof(char) + sizeof(char)) * config.size_str_data; // size DataCollectionStr

		return result;
	}

	size_t AdapterSharedMemory::TakeOffset(const TypeValue& type_value, const size_t& ofs) const
	{
		if (type_value == TypeValue::INT)
		{
			if (ofs >= config.size_int_data) return 0;
			return offset_int + ofs * (sizeof(int) + sizeof(char));
		}

		if (type_value == TypeValue::FLOAT)
		{
			if (ofs >= config.size_float_data) return 0;
			return offset_float + ofs * (sizeof(float) + sizeof(char));
		}

		if (type_value == TypeValue::DOUBLE)
		{
			if (ofs >= config.size_double_data) return 0;
			return offset_double + ofs * (sizeof(double) + sizeof(char));
		}

		if (type_value == TypeValue::DOUBLE)
		{
			if (ofs >= config.size_double_data) return 0;
			return offset_double + ofs * (sizeof(double) + sizeof(char));
		}

		if (type_value == TypeValue::CHAR)
		{
			if (ofs >= config.size_char_data) return 0;
			return offset_char + ofs * (sizeof(char) + sizeof(char));
		}

		if (type_value == TypeValue::STRING)
		{
			if (ofs >= config.size_str_data) return 0;
			return offset_char + ofs * (config.size_str + sizeof(char));
		}

		return 0;
	}

	std::shared_ptr<AnswerSharedMemoryHeaderData> AdapterSharedMemory::AnswerRequestHeaderData()
	{
		std::shared_ptr<AnswerSharedMemoryHeaderData> point = std::make_shared<AnswerSharedMemoryHeaderData>();
		HeaderSharedMemory* head = (HeaderSharedMemory*)buf_data;
		//DWORD mutex_win32 = 0;
		//DWORD err_win32 = 0;

		/// <summary>
		/// ��������
		/// </summary>
		return 	point;
	}

	void AdapterSharedMemory::init_deque()
	{
		data.resize(1);
		SetTags& set_data = *data.begin();

		for (InfoTag& tag : this->config.vec_tags_source)
		{
			set_data.map_data[tag];
			auto it = set_data.map_data.find(tag);
			it->second.time = 0;
			it->second.quality = 0;

			if (tag.type == TypeValue::INT)
			{
				it->second.value = 0;
				continue;
			}

			if (tag.type == TypeValue::FLOAT)
			{
				it->second.value = (float).0;
				continue;
			}

			if (tag.type == TypeValue::DOUBLE)
			{
				it->second.value = (double).0;
				continue;
			}
			if (tag.type == TypeValue::CHAR)
			{
				it->second.value = (char)0;
				continue;
			}
			if (tag.type == TypeValue::STRING)
			{
				it->second.value = "";
				continue;
			}
		}

		return;
	}
}