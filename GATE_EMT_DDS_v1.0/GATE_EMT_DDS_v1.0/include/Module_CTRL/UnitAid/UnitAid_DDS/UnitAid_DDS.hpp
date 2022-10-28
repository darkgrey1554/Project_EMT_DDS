#pragma once
#include <Module_CTRL/UnitAid/UnitAid.hpp>
#include <LoggerScada.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <ddsformat/TypeTopics.h>

namespace _dds = eprosima::fastdds::dds;

namespace atech::srv::io::ctrl
{
	class ConfigUnitAid_DDS : public IConfigUnitAid
	{

	};

	class UnitAid_DDS : public UnitAid
	{

	protected:

		LoggerSpaceScada::ILoggerScada_ptr log = nullptr;
		ConfigUnitAid_DDS config;

		_dds::DomainParticipant* _participant = nullptr;
		_dds::Publisher* _publisher = nullptr;
		_dds::Subscriber* _subscriber = nullptr;
		_dds::Topic* topic_command = nullptr;
		_dds::Topic* topic_respond = nullptr;
		_dds::DataReader* _reader_command = nullptr;
		_dds::DataWriter* _responder = nullptr;
		_dds::TypeSupport type_support_topic_command;
		_dds::TypeSupport type_support_topic_respond;
		std::string topic_name_config{ "DdsConfig" };


		class SubListener : public _dds::DataReaderListener
		{
		public:
			UnitAid_DDS* _master;
			SubListener(UnitAid_DDS* master) : _master(master) {};
			void on_subscription_matched(_dds::DataReader*, const _dds::SubscriptionMatchedStatus& info) override;
			void on_data_available(_dds::DataReader* reader) override;
		};
		friend class SubListener;
		std::shared_ptr<SubListener> _listener = std::make_shared<SubListener>(this);
;
		ResultReqest clear_properties();
		ResultReqest init_participant();
		ResultReqest init_subscriber();
		ResultReqest init_publisher();
		ResultReqest registration_types();
		ResultReqest create_topics();

		std::string get_name_participant_profile();
		std::string get_name_TopicCommand();
		std::string get_name_TopicRespond();
		std::string get_name_topic_profile_command();
		std::string get_name_topic_profile_respond();
		std::string get_name_subscriber_profile();
		std::string get_name_datawriter_profile();
		std::string get_name_publisher_profile();
		std::string get_name_datareader_profile();
		std::string get_name_topic_config_profile();
		std::string get_name_datareader_config_profile();

		DdsStatus broadcast_command(DdsCommand& cmd) override;


	public:

		UnitAid_DDS(std::shared_ptr<IConfigUnitAid> config);
		~UnitAid_DDS();
		ResultReqest RespondStatus(DdsStatus& status) override;
		ResultReqest TakeServiceConfig(size_t size_data, std::string& str) override;
		StatusUnitAid GetStatus() override;
		ResultReqest InitUnitAid() override;
	};
}
