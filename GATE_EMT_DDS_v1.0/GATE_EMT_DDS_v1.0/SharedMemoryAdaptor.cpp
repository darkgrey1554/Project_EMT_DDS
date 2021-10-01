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

	ResultReqest SharedMemoryAdaptor::InitAdaptor(void* conf)
	{
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
			return res;
		}
		
		/// --- defining units of data --- ///
		int size_type = 0;
		if (config.DataType == TypeData::ANALOG) { size_type = sizeof(float); }
		else if (config.DataType == TypeData::DISCRETE) { size_type = sizeof(int); }
		else if (config.DataType == TypeData::BINAR) { size_type = sizeof(char); }
		else { size_type = 0; };

		/// --- initialization handle of shared memory --- ///
		if (SM_Handle != NULL)
		{
			log->WriteLogWARNING("Error init shared memory", 1, 0);
			res = ResultReqest::ERR;
			return res;
		}

		namememory = "Global\\" + CreateSMName(config.NameMemory);
		namemutex = "Global\\Mutex_" + CreateSMName(config.NameMemory);

		Mutex_SM = CreateMutexA(&security_attr.getsecurityattrebut(), TRUE, namemutex.c_str());
		if (Mutex_SM == NULL)
		{
			log->WriteLogERR("Error init shared memory", 2, GetLastError());
			res = ResultReqest::ERR;
			return res;
		}

		SM_Handle = CreateFileMappingA(&security_attr.getsecurityattrebut(), NULL, PAGE_READWRITE, 0, config.size*size_type + sizeof(HeaderSharedMemory), namememory.c_str());
		if (SM_Handle == NULL)
		{
			log->WriteLogERR("Error init shared memory", 3, GetLastError());
			ReleaseMutex(Mutex_SM);
			CloseHandle(Mutex_SM);
			res = ResultReqest::ERR;
			return res;
		}

		buf_data = (char*)MapViewOfFile(SM_Handle, FILE_MAP_ALL_ACCESS, 0, 0, config.size * size_type + sizeof(HeaderSharedMemory) * sizeof(float) + sizeof(HeaderSharedMemory));
		if (buf_data == NULL)
		{
			log->WriteLogERR("Error init shared memory", 4, GetLastError());
			ReleaseMutex(Mutex_SM);
			CloseHandle(Mutex_SM);
			CloseHandle(SM_Handle);
			res = ResultReqest::ERR;
			return res;
		}
		
		return res;
	}

	std::unique_ptr<void> SharedMemoryAdaptor::GetInfoAdaptor(ParamInfoAdapter param)
	{
		std::string str;
		if (param == ParamInfoAdapter::Type)
		{
			std::unique_ptr<TypeAdapter> point= std::make_unique<TypeAdapter>(TypeAdapter::SharedMemory);
			return std::move(point);
		}


		return nullptr;
	}
	
	ResultReqest SharedMemoryAdaptor::ReadData(void* buf, unsigned int size)
	{
		return ResultReqest::ERR;
	}

	ResultReqest SharedMemoryAdaptor::WriteData(void* buf, unsigned int size)
	{
		return ResultReqest::ERR;
	}
}

