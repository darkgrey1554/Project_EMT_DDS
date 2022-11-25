#pragma once

#include <Module_IO/UnitTransfer/adapters/Adapters.hpp>
#include <thread>
#include <LoggerScada.hpp>
#include <structs/TimeConverter.hpp>
#include <queue>
#include <future>
#include <structs/interfaces.h>

namespace scada_ate::gate::adapter
{
	struct Mapping
	{
		int64_t id;
		int64_t frequency;
		std::vector<LinkTags> vec_links;
	};

	struct ConfigUnitTransfer
	{
		int64_t id;
		int64_t frequency;
		std::vector<std::shared_ptr<IConfigAdapter>> config_input_unit;
		std::vector<std::shared_ptr<IConfigAdapter>> config_output_unit;
		std::vector<Mapping> mapping;
	};

	class BuilderUnitTransfer;

	class UnitTransfer: public atech::common::IControl
	{
		std::atomic<atech::common::Status> _status = atech::common::Status::NONE;
		std::shared_ptr<LoggerSpaceScada::ILoggerScada> log;
		int64_t id = 0;
		std::mutex _guarder;

		scada_ate::gate::adapter::IAdapter_ptr _adapter_source;
		std::map<int, scada_ate::gate::adapter::IAdapter_ptr> _adapters_target;
		std::deque<SetTags>* _data;

		struct frq_observer
		{
			int id;
			int64_t frq;
			long long time;
		};
		std::vector<frq_observer> _vec_frq;
		std::deque<int> _queue_transfer;
		const long long mcs_off = 25000;

		std::thread _thread_ctrl_adapters;
		std::thread _thread_transfer;
		std::atomic<int> _command_thread_transfer = 0;
		std::atomic<int> _command_thread_ctrl_adapters = 0;
		int64_t frq_transfer = 1000000; //1s


		void transfer_thread();
		void ctrl_adapter_thread();
		ResultReqest start_transfer();
		ResultReqest stop_transfer();

		//ResultReqest update_mapping_output_adapter();
		//ResultReqest update_mapping_input_adapter();

		friend BuilderUnitTransfer;

	public:

		UnitTransfer();
		~UnitTransfer();
		//ResultReqest InitUnit();
		//ResultReqest MountAdapterSource(scada_ate::gate::adapter::IConfigAdapter& config);
		//ResultReqest AddAdapterTarget(const scada_ate::gate::adapter::IConfigAdapter& config, const Mapping& mapping);
		uint32_t GetId() override;
		ResultReqest GetStatus(std::deque<std::pair<uint32_t,atech::common::Status>>& st,uint32_t id = 0) override;
		ResultReqest Start(std::deque<std::pair<uint32_t, atech::common::Status>>& st, uint32_t id = 0) override;
		ResultReqest Stop(std::deque<std::pair<uint32_t, atech::common::Status>>& st, uint32_t id = 0) override;
		ResultReqest ReInit(std::deque<std::pair<uint32_t, atech::common::Status>>& st, uint32_t id = 0) override;


	};

	class BuilderUnitTransfer
	{
		std::unique_ptr<UnitTransfer> _unit = nullptr;
		std::shared_ptr<LoggerSpaceScada::ILoggerScada> log;

		public:

		BuilderUnitTransfer();
		~BuilderUnitTransfer();
		ResultReqest CreateUnit(int64_t id);
		std::unique_ptr<UnitTransfer> GetUnit();
		ResultReqest SetFrqTransfer(int64_t frq);
		ResultReqest SetAdapterSource(IConfigAdapter_ptr config);
		ResultReqest AddAdapterTarget(IConfigAdapter_ptr config, int64_t frq);
	};
};

