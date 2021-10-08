#pragma once

#include "structs.h"
#include <windows.h>
#include <logger.h>
#include <chrono>
#include <time.h>
#include <stdio.h>
#include <atomic>
#include "Adapters.h"
#include "security_handle.h"

/// <summary>
///  класс реализует взаимодействие между ЕМТ и шлюзом DDS по Shared Memory
/// </summary>

namespace gate
{
	std::string CreateSMName(std::string source);

	class SharedMemoryAdaptor : public Adapter
	{

		HANDLE SM_Handle = NULL;
		char* buf_data = NULL;
		unsigned int size_memory = 0;
		unsigned int size_type = 0;
		HANDLE Mutex_SM = NULL;
		SecurityHandle security_attr;

		ConfigSharedMemoryAdapter config;
		std::atomic<StatusAdapter> current_status = StatusAdapter::Null;
		LoggerSpace::Logger* log;


		//ResultReqest CreateMemoryAnalog(TypeDirection val, unsigned int size, std::string name);
		//ResultReqest CreateMemoryDiscrete(TypeDirection val, unsigned int size, std::string name);
		//ResultReqest CreateMemoryBinar(TypeDirection val, unsigned int size, std::string name);

	public:

		ResultReqest InitAdapter(void* config);
		ResultReqest ReadData(void* buf, unsigned int size);
		ResultReqest WriteData(void* buf, unsigned int size);
		TypeAdapter GetTypeAdapter();
		StatusAdapter GetStatusAdapter();
		std::shared_ptr<BaseAnswer> GetInfoAdapter(ParamInfoAdapter param);

		//ResultReqest CreateMemory(TypeData type, TypeDirection val, unsigned int size, std::string name);
		//ResultReqest ReadMemory(TypeData type, void* buf, unsigned int size);
		//ResultReqest WriteMemory(TypeData type, void* buf, int size);
		//HeaderSharedMemory ReadHead(TypeData type);
		//void DeleteMemory(TypeData type);

		SharedMemoryAdaptor();
		~SharedMemoryAdaptor();

	};
}

