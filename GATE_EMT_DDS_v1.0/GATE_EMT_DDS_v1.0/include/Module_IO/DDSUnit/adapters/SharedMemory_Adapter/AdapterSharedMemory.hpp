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
///  ����� ��������� �������������� ����� ��� � ������ DDS �� Shared Memory
/// </summary>

namespace gate
{
	

	class AdapterSharedMemory : public IAdapter
	{
		
		/// --- ���������� shared memory (WIN32)--- ///
		HANDLE SM_Handle = NULL;   /// handle ������� shared memoty ����
		char* buf_data = nullptr; /// ��������� �� ���� ������
		size_t size_memory = 0; /// ���-�� �������� ������ shared memory
		size_t size_type = 0; /// ������ ��������� ���� � ������
		HANDLE Mutex_SM = NULL; /// handle �������� ��� ������� � shared memory
		std::shared_ptr<SecurityHandle> security_attr; /// handle �������� ������������, ���  

		/// --- ��������������� ���������� --- /// 
		std::mutex mutex_init;
		ConfigAdapterSharedMemory config; /// ������������ ��������
		std::atomic<StatusAdapter> current_status = StatusAdapter::Null; /// ���������� ������� �������� 
		std::shared_ptr<LoggerSpaceScada::ILoggerScada> log; /// ������

		/// --- ������� ������������ ��������� ������ �� ������ HeaderData --- ///
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

