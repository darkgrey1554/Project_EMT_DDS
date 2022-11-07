#pragma once
#include <Module_IO/UnitTransfer/UnitTransfer.hpp>
#include <structs/interfaces.h>

namespace scada_ate::gate
{

	class Module_IO : public atech::common::IControl 
	{
		std::map<int, std::unique_ptr<adapter::UnitTransfer>> _map_units;
		std::shared_ptr<LoggerSpaceScada::ILoggerScada> log;
		int _node_id = 0;

		ResultReqest verification_config_unit(const adapter::ConfigUnitTransfer& config);
		public:

		Module_IO();
		~Module_IO();

		
		ResultReqest SetNodeID(int64_t id);
		ResultReqest RemoveUnit(int64_t id);
		ResultReqest ClearModule();
		ResultReqest AddUnit(const adapter::ConfigUnitTransfer& config);
		uint32_t GetId() override;
		ResultReqest GetStatus(std::deque<std::pair<uint32_t, atech::common::Status>>& st, uint32_t id = 0) override;
		ResultReqest Start(std::deque<std::pair<uint32_t, atech::common::Status>>& st, uint32_t id = 0) override;
		ResultReqest Stop(std::deque<std::pair<uint32_t, atech::common::Status>>& st, uint32_t id = 0) override;
		ResultReqest ReInit(std::deque<std::pair<uint32_t, atech::common::Status>>& st, uint32_t id = 0) override;
				
	};
}
