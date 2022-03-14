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

			log->Debug("AdapterSharedMemory {}: Init START : type {}", this->config.NameChannel,(int)this->config.DataType);

			if ((int)config.DataType > 2)
			{
				sys_error = 0;
				current_status.store(StatusAdapter::ERROR_INIT, std::memory_order_relaxed);
				throw 7;
			}

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
			head->type_data = config.DataType;
			head->TimeLastRead = 0;
			head->TimeLastWrite = 0;
			head->count_read = 0;
			head->count_write = 0;
			head->size_alarms = config.size_alarms;
			head->size_data_int = config.size_data_int;
			head->size_data_float = config.size_data_float;
			head->size_data_double = config.size_data_double;
			head->size_data_char = config.size_data_char;
			head->size_str = config.size_str;
			ReleaseMutex(Mutex_SM);

			if (config.DataType == TypeData::Extended)
			{
				size_t offset = 0;
				for (int i = 0; i < config.size_data_int; i++)
				{
					map_TagToPoint[config.v_tags[i] + offset] = i;
				}

				offset += config.size_data_int;
				for (int i = 0; i < config.size_data_float; i++)
				{
					map_TagToPoint[config.v_tags[i + offset]] = i;
				}

				offset += config.size_data_float;
				for (int i = 0; i < config.size_data_double; i++)
				{
					map_TagToPoint[config.v_tags[i + offset]] = i;
				}

				offset += config.size_data_double;
				for (int i = 0; i < config.size_data_char; i++)
				{
					map_TagToPoint[config.v_tags[i + offset]] = i;
				}
			}
			
			current_status.store(StatusAdapter::OK, std::memory_order_relaxed);
			log->Debug("AdapterSharedMemory {} : Init DONE", this->config.NameChannel);
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
	ResultReqest AdapterSharedMemory::ReadData(std::shared_ptr<DDSData>& buf, ModeRead rise_data)
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

			if (config.InfoType != TypeInfo::Data)
			{
				log->Warning("AdapterSharedMemory {}: ReadData IGNOR: error {}", config.NameChannel, 2);
				return ResultReqest::IGNOR;
			}

			log->Debug("AdapterSharedMemory {}: ReadData START", config.NameChannel);

			if (data_ == nullptr)
			{
				data_ = std::make_shared<DDSData>();
				data_.get()->data_int().value().resize(config.size_data_int);
				data_.get()->data_int().quality().resize(config.size_data_int);
				data_.get()->data_float().value().resize(config.size_data_float);
				data_.get()->data_float().quality().resize(config.size_data_float);
				data_.get()->data_double().value().resize(config.size_data_double);
				data_.get()->data_double().quality().resize(config.size_data_double);
				data_.get()->data_char().value().resize(config.size_data_char);
				data_.get()->data_char().quality().resize(config.size_data_char);
				for (int i = 0; i < config.size_data_char; i++)
				{
					data_.get()->data_char().value()[i].value().resize(config.size_str);
				}
			}

			mutex_win32 = WaitForSingleObject(Mutex_SM, 5000);
			if (mutex_win32 != WAIT_OBJECT_0)
			{
				err_win32 = GetLastError();
				throw 1;
			}

			{
				char* buf_in = buf_data + Offset(TypeValue::INT);
				std::vector<int>& value = data_.get()->data_int().value();
				std::vector<char>& quality = data_.get()->data_int().quality();
				for (int i = 0; i < config.size_data_int; i++)
				{
					value[i] = *((int*)buf_in);
					buf_in += sizeof(int);
					quality[i] = *(buf_in);
					buf_in += sizeof(char);
				}
			}

			{
				char* buf_in = buf_data  + Offset(TypeValue::FLOAT);
				std::vector<float>& value = data_.get()->data_float().value();
				std::vector<char>& quality = data_.get()->data_float().quality();
				for (int i = 0; i < config.size_data_float; i++)
				{
					value[i] = *((float*)buf_in);
					buf_in += sizeof(float);
					quality[i] = *(buf_in);
					buf_in += sizeof(char);
				}
			}

			{
				char* buf_in = buf_data + Offset(TypeValue::FLOAT);
				std::vector<double>& value = data_.get()->data_double().value();
				std::vector<char>& quality = data_.get()->data_double().quality();
				for (int i = 0; i < config.size_data_double; i++)
				{
					value[i] = *((double*)buf_in);
					buf_in += sizeof(double);
					quality[i] = *(buf_in);
					buf_in += sizeof(char);
				}
			}

			{
				char* buf_in = buf_data + Offset(TypeValue::CHAR);
				std::vector<DataChar>& value = data_.get()->data_char().value();
				std::vector<char>& quality = data_.get()->data_char().quality();
				for (int i = 0; i < config.size_data_char; i++)
				{
					auto data_vec = value[i].value();
					for (int j = 0; j < config.size_str; j++)  /// can be faster
					{
						data_vec[j] = *buf_in;
						buf_in++;
					}
					quality[i] = *buf_in;
					buf_in++;
				}
			}

			HeaderSharedMemory* head = (HeaderSharedMemory*)buf_data;
			data_.get()->time_source(head->TimeLastWrite);
			
			head->TimeLastRead = TimeConverter::GetTime_LLmcs();

			/// --- write time in header --- /// 
			head->count_read++;
			head->TimeLastRead = TimeConverter::GetTime_LLmcs();	

			buf = data_;
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

	//////////////////////////////////////////////////////
	/// --- функция записи данных в SharedMemory --- ///
	/////////////////////////////////////////////////////
	/// <param>
	/// buf - буфер, откуда будут считаны записываемые данные  (размер буфера должен соотвествовать типу и количеству данных, size*size_type байт)
	/// size -  количество данных (ед.)
	/// </param>
	/// <result> 
	/// - возвращает результат выполнения функции в типе ResultReqest
	ResultReqest AdapterSharedMemory::WriteData(std::shared_ptr<DDSData>& buf)
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

			if (config.DataType != TypeData::Base)
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

			{
				char* buf_out = buf_data + Offset(TypeValue::INT);
				auto value = buf.get()->data_int().value();
				auto quality = buf.get()->data_int().quality();
				int size = config.size_data_int < value.size() ? config.size_data_int : value.size();
				for (int i = 0; i < size; i++)
				{
					*((int*)buf_out) = value[i];
					buf_out += sizeof(int);
					*(buf_out) = quality[i];
					buf_out++;
				}
			}


			{
				char* buf_out = buf_data + Offset(TypeValue::FLOAT);
				auto value = buf.get()->data_float().value();
				auto quality = buf.get()->data_float().quality();
				int size = config.size_data_float < value.size() ? config.size_data_float : value.size();
				for (int i = 0; i < size; i++)
				{
					*((float*)buf_out) = value[i];
					buf_out += sizeof(float);
					*(buf_out) = quality[i];
					buf_out++;
				}
			}

			{
				char* buf_out = buf_data + Offset(TypeValue::DOUBLE);
				auto value = buf.get()->data_double().value();
				auto quality = buf.get()->data_double().quality();
				int size = config.size_data_double < value.size() ? config.size_data_double : value.size();
				for (int i = 0; i < size; i++)
				{
					*((double*)buf_out) = value[i];
					buf_out += sizeof(double);
					*(buf_out) = quality[i];
					buf_out++;
				}
			}

			{
				char* buf_out = buf_data + Offset(TypeValue::CHAR);
				auto value = buf.get()->data_char().value();
				auto quality = buf.get()->data_char().quality();
				int size = config.size_data_char < value.size() ? config.size_data_char : value.size();
				for (int i = 0; i < size; i++)
				{
					auto data_vec = value[i].value();
					for (int j = 0; j < config.size_str < data_vec.size() ? config.size_str : data_vec.size(); j++)
					{
						data_vec[j] = *buf_out;
						buf_out++;
					}
					quality[i] = *buf_out;
					buf_out++;
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

	ResultReqest AdapterSharedMemory::ReadData(std::shared_ptr<DDSDataEx>& buf, ModeRead mode_read)
	{
		DWORD mutex_win32 = 0;
		DWORD err_win32 = 0;
		ResultReqest res = ResultReqest::OK;

		try
		{
			if (current_status.load(std::memory_order_relaxed) != StatusAdapter::OK)
			{
				log->Debug("AdapterSharedMemory {}: ReadExData IGNOR: error {}", config.NameChannel, 1);
				return ResultReqest::IGNOR;
			}

			if (config.DataType != TypeData::Extended)
			{
				log->Debug("AdapterSharedMemory {}: ReadExData IGNOR: error {}", config.NameChannel, 2);
				return ResultReqest::IGNOR;
			}

			log->Debug("AdapterSharedMemory {}: ReadExData START", config.NameChannel);

			if (data_ == nullptr)
			{
				data_ = std::make_shared<DDSData>();
				data_.get()->data_int().value().resize(config.size_data_int);
				data_.get()->data_int().quality().resize(config.size_data_int);
				data_.get()->data_float().value().resize(config.size_data_float);
				data_.get()->data_float().quality().resize(config.size_data_float);
				data_.get()->data_double().value().resize(config.size_data_double);
				data_.get()->data_double().quality().resize(config.size_data_double);
				data_.get()->data_char().value().resize(config.size_data_char);
				data_.get()->data_char().quality().resize(config.size_data_char);
				for (int i = 0; i < config.size_data_char; i++)
				{
					data_.get()->data_char().value()[i].value().resize(config.size_str);
				}
			}

			HeaderSharedMemory* head = (HeaderSharedMemory*)buf_data;

			mutex_win32 = WaitForSingleObject(Mutex_SM, 5000);
			if (mutex_win32 != WAIT_OBJECT_0)
			{
				err_win32 = GetLastError();
				throw 1;
			}

			{
				char* buf_in = buf_data + Offset(TypeValue::INT);
				std::vector<int>& value = data_.get()->data_int().value();
				std::vector<char>& quality = data_.get()->data_int().quality();
				for (int i = 0; i < config.size_data_int; i++)
				{
					value[i] = *((int*)buf_in);
					buf_in += sizeof(int);
					quality[i] = *(buf_in);
					buf_in += sizeof(char);
				}
			}

			{
				char* buf_in = buf_data + Offset(TypeValue::FLOAT);
				std::vector<float>& value = data_.get()->data_float().value();
				std::vector<char>& quality = data_.get()->data_float().quality();
				for (int i = 0; i < config.size_data_float; i++)
				{
					value[i] = *((float*)buf_in);
					buf_in += sizeof(float);
					quality[i] = *(buf_in);
					buf_in += sizeof(char);
				}
			}

			{
				char* buf_in = buf_data + Offset(TypeValue::FLOAT);
				std::vector<double>& value = data_.get()->data_double().value();
				std::vector<char>& quality = data_.get()->data_double().quality();
				for (int i = 0; i < config.size_data_double; i++)
				{
					value[i] = *((double*)buf_in);
					buf_in += sizeof(double);
					quality[i] = *(buf_in);
					buf_in += sizeof(char);
				}
			}

			{
				char* buf_in = buf_data + Offset(TypeValue::CHAR);
				std::vector<DataChar>& value = data_.get()->data_char().value();
				std::vector<char>& quality = data_.get()->data_char().quality();
				for (int i = 0; i < config.size_data_char; i++)
				{
					auto data_vec = value[i].value();
					for (int j = 0; j < config.size_str; j++)  /// can be faster
					{
						data_vec[j] = *buf_in;
						buf_in++;
					}
					quality[i] = *buf_in;
					buf_in++;
				}
			}

			data_.get()->time_source(head->TimeLastWrite);

			head->TimeLastRead = TimeConverter::GetTime_LLmcs();
			/// --- write time in header --- /// 
			head->count_read++;
			head->TimeLastRead = TimeConverter::GetTime_LLmcs();

			buf = create_extended_data(mode_read);
		}
		catch (int& e)
		{

			log->Critical("AdapterSharedMemory {}: ERROR ReadExData: error: {}, syserror: {}", config.NameChannel, e, err_win32);
			res = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterSharedMemory {}: ERROR ReadExData: error: {}, syserror: {}", config.NameChannel, 0, 0);
			res = ResultReqest::ERR;
		}

		if (mutex_win32 == WAIT_OBJECT_0) ReleaseMutex(Mutex_SM);
		return res;
	}

	ResultReqest AdapterSharedMemory::WriteData(std::shared_ptr<DDSDataEx>& buf)
	{
		DWORD mutex_win32 = 0;
		DWORD err_win32 = 0;
		ResultReqest res = ResultReqest::OK;

		try
		{
			if (current_status.load(std::memory_order_relaxed) != StatusAdapter::OK)
			{
				log->Debug("AdapterSharedMemory {}: WriteExData IGNOR", config.NameChannel);
				return ResultReqest::IGNOR;
			}

			if (config.DataType != TypeData::Extended)
			{
				log->Debug("AdapterSharedMemory {}: WriteExData IGNOR", config.NameChannel);
				return ResultReqest::IGNOR;
			}

			log->Debug("AdapterSharedMemory {}: WriteExData START", config.NameChannel);

			mutex_win32 = WaitForSingleObject(Mutex_SM, 5000);
			if (mutex_win32 != WAIT_OBJECT_0)
			{
				err_win32 = GetLastError();
				throw 1;
			}

			HeaderSharedMemory* head = (HeaderSharedMemory*)buf_data;

			{
				std::vector<DataExInt>& vec_ex = buf.get()->data_int();
				std::map<uint32_t, uint32_t>::iterator map_element;
				char* elements = buf_data + Offset(TypeValue::INT);
				char* element=0;
				for (int i = 0; i < vec_ex.size(); i++)
				{
					map_element = map_TagToPoint.find(vec_ex[i].id_tag());
					if (map_element != map_TagToPoint.end())
					{
						element = elements + map_element->second * (sizeof(int) + sizeof(char));
						*(int*)element = vec_ex[i].value();
						element+= sizeof(int);
						*element = vec_ex[i].quality();
					}
				}
			}

			{
				std::vector<DataExFloat>& vec_ex = buf.get()->data_float();
				std::map<uint32_t, uint32_t>::iterator map_element;
				char* elements = buf_data + Offset(TypeValue::FLOAT);
				char* element = 0;
				for (int i = 0; i < vec_ex.size(); i++)
				{
					map_element = map_TagToPoint.find(vec_ex[i].id_tag());
					if (map_element != map_TagToPoint.end())
					{
						element = elements + map_element->second * (sizeof(float) + sizeof(char));
						*(float*)element = vec_ex[i].value();
						element += sizeof(float);
						*element = vec_ex[i].quality();
					}
				}
			}

			{
				std::vector<DataExDouble>& vec_ex = buf.get()->data_double();
				std::map<uint32_t, uint32_t>::iterator map_element;
				char* elements = buf_data + Offset(TypeValue::DOUBLE);
				char* element = 0;
				for (int i = 0; i < vec_ex.size(); i++)
				{
					map_element = map_TagToPoint.find(vec_ex[i].id_tag());
					if (map_element != map_TagToPoint.end())
					{
						element = elements + map_element->second * (sizeof(double) + sizeof(char));
						*(double*)element = vec_ex[i].value();
						element += sizeof(double);
						*element = vec_ex[i].quality();
					}
				}
			}

			{
				std::vector<DataExChar>& vec_ex = buf.get()->data_char();
				std::map<uint32_t, uint32_t>::iterator map_element;
				char* elements = buf_data + Offset(TypeValue::CHAR);
				char* element = 0;
				for (int i = 0; i < vec_ex.size(); i++)
				{
					map_element = map_TagToPoint.find(vec_ex[i].id_tag());
					if (map_element != map_TagToPoint.end())
					{
						element = elements + map_element->second * (sizeof(char)*config.size_str + sizeof(char));
						for (int j = 0; j < vec_ex[i].value().size(); j++)
						{
							*element = vec_ex[i].value()[j];
							element++;
						}

						for (int j = 0; j < (config.size_str-vec_ex[i].value().size()); j++)
						{
							*element = '\0';
							element++;
						}

						*element = vec_ex[i].quality();
					}
				}
			}

			head->count_write++;
			head->TimeLastWrite = TimeConverter::GetTime_LLmcs();

		}
		catch (int& e)
		{

			log->Critical("AdapterSharedMemory {}: ERROR WriteExData: error: {}, syserror: {}", config.NameChannel, e, err_win32);
			res = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterSharedMemory {}: ERROR WriteExData: error: {}, syserror: {}", config.NameChannel, 0, 0);
			res = ResultReqest::ERR;
		}

		if (mutex_win32 == WAIT_OBJECT_0) ReleaseMutex(Mutex_SM);
		return res;
	}

	ResultReqest AdapterSharedMemory::ReadData(std::shared_ptr<DDSAlarm>& buf, ModeRead rise_data) { return ResultReqest::IGNOR; };
	ResultReqest AdapterSharedMemory::WriteData(std::shared_ptr<DDSAlarm>& buf) { return ResultReqest::IGNOR; };
	ResultReqest AdapterSharedMemory::ReadData(std::shared_ptr<DDSAlarmEx>& buf, ModeRead rise_data) { return ResultReqest::IGNOR; };
	ResultReqest AdapterSharedMemory::WriteData(std::shared_ptr<DDSAlarmEx>& buf) { return ResultReqest::IGNOR; };

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

		if (config.InfoType == TypeInfo::Data)
		{
			result += (sizeof(int) + sizeof(char)) * config.size_data_int; // size DataCollectionInt
			result += (sizeof(float) + sizeof(char)) * config.size_data_float; // size DataCollectionFloat
			result += (sizeof(double) + sizeof(char)) * config.size_data_double; // size DataCollectionDouble
			result += (sizeof(char)*config.size_str + sizeof(char)) * config.size_data_char; // size DataCollectionInt
		}
		else if (config.InfoType == TypeInfo::Alarm)
		{
			result += (sizeof(uint32_t) + sizeof(uint32_t)) * (config.size_alarms/32+1); // size DataCollectionInt
		}

		return result;
	}

	size_t AdapterSharedMemory::Offset(TypeValue type_value)
	{
		size_t result = 0;
		result += sizeof(HeaderSharedMemory);
		if (type_value == TypeValue::INT) return result;
		
		result += (sizeof(int) + sizeof(char)) * config.size_data_int;
		if (type_value == TypeValue::FLOAT) return result;

		result += (sizeof(float) + sizeof(char)) * config.size_data_float;
		if (type_value == TypeValue::DOUBLE) return result;

		result += (sizeof(double) + sizeof(char)) * config.size_data_double;
		if (type_value == TypeValue::CHAR) return result;

		return result;
	}

	std::shared_ptr<DDSDataEx> AdapterSharedMemory::create_extended_data(ModeRead mode_read)
	{
		std::shared_ptr<DDSDataEx> buf_ex;
		if (mode_read == ModeRead::Full || data_last == nullptr)
		{
			buf_ex = create_extended_fulldata();
		}
		else if (mode_read == ModeRead::Regular && data_last != nullptr)
		{
			buf_ex = create_extended_diffdata();
		}

		return buf_ex;
	}

	std::shared_ptr<DDSDataEx> AdapterSharedMemory::create_extended_fulldata()
	{
		size_t iter_tag = 0;
		std::shared_ptr<DDSDataEx> buf_ = std::make_shared<DDSDataEx>();
		buf_.get()->data_int().resize(config.size_data_int);
		buf_.get()->data_float().resize(config.size_data_float);
		buf_.get()->data_double().resize(config.size_data_double);
		buf_.get()->data_char().resize(config.size_data_char);
		for (int i = 0; i < config.size_data_char; i++)
		{
			buf_.get()->data_char()[i].value().resize(config.size_str);
		}

		long long time_ = data_.get()->time_source();
		
		{
			std::vector<DataExInt>& vec_ex = buf_.get()->data_int();
			std::vector<int>& vec_value = data_.get()->data_int().value();
			std::vector<char>& vec_quality = data_.get()->data_int().quality();
			for (int i = 0; i < config.size_data_int; i++)
			{
				vec_ex[i].time_source(time_);
				vec_ex[i].id_tag(config.v_tags[iter_tag]);
				vec_ex[i].value(vec_value[i]);
				vec_ex[i].quality(vec_quality[i]);
				iter_tag++;
			}
		}

		{
			std::vector<DataExFloat>& vec_ex = buf_.get()->data_float();
			std::vector<float>& vec_value = data_.get()->data_float().value();
			std::vector<char>& vec_quality = data_.get()->data_float().quality();
			for (int i = 0; i < config.size_data_float; i++)
			{
				vec_ex[i].time_source(time_);
				vec_ex[i].id_tag(config.v_tags[iter_tag]);
				vec_ex[i].value(vec_value[i]);
				vec_ex[i].quality(vec_quality[i]);
				iter_tag++;
			}
		}

		{
			std::vector<DataExDouble>& vec_ex = buf_.get()->data_double();
			std::vector<double>& vec_value = data_.get()->data_double().value();
			std::vector<char>& vec_quality = data_.get()->data_double().quality();
			for (int i = 0; i < config.size_data_double; i++)
			{
				vec_ex[i].time_source(time_);
				vec_ex[i].id_tag(config.v_tags[iter_tag]);
				vec_ex[i].value(vec_value[i]);
				vec_ex[i].quality(vec_quality[i]);
				iter_tag++;
			}
		}

		{
			std::vector<DataExChar>& vec_ex = buf_.get()->data_char();
			std::vector<DataChar>& vec_value = data_.get()->data_char().value();
			std::vector<char>& vec_quality = data_.get()->data_char().quality();
			for (int i = 0; i < config.size_data_double; i++)
			{
				vec_ex[i].time_source(time_);
				vec_ex[i].id_tag(config.v_tags[iter_tag]);
				vec_ex[i].value(vec_value[i].value());
				vec_ex[i].quality(vec_quality[i]);
				iter_tag++;
			}
		}

		data_last.swap(data_);
		return buf_;
	}

	std::shared_ptr<DDSDataEx> AdapterSharedMemory::create_extended_diffdata()
	{
		size_t iter_tag = 0;
		std::shared_ptr<DDSDataEx> buf_ = std::make_shared<DDSDataEx>();

		long long time_ = data_.get()->time_source();

		{
			std::vector<DataExInt>& vec_ex = buf_.get()->data_int();
			DataExInt value;
			std::vector<int>& vec_value = data_.get()->data_int().value();
			std::vector<char>& vec_quality = data_.get()->data_int().quality();
			std::vector<int>& vec_value_last = data_last.get()->data_int().value();
			std::vector<char>& vec_quality_last = data_last.get()->data_int().quality();
			for (int i = 0; i < config.size_data_int; i++)
			{
				if (vec_value[i] != vec_value_last[i] || vec_quality[i] != vec_quality_last[i])
				{
					value.time_source(time_);
					value.id_tag(config.v_tags[iter_tag]);
					value.value(vec_value[i]);
					value.quality(vec_quality[i]);
					vec_ex.push_back(value);
				}
				iter_tag++;
			}
		}

		{
			std::vector<DataExFloat>& vec_ex = buf_.get()->data_float();
			DataExFloat value;
			std::vector<float>& vec_value = data_.get()->data_float().value();
			std::vector<char>& vec_quality = data_.get()->data_float().quality();
			std::vector<float>& vec_value_last = data_last.get()->data_float().value();
			std::vector<char>& vec_quality_last = data_last.get()->data_float().quality();
			for (int i = 0; i < config.size_data_float; i++)
			{
				if (vec_value[i] != vec_value_last[i] || vec_quality[i] != vec_quality_last[i])
				{
					value.time_source(time_);
					value.id_tag(config.v_tags[iter_tag]);
					value.value(vec_value[i]);
					value.quality(vec_quality[i]);
					vec_ex.push_back(value);
				}
				iter_tag++;
			}
		}

		{
			std::vector<DataExDouble>& vec_ex = buf_.get()->data_double();
			DataExDouble value;
			std::vector<double>& vec_value = data_.get()->data_double().value();
			std::vector<char>& vec_quality = data_.get()->data_double().quality();
			std::vector<double>& vec_value_last = data_last.get()->data_double().value();
			std::vector<char>& vec_quality_last = data_last.get()->data_double().quality();
			for (int i = 0; i < config.size_data_double; i++)
			{
				if (vec_value[i] != vec_value_last[i] || vec_quality[i] != vec_quality_last[i])
				{
					value.time_source(time_);
					value.id_tag(config.v_tags[iter_tag]);
					value.value(vec_value[i]);
					value.quality(vec_quality[i]);
					vec_ex.push_back(value);
				}
				iter_tag++;
			}
		}

		{
			std::vector<DataExChar>& vec_ex = buf_.get()->data_char();
			DataExChar value;
			std::vector<DataChar>& vec_value = data_.get()->data_char().value();
			std::vector<char>& vec_quality = data_.get()->data_char().quality();
			std::vector<DataChar>& vec_value_last = data_last.get()->data_char().value();
			std::vector<char>& vec_quality_last = data_last.get()->data_char().quality();
			for (int i = 0; i < config.size_data_char; i++)
			{
				if (vec_value[i].value() != vec_value_last[i].value() || vec_quality[i] != vec_quality_last[i]) // проверить 
				{
					value.time_source(time_);
					value.id_tag(config.v_tags[iter_tag]);
					value.value(vec_value[i].value());
					value.quality(vec_quality[i]);
					vec_ex.push_back(value);
				}
				iter_tag++;
			}
		}

		data_last.swap(data_);
		return buf_;
	}
}

