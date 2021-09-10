#pragma once
#include "structs.h"
#include "SM_DDS.h"
#include "KKS_Reader.h"
#include <atomic>
#include <vector>
#include <logger.h>

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>
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


class DDSUnit_Subcriber : public DDSUnit
{
	ConfigDDSUnit config;
	SharedMemoryDDS* SharedMemoryUnit;
	KKSReader* readerkks;
	std::atomic<StatusDDSUnit> GlobalStatus = StatusDDSUnit::EMPTY;
	LoggerSpace::Logger* log;

	eprosima::fastrtps::Participant* participant_;
	eprosima::fastrtps::Subscriber* subscriber_;
	DataReader* reader_analog;
	DataReader* reader_discrete;
	DataReader* reader_binar;
	Topic * topic_;
	TypeSupport type_;

	DynamicData_ptr data_analog;
	DynamicData_ptr data_discrete;
	DynamicData_ptr data_binar;

	eprosima::fastrtps::types::DynamicPubSubType m_DynType_analog;
	eprosima::fastrtps::types::DynamicPubSubType m_DynType_discrete;
	eprosima::fastrtps::types::DynamicPubSubType m_DynType_binar;


public:

	DDSUnit_Subcriber(ConfigDDSUnit config);
	~DDSUnit_Subcriber();

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
	eprosima::fastrtps::Subscriber* subscriber_;
	DataReader* reader_analog;
	DataReader* reader_discrete;
	DataReader* reader_binar;
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
		p = new DDSUnit_Subcriber(config);
		break;
	case TypeDDSUnit::PUBLISHER:
		p = new DDSUnit_Publisher(config);
		break;
	default:
		p = NULL;
	}
	return p;
}