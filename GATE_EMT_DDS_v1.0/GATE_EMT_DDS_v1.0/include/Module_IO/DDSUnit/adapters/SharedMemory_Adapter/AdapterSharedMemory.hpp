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
#include <structs/TimeConverter.hpp>

/// <summary>
///  класс реализует взаимодействие между ЕМТ и шлюзом DDS по Shared Memory
/// </summary>

namespace scada_ate::gate::adapter
{
	enum class TypeData
	{
		Base,
		Extended
	};

	struct HeaderSharedMemory
	{
		TypeData type_data = TypeData::Base;
		long long TimeLastWrite;
		long long TimeLastRead;
		unsigned long count_write = 0;
		unsigned long count_read = 0;
		size_t size_data_int = 0;
		size_t size_data_float = 0;
		size_t size_data_double = 0;
		size_t size_data_char = 0;
		size_t size_str = 0;
		size_t current_size_data_int = 0;
		size_t current_size_data_float = 0;
		size_t current_size_data_double = 0;
		size_t current_size_data_char = 0;
	};

	struct ConfigAdapterSharedMemory : public IConfigAdapter
	{
		std::string NameMemory;
		TypeData DataType;
		size_t size_data_int = 0;
		size_t size_data_float = 0;
		size_t size_data_double = 0;
		size_t size_data_char = 0;
		size_t size_str = 0;
	};

	struct AnswerSharedMemoryHeaderData : public IAnswer
	{
		HeaderSharedMemory header;
	};

	class AdapterSharedMemory : public IAdapter
	{
		
		/// --- переменные shared memory (WIN32)--- ///
		ConfigAdapterSharedMemory config; /// конфигурация адапрета
		HANDLE SM_Handle = NULL;   /// handle объекта shared memoty ядра
		char* buf_data = nullptr; /// указатель на блок памяти
		HANDLE Mutex_SM = NULL; /// handle мьютекса ждя доступа к shared memory
		std::shared_ptr<SecurityHandle> security_attr; /// handle атрибута безопасности, для
		std::shared_ptr<DDSData> data_base;
		std::shared_ptr<DDSDataEx> data_extended;
		
		/// --- вспомогательные переменные --- /// 
		std::mutex mutex_init;
		std::atomic<StatusAdapter> current_status = StatusAdapter::Null; /// переменная статуса адаптера 
		std::shared_ptr<LoggerSpaceScada::ILoggerScada> log; /// логгер

		/// --- функция формарования структуры ответа на запрос HeaderData --- ///
		std::shared_ptr<AnswerSharedMemoryHeaderData> AnswerRequestHeaderData();

		std::string CreateSMName(std::string source);
		std::string CreateSMMutexName(std::string source);
		size_t GetSizeMemory();

	public:

		ResultReqest InitAdapter(std::shared_ptr<IConfigAdapter> config);
		ResultReqest ReadData(std::shared_ptr<DDSData>& buf);
		ResultReqest WriteData(std::shared_ptr<DDSData>& buf);
		ResultReqest ReadExData(std::shared_ptr<DDSDataEx>& buf);
		ResultReqest WriteExData(std::shared_ptr<DDSDataEx>& buf);
		TypeAdapter GetTypeAdapter();
		StatusAdapter GetStatusAdapter();
		std::shared_ptr<IAnswer> GetInfoAdapter(ParamInfoAdapter param);


		AdapterSharedMemory();
		~AdapterSharedMemory();

	};

}

