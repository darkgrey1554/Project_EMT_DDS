#pragma once
#include <structs/structs.hpp>

#include <structs/TimeConverter.hpp>
#include <LoggerScada.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <ddsformat/TypeTopics.h>

namespace atech::srv::io
{
	enum class TypeTopic
	{
		DDSData,
		DDSDataEx,
		DDSAlarm,
		DDSAlarmEx,
		DDSConfig,
		DDSCommand,
		DDSStatus
	};


	class FactoryDDS
	{
		static std::shared_ptr<FactoryDDS> p_instance;
		static std::mutex singlton;
		std::mutex guarden;
		
		LoggerSpaceScada::ILoggerScada_ptr log;
		eprosima::fastdds::dds::DomainParticipant* _participant = nullptr;
		eprosima::fastdds::dds::Publisher* _publisher = nullptr;
		eprosima::fastdds::dds::Subscriber* _subscriber = nullptr;
		std::map<std::string, eprosima::fastdds::dds::Topic*> map_topic;
		std::map<TypeTopic, std::string> type_name;
		uint32_t _node_id = 0;

		ResultReqest init_participant();
		ResultReqest init_subscriber();
		ResultReqest init_publisher();
		ResultReqest registration_type_dds();


		std::string get_name_participant_profile();
		std::string get_name_subscriber_profile();
		std::string get_name_publisher_profile();
		std::string get_name_topic_profile(const std::string& name);
		std::string get_name_datawriter_profile(const std::string& name_topic);
		std::string get_name_datareader_profile(const std::string& name_topic);

		public:

		FactoryDDS(const FactoryDDS&) = delete;
		FactoryDDS(FactoryDDS&&) = delete;
		FactoryDDS& operator=(const FactoryDDS&) = delete;
		FactoryDDS& operator=(FactoryDDS&&) = delete;

		FactoryDDS();
		~FactoryDDS();

		ResultReqest registration_topic(const std::string& name,const TypeTopic& type_topic);
		ResultReqest unregistration_topic(std::string name);
		ResultReqest unregistration_type_config();
		ResultReqest registration_type_config();

		eprosima::fastdds::dds::DataWriter* get_datawriter(std::string name_topic);
		eprosima::fastdds::dds::DataReader* get_datareader(std::string name_topic);
		ResultReqest delete_datawriter(eprosima::fastdds::dds::DataWriter* writer);
		ResultReqest delete_datareader(eprosima::fastdds::dds::DataReader* reader);
		ResultReqest delete_dds();
		ResultReqest install_config_xml(const std::string& str_xml);
		ResultReqest init_dds();
		void Set_NodeId(const uint32_t& node_id);
		static std::shared_ptr<FactoryDDS> get_instance();
	};

}

