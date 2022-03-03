#include "Adapters.hpp"
#include "SharedMemory_adapter/AdapterSharedMemory.hpp" 


namespace scada_ate::gate::adapter
{

		std::shared_ptr<IAdapter> CreateAdapter(TypeAdapter type)
		{
			std::shared_ptr<IAdapter> adapter = nullptr;

			if (type == TypeAdapter::SharedMemory) adapter = std::make_shared<sharedmemory::AdapterSharedMemory>();

			return adapter;
		}
}
