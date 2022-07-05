#include "Adapters.hpp"
#include <Module_IO/DDSUnit/adapters/SharedMemory_Adapter/AdapterSharedMemory.hpp>
#include <Module_IO/DDSUnit/adapters/DDS_Adapter/AdapterDDS.hpp>



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

	std::shared_ptr<IAdapter> CreateAdapter(std::shared_ptr<IConfigAdapter> config)
	{
		std::shared_ptr<IAdapter> adapter = nullptr;

		if (config.get()->type_adapter == TypeAdapter::SharedMemory) adapter = std::make_shared<sem::AdapterSharedMemory>(config);

		if (config.get()->type_adapter == TypeAdapter::DDS)
		{
			std::shared_ptr<scada_ate::gate::adapter::dds::ConfigAdapterDDS> config_point = 
				std::reinterpret_pointer_cast<scada_ate::gate::adapter::dds::ConfigAdapterDDS>(config);

			if (config_point->type_data == scada_ate::gate::adapter::dds::TypeDDSData::DDSAlarm) 
				adapter = std::make_shared<dds::AdapterDDS<DDSAlarm>>(config);
			if (config_point->type_data == scada_ate::gate::adapter::dds::TypeDDSData::DDSData)
				adapter = std::make_shared<dds::AdapterDDS<DDSData>>(config);
			if (config_point->type_data == scada_ate::gate::adapter::dds::TypeDDSData::DDSAlarmEx)
				adapter = std::make_shared<dds::AdapterDDS<DDSAlarmEx>>(config);
			if (config_point->type_data == scada_ate::gate::adapter::dds::TypeDDSData::DDSDataEx)
				adapter = std::make_shared<dds::AdapterDDS<DDSDataEx>>(config);
		}

		return adapter;
	};
};


