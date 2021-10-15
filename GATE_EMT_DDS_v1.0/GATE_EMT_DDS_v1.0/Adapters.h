#pragma once
#include "structs.h"

namespace gate
{

	class Adapter
	{
	public:

		virtual ResultReqest InitAdapter(std::shared_ptr<ConfigAdapter> config) = 0;
		virtual ResultReqest ReadData(void* buf, unsigned int size) = 0;
		virtual ResultReqest WriteData(void* buf, unsigned int size) = 0;
		virtual TypeAdapter GetTypeAdapter() = 0;
		virtual StatusAdapter GetStatusAdapter() = 0;
		virtual std::shared_ptr<BaseAnswer> GetInfoAdapter(ParamInfoAdapter param) = 0;
		virtual ~Adapter() {};
	};

	std::shared_ptr<Adapter> CreateAdapter(TypeAdapter type);

}

