#pragma once
#include <structs/structs.hpp>

namespace gate
{

	class IAdapter
	{
	public:

		virtual ResultReqest InitAdapter(std::shared_ptr<IConfigAdapter> config) = 0;
		virtual ResultReqest ReadData(void* buf, size_t size) = 0;
		virtual ResultReqest WriteData(void* buf, size_t size) = 0;
		virtual TypeAdapter GetTypeAdapter() = 0;
		virtual StatusAdapter GetStatusAdapter() = 0;
		virtual std::shared_ptr<IAnswer> GetInfoAdapter(ParamInfoAdapter param) = 0;
		virtual ~IAdapter() {};
	};

	std::shared_ptr<IAdapter> CreateAdapter(TypeAdapter type);

}