/*
ResultReqest SharedMemoryDDS::ReadMemory(TypeData type , void* buf, unsigned int size)
{
	unsigned int iter;
	HeaderSharedMemory* head;

	if (type == TypeData::ANALOG)
	{
		
		if (SM_Analog == NULL || buf_analog == NULL) return ResultReqest::ERR;

		iter = size > size_analog_memory ? size_analog_memory : size;
		
		WaitForSingleObject(Mut_Analog, INFINITE);

		for (int i = 0; i < iter * sizeof(float); i++)
		{
			*((char*)buf + i) = *((char*)buf_analog + sizeof(HeaderSharedMemory)+i);
		}
		head = (HeaderSharedMemory*)buf_analog;
		head->count_read += 1;

		{
			auto now = std::chrono::system_clock::now();
			auto time = std::chrono::system_clock::to_time_t(now);
			auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) -
				std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch());
			auto t = localtime(&time);

			head->TimeLastRead.h = t->tm_hour;
			head->TimeLastRead.m = t->tm_min;
			head->TimeLastRead.s = t->tm_sec;
			head->TimeLastRead.ms = ms.count();	
		}

		ReleaseMutex(Mut_Analog);

	}

	if(type == TypeData::DISCRETE)
	{

		if (SM_Discrete == NULL || buf_discrete == NULL) return ResultReqest::ERR;

		iter = size > size_discrete_memory ? size_discrete_memory : size;

		WaitForSingleObject(Mut_Discrete, INFINITE);

		for (int i = 0; i < iter * sizeof(int); i++)
		{
			*((char*)buf + i) = *((char*)buf_discrete + sizeof(HeaderSharedMemory) + i);
		}
		head = (HeaderSharedMemory*)buf_discrete;
		head->count_read += 1;

		{
			auto now = std::chrono::system_clock::now();
			auto time = std::chrono::system_clock::to_time_t(now);
			auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) -
				std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch());
			auto t = localtime(&time);

			head->TimeLastRead.h = t->tm_hour;
			head->TimeLastRead.m = t->tm_min;
			head->TimeLastRead.s = t->tm_sec;
			head->TimeLastRead.ms = ms.count();
		}

		ReleaseMutex(Mut_Discrete);

	}

	if (type == TypeData::BINAR)
	{

		if (SM_Binar == NULL || buf_binar == NULL) return ResultReqest::ERR;

		iter = size > size_binar_memory ? size_binar_memory : size;

		WaitForSingleObject(Mut_Binar, INFINITE);

		for (int i = 0; i < iter * sizeof(int); i++)
		{
			*((char*)buf + i) = *((char*)buf_binar + sizeof(HeaderSharedMemory) + i);
		}
		head = (HeaderSharedMemory*)buf_binar;
		head->count_read += 1;

		{
			auto now = std::chrono::system_clock::now();
			auto time = std::chrono::system_clock::to_time_t(now);
			auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) -
				std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch());
			auto t = localtime(&time);

			head->TimeLastRead.h = t->tm_hour;
			head->TimeLastRead.m = t->tm_min;
			head->TimeLastRead.s = t->tm_sec;
			head->TimeLastRead.ms = ms.count();
		}

		ReleaseMutex(Mut_Binar);

	}

	return ResultReqest::OK;
}

ResultReqest  SharedMemoryDDS::WriteMemory(TypeData type, void* buf, int size)
{
	unsigned int iter;
	HeaderSharedMemory* head;

	if (type == TypeData::ANALOG)
	{

		if (SM_Analog == NULL || buf_analog == NULL) return ResultReqest::ERR;

		iter = size > size_analog_memory ? size_analog_memory : size;

		WaitForSingleObject(Mut_Analog, INFINITE);

		for (int i = 0; i < iter * sizeof(float); i++)
		{
			*((char*)buf_analog + sizeof(HeaderSharedMemory) + i )= *((char*)buf + i);
		}
		head = (HeaderSharedMemory*)buf_analog;
		head->count_write += 1;

		{
			auto now = std::chrono::system_clock::now();
			auto time = std::chrono::system_clock::to_time_t(now);
			auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) -
				std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch());
			auto t = localtime(&time);

			head->TimeLastWrite.h = t->tm_hour;
			head->TimeLastWrite.m = t->tm_min;
			head->TimeLastWrite.s = t->tm_sec;
			head->TimeLastWrite.ms = ms.count();
		}

		ReleaseMutex(Mut_Analog);

	}

	if (type == TypeData::DISCRETE)
	{

		if (SM_Discrete == NULL || buf_discrete == NULL) return ResultReqest::ERR;

		iter = size > size_discrete_memory ? size_discrete_memory : size;

		WaitForSingleObject(Mut_Discrete, INFINITE);

		for (int i = 0; i < iter * sizeof(int); i++)
		{
			*((char*)buf_discrete + sizeof(HeaderSharedMemory) + i) = *((char*)buf + i);
		}
		head = (HeaderSharedMemory*)buf_discrete;
		head->count_write += 1;

		{
			auto now = std::chrono::system_clock::now();
			auto time = std::chrono::system_clock::to_time_t(now);
			auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) -
				std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch());
			auto t = localtime(&time);

			head->TimeLastWrite.h = t->tm_hour;
			head->TimeLastWrite.m = t->tm_min;
			head->TimeLastWrite.s = t->tm_sec;
			head->TimeLastWrite.ms = ms.count();
		}

		ReleaseMutex(Mut_Discrete);

	}

	if (type == TypeData::BINAR)
	{

		if (SM_Binar == NULL || buf_binar == NULL) return ResultReqest::ERR;

		iter = size > size_binar_memory ? size_binar_memory : size;

		WaitForSingleObject(Mut_Binar, INFINITE);

		for (int i = 0; i < iter * sizeof(int); i++)
		{
			*((char*)buf_binar + sizeof(HeaderSharedMemory) + i) = *((char*)buf + i);
		}
		head = (HeaderSharedMemory*)buf_binar;
		head->count_write += 1;

		{
			auto now = std::chrono::system_clock::now();
			auto time = std::chrono::system_clock::to_time_t(now);
			auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) -
				std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch());
			auto t = localtime(&time);

			head->TimeLastWrite.h = t->tm_hour;
			head->TimeLastWrite.m = t->tm_min;
			head->TimeLastWrite.s = t->tm_sec;
			head->TimeLastWrite.ms = ms.count();
		}

		ReleaseMutex(Mut_Binar);

	}

	return ResultReqest::OK;
}

void  SharedMemoryDDS::DeleteMemory(TypeData type)
{

	if (type == TypeData::ANALOG)
	{
		UnmapViewOfFile(buf_analog);
		CloseHandle(Mut_Analog);
		CloseHandle(SM_Analog);
		return;
	}

	if (type == TypeData::DISCRETE)
	{
		UnmapViewOfFile(buf_discrete);
		CloseHandle(Mut_Discrete);
		CloseHandle(SM_Discrete);
		return;
	}

	if (type == TypeData::BINAR)
	{
		UnmapViewOfFile(buf_binar);
		CloseHandle(Mut_Binar);
		CloseHandle(SM_Binar);
		return;
	}
};

HeaderSharedMemory SharedMemoryDDS::ReadHead(TypeData type)
{
	HeaderSharedMemory head;

	if (type == TypeData::ANALOG)
	{
		if (buf_analog == NULL || SM_Analog == NULL) return head;

		WaitForSingleObject(Mut_Analog, INFINITE);

		head = *(HeaderSharedMemory*)buf_analog;

		ReleaseMutex(Mut_Analog);
	}

	if (type == TypeData::DISCRETE)
	{
		if (buf_discrete == NULL || SM_Discrete == NULL) return head;

		WaitForSingleObject(Mut_Discrete, INFINITE);

		head = *(HeaderSharedMemory*)buf_discrete;

		ReleaseMutex(Mut_Discrete);
	}

	if (type == TypeData::BINAR)
	{
		if (buf_binar == NULL || SM_Binar == NULL) return head;

		WaitForSingleObject(Mut_Binar, INFINITE);

		head = *(HeaderSharedMemory*)buf_binar;

		ReleaseMutex(Mut_Binar);
	}

	return head;
}
*/
