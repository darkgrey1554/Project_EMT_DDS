#pragma once

#ifdef _WIN32
	#include <windows.h>
	#include "security_handle.hpp"
#endif // WIN32

#ifdef __linux__
	#include <sys/mman.h>
	#include <sys/stat.h>
	#include <fcntl.h>
	#include <cstring>
	#include <unistd.h>
	#include <semaphore.h>
#endif // __linux__

#include <LoggerScada.hpp>
#include <chrono>
#include <time.h>
#include <stdio.h>
#include <atomic>
#include <map>
#include <math.h>

#include <Module_IO/UnitTransfer/adapters/Adapters.hpp>
#include <structs/TimeConverter.hpp>

namespace scada_ate::gate::adapter::sem
{
	struct HeaderSharedMemory
	{
		long long TimeLastWrite;
		long long TimeLastRead;
		unsigned long count_write = 0;
		unsigned long count_read = 0;
		size_t size_data_int = 0;
		size_t size_data_float = 0;
		size_t size_data_double = 0;
		size_t size_data_char = 0;
		size_t size_str = 0;
	};

	struct ConfigAdapterSharedMemory : public IConfigAdapter
	{
		std::string NameChannel = "";
		size_t size_int_data = 0;
		size_t size_float_data = 0;
		size_t size_double_data = 0;
		size_t size_char_data = 0;
		size_t size_str_data = 0;
		size_t size_str = 0;

	};

	struct AnswerSharedMemoryHeaderData : public IAnswer
	{
		HeaderSharedMemory header;
	};

	#ifdef _WIN32
	class AdapterSharedMemory : public IAdapter
	{

		/// --- переменные shared memory (WIN32)--- ///
		ConfigAdapterSharedMemory config; /// конфигурация адапрета
		HANDLE SM_Handle = NULL;   /// handle объекта shared memoty ядра
		char* buf_data = nullptr; /// указатель на блок памяти
		HANDLE Mutex_SM = NULL; /// handle мьютекса ждя доступа к shared memory
		std::shared_ptr<SecurityHandle> security_attr; /// handle атрибута безопасности, для

		std::mutex mutex_init;
		std::atomic<StatusAdapter> current_status = StatusAdapter::Null; /// переменная статуса адаптера 
		std::shared_ptr<LoggerSpaceScada::ILoggerScada> log; /// логгер

		/// --- функция формарования структуры ответа на запрос HeaderData --- ///
		std::shared_ptr<AnswerSharedMemoryHeaderData> AnswerRequestHeaderData();

		std::string CreateSMName();
		std::string CreateSMMutexName();
		size_t GetSizeMemory();
		size_t TakeOffset(const TypeValue& type_value, const size_t& ofs) const;
		int demask(const int& value, int mask_source,const int& value_target, const int& mask_target);

		size_t offset_int = 0;
		size_t offset_float = 0;
		size_t offset_double = 0;
		size_t offset_char = 0;
		size_t offset_str = 0;

		void set_data(const ValueInt& value, const LinkTags& link);
		void set_data(const ValueFloat& value, const LinkTags& link);
		void set_data(const ValueDouble& value, const LinkTags& link);
		void set_data(const ValueChar& value, const LinkTags& link);
		void set_data(const ValueString& value, const LinkTags& link);

	public:

		ResultReqest InitAdapter() override;
		ResultReqest ReadData(std::deque<SetTags>** buf) override;
		ResultReqest WriteData(const std::deque<SetTags>& buf) override;
		TypeAdapter GetTypeAdapter() override;
		StatusAdapter GetStatusAdapter() override;
		std::shared_ptr<IAnswer> GetInfoAdapter(ParamInfoAdapter param) override;

		AdapterSharedMemory(std::shared_ptr<IConfigAdapter> config);
		~AdapterSharedMemory();

	};
	#endif

	#ifdef __linux__
	class AdapterSharedMemory : public IAdapter
	{
		/// --- переменные shared memory (Linux)--- ///
		ConfigAdapterSharedMemory config;
		char* buf_data = nullptr;
		int hnd_sm = 0;
		sem_t* _semaphor = nullptr;
		size_t size_memory = 0;


		std::mutex mutex_init;
		std::atomic<StatusAdapter> current_status = StatusAdapter::Null; /// переменная статуса адаптера 
		std::shared_ptr<LoggerSpaceScada::ILoggerScada> log; /// логгер

		/// --- функция формарования структуры ответа на запрос HeaderData --- ///
		std::shared_ptr<AnswerSharedMemoryHeaderData> AnswerRequestHeaderData();

		ResultReqest init_mutex();
		ResultReqest init_shared_memory();
		ResultReqest allocate_memory(size_t size_memory);
		ResultReqest mapping_memory(size_t size_memory);
		void destroy();

		ResultReqest lock_semaphore();
		ResultReqest unlock_semaphore();


		std::string CreateSMName();
		std::string CreateSMMutexName();
		size_t GetSizeMemory();
		size_t TakeOffset(const TypeValue& type_value, const size_t& ofs) const;
		int demask(const int& value, int mask_source, const int& value_target, const int& mask_target);

		size_t offset_int = 0;
		size_t offset_float = 0;
		size_t offset_double = 0;
		size_t offset_char = 0;
		size_t offset_str = 0;

		void set_data(const ValueInt& value, const LinkTags& link);
		void set_data(const ValueFloat& value, const LinkTags& link);
		void set_data(const ValueDouble& value, const LinkTags& link);
		void set_data(const ValueChar& value, const LinkTags& link);
		void set_data(const ValueString& value, const LinkTags& link);

	public:

		ResultReqest InitAdapter() override;
		ResultReqest ReadData(std::deque<SetTags>** buf) override;
		ResultReqest WriteData(const std::deque<SetTags>& buf) override;
		TypeAdapter GetTypeAdapter() override;
		StatusAdapter GetStatusAdapter() override;
		std::shared_ptr<IAnswer> GetInfoAdapter(ParamInfoAdapter param) override;

		AdapterSharedMemory(std::shared_ptr<IConfigAdapter> config);
		~AdapterSharedMemory();

};
	#endif
}

