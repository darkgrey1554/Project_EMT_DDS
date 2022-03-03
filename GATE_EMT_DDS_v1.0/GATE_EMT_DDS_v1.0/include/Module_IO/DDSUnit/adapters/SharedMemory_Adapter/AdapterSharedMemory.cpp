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

			log->Debug("AdapterSharedMemory {}: Init START : type-{}", (char)this->config.DataType);

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
			
			log->Debug("AdapterSharedMemory {} : InitSecurityAttributs DONE", this->config.NameMemory);

			/// --- defining size units of data --- ///
			size_memory = GetSizeMemory();

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

			SM_Handle = CreateFileMappingA(INVALID_HANDLE_VALUE, &security_attr->getsecurityattrebut(), PAGE_READWRITE, 0, size_memory, namememory.c_str());
			if (SM_Handle == NULL)
			{
				sys_error = GetLastError();
				current_status.store(StatusAdapter::ERROR_INIT, std::memory_order_relaxed);
				res = ResultReqest::ERR;
				throw 5;
			}

			log->Debug("AdapterSharedMemory {} : CreateFileMapping DONE", this->config.NameMemory);

			buf_data = (char*)MapViewOfFile(SM_Handle, FILE_MAP_ALL_ACCESS, 0, 0, size_memory);
			if (buf_data == NULL)
			{
				current_status.store(StatusAdapter::ERROR_INIT, std::memory_order_relaxed);
				res = ResultReqest::ERR;
				throw 6;
			}

			log->Debug("AdapterSharedMemory {} : MapViewOfFileg DONE", this->config.NameMemory);

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
			head->size_data_int = config.size_data_int;
			head->size_data_float = config.size_data_float;
			head->size_data_double = config.size_data_double;
			head->size_data_char = config.size_data_char;
			head->size_str = config.size_str;
			head->current_size_data_int = 0;
			head->current_size_data_float = 0;
			head->current_size_data_double = 0;
			head->current_size_data_char = 0;

			ReleaseMutex(Mutex_SM);
			if (config.DataType == TypeData::Base)
			{
				data_base = std::make_shared<DDSData>();
				data_base.get()->data_int().value().resize(config.size_data_int);
				data_base.get()->data_int().quality().resize(config.size_data_int);
				data_base.get()->data_float().value().resize(config.size_data_float);
				data_base.get()->data_float().quality().resize(config.size_data_float);
				data_base.get()->data_double().value().resize(config.size_data_double);
				data_base.get()->data_double().quality().resize(config.size_data_double);
				data_base.get()->data_char().value().resize(config.size_data_char);
				data_base.get()->data_char().quality().resize(config.size_data_char);
				for (int i = 0; i < config.size_data_char; i++)
				{
					data_base.get()->data_char().value()[i].value().resize(config.size_str);
				}
			}
			else if (config.DataType == TypeData::Extended)
			{
				data_extended = std::make_shared<DDSDataEx>();
				data_extended.get()->data_int().resize(config.size_data_int);
				data_extended.get()->data_float().resize(config.size_data_float);
				data_extended.get()->data_double().resize(config.size_data_double);
				data_extended.get()->data_char().resize(config.size_data_char);
				for (int i = 0; i < config.size_data_char; i++)
				{
					data_extended.get()->data_char()[i].value().resize(config.size_str);
				}
			}

			current_status.store(StatusAdapter::OK, std::memory_order_relaxed);
			log->Debug("AdapterSharedMemory {} : Init DONE", this->config.NameMemory);
		}
		catch (int& e)
		{
			log->Critical("AdapterSharedMemory {}: Error Init : error: {}, syserror: {}", this->config.NameMemory, e, sys_error);
			if (Mutex_SM != NULL) ReleaseMutex(Mutex_SM);
			CloseHandle(Mutex_SM);
			UnmapViewOfFile(buf_data);
			CloseHandle(SM_Handle);
			res = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterSharedMemory {}: Error Init : error: {}, syserror: {}", this->config.NameMemory, 0, 0);
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
	ResultReqest AdapterSharedMemory::ReadData(std::shared_ptr<DDSData> buf)
	{	
		DWORD mutex_win32 = 0;
		DWORD err_win32 = 0;
		ResultReqest res = ResultReqest::OK;

		try
		{
			if (current_status.load(std::memory_order_relaxed) != StatusAdapter::OK)
			{
				log->Debug("AdapterSharedMemory {}: ReadData IGNOR", config.NameMemory);
				return ResultReqest::IGNOR;
			}

			if (config.DataType != TypeData::Base)
			{
				log->Debug("AdapterSharedMemory {}: ReadData IGNOR", config.NameMemory);
				return ResultReqest::IGNOR;
			}

			log->Debug("AdapterSharedMemory {}: ReadData START", config.NameMemory);

			mutex_win32 = WaitForSingleObject(Mutex_SM, 5000);
			if (mutex_win32 != WAIT_OBJECT_0)
			{
				err_win32 = GetLastError();
				throw 1;
			}

			
			{
				char* buf_in = buf_data + Offset(TypeData::Base, TypeValue::INT);
				std::vector<int>& value = data_base.get()->data_int().value();
				std::vector<char>& quality = data_base.get()->data_int().quality();
				for (int i = 0; i < config.size_data_int; i++)
				{
					value[i] = *((int*)buf_in);
					buf_in += sizeof(int);
					quality[i] = *(buf_in);
					buf_in += sizeof(char);
				}
			}

			{
				char* buf_in = buf_data  + Offset(TypeData::Base, TypeValue::FLOAT);
				std::vector<float>& value = data_base.get()->data_float().value();
				std::vector<char>& quality = data_base.get()->data_float().quality();
				for (int i = 0; i < config.size_data_float; i++)
				{
					value[i] = *((float*)buf_in);
					buf_in += sizeof(float);
					quality[i] = *(buf_in);
					buf_in += sizeof(char);
				}
			}

			{
				char* buf_in = buf_data + Offset(TypeData::Base, TypeValue::FLOAT);
				std::vector<double>& value = data_base.get()->data_double().value();
				std::vector<char>& quality = data_base.get()->data_double().quality();
				for (int i = 0; i < config.size_data_double; i++)
				{
					value[i] = *((double*)buf_in);
					buf_in += sizeof(double);
					quality[i] = *(buf_in);
					buf_in += sizeof(char);
				}
			}

			{
				char* buf_in = buf_data + Offset(TypeData::Base, TypeValue::CHAR);
				std::vector<DataChar>& value = data_base.get()->data_char().value();
				std::vector<char>& quality = data_base.get()->data_char().quality();
				for (int i = 0; i < config.size_data_char; i++)
				{
					auto data_vec = value[i].value();
					for (int j = 0; j < config.size_str; j++)
					{
						data_vec[j] = *buf_in;
						buf_in++;
					}
					quality[i] = *buf_in;
					buf_in++;
				}
			}

			HeaderSharedMemory* head = (HeaderSharedMemory*)buf_data;
			data_base.get()->time_source(head->TimeLastWrite);
			
			head->TimeLastRead = TimeConverter::GetTime_LLmcs();

			/// --- write time in header --- /// 
			head->count_read++;
			head->TimeLastRead = TimeConverter::GetTime_LLmcs();	

			buf = data_base;
		}
		catch (int& e)
		{
			
			log->Critical("AdapterSharedMemory {}: ERROR ReadData: error: {}, syserror: {}", config.NameMemory, e, err_win32);
			res = ResultReqest::ERR;

		}
		catch (...)
		{
			log->Critical("AdapterSharedMemory {}: ERROR ReadData: error: {}, syserror: {}", config.NameMemory, 0, 0);
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
	ResultReqest AdapterSharedMemory::WriteData(std::shared_ptr<DDSData> buf)
	{	
		DWORD mutex_win32 = 0;
		DWORD err_win32 = 0;
		ResultReqest res = ResultReqest::OK;

		try
		{
			if (current_status.load(std::memory_order_relaxed) != StatusAdapter::OK)
			{
				log->Debug("AdapterSharedMemory {}: ReadData IGNOR", config.NameMemory);
				return ResultReqest::IGNOR;
			}

			if (config.DataType != TypeData::Base)
			{
				log->Debug("AdapterSharedMemory {}: ReadData IGNOR", config.NameMemory);
				return ResultReqest::IGNOR;
			}

			log->Debug("AdapterSharedMemory {}: ReadData START", config.NameMemory);

			mutex_win32 = WaitForSingleObject(Mutex_SM, 5000);
			if (mutex_win32 != WAIT_OBJECT_0)
			{
				err_win32 = GetLastError();
				throw 1;
			}

			{
				char* buf_out = buf_data + Offset(TypeData::Base, TypeValue::INT);
				auto value = buf.get()->data_int().value();
				auto quality = buf.get()->data_int().quality();
				for (int i = 0; i < config.size_data_int < value.size() ? config.size_data_int : value.size(); i++)
				{
					*((int*)buf_out) = value[i];
					buf_out += sizeof(int);
					*(buf_out) = quality[i];
					buf_out++;
				}
			}


			{
				char* buf_out = buf_data + Offset(TypeData::Base, TypeValue::FLOAT);
				auto value = buf.get()->data_float().value();
				auto quality = buf.get()->data_float().quality();
				for (int i = 0; i < config.size_data_float < value.size() ? config.size_data_float : value.size(); i++)
				{
					*((float*)buf_out) = value[i];
					buf_out += sizeof(float);
					*(buf_out) = quality[i];
					buf_out++;
				}
			}

			{
				char* buf_out = buf_data + Offset(TypeData::Base, TypeValue::DOUBLE);
				auto value = buf.get()->data_double().value();
				auto quality = buf.get()->data_double().quality();
				for (int i = 0; i < config.size_data_double < value.size() ? config.size_data_double : value.size(); i++)
				{
					*((double*)buf_out) = value[i];
					buf_out += sizeof(double);
					*(buf_out) = quality[i];
					buf_out++;
				}
			}

			{
				char* buf_out = buf_data + Offset(TypeData::Base, TypeValue::CHAR);
				auto value = buf.get()->data_char().value();
				auto quality = buf.get()->data_char().quality();
				for (int i = 0; i < config.size_data_char < value.size() ? config.size_data_char : value.size(); i++)
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
			
			log->Critical("AdapterSharedMemory {}: ERROR ReadData: error: {}, syserror: {}", config.NameMemory, e, err_win32);
			res = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterSharedMemory {}: ERROR ReadData: error: {}, syserror: {}", config.NameMemory, 0, 0);
			res = ResultReqest::ERR;
		}		

		if (mutex_win32 == WAIT_OBJECT_0) ReleaseMutex(Mutex_SM);
		return res;
	}

	ResultReqest AdapterSharedMemory::ReadExData(std::shared_ptr<DDSDataEx> buf)
	{
		DWORD mutex_win32 = 0;
		DWORD err_win32 = 0;
		ResultReqest res = ResultReqest::OK;


		try
		{
			if (current_status.load(std::memory_order_relaxed) != StatusAdapter::OK)
			{
				log->Debug("AdapterSharedMemory {}: ReadExData IGNOR", config.NameMemory);
				return ResultReqest::IGNOR;
			}

			if (config.DataType != TypeData::Extended)
			{
				log->Debug("AdapterSharedMemory {}: ReadExData IGNOR", config.NameMemory);
				return ResultReqest::IGNOR;
			}

			log->Debug("AdapterSharedMemory {}: ReadExData START", config.NameMemory);

			mutex_win32 = WaitForSingleObject(Mutex_SM, 5000);
			if (mutex_win32 != WAIT_OBJECT_0)
			{
				err_win32 = GetLastError();
				throw 1;
			}

			HeaderSharedMemory* head = (HeaderSharedMemory*)buf_data;

			{
				char* buf_out = buf_data + Offset(TypeData::Extended, TypeValue::INT);
				auto value = data_extended.get()->data_int();
				value.resize(head->current_size_data_int);
				for (int i = 0; i < head->current_size_data_int; i++)
				{
					DataExInt& data = value[i];
					data.time_source(*((long long*)buf_out));
					buf_out += sizeof(long long);
					data.id_tag(*((uint32_t*)buf_out));
					buf_out += sizeof(uint32_t);
					data.value(*((int*)buf_out));
					buf_out += sizeof(int);
					data.quality(*((char*)buf_out));
					buf_out += sizeof(char);
				}
			}

			{
				char* buf_out = buf_data + Offset(TypeData::Extended, TypeValue::FLOAT);
				auto value = data_extended.get()->data_float();
				value.resize(head->current_size_data_float);
				for (int i = 0; i < head->current_size_data_float; i++)
				{
					DataExFloat& data = value[i];
					data.time_source(*((long long*)buf_out));
					buf_out += sizeof(long long);
					data.id_tag(*((uint32_t*)buf_out));
					buf_out += sizeof(uint32_t);
					data.value(*((float*)buf_out));
					buf_out += sizeof(float);
					data.quality(*((char*)buf_out));
					buf_out += sizeof(char);
				}
			}


			{
				char* buf_out = buf_data + Offset(TypeData::Extended, TypeValue::DOUBLE);
				auto value = data_extended.get()->data_double();
				value.resize(head->current_size_data_float);
				for (int i = 0; i < head->current_size_data_float; i++)
				{
					DataExDouble& data = value[i];
					data.time_source(*((long long*)buf_out));
					buf_out += sizeof(long long);
					data.id_tag(*((uint32_t*)buf_out));
					buf_out += sizeof(uint32_t);
					data.value(*((double*)buf_out));
					buf_out += sizeof(double);
					data.quality(*((char*)buf_out));
					buf_out += sizeof(char);
				}
			}

			{
				char* buf_out = buf_data + Offset(TypeData::Extended, TypeValue::CHAR);
				auto value = data_extended.get()->data_char();
				value.resize(head->current_size_data_char);
				for (int i = 0; i < head->current_size_data_char; i++)
				{
					DataExChar& data = value[i];
					data.time_source(*((long long*)buf_out));
					buf_out += sizeof(long long);
					data.id_tag(*((uint32_t*)buf_out));
					buf_out += sizeof(uint32_t);
					data.value().resize(config.size_str);
					for (auto c : data.value())
					{
						c = *buf_out;
						buf_out++;
					}
					data.quality(*((char*)buf_out));
					buf_out += sizeof(char);
				}
			}

			head->count_read++;
			head->TimeLastRead = TimeConverter::GetTime_LLmcs();

		}
		catch (int& e)
		{

			log->Critical("AdapterSharedMemory {}: ERROR ReadExData: error: {}, syserror: {}", config.NameMemory, e, err_win32);
			res = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterSharedMemory {}: ERROR ReadExData: error: {}, syserror: {}", config.NameMemory, 0, 0);
			res = ResultReqest::ERR;
		}

		if (mutex_win32 == WAIT_OBJECT_0) ReleaseMutex(Mutex_SM);
		return res;
	}

	ResultReqest AdapterSharedMemory::WriteExData(std::shared_ptr<DDSDataEx> buf)
	{
		DWORD mutex_win32 = 0;
		DWORD err_win32 = 0;
		ResultReqest res = ResultReqest::OK;

		try
		{
			if (current_status.load(std::memory_order_relaxed) != StatusAdapter::OK)
			{
				log->Debug("AdapterSharedMemory {}: WriteExData IGNOR", config.NameMemory);
				return ResultReqest::IGNOR;
			}

			if (config.DataType != TypeData::Extended)
			{
				log->Debug("AdapterSharedMemory {}: WriteExData IGNOR", config.NameMemory);
				return ResultReqest::IGNOR;
			}

			log->Debug("AdapterSharedMemory {}: WriteExData START", config.NameMemory);

			mutex_win32 = WaitForSingleObject(Mutex_SM, 5000);
			if (mutex_win32 != WAIT_OBJECT_0)
			{
				err_win32 = GetLastError();
				throw 1;
			}

			HeaderSharedMemory* head = (HeaderSharedMemory*)buf_data;

			{
				char* buf_out = buf_data + Offset(TypeData::Extended, TypeValue::INT);
				auto value = data_extended.get()->data_int();
				int size = config.size_data_int < value.size() ? config.size_data_int : value.size();
				DataExInt& data = value[0];
				for (int i = 0; i < size; i++)
				{
					data = value[i];
					*((long long*)buf_out) = data.time_source();
					buf_out += sizeof(long long);
					*((uint32_t*)buf_out) = data.id_tag();
					buf_out += sizeof(uint32_t);
					*((int*)buf_out) = data.value();
					buf_out += sizeof(int);
					*((char*)buf_out) = data.quality();
					buf_out += sizeof(char);
				}
			}

			{
				char* buf_out = buf_data + Offset(TypeData::Extended, TypeValue::FLOAT);
				auto value = data_extended.get()->data_float();
				int size = config.size_data_float < value.size() ? config.size_data_float : value.size();
				DataExFloat& data = value[0];
				for (int i = 0; i < size; i++)
				{
					data = value[i];
					*((long long*)buf_out) = data.time_source();
					buf_out += sizeof(long long);
					*((uint32_t*)buf_out) = data.id_tag();
					buf_out += sizeof(uint32_t);
					*((float*)buf_out) = data.value();
					buf_out += sizeof(float);
					*((char*)buf_out) = data.quality();
					buf_out += sizeof(char);
				}
			}

			{
				char* buf_out = buf_data + Offset(TypeData::Extended, TypeValue::DOUBLE);
				auto value = data_extended.get()->data_double();
				int size = config.size_data_double < value.size() ? config.size_data_double : value.size();
				DataExDouble& data = value[0];
				for (int i = 0; i < size; i++)
				{
					data = value[i];
					*((long long*)buf_out) = data.time_source();
					buf_out += sizeof(long long);
					*((uint32_t*)buf_out) = data.id_tag();
					buf_out += sizeof(uint32_t);
					*((double*)buf_out) = data.value();
					buf_out += sizeof(double);
					*((char*)buf_out) = data.quality();
					buf_out += sizeof(char);
				}
			}

			{
				char* buf_out = buf_data + Offset(TypeData::Extended, TypeValue::CHAR);
				auto value = data_extended.get()->data_char();
				int size = config.size_data_char < value.size() ? config.size_data_double : value.size();
				DataExChar& data = value[0];
				for (int i = 0; i < size; i++)
				{
					data = value[i];
					*((long long*)buf_out) = data.time_source();
					buf_out += sizeof(long long);
					*((uint32_t*)buf_out) = data.id_tag();
					buf_out += sizeof(uint32_t);
					std::vector<char>::iterator ivec_char = data.value().begin();
					size_t size_value = config.size_str < data.value().size() ? config.size_str : data.value().size();
					for (size_t i = 0; i < size_value; i++)
					{
						*buf_out = *ivec_char;
						ivec_char++;
						buf_out++;
					}
					for (size_t i = 0; i < config.size_str - size_value; i++)
					{
						*buf_out = 0;
						buf_out++;
					}

					*((char*)buf_out) = data.quality();
					buf_out += sizeof(char);
				}
			}

			head->count_write++;
			head->TimeLastWrite = TimeConverter::GetTime_LLmcs();

		}
		catch (int& e)
		{

			log->Critical("AdapterSharedMemory {}: ERROR WriteExData: error: {}, syserror: {}", config.NameMemory, e, err_win32);
			res = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterSharedMemory {}: ERROR WriteExData: error: {}, syserror: {}", config.NameMemory, 0, 0);
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

		if (config.DataType == TypeData::Base)
		{
			result += (sizeof(int) + sizeof(char)) * config.size_data_int; // size DataCollectionInt
			result += (sizeof(float) + sizeof(char)) * config.size_data_float; // size DataCollectionFloat
			result += (sizeof(double) + sizeof(char)) * config.size_data_double; // size DataCollectionDouble
			result += (sizeof(char)*config.size_str + sizeof(char)) * config.size_data_char; // size DataCollectionInt
		}
		else if (config.DataType == TypeData::Extended)
		{
			result += (sizeof(long long) + sizeof(uint32_t) + sizeof(int) + sizeof(char))*config.size_data_int; // size DataExInt
			result += (sizeof(long long) + sizeof(uint32_t) + sizeof(float) + sizeof(char)) * config.size_data_float; // size DataExFloat
			result += (sizeof(long long) + sizeof(uint32_t) + sizeof(double) + sizeof(char)) * config.size_data_double; // size DataExDouble
			result += (sizeof(long long) + sizeof(uint32_t) + sizeof(char)*config.size_str + sizeof(char)) * config.size_data_int; // size DataExChar
		}

		return result;
	}

	size_t AdapterSharedMemory::Offset(TypeData type_data, TypeValue type_value)
	{
		size_t result = 0;
		result += sizeof(HeaderSharedMemory);
		if (type_value == TypeValue::INT) return result;

		if (type_data == TypeData::Base) result += (sizeof(int) + sizeof(char)) * config.size_data_int;
		if (type_data == TypeData::Extended) result += (sizeof(long long) + sizeof(uint32_t) + sizeof(int) + sizeof(char)) * config.size_data_int;
		if (type_value == TypeValue::FLOAT) return result;

		if (type_data == TypeData::Base) result += (sizeof(float) + sizeof(char)) * config.size_data_float;
		if (type_data == TypeData::Extended) result += (sizeof(long long) + sizeof(uint32_t) + sizeof(float) + sizeof(char)) * config.size_data_float;
		if (type_value == TypeValue::DOUBLE) return result;

		if (type_data == TypeData::Base) (sizeof(double) + sizeof(char))* config.size_data_double;
		if (type_data == TypeData::Extended) result += (sizeof(long long) + sizeof(uint32_t) + sizeof(double) + sizeof(char)) * config.size_data_double;
		if (type_value == TypeValue::CHAR) return result;

		return result;
	}
}

