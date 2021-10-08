#include "SharedMemoryAdaptor.h"

namespace gate
{

	std::string CreateSMName(std::string source)
	{
			return source;
	}

	SharedMemoryAdaptor::SharedMemoryAdaptor()
	{
		log = LoggerSpace::Logger::getpointcontact();
	};

	SharedMemoryAdaptor::~SharedMemoryAdaptor()
	{
		UnmapViewOfFile(buf_data);
		CloseHandle(SM_Handle);
		CloseHandle(Mutex_SM);
	}

	ResultReqest SharedMemoryAdaptor::InitAdapter(void* conf)
	{
		current_status.store(StatusAdapter::INITIALIZATION, std::memory_order_relaxed);
		ResultReqest res = ResultReqest::OK;
		unsigned int result = 0;
		ConfigSharedMemoryAdapter* config_point = static_cast<ConfigSharedMemoryAdapter*>(conf);
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

	std::shared_ptr<BaseAnswer> SharedMemoryAdaptor::GetInfoAdapter(ParamInfoAdapter param)
	{
		std::shared_ptr<BaseAnswer> answer = nullptr;

		switch (param)
		{
		case ParamInfoAdapter::HeaderData:

			std::shared_ptr<HeaderDataAnswerSM> answer_point = std::make_shared<HeaderDataAnswerSM>();
			answer_point->param = param;
			answer_point->typeadapter = TypeAdapter::SharedMemory;
			answer_point->result = ResultReqest::OK;

			WaitForSingleObject(Mutex_SM, INFINITY);
			HeaderSharedMemory* head = (HeaderSharedMemory*)buf_data;
			answer_point->header.count_read = head->count_read;
			answer_point->header.count_write = head->count_write;
			answer_point->header.size_data = head->size_data;
			answer_point->header.TimeLastRead = head->TimeLastRead;
			answer_point->header.TimeLastWrite = head->TimeLastWrite;
			answer_point->header.typedata = head->typedata;
			ReleaseMutex(Mutex_SM);
			answer = std::reinterpret_pointer_cast<BaseAnswer>(answer_point);
			break;

		default:
			break;
		}
				
		return answer;
	}

	TypeAdapter SharedMemoryAdaptor::GetTypeAdapter()
	{
		return TypeAdapter::SharedMemory;
	}

	StatusAdapter SharedMemoryAdaptor::GetStatusAdapter()
	{
		return current_status.load(std::memory_order::memory_order_relaxed);
	}
	
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

}

