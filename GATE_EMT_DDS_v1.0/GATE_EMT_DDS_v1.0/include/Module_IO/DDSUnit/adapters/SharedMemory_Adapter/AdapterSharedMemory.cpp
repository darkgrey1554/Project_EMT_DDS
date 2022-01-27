#include "AdapterSharedMemory.hpp"

namespace gate
{
	////////////////////////////////////////
	/// --- задает имя shared memory --- ///
	///////////////////////////////////////
	/// <param>
	/// source - уникальная часть имени памяти
	/// </param>
	/// <result> 
	/// - имя shared memory
	/// 
	std::string AdapterSharedMemory::CreateSMName(std::string source)
	{
			return "Global\\" + source;
	}

	std::string AdapterSharedMemory::CreateSMMutexName(std::string source)
	{
		return "Global\\Mutex_" + source;
	}
	/////////////////////////////////////////////////
	/// --- конструктор адаптера SharedMemory --- ///
	////////////////////////////////////////////////

	AdapterSharedMemory::AdapterSharedMemory()
	{
		log = LoggerSpaceScada::GetLoggerScada(LoggerSpaceScada::TypeLogger::SPDLOG);
	};

	/////////////////////////////////////////////////
	/// --- деструктор адаптера SharedMemory --- ///
	////////////////////////////////////////////////

	AdapterSharedMemory::~AdapterSharedMemory()
	{
		UnmapViewOfFile(buf_data);
		CloseHandle(SM_Handle);
		CloseHandle(Mutex_SM);
	}

	/////////////////////////////////////////////////
	/// --- деструктор адаптера SharedMemory --- ///
	////////////////////////////////////////////////
	/// <param>
	/// conf - указатель на конфигурация адаптера (conf должен иметь тип ConfigSharedMemoryAdapter*)
	/// </param>
	/// <result> 
	/// - результаи операции в типе ResultReqest. В случаи ести результат равен ResultReqest::ERR 
	/// инициальзация прошла с ошибкой, подробности описанны в логировании.
	
