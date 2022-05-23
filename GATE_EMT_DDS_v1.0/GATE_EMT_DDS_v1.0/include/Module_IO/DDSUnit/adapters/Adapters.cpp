#include "Adapters.hpp"
#include <Module_IO/DDSUnit/adapters/SharedMemory_Adapter/AdapterSharedMemory.hpp>

namespace scada_ate::gate::adapter
{

	bool operator < (const InfoTag& lv, const InfoTag rv)
	{
		if (lv.id_tag < rv.id_tag || lv.tag < rv.tag) return true;

		if (lv.id_tag == rv.id_tag && lv.tag == rv.tag)
		{
			if (lv.offset < rv.offset) return true;
		}
		return false;
	}

	bool operator == (const InfoTag& lv, const InfoTag rv)
	{
		if (lv.id_tag != lv.id_tag) return false;
		if (lv.offset != rv.offset) return false;
		if (lv.tag != rv.tag) return false;

		return true;
	}

	std::shared_ptr<IAdapter> CreateAdapter(TypeAdapter type)
	{
		std::shared_ptr<IAdapter> adapter = nullptr;

		if (type == TypeAdapter::SharedMemory) adapter = std::make_shared<sem::AdapterSharedMemory>();

		return adapter;
	};
};


