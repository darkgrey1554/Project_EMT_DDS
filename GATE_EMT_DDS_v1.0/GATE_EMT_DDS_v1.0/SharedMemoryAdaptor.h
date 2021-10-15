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
///  ����� ��������� �������������� ����� ��� � ������ DDS �� Shared Memory
/// </summary>

namespace gate
{
	

	class SharedMemoryAdaptor : public Adapter
	{
		
		/// --- ���������� shared memory (WIN32)--- ///
		HANDLE SM_Handle = NULL;   /// handle ������� shared memoty ����
		char* buf_data = nullptr; /// ��������� �� ���� ������
		unsigned int size_memory = 0; /// ���-�� �������� ������ shared memory
		unsigned int size_type = 0; /// ������ ��������� ���� � ������
		HANDLE Mutex_SM = NULL; /// handle �������� ��� ������� � shared memory
		SecurityHandle security_attr; /// handle �������� ������������, ���  

		/// --- ��������������� ���������� --- /// 
		ConfigSharedMemoryAdapter config; /// ������������ ��������
		std::atomic<StatusAdapter> current_status = StatusAdapter::Null; /// ���������� ������� �������� 
		LoggerSpace::Logger* log; /// ������

		/// --- ������� ������������ ��������� ������ �� ������ HeaderData --- ///
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

