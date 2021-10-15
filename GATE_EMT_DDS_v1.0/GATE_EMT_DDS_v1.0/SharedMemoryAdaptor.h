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
	

	class SharedMemoryAdaptor : public Adapter
	{
		
		/// --- переменные shared memory (WIN32)--- ///
		HANDLE SM_Handle = NULL;   /// handle объекта shared memoty ядра
		char* buf_data = nullptr; /// указатель на блок памяти
		unsigned int size_memory = 0; /// кол-во хранимых данных shared memory
		unsigned int size_type = 0; /// размер хранимого типа в байтах
		HANDLE Mutex_SM = NULL; /// handle мьютекса ждя доступа к shared memory
		SecurityHandle security_attr; /// handle атрибута безопасности, для  

		/// --- вспомогательные переменные --- /// 
		ConfigSharedMemoryAdapter config; /// конфигурация адапрета
		std::atomic<StatusAdapter> current_status = StatusAdapter::Null; /// переменная статуса адаптера 
		LoggerSpace::Logger* log; /// логгре

		/// --- функция формарования структуры ответа на запрос HeaderData --- ///
		std::shared_ptr<HeaderDataAnswerSM> AnswerRequestHeaderData();
		std::string CreateSMName(std::string source);

	public:

		ResultReqest InitAdapter(std::shared_ptr<ConfigAdapter> config);
		ResultReqest ReadData(void* buf, unsigned int size);
		ResultReqest WriteData(void* buf, unsigned int size);
		TypeAdapter GetTypeAdapter();
		StatusAdapter GetStatusAdapter();
		std::shared_ptr<BaseAnswer> GetInfoAdapter(ParamInfoAdapter param);

		SharedMemoryAdaptor();
		~SharedMemoryAdaptor();

	};
}