	ResultReqest AdapterSharedMemory::InitAdapter(std::shared_ptr<IConfigAdapter> conf)
	{
		
		ResultReqest res = ResultReqest::OK;
		unsigned int result = 0;
		unsigned long sys_error =0;

		/// --- guard from repeated usage --- ///
		const std::lock_guard<std::mutex> lock_init(mutex_init);

		StatusAdapter status = current_status.load(std::memory_order::memory_order_relaxed);
		if (status == StatusAdapter::INITIALIZATION || status == StatusAdapter::OK)
		{
			ResultReqest res = ResultReqest::IGNOR;
			return res;
		}
		current_status.store(StatusAdapter::INITIALIZATION, std::memory_order_relaxed);

		std::shared_ptr<ConfigAdapterSharedMemory> config_point = std::reinterpret_pointer_cast<ConfigAdapterSharedMemory>(conf);
		security_attr = std::make_shared<SecurityHandle>();
		std::string namememory;
		std::string namemutex;

		try
		{
			if (config_point == nullptr || config_point->type_adapter != TypeAdapter::SharedMemory) { res = ResultReqest::ERR; throw 1;}

			/// --- coping of configuration --- ///
			this->config.DataType = config_point->DataType;
			this->config.NameMemory = config_point->NameMemory;
			this->config.size = config_point->size;

			log->Debug("AdapterSharedMemory {}: Init START : size-{} type-{}", this->config.size, (char)this->config.DataType);

			/// --- initialization of security attributes --- ///
			if (security_attr == nullptr)
			{
				sys_error = 0;
				current_status.store(StatusAdapter::ERROR_INIT, std::memory_order_relaxed);
				throw 2;
			}
			
			result = security_attr->InitSecurityAttrubuts();
			if (result != 0)
			{
				sys_error = result;
				current_status.store(StatusAdapter::ERROR_INIT, std::memory_order_relaxed);
				throw 2;
			}
			
			log->Debug("AdapterSharedMemory {} : InitSecurityAttributs DONE", this->config.NameMemory);

			/// --- defining units of data --- ///
			if (config.DataType == TypeData::ANALOG) { size_type = sizeof(float); }
			else if (config.DataType == TypeData::DISCRETE) { size_type = sizeof(int); }
			else if (config.DataType == TypeData::BINAR) { size_type = sizeof(char); }
			else { size_type = 0; };

			/// --- initialization handle of shared memory --- ///
			if (SM_Handle != NULL)
			{
				current_status.store(StatusAdapter::ERROR_INIT, std::memory_order_relaxed);
				throw 3;
			}

			namememory = CreateSMName(config.NameMemory);
			namemutex =  CreateSMMutexName(config.NameMemory);

			Mutex_SM = CreateMutexA(&security_attr->getsecurityattrebut(), TRUE, namemutex.c_str());
			if (Mutex_SM == NULL)
			{
				sys_error = GetLastError();
				current_status.store(StatusAdapter::ERROR_INIT, std::memory_order_relaxed);
				throw 4;
			}

			log->Debug("AdapterSharedMemory {} : CreateMutex DONE", this->config.NameMemory);

			SM_Handle = CreateFileMappingA(INVALID_HANDLE_VALUE, &security_attr->getsecurityattrebut(), PAGE_READWRITE, 0, config.size * size_type + sizeof(HeaderSharedMemory), namememory.c_str());
			if (SM_Handle == NULL)
			{
				sys_error = GetLastError();
				current_status.store(StatusAdapter::ERROR_INIT, std::memory_order_relaxed);
				res = ResultReqest::ERR;
				throw 5;
			}

			log->Debug("AdapterSharedMemory {} : CreateFileMapping DONE", this->config.NameMemory);

			buf_data = (char*)MapViewOfFile(SM_Handle, FILE_MAP_ALL_ACCESS, 0, 0, config.size * size_type + sizeof(HeaderSharedMemory));
			if (buf_data == NULL)
			{
				current_status.store(StatusAdapter::ERROR_INIT, std::memory_order_relaxed);
				res = ResultReqest::ERR;
				throw 6;
			}

			log->Debug("AdapterSharedMemory {} : MapViewOfFileg DONE", this->config.NameMemory);

			/// --- clear shared memory --- ///
			for (unsigned int i = 0; i < config.size * size_type + sizeof(HeaderSharedMemory); i++)
			{
				*(buf_data + i) = 0;
			}

			/// --- initilization header shared memory --- ///
			HeaderSharedMemory* head = reinterpret_cast<HeaderSharedMemory*>(buf_data);
			head->size_data = config.size;
			head->typedata = config.DataType;

			ReleaseMutex(Mutex_SM);
			current_status.store(StatusAdapter::OK, std::memory_order_relaxed);

			log->Debug("AdapterSharedMemory {} : Init DONE", this->config.NameMemory);
		}
		catch (int& e)
		{
			log->Critical("Error init shared memory. name: {}, error: {}, syserror: {}", this->config.NameMemory, e, sys_error);
			if (Mutex_SM != NULL) ReleaseMutex(Mutex_SM);
			CloseHandle(Mutex_SM);
			UnmapViewOfFile(buf_data);
			CloseHandle(SM_Handle);
			res = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("Error init shared memory. name: {}, error: {}, syserror: {}", this->config.NameMemory, 0, 0);
			if (Mutex_SM != NULL) ReleaseMutex(Mutex_SM);
			CloseHandle(Mutex_SM);
			UnmapViewOfFile(buf_data);
			CloseHandle(SM_Handle);
			res = ResultReqest::ERR;
		}		

		return res;
	}

	///////////////////////////////////////////////////////////////////
	/// --- запрос специфичного параметра адаптера SharedMemory --- ///
	//////////////////////////////////////////////////////////////////
	/// <param>
	/// param - запрашиваемый параметр (enum ParamInfoAdapter) 
	/// </param>
	/// <result> 
	/// - возвращает информацию в типе std::shared_ptr<BaseAnswer>, который является родительским для типа std::shared_ptr<HeaderDataAnswerSM>

	std::shared_ptr<IAnswer> AdapterSharedMemory::GetInfoAdapter(ParamInfoAdapter param)
	{
		std::shared_ptr<IAnswer> answer = nullptr;
		std::shared_ptr<AnswerSharedMemoryHeaderData> answerHeaderData = nullptr;

		switch (param)
		{

		/// --- ответ на запрос Header DATA --- ///
		case ParamInfoAdapter::HeaderData:

			
			answerHeaderData = AnswerRequestHeaderData();
			answer = std::reinterpret_pointer_cast<IAnswer>(answerHeaderData);
			break;

		/// --- ответ не предусмотрен данным типом адапрета (ResultReqest::IGNOR) --- ///
		default:

			answer = std::make_shared<IAnswer>();
			answer->param = param;
			answer->result = ResultReqest::IGNOR;
			answer->type_adapter = TypeAdapter::SharedMemory;

			break;
		}
				
		return answer;
	}

