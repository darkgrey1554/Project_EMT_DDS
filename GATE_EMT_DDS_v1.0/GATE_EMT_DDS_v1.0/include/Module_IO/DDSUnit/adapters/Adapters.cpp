#include "Adapters.hpp"
#include <Module_IO/DDSUnit/adapters/SharedMemory_Adapter/AdapterSharedMemory.hpp>


namespace scada_ate::gate::adapter
{
	std::shared_ptr<IAdapter> CreateAdapter(TypeAdapter type)
	{
		std::shared_ptr<IAdapter> adapter = nullptr;

		if (type == TypeAdapter::SharedMemory) adapter = std::make_shared<sem::AdapterSharedMemory>();

		return adapter;
	};
}
