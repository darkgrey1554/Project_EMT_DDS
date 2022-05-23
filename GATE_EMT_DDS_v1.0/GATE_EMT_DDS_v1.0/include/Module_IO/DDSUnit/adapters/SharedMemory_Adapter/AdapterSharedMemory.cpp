#include "AdapterSharedMemory.hpp"

namespace scada_ate::gate::adapter::sem
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
	/// --- конструктор адаптера SharedMemory --- ///
	/////////////////////////////////////////////////

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
		size_t size_memory;

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
			if (config_point == nullptr || config_point->type_adapter != TypeAdapter::SharedMemory) throw 1;

			/// --- coping of configuration --- ///
			this->config = *config_point;
			vec_link_tags = config.vec_link_tasg;
			vec_tags_source = config.vec_tags_source;

			log->Info("AdapterSharedMemory {}: Init START", this->config.NameChannel);

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
			
			log->Debug("AdapterSharedMemory {} : InitSecurityAttributs DONE", this->config.NameChannel);

			/// --- defining size units of data --- ///
			size_memory = GetSizeMemory();

			/// --- initialization handle of shared memory --- ///
			if (SM_Handle != NULL)
			{
				current_status.store(StatusAdapter::ERROR_INIT, std::memory_order_relaxed);
				throw 3;
			}

			namememory = CreateSMName();
			namemutex =  CreateSMMutexName();

			Mutex_SM = CreateMutexA(&security_attr->getsecurityattrebut(), TRUE, namemutex.c_str());
			if (Mutex_SM == NULL)
			{
				sys_error = GetLastError();
				current_status.store(StatusAdapter::ERROR_INIT, std::memory_order_relaxed);
				throw 4;
			}

			log->Debug("AdapterSharedMemory {} : CreateMutex DONE", this->config.NameChannel);

			SM_Handle = CreateFileMappingA(INVALID_HANDLE_VALUE, &security_attr->getsecurityattrebut(), PAGE_READWRITE, 0, size_memory, namememory.c_str());
			if (SM_Handle == NULL)
			{
				sys_error = GetLastError();
				current_status.store(StatusAdapter::ERROR_INIT, std::memory_order_relaxed);
				res = ResultReqest::ERR;
				throw 5;
			}

			log->Debug("AdapterSharedMemory {} : CreateFileMapping DONE", this->config.NameChannel);

			buf_data = (char*)MapViewOfFile(SM_Handle, FILE_MAP_ALL_ACCESS, 0, 0, size_memory);
			if (buf_data == NULL)
			{
				current_status.store(StatusAdapter::ERROR_INIT, std::memory_order_relaxed);
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
			head->size_str = config.size_str;
			ReleaseMutex(Mutex_SM);
			
			/// --- init array GetTags --- /// 

			for (InfoTag& tag : vec_tags_source)
			{
				if (tag.type == TypeValue::INT)
				{
					_gendata_recive.map_int_data[tag] = {0,0,0};
					continue;
				}
				if (tag.type == TypeValue::FLOAT)
				{
					_gendata_recive.map_float_data[tag] = { 0,0,0 };
					continue;
				}
				if (tag.type == TypeValue::DOUBLE)
				{
					_gendata_recive.map_double_data[tag] = { 0,0,0 };
					continue;
				}
				if (tag.type == TypeValue::CHAR)
				{
					_gendata_recive.map_char_data[tag] = { 0,'\0',0 };
					continue;
				}
				if (tag.type == TypeValue::STRING)
				{
					_gendata_recive.map_str_data[tag] = { 0,"",0};
					continue;
				}
			}

			offset_int = sizeof(HeaderSharedMemory);
			offset_float = offset_int + (sizeof(char) + sizeof(int)) * config.size_int_data;
			

			/*for (LinkTags& tag : vec_link_tasg)
			{
				if (tag..type == TypeValue::INT)
				{
					_gendata.map_int_data[tag.first] = { 0,0,0 };
					continue;
				}
				if (tag.first.type == TypeValue::FLOAT)
				{
					_gendata.map_float_data[tag.first] = { 0,0,0 };
					continue;
				}
				if (tag.first.type == TypeValue::DOUBLE)
				{
					_gendata.map_double_data[tag.first] = { 0,0,0 };
					continue;
				}
				if (tag.first.type == TypeValue::CHAR)
				{
					_gendata.map_char_data[tag.first] = { 0,'\0',0 };
					continue;
				}
				if (tag.first.type == TypeValue::STRING)
				{
					_gendata.map_str_data[tag.first] = { 0,"",0 };
					continue;
				}
			}*/

			current_status.store(StatusAdapter::OK, std::memory_order_relaxed);
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
	ResultReqest AdapterSharedMemory::ReadData(GenTags& data)
	{	
		DWORD mutex_win32 = 0;
		DWORD err_win32 = 0;
		ResultReqest res = ResultReqest::OK;

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
			size_t offset=0;
			long long time_data;

			ValueInt val_int;
			ValueFloat val_float;

			mutex_win32 = WaitForSingleObject(Mutex_SM, 5000);
			if (mutex_win32 != WAIT_OBJECT_0)
			{
				err_win32 = GetLastError();
				throw 1;
			}

			time_data = head->TimeLastWrite;

			for (auto it = _gendata_recive.map_int_data.begin(); it != _gendata_recive.map_int_data.end(); it++)
			{
				_buf = buf_data + offset_int + it->first.offset * (sizeof(int) + sizeof(char));
				it->second.value = *(int*)_buf;
				_buf = _buf + sizeof(int);
				it->second.quality = *_buf;
				it->second.time = time_data;
			}

			for (auto it = _gendata_recive.map_float_data.begin(); it != _gendata_recive.map_float_data.end(); it++)
			{
				_buf = buf_data + offset_float + it->first.offset * (sizeof(float) + sizeof(char));
				it->second.value = *(float*)_buf;
				_buf = _buf + sizeof(float);
				it->second.quality = *_buf;
				it->second.time = time_data;
			}

			for (auto it = _gendata_recive.map_double_data.begin(); it != _gendata_recive.map_double_data.end(); it++)
			{
				_buf = buf_data + TakeOffset(it->first.type, it->first.offset);
				it->second.value = *(double*)_buf;
				_buf = _buf + sizeof(double);
				it->second.quality = *_buf;
				it->second.time = time_data;
			}

			for (auto it = _gendata_recive.map_char_data.begin(); it != _gendata_recive.map_char_data.end(); it++)
			{
				_buf = buf_data + TakeOffset(it->first.type, it->first.offset);
				it->second.value = *_buf;
				_buf = _buf + sizeof(char);
				it->second.quality = *_buf;
				it->second.time = time_data;
			}

			for (auto it = _gendata_recive.map_str_data.begin(); it != _gendata_recive.map_str_data.end(); it++)
			{
				_buf = buf_data + TakeOffset(it->first.type, it->first.offset);
				it->second.value = *_buf;
				_buf = _buf + config.size_str;
				it->second.quality = *_buf;
				it->second.time = time_data;
			}

			/// --- write time in header --- /// 
			_gendata_recive.time_source = head->TimeLastWrite;
			head->TimeLastRead = TimeConverter::GetTime_LLmcs();
			head->count_read++;

			data = _gendata_recive;
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

		//if (mutex_win32 == WAIT_OBJECT_0) ReleaseMutex(Mutex_SM);
		return res;
	}

	/*ResultReqest AdapterSharedMemory::ReadData(GenTags& data)
	{
		DWORD mutex_win32 = 0;
		DWORD err_win32 = 0;
		ResultReqest res = ResultReqest::OK;

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
			size_t offset=0;
			long long time_data;

			ValueInt val_int;
			ValueFloat val_float;

			mutex_win32 = WaitForSingleObject(Mutex_SM, 5000);
			if (mutex_win32 != WAIT_OBJECT_0)
			{
				err_win32 = GetLastError();
				throw 1;
			}

			time_data = head->TimeLastWrite;
			val_int.time = time_data;
			val_float.time = time_data;

			for (InfoTag& tag : vec_tags_source)
			{
				if (tag.type == TypeValue::INT)
				{
					offset = TakeOffset(tag.type, tag.offset);
					if (offset == 0) continue;
					_buf = buf_data + offset;
					int_value = *((int*)_buf);
					quality = *(_buf+sizeof(int));
					_gendata_recive.map_int_data[tag] = { time_data, int_value, quality };
					continue;
				}

				if (tag.type == TypeValue::FLOAT)
				{
					offset = TakeOffset(tag.type, tag.offset);
					if (offset == 0) continue;
					_buf = buf_data + offset;
					float_value = *((float*)_buf);
					quality = *(_buf + sizeof(float));
					_gendata_recive.map_float_data[tag] = { time_data, float_value, quality };
					continue;
				}

				if (tag.type == TypeValue::DOUBLE)
				{
					offset = TakeOffset(tag.type, tag.offset);
					if (offset == 0) continue;
					_buf = buf_data + offset;
					double_value = *((double*)_buf);
					quality = *(_buf + sizeof(double));
					_gendata_recive.map_double_data[tag] = { time_data, double_value, quality };
					continue;
				}

				if (tag.type == TypeValue::CHAR)
				{
					offset = TakeOffset(tag.type, tag.offset);
					if (offset == 0) continue;
					_buf = buf_data + offset;
					char_value = *(_buf);
					quality = *(_buf + sizeof(char));
					_gendata_recive.map_char_data[tag] = { time_data, char_value, quality };
					continue;
				}

				if (tag.type == TypeValue::STRING)
				{
					offset = TakeOffset(tag.type, tag.offset);
					if (offset == 0) continue;
					_buf = buf_data + offset;
					str_value = _buf;
					quality = *(_buf + sizeof(char)*config.size_str);
					_gendata_recive.map_str_data[tag] = { time_data, str_value, quality };
					continue;
				}

			}

			/// --- write time in header --- ///
			_gendata_recive.time_source = head->TimeLastWrite;
			head->TimeLastRead = TimeConverter::GetTime_LLmcs();
			head->count_read++;

			data = _gendata_recive;
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

		//if (mutex_win32 == WAIT_OBJECT_0) ReleaseMutex(Mutex_SM);
		return res;
	}*/

	//////////////////////////////////////////////////////
	/// --- функция записи данных в SharedMemory --- ///
	/////////////////////////////////////////////////////
	/// <param>
	/// buf - буфер, откуда будут считаны записываемые данные  (размер буфера должен соотвествовать типу и количеству данных, size*size_type байт)
	/// size -  количество данных (ед.)
	/// </param>
	/// <result> 
	/// - возвращает результат выполнения функции в типе ResultReqest
	ResultReqest AdapterSharedMemory::WriteData(const GenTags& data)
	{	
		DWORD mutex_win32 = 0;
		DWORD err_win32 = 0;
		ResultReqest res = ResultReqest::OK;

		try
		{
			if (current_status.load(std::memory_order_relaxed) != StatusAdapter::OK)
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

			ValueInt value_int;
			ValueFloat value_float;
			ValueDouble value_double;
			ValueChar value_char;
			ValueString value_str;


			for (const LinkTags& link_tag : vec_link_tags)
			{

				if (link_tag.source.type == TypeValue::INT)
				{
					value_int = data.map_int_data.at(link_tag.source);
					set_data(value_int, link_tag);
					continue;
				}

				if (link_tag.source.type == TypeValue::FLOAT)
				{
					value_float = data.map_float_data.at(link_tag.source);
					set_data(value_float, link_tag);
					continue;
				}

				if (link_tag.source.type == TypeValue::DOUBLE)
				{
					value_double = data.map_double_data.at(link_tag.source);
					set_data(value_double, link_tag);
					continue;
				}

				if (link_tag.source.type == TypeValue::CHAR)
				{
					value_char = data.map_char_data.at(link_tag.source);
					set_data(value_char, link_tag);
					continue;
				}

				if (link_tag.source.type == TypeValue::STRING)
				{
					value_str = data.map_str_data.at(link_tag.source);
					set_data(value_str, link_tag);
					continue;
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


	//////////////////////////////////////////////////////
	/// --- функция чтения заголовка SharedMemory --- ///
	/////////////////////////////////////////////////////
	/// <result> 
	/// - возвращает данные в виде указателя на структуру HeaderDataAnswerSM

	std::shared_ptr<AnswerSharedMemoryHeaderData> AdapterSharedMemory::AnswerRequestHeaderData()
	{
		std::shared_ptr<AnswerSharedMemoryHeaderData> point = std::make_shared<AnswerSharedMemoryHeaderData>();
		HeaderSharedMemory* head = (HeaderSharedMemory*)buf_data;
		DWORD mutex_win32 = 0;
		DWORD err_win32 = 0;

		/// <summary>
		/// доделать
		/// </summary>
		return 	point;
	}

	size_t AdapterSharedMemory::GetSizeMemory()
	{
		size_t result = 0;
		result += sizeof(HeaderSharedMemory);

		result += (sizeof(int) + sizeof(char)) * config.size_int_data; // size DataCollectionInt
		result += (sizeof(float) + sizeof(char)) * config.size_float_data; // size DataCollectionFloat
		result += (sizeof(double) + sizeof(char)) * config.size_double_data; // size DataCollectionDouble
		result += (sizeof(char) + sizeof(char)) * config.size_char_data; // size DataCollectionInt
		result += (sizeof(char)*config.size_str + sizeof(char) + sizeof(char)) * config.size_str_data; // size DataCollectionInt

		return result;
	}

	size_t AdapterSharedMemory::TakeOffset(const TypeValue& type_value, const size_t& ofs) const
	{
		size_t result = 0;
		result += sizeof(HeaderSharedMemory);
		if (type_value == TypeValue::INT)
		{
			if (ofs > config.size_int_data) return 0;
			result += ofs * (sizeof(int) + sizeof(char));
			return result;
		}

		result += (sizeof(int) + sizeof(char)) * config.size_int_data;
		if (type_value == TypeValue::FLOAT)
		{
			if (ofs > config.size_float_data) return 0;
			result += ofs * (sizeof(float) + sizeof(char));
			return result;
		}

		result += (sizeof(float) + sizeof(char)) * config.size_float_data;
		if (type_value == TypeValue::DOUBLE)
		{
			if (ofs > config.size_double_data) return 0;
			result += ofs * (sizeof(double) + sizeof(char));
			return result;
		}

		result += (sizeof(double) + sizeof(char)) * config.size_double_data;
		if (type_value == TypeValue::CHAR)
		{
			if (ofs > config.size_char_data) return 0;
			result += ofs * (sizeof(char) + sizeof(char));
			return result;
		}

		result += (sizeof(char) + sizeof(char)) * config.size_char_data;
		if (type_value == TypeValue::STRING)
		{
			if (ofs > config.size_str_data) return 0;
			result += ofs * (sizeof(char) * config.size_str + sizeof(char) + sizeof(char));
			return result;
		}

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

	void AdapterSharedMemory::set_data(const ValueInt& value, const LinkTags& link)
	{
		char* buf;
		int val_last;
		int val_current = 0;

		buf = buf_data + TakeOffset(link.target.type, link.target.offset);
		val_last = *((int*)buf);
		
		if (link.source.mask != 0)
		{
			val_current = demask(value.value, link.source.mask, val_last, link.target.mask);
		}
		else
		{
			val_current = value.value;
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

	void AdapterSharedMemory::set_data(const ValueFloat& value, const LinkTags& link)
	{
		char* buf;
		float val_last;

		buf = buf_data + TakeOffset(link.target.type, link.target.offset);
		val_last = *((float*)buf);

		if (link.type_registration == TypeRegistration::RECIVE)
		{

			*((float*)buf) = value.value;
			buf += sizeof(float);
			*buf = value.quality;
			return;
		}

		if (link.type_registration == TypeRegistration::UPDATE)
		{
			if (val_last == value.value) return;
			*((float*)buf) = value.value;
			buf += sizeof(float);
			*buf = value.quality;
			return;
		}

		if (link.type_registration == TypeRegistration::DELTA)
		{
			if (fabs(val_last - value.value) > link.delta)
			{
				*((float*)buf) = value.value;
				buf += sizeof(float);
				*buf = value.quality;
				return;
			}
		}
	}

	void AdapterSharedMemory::set_data(const ValueDouble& value, const LinkTags& link)
	{
		char* buf;
		double val_last;

		buf = buf_data + TakeOffset(link.target.type, link.target.offset);
		val_last = *((double*)buf);

		if (link.type_registration == TypeRegistration::RECIVE)
		{

			*((double*)buf) = value.value;
			buf += sizeof(double);
			*buf = value.quality;
			return;
		}

		if (link.type_registration == TypeRegistration::UPDATE)
		{
			if (val_last == value.value) return;
			*((double*)buf) = value.value;
			buf += sizeof(double);
			*buf = value.quality;
			return;
		}

		if (link.type_registration == TypeRegistration::DELTA)
		{
			if (fabs(val_last - value.value) > link.delta)
			{
				*((double*)buf) = value.value;
				buf += sizeof(double);
				*buf = value.quality;
				return;
			}
		}
	}

	void AdapterSharedMemory::set_data(const ValueChar& value, const LinkTags& link)
	{
		char* buf;
		char val_last;

		buf = buf_data + TakeOffset(link.target.type, link.target.offset);
		val_last = *((char*)buf);

		if (link.type_registration == TypeRegistration::RECIVE)
		{

			*((char*)buf) = value.value;
			buf += sizeof(char);
			*buf = value.quality;
			return;
		}

		if (link.type_registration == TypeRegistration::UPDATE)
		{
			if (val_last == value.value) return;
			*((char*)buf) = value.value;
			buf += sizeof(char);
			*buf = value.quality;
			return;
		}

		if (link.type_registration == TypeRegistration::DELTA)
		{
			if (fabs(val_last - value.value) > link.delta)
			{
				*((char*)buf) = value.value;
				buf += sizeof(char);
				*buf = value.quality;
				return;
			}
		}
	}

	void AdapterSharedMemory::set_data(const ValueString& value, const LinkTags& link)
	{
		char* buf;
		std::string val_last;

		buf = buf_data + TakeOffset(link.target.type, link.target.offset);
		val_last = *((char*)buf);

		if (link.type_registration == TypeRegistration::RECIVE)
		{
			size_t i = 0;
			const char* str = value.value.c_str();
			while (i < config.size_str && i < value.value.size()) 
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
			if (val_last.compare(value.value) == 0) return;
			size_t i = 0;
			const char* str = value.value.c_str();
			while (i < config.size_str && i < value.value.size())
			{
				*((char*)buf) = *(str + i);
				buf += sizeof(char);
				i++;
			}
			*buf = value.quality;
			return;
		}
	}
}

