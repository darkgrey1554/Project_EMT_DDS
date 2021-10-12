#pragma once
#include "structs.h"
#include "Adapters.h"
#include <atomic>
#include <vector>
#include <logger.h>
#include <thread>
#include <chrono>

//#include <fastrtps/xmlparser/XMLProfileManager.h>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
//#include <fastdds/dds/subscriber/DataReaderListener.hpp>
//#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
//#include <fastdds/dds/publisher/DataWriterListener.hpp>
//#include <fastdds/dds/publisher/qos/DataWriterQos.hpp>
//#include <fastdds/dds/subscriber/SampleInfo.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastrtps/types/DynamicType.h>
#include <fastrtps/types/DynamicData.h>
#include <fastrtps/types/DynamicDataFactory.h>
#include <fastrtps/types/DynamicTypeBuilderFactory.h>
#include <fastrtps/types/DynamicTypeBuilderPtr.h>
#include <fastrtps/types/DynamicTypeBuilder.h>
//#include <fastrtps/attributes/ParticipantAttributes.h>
//#include <fastrtps/attributes/SubscriberAttributes.h>
//#include <fastrtps/Domain.h>


using namespace eprosima::fastdds::dds;
using namespace eprosima::fastrtps::types;
//using namespace eprosima::fastrtps;
//using namespace eprosima::fastrtps::rtps;
using namespace std::chrono_literals;

namespace gate
{
	class DDSUnit
	{

	public:

		virtual ResultReqest Stop() = 0;
		virtual ResultReqest Start() = 0;
		virtual StatusDDSUnit GetCurrentStatus() = 0;
		virtual ConfigDDSUnit GetConfig() = 0;
		virtual ResultReqest SetConfig() = 0;
		virtual ResultReqest Restart() = 0;
		virtual void Delete() = 0;
		virtual TypeDDSUnit GetType() = 0;
		virtual ~DDSUnit() {};
	};

	std::shared_ptr<DDSUnit> CreateDDSUnit(ConfigDDSUnit config);

	class DDSUnit_Subscriber : public DDSUnit
	{
		ConfigDDSUnit config;
		gate::Adapter* Adapter;

		std::atomic<StatusDDSUnit> GlobalStatus = StatusDDSUnit::EMPTY;
		LoggerSpace::Logger* log;

		std::shared_ptr<DomainParticipant> participant_ = nullptr;
		std::shared_ptr<eprosima::fastdds::dds::Subscriber> subscriber_ = nullptr;
		std::shared_ptr <Topic> topic_data;
		TypeSupport type_;
		DynamicData_ptr type_data;
		DynamicType_ptr base_type_data;

		std::shared_ptr<eprosima::fastdds::dds::DataReader> readerr;

		void thread_transmite(TypeData type_data_thread);
		void SetStatus(StatusDDSUnit status);

		class SubListener : public DataReaderListener
		{
		public:

			SubListener() : samples_(0) {};
			~SubListener() override {};
			void on_subscription_matched(DataReader*, const SubscriptionMatchedStatus& info) override;
			void on_data_available(DataReader* reader) override;
			std::atomic_int samples_;
		} listener_;

	public:

		DDSUnit_Subscriber(ConfigDDSUnit config);
		~DDSUnit_Subscriber();

		ResultReqest Stop();
		ResultReqest Start();
		StatusDDSUnit GetCurrentStatus();
		ConfigDDSUnit GetConfig();
		ResultReqest SetConfig();
		ResultReqest Restart();
		void Delete();
		TypeDDSUnit GetType();

	};

	class DDSUnit_Publisher : public DDSUnit
	{
		ConfigDDSUnit config;
		gate::Adapter* Adapter;

		std::atomic<StatusDDSUnit> GlobalStatus = StatusDDSUnit::EMPTY;
		LoggerSpace::Logger* log;

		DomainParticipant* participant_;
		eprosima::fastdds::dds::Publisher* publisher_;

		Topic* topic_data;
		TypeSupport type_;
		DynamicData_ptr type_data;
		DynamicType_ptr base_type_data;

		DataWriter* writerr = nullptr;

		void thread_transmite(TypeData type_data_thread);

	public:

		DDSUnit_Publisher(ConfigDDSUnit config);
		~DDSUnit_Publisher();

		ResultReqest Stop();
		ResultReqest Start();
		StatusDDSUnit GetCurrentStatus();
		ConfigDDSUnit GetConfig();
		ResultReqest SetConfig();
		ResultReqest Restart();
		void Delete();
		TypeDDSUnit GetType();
	};
}