	/////////////////////////////////////////////////
	/// --- запрос типа адаптера SharedMemory --- ///
	/////////////////////////////////////////////////
	/// <result> 
	/// - возвращает тип адаптера (TypeAdapter::SharedMemory;)

	TypeAdapter AdapterSharedMemory::GetTypeAdapter()
	{
		return TypeAdapter::SharedMemory;
	}
	
	/////////////////////////////////////////////////////////////
	/// --- запрос текущего статуса адаптера SharedMemory --- ///
	////////////////////////////////////////////////////////////
	/// <result> 
	/// - возвращает статус адаптера (StatusAdapter)
	StatusAdapter AdapterSharedMemory::GetStatusAdapter()
	{
		return current_status.load(std::memory_order::memory_order_relaxed);
	}
	
	//////////////////////////////////////////////////////
	/// --- функция чтения данных из SharedMemory --- ///
	/////////////////////////////////////////////////////
	/// <param>
	/// buf - буфер, куда будут записаны читаемые данные  (размер буфера должен соотвествовать типу и количеству данных, size*size_type байт)
	/// size -  количество данных (ед.)
	/// </param>
	/// <result> 
	/// - возвращает результат выполнения функции в типе ResultReqest
	/// 
	ResultReqest AdapterSharedMemory::ReadData(void* buf, size_t size)
	{
		log->Debug("AdapterSharedMemory {}: ReadData START", config.NameMemory);

		if (current_status.load(std::memory_order_relaxed) != StatusAdapter::OK)
		{
			log->Debug("AdapterSharedMemory {}: ReadData IGNOR", config.NameMemory);
			return ResultReqest::IGNOR;
		}

		int read_size = size > config.size ? config.size : size;
		DWORD result_win32 = 0;
		std::string helpstr;
		char* buf_in = (char*)buf;
		char* buf_out = buf_data + sizeof(HeaderSharedMemory);
		
		std::time_t time_p;
		std::tm* time_now;
		std::chrono::system_clock::time_point time;
		std::chrono::milliseconds msec;

		result_win32=WaitForSingleObject(Mutex_SM, 5000);
		if (result_win32 != WAIT_OBJECT_0)
		{
			helpstr.clear();
			if (result_win32 == WAIT_TIMEOUT) helpstr = "Error function ReadData (WAIT_TIMEOUT) Adapter:SharedMemory Name: " + config.NameMemory;
			if (result_win32 == WAIT_ABANDONED) helpstr = "Error function ReadData (WAIT_ABANDONED) Adapter:SharedMemory Name: " + config.NameMemory;
			if (result_win32 == WAIT_FAILED) helpstr = "Error function ReadData (WAIT_FAILED) Adapter:SharedMemory Name: " + config.NameMemory;
			log->WriteLogERR(helpstr.c_str(), 0, GetLastError());
			return ResultReqest::ERR;
		}

		for (int i = 0; i < size_type * read_size; i++)
		{
			*(buf_in + i) = *(buf_out+i);
		}

		HeaderSharedMemory* head = (HeaderSharedMemory*)buf_data;

		/// --- take time --- /// 

		time = std::chrono::system_clock::now();
		time_p = std::chrono::system_clock::to_time_t(time);
		time_now = std::localtime(&time_p);
		msec = std::chrono::duration_cast<std::chrono::milliseconds>(time.time_since_epoch())
			- std::chrono::duration_cast<std::chrono::milliseconds>
			(std::chrono::duration_cast<std::chrono::seconds>(time.time_since_epoch()));

		/// --- write time in header --- /// 
		head->count_read++;
		head->TimeLastRead.h = time_now->tm_hour;
		head->TimeLastRead.m = time_now->tm_min;
		head->TimeLastRead.s = time_now->tm_sec;
		head->TimeLastRead.ms = msec.count();

		ReleaseMutex(Mutex_SM);

		return ResultReqest::OK;

	}

