#pragma once
#include "DDSUnit.h"
#include "Config_Reader.h"
#include <map>
#include <mutex>

namespace scada_ate
{
	namespace module_io
	{
		class Module_IO
		{
		protected:

			unsigned int ID_Gate = 0;

			LoggerSpace::Logger* log;
			ConfigGate config_gate;
			std::unique_ptr<ConfigReader> reader_config = std::make_unique<ConfigReader>();
			std::vector<ConfigDDSUnit> config_DDSUnits;
			std::atomic<StatusModeluIO> status;
			std::mutex mutex_guard_interface;

			DomainParticipant* participant_ = nullptr;
			eprosima::fastdds::dds::Subscriber* subscriber_ = nullptr;
			Topic* topic_InfoDDSUnit = nullptr;

			DynamicType_ptr type_topic_infoddsunits;

			std::map<std::string, std::shared_ptr<gate::DDSUnit>> Map_DDSUnits;  /// name + shared_ptr to unit

			ResultReqest init();
			ResultReqest clear_properties();
			ResultReqest create_type_topic_infoddsunits();
			ResultReqest init_participant();
			ResultReqest init_subscriber();
			ResultReqest registration_types();
			ResultReqest create_topics();
			ResultReqest UpdateConfigDDSUnits();
			ResultReqest UpdateFileConfigUnits(std::shared_ptr<char> data, unsigned int size);
			ResultReqest Clear();

			std::string CreateNameStructInfoUnits();
			std::string CreateNameTopicInfoUnits(std::string source);
			std::string CreateNameTopicConfigDDSUnits();

			void SetCurrentStatus(StatusModeluIO value)
			{
				status.store(value, std::memory_order_relaxed);
				return;
			};

		public:

			Module_IO();
			~Module_IO();
			ResultReqest InitModule();
			StatusModeluIO GetCurrentStatus();
			ResultReqest StopTransfer();
			ResultReqest StartTransfer();
			ResultReqest ReInitModule();
			ResultReqest UpdateUnits();

		};
	}
	
}