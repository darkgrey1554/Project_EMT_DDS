#pragma once

#include <Module_IO/UnitTransfer/adapters/Adapters.hpp>
#include <thread>

class UnitTransfer
{
	scada_ate::gate::adapter::IAdapter_ptr _adapte_source;
	std::map<int, scada_ate::gate::adapter::IAdapter_ptr> _adapters_targer;

	std::jthread thread_transfer;

public:

	ResultReqest MountAdapterSource(scada_ate::gate::adapter::IConfigAdapter config);
	ResultReqest AddAdapterTarget(scada_ate::gate::adapter::IConfigAdapter config);


};