	//////////////////////////////////////////////////////
	/// --- функция записи данных в SharedMemory --- ///
	/////////////////////////////////////////////////////
	/// <param>
	/// buf - буфер, откуда будут считаны записываемые данные  (размер буфера должен соотвествовать типу и количеству данных, size*size_type байт)
	/// size -  количество данных (ед.)
	/// </param>
	/// <result> 
	/// - возвращает результат выполнения функции в типе ResultReqest
	ResultReqest AdapterSharedMemory::WriteData(void* buf, size_t size)
	{
		if (current_status.load(std::memory_order_relaxed) != StatusAdapter::OK)
		{
			return ResultReqest::IGNOR;
		}

		int read_size = size > config.size ? config.size : size;
		DWORD result_win32 = 0;
		std::string helpstr;
		char* buf_in = (char*)buf;
		char* buf_out = buf_data + sizeof(HeaderSharedMemory);

		std::time_t time_p;
		std::tm* time_now;
		std::chrono::system_clock::time_point time;
		std::chrono::milliseconds msec;


		result_win32 = WaitForSingleObject(Mutex_SM, 5000);
		if (result_win32 != WAIT_OBJECT_0)
		{
			helpstr.clear();
			if (result_win32 == WAIT_TIMEOUT) helpstr = "Error function WriteData (WAIT_TIMEOUT) Adapter:SharedMemory Name: " + config.NameMemory;
			if (result_win32 == WAIT_ABANDONED) helpstr = "Error function WriteData (WAIT_ABANDONED) Adapter:SharedMemory Name: " + config.NameMemory;
			if (result_win32 == WAIT_FAILED) helpstr = "Error function WriteData (WAIT_FAILED) Adapter:SharedMemory Name: " + config.NameMemory;
			log->WriteLogERR(helpstr.c_str(), 0, GetLastError());
			return ResultReqest::ERR;
		}

		for (int i = 0; i < size_type * read_size; i++)
		{
			*(buf_out + i) = *(buf_in + i);
		}

		HeaderSharedMemory* head = (HeaderSharedMemory*)buf_data;

		/// --- take time --- /// 
		time = std::chrono::system_clock::now();
		time_p = std::chrono::system_clock::to_time_t(time);
		time_now = std::localtime(&time_p);		
		msec = std::chrono::duration_cast<std::chrono::milliseconds>(time.time_since_epoch())
			- std::chrono::duration_cast<std::chrono::milliseconds>
			(std::chrono::duration_cast<std::chrono::seconds>(time.time_since_epoch()));

		/// --- write time in header --- /// 
		head->count_write++;
		head->TimeLastWrite.h = time_now->tm_hour;
		head->TimeLastWrite.m = time_now->tm_min;
		head->TimeLastWrite.s = time_now->tm_sec;
		head->TimeLastWrite.ms = msec.count();

		ReleaseMutex(Mutex_SM);

		return ResultReqest::OK;

	}

	//////////////////////////////////////////////////////
	/// --- функция чтения заголовка SharedMemory --- ///
	/////////////////////////////////////////////////////
	/// <result> 
	/// - возвращает данные в виде указателя на структуру HeaderDataAnswerSM

	std::shared_ptr<AnswerSharedMemoryHeaderData> AdapterSharedMemory::AnswerRequestHeaderData()
	{
		std::shared_ptr<AnswerSharedMemoryHeaderData> point = std::make_shared<AnswerSharedMemoryHeaderData>();
		HeaderSharedMemory* head = (HeaderSharedMemory*)buf_data;

		if (buf_data == nullptr)
		{
			point->header.count_read = 0;
			point->header.count_write = 0;
			point->header.size_data = 0;
			point->header.TimeLastRead.h = 0; point->header.TimeLastRead.m = 0;
			point->header.TimeLastRead.s = 0; point->header.TimeLastRead.ms = 0;
			point->header.typedata = TypeData::ZERO;
			point->param = ParamInfoAdapter::HeaderData;
			point->type_adapter = TypeAdapter::SharedMemory;
			point->result = ResultReqest::ERR;
			return point;
		}

		WaitForSingleObject(Mutex_SM, INFINITE);		
		point->header.count_read = head->count_read;
		point->header.count_write = head->count_write;
		point->header.size_data = head->size_data;
		point->header.TimeLastRead = head->TimeLastRead;
		point->header.TimeLastWrite = head->TimeLastWrite;
		point->header.typedata = head->typedata;
		ReleaseMutex(Mutex_SM);

		point->param = ParamInfoAdapter::HeaderData;
		point->type_adapter = TypeAdapter::SharedMemory;
		point->result = ResultReqest::OK;

		return point;
	}

}

