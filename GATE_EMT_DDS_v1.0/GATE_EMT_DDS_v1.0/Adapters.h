#pragma once
#include "structs.h"

namespace gate
{

	class Adapter
	{
	public:

		virtual ResultReqest InitAdaptor(void* config) = 0;
		virtual ResultReqest ReadData(TypeData type, void* buf, unsigned int size) = 0;
		virtual ResultReqest WriteData(TypeData type, void* buf, unsigned int size) = 0;
		virtual ResultReqest GetInfoAdaptor() = 0;
		virtual ~Adapter() {};
	};

	Adapter* CreateAdaptor(TypeAdapter type);
	
}

