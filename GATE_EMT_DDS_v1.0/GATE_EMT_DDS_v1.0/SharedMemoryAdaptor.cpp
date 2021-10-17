#include "SharedMemoryAdaptor.h"

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
	std::string SharedMemoryAdaptor::CreateSMName(std::string source)
	{
			return source;
	}

	/////////////////////////////////////////////////
	/// --- конструктор адаптера SharedMemory --- ///
	////////////////////////////////////////////////

	SharedMemoryAdaptor::SharedMemoryAdaptor()
	{
		log = LoggerSpace::Logger::getpointcontact();
	};

	/////////////////////////////////////////////////
	/// --- деструктор адаптера SharedMemory --- ///
	////////////////////////////////////////////////

	SharedMemoryAdaptor::~SharedMemoryAdaptor()
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
	
	ResultReqest SharedMemoryAdaptor::InitAdapter(std::shared_ptr<ConfigAdapter> conf)
	{
		current_status.store(StatusAdapter::INITIALIZATION, std::memory_order_relaxed);
		ResultReqest res = ResultReqest::OK;
		unsigned int result = 0;
		std::shared_ptr<ConfigSharedMemoryAdapter> config_point = std::reinterpret_pointer_cast<ConfigSharedMemoryAdapter>(conf);
		if (config_point == nullptr) { res = ResultReqest::ERR; return res; }
		std::string namememory;
		std::string namemutex;

		/// --- coping of configuration --- ///
		this->config.DataType = config_point->DataType;
		this->config.NameMemory = config_point->NameMemory;
		this->config.size = config_point->size;

		/// --- initialization of security attributes --- ///
		result = security_attr.InitSecurityAttrubuts();
		if (result != 0)
		{
			log->WriteLogERR("Error init sharedmemory: error of security attributs", result, security_attr.getlasterror());
			res = ResultReqest::ERR;
			current_status.store(StatusAdapter::ERROR_INIT, std::memory_order_relaxed);
			return res;
		}
		
		/// --- defining units of data --- ///
		if (config.DataType == TypeData::ANALOG) { size_type = sizeof(float); }
		else if (config.DataType == TypeData::DISCRETE) { size_type = sizeof(int); }
		else if (config.DataType == TypeData::BINAR) { size_type = sizeof(char); }
		else { size_type = 0; };

		/// --- initialization handle of shared memory --- ///
		if (SM_Handle != NULL)
		{
			log->WriteLogWARNING("Error init shared memory", 1, 0);
			res = ResultReqest::ERR;
			current_status.store(StatusAdapter::ERROR_INIT, std::memory_order_relaxed);
			return res;
		}

		namememory = "Global\\" + CreateSMName(config.NameMemory);
		namemutex = "Global\\Mutex_" + CreateSMName(config.NameMemory);

		Mutex_SM = CreateMutexA(&security_attr.getsecurityattrebut(), TRUE, namemutex.c_str());
		if (Mutex_SM == NULL)
		{
			log->WriteLogERR("Error init shared memory", 2, GetLastError());
			res = ResultReqest::ERR;
			current_status.store(StatusAdapter::ERROR_INIT, std::memory_order_relaxed);
			return res;
		}

		SM_Handle = CreateFileMappingA(INVALID_HANDLE_VALUE,&security_attr.getsecurityattrebut(), PAGE_READWRITE, 0, config.size*size_type + sizeof(HeaderSharedMemory), namememory.c_str());
		if (SM_Handle == NULL)
		{
			log->WriteLogERR("Error init shared memory", 3, GetLastError());
			ReleaseMutex(Mutex_SM);
			CloseHandle(Mutex_SM);
			current_status.store(StatusAdapter::ERROR_INIT, std::memory_order_relaxed);
			res = ResultReqest::ERR;
			return res;
		}

		buf_data = (char*)MapViewOfFile(SM_Handle, FILE_MAP_ALL_ACCESS, 0, 0, config.size * size_type + sizeof(HeaderSharedMemory));
		if (buf_data == NULL)
		{
			log->WriteLogERR("Error init shared memory", 4, GetLastError());
			ReleaseMutex(Mutex_SM);
			CloseHandle(Mutex_SM);
			CloseHandle(SM_Handle);
			current_status.store(StatusAdapter::ERROR_INIT, std::memory_order_relaxed);
			res = ResultReqest::ERR;
			return res;
		}
		
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

	std::shared_ptr<BaseAnswer> SharedMemoryAdaptor::GetInfoAdapter(ParamInfoAdapter param)
	{
		std::shared_ptr<BaseAnswer> answer = nullptr;
		std::shared_ptr<HeaderDataAnswerSM> answerHeaderData = nullptr;

		switch (param)
		{

		/// --- ответ на запрос Header DATA --- ///
		case ParamInfoAdapter::HeaderData:

			answerHeaderData = AnswerRequestHeaderData();
			answer = std::reinterpret_pointer_cast<BaseAnswer>(answerHeaderData);
			break;

		/// --- ответ не предусмотрен данным типом адапрета (ResultReqest::IGNOR) --- ///
		default:

			answer = std::make_shared<BaseAnswer>();
			answer->param = param;
			answer->result = ResultReqest::IGNOR;
			answer->typeadapter = TypeAdapter::SharedMemory;

			break;
		}
				
		return answer;
	}

	/////////////////////////////////////////////////
	/// --- запрос типа адаптера SharedMemory --- ///
	/////////////////////////////////////////////////
	/// <result> 
	/// - возвращает тип адаптера (TypeAdapter::SharedMemory;)

	TypeAdapter SharedMemoryAdaptor::GetTypeAdapter()
	{
		return TypeAdapter::SharedMemory;
	}
	
	/////////////////////////////////////////////////////////////
	/// --- запрос текущего статуса адаптера SharedMemory --- ///
	////////////////////////////////////////////////////////////
	/// <result> 
	/// - возвращает статус адаптера (StatusAdapter)
	StatusAdapter SharedMemoryAdaptor::GetStatusAdapter()
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
	ResultReqest SharedMemoryAdaptor::ReadData(void* buf, unsigned int size)
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

		time_p = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		time_now = std::localtime(&time_p);		
		msec = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
			- std::chrono::duration_cast<std::chrono::milliseconds>
			(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()));

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
	ResultReqest SharedMemoryAdaptor::WriteData(void* buf, unsigned int size)
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

		time_p = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		time_now = std::localtime(&time_p);		
		msec = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
			- std::chrono::duration_cast<std::chrono::milliseconds>
			(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()));

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

	std::shared_ptr<HeaderDataAnswerSM> SharedMemoryAdaptor::AnswerRequestHeaderData()
	{
		std::shared_ptr<HeaderDataAnswerSM> point = std::make_shared<HeaderDataAnswerSM>();
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
			point->typeadapter = TypeAdapter::SharedMemory;
			point->result = ResultReqest::ERR;
			return point;
		}

		WaitForSingleObject(Mutex_SM, INFINITY);		
		point->header.count_read = head->count_read;
		point->header.count_write = head->count_write;
		point->header.size_data = head->size_data;
		point->header.TimeLastRead = head->TimeLastRead;
		point->header.TimeLastWrite = head->TimeLastWrite;
		point->header.typedata = head->typedata;
		ReleaseMutex(Mutex_SM);

		point->param = ParamInfoAdapter::HeaderData;
		point->typeadapter = TypeAdapter::SharedMemory;
		point->result = ResultReqest::OK;

		return point;
	}

}

