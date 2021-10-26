#pragma once
#include "DDSUnit.h"
#include "Config_Reader.h"
#include <map>

namespace scada_ate
{
	class Module_IO
	{
	protected:

		unsigned int ID_Gate = 0;

		LoggerSpace::Logger* log;
		ConfigGate config_gate;
		std::unique_ptr<ConfigReader> reader_config = std::make_unique<ConfigReader>();
		std::vector<ConfigDDSUnit> config_DDSUnits;

		DomainParticipant* participant_ = nullptr;
		eprosima::fastdds::dds::Publisher* publisher_ = nullptr;
		eprosima::fastdds::dds::Subscriber* subscriber_ = nullptr;
		Topic* topic_command = nullptr;
		Topic* topic_answer = nullptr;
		Topic* topic_InfoDDSUnit = nullptr;
		DataReader* reader_command = nullptr;
		DataWriter* aswerer = nullptr;

		DynamicType_ptr type_topic_command;
		DynamicType_ptr type_topic_answer;
		DynamicType_ptr type_topic_infoddsunits;

		std::map<std::string, std::shared_ptr<gate::DDSUnit>> Map_DDSUnits;  /// name + shared_ptr to unit

		class SubListener : public DataReaderListener
		{
		public:
			Module_IO* master;
			SubListener(Module_IO* master) : master(master) {};
			void on_subscription_matched(DataReader*, const SubscriptionMatchedStatus& info) override;
			void on_data_available(DataReader* reader) override;
		};
		friend class SubListener;
		std::shared_ptr<SubListener> listener_ = std::make_shared<SubListener>(this);

		ResultReqest create_type_topic_command();
		ResultReqest create_type_topic_answer();
		ResultReqest create_type_topic_infoddsunits();

		

		std::string CreateNameTopicCommand(std::string source);
		std::string CreateNameTopicAnswer(std::string source);
		std::string CreateNameTopicInfoUnits(std::string source);

	public:


		Module_IO();
		~Module_IO();
		ResultReqest InitModule();

	};
}