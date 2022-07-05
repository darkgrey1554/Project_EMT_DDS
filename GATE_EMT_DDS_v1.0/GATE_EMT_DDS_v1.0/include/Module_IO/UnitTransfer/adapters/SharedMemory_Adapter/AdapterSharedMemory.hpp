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
#include <math.h>

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

	class AdapterSharedMemory : public IAdapter
	{

		/// --- ���������� shared memory (WIN32)--- ///
		ConfigAdapterSharedMemory config; /// ������������ ��������
		HANDLE SM_Handle = NULL;   /// handle ������� shared memoty ����
		char* buf_data = nullptr; /// ��������� �� ���� ������
		HANDLE Mutex_SM = NULL; /// handle �������� ��� ������� � shared memory
		std::shared_ptr<SecurityHandle> security_attr; /// handle �������� ������������, ���

		std::mutex mutex_init;
		std::atomic<StatusAdapter> current_status = StatusAdapter::Null; /// ���������� ������� �������� 
		std::shared_ptr<LoggerSpaceScada::ILoggerScada> log; /// ������

		/// --- ������� ������������ ��������� ������ �� ������ HeaderData --- ///
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

}

