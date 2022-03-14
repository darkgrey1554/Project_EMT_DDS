#pragma once

#include <windows.h>
#include <LoggerScada.hpp>
#include <chrono>
#include <time.h>
#include <stdio.h>
#include <atomic>
#include <Module_IO/DDSUnit/adapters/Adapters.hpp>
#include "security_handle.hpp"
#include <structs/TimeConverter.hpp>
#include <map>

namespace scada_ate::gate::adapter::sem
{

	struct HeaderSharedMemory
	{
		TypeInfo InfoType;
		TypeData type_data;
		long long TimeLastWrite;
		long long TimeLastRead;
		unsigned long count_write = 0;
		unsigned long count_read = 0;
		size_t size_alarms = 0;
		size_t size_data_int = 0;
		size_t size_data_float = 0;
		size_t size_data_double = 0;
		size_t size_data_char = 0;
		size_t size_str = 0;
	};

	struct ConfigAdapterSharedMemory : public IConfigAdapter
	{
		std::string NameChannel;
		TypeInfo InfoType;
		TypeData DataType;
		std::vector<uint32_t> v_tags;
		size_t size_alarms = 0;
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
		std::shared_ptr<DDSData> data_ = nullptr;
		std::shared_ptr<DDSData> data_last = nullptr;
		std::shared_ptr<DDSAlarm> alarms_ = nullptr;
		std::shared_ptr<DDSAlarm> alarms_last = nullptr;

		std::map<uint32_t, uint32_t> map_TagToPoint;

		/// --- вспомогательные переменные --- /// 
		enum class TypeValue
		{
			INT,
			FLOAT,
			DOUBLE,
			CHAR
		};

		std::mutex mutex_init;
		std::atomic<StatusAdapter> current_status = StatusAdapter::Null; /// переменная статуса адаптера 
		std::shared_ptr<LoggerSpaceScada::ILoggerScada> log; /// логгер

		/// --- функция формарования структуры ответа на запрос HeaderData --- ///
		std::shared_ptr<AnswerSharedMemoryHeaderData> AnswerRequestHeaderData();

		std::string CreateSMName();
		std::string CreateSMMutexName();
		size_t GetSizeMemory();
		size_t Offset(TypeValue type_value);
		std::shared_ptr<DDSDataEx> create_extended_data(ModeRead mode_read);
		std::shared_ptr<DDSDataEx> create_extended_fulldata();
		std::shared_ptr<DDSDataEx> create_extended_diffdata();

	public:

		ResultReqest InitAdapter(std::shared_ptr<IConfigAdapter> config) override;
		ResultReqest ReadData(std::shared_ptr<DDSData>& buf, ModeRead rise_data = ModeRead::Regular) override;
		ResultReqest WriteData(std::shared_ptr<DDSData>& buf) override;
		ResultReqest ReadData(std::shared_ptr<DDSDataEx>& buf, ModeRead rise_data = ModeRead::Regular) override;
		ResultReqest WriteData(std::shared_ptr<DDSDataEx>& buf) override;
		ResultReqest ReadData(std::shared_ptr<DDSAlarm>& buf, ModeRead rise_data = ModeRead::Regular) override;
		ResultReqest WriteData(std::shared_ptr<DDSAlarm>& buf) override;
		ResultReqest ReadData(std::shared_ptr<DDSAlarmEx>& buf, ModeRead rise_data = ModeRead::Regular) override;
		ResultReqest WriteData(std::shared_ptr<DDSAlarmEx>& buf) override;
		TypeAdapter GetTypeAdapter() override;
		StatusAdapter GetStatusAdapter() override;
		std::shared_ptr<IAnswer> GetInfoAdapter(ParamInfoAdapter param) override;


		AdapterSharedMemory();
		~AdapterSharedMemory();

	};

}

