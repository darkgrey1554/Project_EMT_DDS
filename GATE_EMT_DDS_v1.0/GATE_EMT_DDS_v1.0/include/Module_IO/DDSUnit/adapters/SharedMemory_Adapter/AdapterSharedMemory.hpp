#pragma once

#include "structs/structs.hpp"
#include <windows.h>
#include <LoggerScada.hpp>
#include <chrono>
#include <time.h>
#include <stdio.h>
#include <atomic>
#include <Module_IO/DDSUnit/adapters/Adapters.hpp>
#include "security_handle.hpp"

/// <summary>
///  класс реализует взаимодействие между ЕМТ и шлюзом DDS по Shared Memory
/// </summary>

namespace gate
{
	

	class AdapterSharedMemory : public IAdapter
	{
		
		/// --- переменные shared memory (WIN32)--- ///
		HANDLE SM_Handle = NULL;   /// handle объекта shared memoty ядра
		char* buf_data = nullptr; /// указатель на блок памяти
		size_t size_memory = 0; /// кол-во хранимых данных shared memory
		size_t size_type = 0; /// размер хранимого типа в байтах
		HANDLE Mutex_SM = NULL; /// handle мьютекса ждя доступа к shared memory
		std::shared_ptr<SecurityHandle> security_attr; /// handle атрибута безопасности, для  

		/// --- вспомогательные переменные --- /// 
		std::mutex mutex_init;
		ConfigAdapterSharedMemory config; /// конфигурация адапрета
		std::atomic<StatusAdapter> current_status = StatusAdapter::Null; /// переменная статуса адаптера 
		std::shared_ptr<LoggerSpaceScada::ILoggerScada> log; /// логгер

		/// --- функция формарования структуры ответа на запрос HeaderData --- ///
		std::shared_ptr<AnswerSharedMemoryHeaderData> AnswerRequestHeaderData();

		std::string CreateSMName(std::string source);
		std::string CreateSMMutexName(std::string source);

	public:

		ResultReqest InitAdapter(std::shared_ptr<IConfigAdapter> config);
		ResultReqest ReadData(void* buf, size_t size);
		ResultReqest WriteData(void* buf, size_t size);
		TypeAdapter GetTypeAdapter();
		StatusAdapter GetStatusAdapter();
		std::shared_ptr<IAnswer> GetInfoAdapter(ParamInfoAdapter param);


		AdapterSharedMemory();
		~AdapterSharedMemory();

	};
}

