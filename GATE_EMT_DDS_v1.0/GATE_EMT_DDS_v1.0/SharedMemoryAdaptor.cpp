#include "SharedMemoryAdaptor.h"

namespace gate
{

	std::string CreatePointName(std::string source)
	{
			return source;
	}

	SharedMemoryAdaptor::SharedMemoryAdaptor()
	{
		log = LoggerSpace::Logger::getpointcontact();
	};

	SharedMemoryAdaptor::~SharedMemoryAdaptor()
	{

	}

	ResultReqest SharedMemoryAdaptor::InitAdaptor(void* conf)
	{
		ResultReqest res = ResultReqest::OK;
		ConfigSharedMemoryAdapter* config_point = static_cast<ConfigSharedMemoryAdapter*>(conf);
		std::string namememory;
		std::string mutex_name;
		
		this->config.DataType = config_point->DataType;
		this->config.NameMemory = config_point->NameMemory;
		this->config.size = config_point->size;

		/// --- русские коменты ? --- /// 

		int size_type = 0;
		if (config.DataType == TypeData::ANALOG) { size_type = sizeof(float); }
		else if (config.DataType == TypeData::DISCRETE) { size_type = sizeof(int); }
		else if (config.DataType == TypeData::BINAR) { size_type = sizeof(char); }
		else { size_type = 0; };

		if (SM_Handle != NULL)
		{
			log->WriteLogWARNING("ERROR INIT SHARED MEMORY", 1, 0);
			res = ResultReqest::ERR;
			return res;
		}

		namememory = "Global\\" + CreatePointName(config.NameMemory);
		mutex_name = "Global\\Mutex_" + CreatePointName(config.NameMemory);



	}
	

}




/*std::string CreateNameMemoryDDS(TypeData type, TypeDirection val, unsigned int domen)
{
	std::string str;
	str.clear();
	str += "SM_";
	if (type == TypeData::ANALOG) str += "ANALOG_";
	if (type == TypeData::DISCRETE) str += "DISCRETE_";
	if (type == TypeData::BINAR) str += "BINAR_";
	if (val == TypeDirection::EMTtoDDS) str += "EMTtoDDS_";
	if (val == TypeDirection::EMTfromDDS) str += "EMTfromDDS_";
	str += "DOMEN_";
	str += std::to_string(domen);

	return str;
}*/




ResultReqest SharedMemoryDDS::CreateMemoryAnalog(TypeDirection val, unsigned int size, std::string name)
{
	if (SM_Analog != NULL)
	{
		log->WriteLogERR("ERROR INIT SHARED MEMORY", 1, 0);
		return ResultReqest::ERR;
	}
	
	std::string name_mutex = "Mutex_" + name;
	HeaderSharedMemory* head;


	Mut_Analog = CreateMutexA(NULL, TRUE, name_mutex.c_str());
	if (Mut_Analog == NULL)
	{
		log->WriteLogERR("ERROR INIT SHARED MEMORY", 2, GetLastError());
		return ResultReqest::ERR;
	}

	SM_Analog = CreateFileMappingA(NULL, NULL, PAGE_READWRITE, 0, size*sizeof(float)+sizeof(HeaderSharedMemory), name.c_str());
	if (SM_Analog == NULL)
	{
		log->WriteLogERR("ERROR INIT SHARED MEMORY", 3, GetLastError());
		ReleaseMutex(Mut_Analog);
		CloseHandle(Mut_Analog);
		return ResultReqest::ERR;
	}

	buf_analog = (char*)MapViewOfFile(SM_Analog, FILE_MAP_ALL_ACCESS, 0, 0, size * sizeof(float) + sizeof(HeaderSharedMemory));
	if (buf_analog == NULL)
	{
		log->WriteLogERR("ERROR INIT SHARED MEMORY", 4, GetLastError());
		ReleaseMutex(Mut_Analog);
		CloseHandle(Mut_Analog);
		CloseHandle(SM_Analog);
		return ResultReqest::ERR;
	}

	for (unsigned int i = 0; i < size * sizeof(float) + sizeof(HeaderSharedMemory); i++)
	{
		*(((char*)buf_analog) + i) = 0;
	}

	head = (HeaderSharedMemory*)buf_analog;
	head->size_data = size;
	head->typedata = TypeData::ANALOG;
	head->typedirection = val;
	
	ReleaseMutex(Mut_Analog);
	return  ResultReqest::OK;
}

