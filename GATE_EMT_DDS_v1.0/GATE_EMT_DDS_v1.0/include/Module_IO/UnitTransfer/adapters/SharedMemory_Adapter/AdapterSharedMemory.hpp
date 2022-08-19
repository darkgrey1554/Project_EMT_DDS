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

	//#ifdef _WIN32
	class AdapterSharedMemory : public IAdapter
	{

		/// --- ���������� shared memory (WIN32)--- ///
		ConfigAdapterSharedMemory config; /// ������������ ��������
		char* buf_data = nullptr; /// ��������� �� ���� ������
		
		#ifdef _WIN32
		HANDLE SM_Handle = NULL;   /// handle ������� shared memoty ����
		HANDLE Mutex_SM = NULL; /// handle �������� ��� ������� � shared memory
		std::shared_ptr<SecurityHandle> security_attr; /// handle �������� ������������, ���
		#endif // _WIN32

		
		#ifdef __linux__
		int hnd_sm = 0;
		sem_t* _semaphor = nullptr;
		size_t size_memory = 0;

		ResultReqest init_mutex();
		ResultReqest init_shared_memory();
		ResultReqest allocate_memory(size_t size_memory);
		ResultReqest mapping_memory(size_t size_memory);
		void destroy();

		ResultReqest lock_semaphore();
		ResultReqest unlock_semaphore();
		#endif // __linux__

		

		std::mutex mutex_init;
		std::atomic<StatusAdapter> current_status = StatusAdapter::Null; /// ���������� ������� �������� 
		std::shared_ptr<LoggerSpaceScada::ILoggerScada> log; /// ������

		/// --- ������� ������������ ��������� ������ �� ������ HeaderData --- ///
		std::shared_ptr<AnswerSharedMemoryHeaderData> AnswerRequestHeaderData();

		void init_deque();
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

		void set_data(TypeValue& type, const Value& value, const LinkTags& link);
		void set_data_int(const Value& value, const LinkTags& link);
		void set_data_float(const Value& value, const LinkTags& link);
		void set_data_double(const Value& value, const LinkTags& link);
		void set_data_char(const Value& value, const LinkTags& link);
		void set_data_string(const Value& value, const LinkTags& link);

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

	//#endif

	/*#ifdef __linux__
	class AdapterSharedMemory : public IAdapter
	{
		/// --- ���������� shared memory (Linux)--- ///
		

		std::mutex mutex_init;
		std::atomic<StatusAdapter> current_status = StatusAdapter::Null; /// ���������� ������� �������� 
		std::shared_ptr<LoggerSpaceScada::ILoggerScada> log; /// ������

		/// --- ������� ������������ ��������� ������ �� ������ HeaderData --- ///
		std::shared_ptr<AnswerSharedMemoryHeaderData> AnswerRequestHeaderData();


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

		void set_data(const int& value, const LinkTags& link);
		void set_data(const float& value, const LinkTags& link);
		void set_data(const double& value, const LinkTags& link);
		void set_data(const char& value, const LinkTags& link);
		void set_data(const std::string& value, const LinkTags& link);

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
	#endif*/
}

