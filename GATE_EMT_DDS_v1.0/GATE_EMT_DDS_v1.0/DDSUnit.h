#pragma once
#include "structs.h"
#include "SM_DDS.h"
#include "KKS_Reader.h"
#include <atomic>
#include <vector>
#include <logger.h>
#include <thread>#
#include <chrono>

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/DataWriterListener.hpp>
#include <fastdds/dds/publisher/qos/DataWriterQos.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastrtps/types/DynamicType.h>
#include <fastrtps/types/DynamicData.h>
#include <fastrtps/types/DynamicDataFactory.h>
#include <fastrtps/types/DynamicTypeBuilderFactory.h>
#include <fastrtps/types/DynamicTypeBuilderPtr.h>
#include <fastrtps/types/DynamicTypeBuilder.h>
#include <fastrtps/attributes/ParticipantAttributes.h>
#include <fastrtps/attributes/SubscriberAttributes.h>
#include <fastrtps/Domain.h>


using namespace eprosima::fastdds::dds;
using namespace eprosima::fastrtps::types;
using namespace eprosima::fastrtps;
using namespace eprosima::fastrtps::rtps;
using namespace std::chrono_literals;

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
	virtual TypeDDSUnit MyType() = 0;
};


class DDSUnit_Subscriber : public DDSUnit
{
	ConfigDDSUnit config;
	SharedMemoryDDS* SharedMemoryUnit;
	KKSReader* readerkks;
	std::atomic<StatusDDSUnit> GlobalStatus = StatusDDSUnit::EMPTY;
	LoggerSpace::Logger* log;

	DomainParticipant* participant_;
	eprosima::fastdds::dds::Subscriber* subscriber_;
	Topic* topic_analog;
	Topic* topic_discrete;
	Topic* topic_binar;
	TypeSupport type_;

	DynamicData_ptr data_analog;
	DynamicData_ptr data_discrete;
	DynamicData_ptr data_binar;

	DynamicType_ptr base_type_array_analog;
	DynamicType_ptr base_type_array_discrete;
	DynamicType_ptr base_type_array_binar;

	eprosima::fastrtps::types::DynamicPubSubType m_DynType_analog;
	eprosima::fastrtps::types::DynamicPubSubType m_DynType_discrete;
	eprosima::fastrtps::types::DynamicPubSubType m_DynType_binar;

	std::atomic<int> control_analog_thread = 0;
	std::atomic<int> control_discrete_thread = 0;
	std::atomic<int> control_binar_thread = 0;

	void thread_transmite(TypeData type_data_thread);


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
	TypeDDSUnit MyType();
};


class DDSUnit_Publisher : public DDSUnit
{
	ConfigDDSUnit config;
	SharedMemoryDDS* SharedMemoryUnit;
	KKSReader* readerkks;
	std::atomic<StatusDDSUnit> GlobalStatus = StatusDDSUnit::EMPTY;
	LoggerSpace::Logger* log;

	DomainParticipant* participant_;
	eprosima::fastdds::dds::Publisher* publisher_;

	Topic* topic_analog;
	Topic* topic_discrete;
	Topic* topic_binar;
	TypeSupport type_;

	DynamicData_ptr data_analog;
	DynamicData_ptr data_discrete;
	DynamicData_ptr data_binar;

	eprosima::fastrtps::types::DynamicPubSubType m_DynType_analog;
	eprosima::fastrtps::types::DynamicPubSubType m_DynType_discrete;
	eprosima::fastrtps::types::DynamicPubSubType m_DynType_binar;

	DynamicType_ptr base_type_array_analog;
	DynamicType_ptr base_type_array_discrete;
	DynamicType_ptr base_type_array_binar;

	std::atomic<int> control_analog_thread = 0;
	std::atomic<int> control_discrete_thread = 0;
	std::atomic<int> control_binar_thread = 0;

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
	TypeDDSUnit MyType();
};

DDSUnit* CreateDDSUnit(TypeDDSUnit type, ConfigDDSUnit config)
{
	DDSUnit* p = NULL;
	switch (type)
	{
	case TypeDDSUnit::SUBSCRIBER:
		p = new DDSUnit_Subscriber(config);
		break;
	case TypeDDSUnit::PUBLISHER:
		p = new DDSUnit_Publisher(config);
		break;
	default:
		p = NULL;
	}
	return p;
}