ResultReqest SharedMemoryDDS::CreateMemoryDiscrete(TypeDirection val, unsigned int size, std::string name)
{
	if (SM_Discrete != NULL)
	{
		log->WriteLogERR("ERROR INIT SHARED MEMORY", 1, 0);
		return ResultReqest::ERR;
	}

	std::string name_mutex = "Mutex_" + name;
	HeaderSharedMemory* head;


	Mut_Discrete = CreateMutexA(NULL, TRUE, name_mutex.c_str());
	if (Mut_Discrete == NULL)
	{
		log->WriteLogERR("ERROR INIT SHARED MEMORY", 2, GetLastError());
		return ResultReqest::ERR;
	}

	SM_Discrete = CreateFileMappingA(NULL, NULL, PAGE_READWRITE, 0, size * sizeof(int) + sizeof(HeaderSharedMemory), name.c_str());
	if (SM_Discrete == NULL)
	{
		log->WriteLogERR("ERROR INIT SHARED MEMORY", 3, GetLastError());
		ReleaseMutex(Mut_Discrete);
		CloseHandle(Mut_Discrete);
		return ResultReqest::ERR;
	}

	buf_discrete = (char*)MapViewOfFile(SM_Analog, FILE_MAP_ALL_ACCESS, 0, 0, size * sizeof(int) + sizeof(HeaderSharedMemory));
	if (buf_discrete == NULL)
	{
		log->WriteLogERR("ERROR INIT SHARED MEMORY", 4, GetLastError());
		ReleaseMutex(Mut_Discrete);
		CloseHandle(Mut_Discrete);
		CloseHandle(SM_Discrete);
		return ResultReqest::ERR;
	}

	for (unsigned int i = 0; i < size * sizeof(int) + sizeof(HeaderSharedMemory); i++)
	{
		*(((char*)buf_discrete) + i) = 0;
	}

	head = (HeaderSharedMemory*)buf_discrete;
	head->size_data = size;
	head->typedata = TypeData::DISCRETE;
	head->typedirection = val;

	ReleaseMutex(Mut_Discrete);
	return  ResultReqest::OK;
}

ResultReqest SharedMemoryDDS::CreateMemoryBinar(TypeDirection val, unsigned int size, std::string name)
{
	if (SM_Binar != NULL)
	{
		log->WriteLogERR("ERROR INIT SHARED MEMORY", 1, 0);
		return ResultReqest::ERR;
	}

	std::string name_mutex = "Mutex_" + name;
	HeaderSharedMemory* head;


	Mut_Binar = CreateMutexA(NULL, TRUE, name_mutex.c_str());
	if (Mut_Binar == NULL)
	{
		log->WriteLogERR("ERROR INIT SHARED MEMORY", 2, GetLastError());
		return ResultReqest::ERR;
	}

	SM_Binar = CreateFileMappingA(NULL, NULL, PAGE_READWRITE, 0, size * sizeof(char) + sizeof(HeaderSharedMemory), name.c_str());
	if (SM_Binar == NULL)
	{
		log->WriteLogERR("ERROR INIT SHARED MEMORY", 3, GetLastError());
		ReleaseMutex(Mut_Binar);
		CloseHandle(Mut_Binar);
		return ResultReqest::ERR;
	}

	buf_binar = (char*)MapViewOfFile(SM_Analog, FILE_MAP_ALL_ACCESS, 0, 0, size * sizeof(char) + sizeof(HeaderSharedMemory));
	if (buf_binar == NULL)
	{
		log->WriteLogERR("ERROR INIT SHARED MEMORY", 4, GetLastError());
		ReleaseMutex(Mut_Binar);
		CloseHandle(Mut_Binar);
		CloseHandle(SM_Binar);
		return ResultReqest::ERR;
	}

	for (unsigned int i = 0; i < size * sizeof(float) + sizeof(HeaderSharedMemory); i++)
	{
		*(((char*)buf_binar) + i) = 0;
	}

	head = (HeaderSharedMemory*)buf_binar;
	head->size_data = size;
	head->typedata = TypeData::BINAR;
	head->typedirection = val;

	ReleaseMutex(Mut_Binar);
	return  ResultReqest::OK;
}

ResultReqest SharedMemoryDDS::CreateMemory(TypeData type, TypeDirection val, unsigned int size, std::string name)
{
	ResultReqest result = ResultReqest::ERR;
	if (type == TypeData::ANALOG) result = CreateMemoryAnalog(val, size, name);
	if (type == TypeData::DISCRETE) result = CreateMemoryDiscrete(val, size, name);
	if (type == TypeData::BINAR) result = CreateMemoryBinar(val, size, name);

	return result;
}

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

