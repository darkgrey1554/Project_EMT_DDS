#include "AdapterSharedMemory.hpp"

namespace scada_ate::gate::adapter
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
	ResultReqest AdapterSharedMemory::ReadData(std::shared_ptr<DDSData>& buf)
	{	
		DWORD mutex_win32 = 0;
		DWORD err_win32 = 0;
		ResultReqest res = ResultReqest::OK;

		if (config.DataType != TypeData::Base)
		{
			ResultReqest res = ResultReqest::IGNOR;
			return res;
		}

		try
		{
			if (current_status.load(std::memory_order_relaxed) != StatusAdapter::OK)
			{
				log->Debug("AdapterSharedMemory {}: ReadData IGNOR", config.NameMemory);
				return ResultReqest::IGNOR;
			}

			log->Debug("AdapterSharedMemory {}: ReadData START", config.NameMemory);

			size_t read_size = this->size_type * config.size;
			std::shared_ptr<char> ptr(new char[read_size], std::default_delete<char[]>());
			char* buf_in = ptr.get();
			char* buf_out = buf_data + sizeof(HeaderSharedMemory);

			mutex_win32 = WaitForSingleObject(Mutex_SM, 5000);
			if (mutex_win32 != WAIT_OBJECT_0)
			{
				err_win32 = GetLastError();
				throw 1;
			}

			for (int i = 0; i < read_size; i++)
			{
				*(buf_in + i) = *(buf_out + i);
			}

			HeaderSharedMemory* head = (HeaderSharedMemory*)buf_data;

			/// --- write time in header --- /// 
			head->count_read++;
			head->TimeLastRead = TimeConverter::GetTime_LLmcs();	

			info.size_data = this->config.size;
			info.time = head->TimeLastWrite;
			buf = ptr;
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
	ResultReqest AdapterSharedMemory::WriteData(std::shared_ptr<void> buf, SampleInfo& info)
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

			log->Debug("AdapterSharedMemory {}: ReadData START", config.NameMemory);

			mutex_win32 = WaitForSingleObject(Mutex_SM, 5000);
			if (mutex_win32 != WAIT_OBJECT_0)
			{
				err_win32 = GetLastError();
				throw 1;
			}

			int write_size = size_type*(info.size_data > config.size ? config.size : info.size_data);
			std::shared_ptr<char> ptr(new char[write_size], std::default_delete<char[]>());

			char* buf_out = buf_data + sizeof(HeaderSharedMemory);
			char* buf_in = ptr.get();
			for (int i = 0; i < write_size; i++)
			{
				*(buf_out + i) = *(buf_in + i);
			}

			HeaderSharedMemory* head = (HeaderSharedMemory*)buf_data;
			head->count_write++;
			head->TimeLastWrite = TimeConverter::GetTime_LLmcs();

			info.size_data = config.size;
			info.time = head->TimeLastWrite;
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

	ResultReqest AdapterSharedMemory::ReadExData(std::shared_ptr<void> buf, SampleInfo& info)
	{
		return ResultReqest::IGNOR;
	}

	ResultReqest AdapterSharedMemory::WriteExData(std::shared_ptr<void> buf, SampleInfo& info)
	{
		return ResultReqest::IGNOR;
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

		try
		{
			point->header.count_read = 0;
			point->header.count_write = 0;
			point->header.size_data = 0;
			point->header.TimeLastRead = 0;
			point->header.TimeLastWrite = 0;
			point->header.typedata = TypeData::ZERO;
			point->param = ParamInfoAdapter::HeaderData;
			point->type_adapter = TypeAdapter::SharedMemory;
			point->result = ResultReqest::ERR;

			mutex_win32 = WaitForSingleObject(Mutex_SM, INFINITE);
			if (mutex_win32 != WAIT_OBJECT_0)
			{
				err_win32 = GetLastError();
				throw 1;
			}

			point->header.count_read = head->count_read;
			point->header.count_write = head->count_write;
			point->header.size_data = head->size_data;
			point->header.TimeLastRead = head->TimeLastRead;
			point->header.TimeLastWrite = head->TimeLastWrite;
			point->header.typedata = head->typedata;
			point->param = ParamInfoAdapter::HeaderData;
			point->type_adapter = TypeAdapter::SharedMemory;
			point->result = ResultReqest::OK;
		}
		catch (int& e)
		{
			log->Critical("AdapterSharedMemory {}: ERROR AnswerRequestHeaderData: error: {}, syserror: {}", config.NameMemory, e, err_win32);
		}
		catch (...)
		{
			log->Critical("AdapterSharedMemory {}: ERROR AnswerRequestHeaderData: error: {}, syserror: {}", config.NameMemory, 0, 0);
		}

		if (mutex_win32 == WAIT_OBJECT_0) ReleaseMutex(Mutex_SM);
		return point;		
	}

	size_t AdapterSharedMemory::GetSizeMemory()
	{
		size_t result = 0;
		result += sizeof(HeaderSharedMemory);

		if (config.DataType == TypeData::Base)
		{
			result += (sizeof(int) + sizeof(char)) * config.size_data_int; // size DataCollectionInt
			result += (sizeof(float) + sizeof(char)) * config.size_data_int; // size DataCollectionFloat
			result += (sizeof(double) + sizeof(char)) * config.size_data_int; // size DataCollectionDouble
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

}

