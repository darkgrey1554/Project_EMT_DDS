#pragma once
#include "structs.h"

namespace gate
{

	class Adapter
	{
	public:

		virtual ResultReqest InitAdaptor(void* config) = 0;
		virtual ResultReqest ReadData(void* buf, unsigned int size) = 0;
		virtual ResultReqest WriteData(void* buf, unsigned int size) = 0;
		virtual std::unique_ptr<void> GetInfoAdaptor(ParamInfoAdapter param) = 0;
		virtual ~Adapter() {};
	};

	Adapter* CreateAdaptor(TypeAdapter type);
	
}

