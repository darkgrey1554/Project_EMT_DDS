#include "Adapters.h"
#include "SharedMemoryAdaptor.h" 


namespace gate
{
	std::shared_ptr<Adapter> CreateAdapter(TypeAdapter type)
	{
		std::shared_ptr<Adapter> adapter = nullptr;

		if (type == TypeAdapter::SharedMemory) adapter = std::make_shared<SharedMemoryAdaptor>();

		return adapter;
	}
}
