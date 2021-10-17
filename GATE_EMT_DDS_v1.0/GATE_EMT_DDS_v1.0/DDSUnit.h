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

	protected:

		const unsigned int frequency_scatter = 25; // 25 ms 

	public:

		virtual ResultReqest Stop() = 0;
		virtual ResultReqest Start() = 0;
		virtual StatusDDSUnit GetCurrentStatus() const = 0;
		virtual ConfigDDSUnit GetConfig() const = 0;
		virtual ResultReqest SetConfig() = 0;
		virtual ResultReqest Restart() = 0;
		virtual void Delete() = 0;
		virtual TypeDDSUnit GetType() const = 0;
		virtual ~DDSUnit() {};
	};

	std::shared_ptr<DDSUnit> CreateDDSUnit(ConfigDDSUnit config);

	class DDSUnit_Subscriber : public DDSUnit
	{
	private:

		ConfigDDSUnit config;
		std::shared_ptr<gate::Adapter> AdapterUnit;

		std::atomic<StatusDDSUnit> GlobalStatus = StatusDDSUnit::EMPTY;
		LoggerSpace::Logger* log;
		std::atomic <ControlThreadDSSUnit> control_thread = ControlThreadDSSUnit::NONE;

		DomainParticipant* participant_ = nullptr;
		eprosima::fastdds::dds::Subscriber* subscriber_ = nullptr;
		Topic* topic_data;
		DynamicData_ptr data;
		DynamicType_ptr type_data;

		eprosima::fastdds::dds::DataReader* reader_data = nullptr;
		std::jthread thread_transmite;
		


		class SubListener : public DataReaderListener
		{
		public:

			SubListener() : samples_(0) {};
			~SubListener() override {};
			void on_subscription_matched(DataReader*, const SubscriptionMatchedStatus& info) override;
			void on_data_available(DataReader* reader) override;
			std::atomic_int samples_;
		} listener_;

		void function_thread_transmite(std::stop_token stop_token);
		void SetStatus(StatusDDSUnit status);
		std::string CreateNameTopic(std::string short_name);
		std::string CreateNameType(std::string short_name);

		/// --- функция инициализации participant --- ///
		ResultReqest init_participant();

		/// --- функция инициализации subscriber --- ///
		ResultReqest init_subscriber();

		/// --- функция инициализации DynamicDataType --- ///
		ResultReqest create_dynamic_data_type();

		/// --- функция регистрации типа --- ///
		ResultReqest register_type();

		/// --- функция создания топика --- ///
		ResultReqest register_topic();

		/// --- функция создания reader --- ///
		ResultReqest init_reader_data();

		/// --- функция создания топика --- ///
		ResultReqest init_adapter();

		std::shared_ptr<ConfigAdapter> create_config_adapter();

		std::shared_ptr<void>  create_buffer();

	public:

		DDSUnit_Subscriber(ConfigDDSUnit config);
		~DDSUnit_Subscriber();

		ResultReqest Stop();
		ResultReqest Start();
		StatusDDSUnit GetCurrentStatus() const;
		ConfigDDSUnit GetConfig() const;
		ResultReqest SetConfig();
		ResultReqest Restart();
		void Delete();
		TypeDDSUnit GetType() const;

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



