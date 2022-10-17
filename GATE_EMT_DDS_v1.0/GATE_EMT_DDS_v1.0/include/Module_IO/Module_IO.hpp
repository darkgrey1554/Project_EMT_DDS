#pragma once
#include <Module_IO/UnitTransfer/UnitTransfer.hpp>

namespace scada_ate::gate
{

	class Module_IO
	{
		std::map<int, std::unique_ptr<adapter::UnitTransfer>> _map_units;
		std::shared_ptr<LoggerSpaceScada::ILoggerScada> log;
		int _node_id = 0;

		ResultReqest verification_config_unit(const adapter::ConfigUnitTransfer& config);
		public:

		Module_IO();
		~Module_IO();

		ResultReqest AddUnit(const adapter::ConfigUnitTransfer& config);
		ResultReqest GetStatusUnit(adapter::StatusUnitTransfer& _status, int64_t id);
		ResultReqest StartUnit(int64_t id);
		ResultReqest StopUnit(int64_t id);
		ResultReqest ReinitUnit(int64_t id);
		ResultReqest SetNodeID(int64_t id);
		ResultReqest RemoveUnit(int64_t id);
		ResultReqest ClearModule();		
	};
}
