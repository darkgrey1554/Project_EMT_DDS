#pragma once

#include "structs.h"
#include "Config_Reader.h"
#include "DDSUnit.h"
#include <vector>

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastrtps/types/DynamicType.h>
#include <fastrtps/types/DynamicData.h>
#include <fastrtps/types/DynamicDataFactory.h>
#include <fastrtps/types/DynamicTypeBuilderFactory.h>
#include <fastrtps/types/DynamicTypeBuilderPtr.h>
#include <fastrtps/types/DynamicTypeBuilder.h>

using namespace eprosima::fastdds::dds;
using namespace eprosima::fastrtps::types;
//using namespace eprosima::fastrtps;
//using namespace eprosima::fastrtps::rtps;
using namespace std::chrono_literals;

class Gate;

Gate* CreateGate(Type_Gate type);

class Gate
{

public:

	virtual ~Gate() = default;
	virtual ResultReqest init_gate() = 0;
	virtual void GetInfoAboutGate() = 0;
	virtual void GetStatusGate() = 0;
	virtual void StartGate() = 0;
	virtual void StopGate() = 0;
	virtual void RestartGate() = 0;
	virtual void RebornGate() = 0;
};


class DDS_Gate : public Gate
{

	LoggerSpace::Logger* log;
	ConfigReaderDDS* config_reader;
	ConfigGate config;
	ConfigDDSUnit config_units;


	
	DomainParticipant* participant_;
	eprosima::fastdds::dds::Subscriber* subscriber_;
	eprosima::fastdds::dds::Publisher* publisher_;
	DataReader* reader_command;
	Topic* topic_command;
	Topic* topic_config_units;
	Topic* topic_answer;

	std::vector<DDSUnit> units_transfer;

	void TakeConfigUnits();
	void WriteConfigUnitsInFile();
	void ParseCommand();
	void AnswerCommand();


    class SubListener : public DataReaderListener
    {
    public:
        SubListener();
		~SubListener();
		void on_subscription_matched(DataReader*, const SubscriptionMatchedStatus& info) override;
		void on_data_available(DataReader* reader) override;

    } listener_;

public :

	DDS_Gate();
	~DDS_Gate();
	ResultReqest init_gate();
	void GetInfoAboutGate();
	void GetStatusGate();
	void StartGate();
	void StopGate();
	void RestartGate();
	void RebornGate();
};


