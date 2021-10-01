#include "Adapters.h"
#include "SharedMemoryAdaptor.h" 


namespace gate
{
	Adapter* CreateAdapter(TypeAdapter type)
	{
		Adapter* adapter = nullptr;

		if (type == TypeAdapter::SharedMemory) adapter = new SharedMemoryAdaptor();

		return std::move(adapter);
	}
}
