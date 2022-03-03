#pragma once
#include <structs/structs.hpp>
#include <TypeTopicDDS/TypeTopics.h>
#include <Module_IO/DDSUnit/adapters/SharedMemory_Adapter/AdapterSharedMemory.hpp>

namespace scada_ate::gate::adapter
{
	enum class TypeData
	{
		Base,
		Extended
	};

	enum class TypeAdapter
	{
		SharedMemory,
		DDS,
		DTS,
		OPC_UA,
		SMTP,
		Null
	};

	struct IConfigAdapter
	{
		TypeAdapter type_adapter;
	};

	struct IAnswer
	{
		TypeAdapter type_adapter;
		ParamInfoAdapter param;
		ResultReqest result;
	};

	class IAdapter
	{
	public:

		virtual ResultReqest InitAdapter(std::shared_ptr<IConfigAdapter> config) = 0;
		virtual ResultReqest ReadData(std::shared_ptr<DDSData> buf) = 0;
		virtual ResultReqest WriteData(std::shared_ptr<DDSData> buf) = 0;
		virtual ResultReqest ReadExData(std::shared_ptr<DDSDataEx> buf) = 0;
		virtual ResultReqest WriteExData(std::shared_ptr<DDSDataEx> buf) = 0;
		virtual TypeAdapter GetTypeAdapter() = 0;
		virtual StatusAdapter GetStatusAdapter() = 0;
		virtual std::shared_ptr<IAnswer> GetInfoAdapter(ParamInfoAdapter param) = 0;
		virtual ~IAdapter() {};
	};

	std::shared_ptr<IAdapter> CreateAdapter(TypeAdapter type);
		
}

