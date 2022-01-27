#include "Adapters.hpp"
#include "SharedMemory_adapter/AdapterSharedMemory.hpp" 


namespace gate
{
	std::shared_ptr<IAdapter> CreateAdapter(TypeAdapter type)
	{
		std::shared_ptr<IAdapter> adapter = nullptr;

		if (type == TypeAdapter::SharedMemory) adapter = std::make_shared<AdapterSharedMemory>();

		return adapter;
	}
}
