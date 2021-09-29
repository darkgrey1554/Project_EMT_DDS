#include "Adapters.h"
#include "SharedMemoryAdaptor.h" 


namespace Gate
{
	Adaptor* CreateAdaptor(TypeAdapter type)
	{
		Adaptor* adaptor = nullptr;

		if (type == TypeAdapter::SharedMemory) adaptor = new SharedMemoryAdaptor();

		return adaptor;
	}
}
