// Copyright 2019 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @file HelloWorld_main.cpp
 *
 */

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
#include "HelloWorld.h"
#include "HelloWorldPubSubTypes.h"


using namespace eprosima::fastdds::dds;
using namespace eprosima::fastrtps::types;
using namespace std::chrono_literals;

unsigned int size{ 100000 };

static int num = 10000;

class CustomDataReaderListener : public DataReaderListener
{
	HelloWorld hy;
public:

	CustomDataReaderListener()
		: DataReaderListener()
	{

	}

	virtual ~CustomDataReaderListener()
	{
	}

	virtual void on_data_available(
		DataReader* reader)
	{
		SampleInfo info;
		std::cout << "Received new data message" << std::endl;
		reader->take_next_sample(&hy, &info);

		std::cout << hy.message() << std::endl;
	}

	virtual void on_subscription_matched(
		DataReader* reader,
		const SubscriptionMatchedStatus& info)
	{
		(void)reader;
		if (info.current_count_change == 1)
		{
			std::cout << "Matched a remote DataWriter" << std::endl;
		}
		else if (info.current_count_change == -1)
		{
			std::cout << "Unmatched a remote DataWriter" << std::endl;
		}
	}

	virtual void on_requested_deadline_missed(
		DataReader* reader,
		const eprosima::fastrtps::RequestedDeadlineMissedStatus& info)
	{
		(void)reader, (void)info;
		std::cout << "Some data was not received on time" << std::endl;
	}

	virtual void on_liveliness_changed(
		DataReader* reader,
		const eprosima::fastrtps::LivelinessChangedStatus& info)
	{
		(void)reader;
		if (info.alive_count_change == 1)
		{
			std::cout << "A matched DataWriter has become active" << std::endl;
		}
		else if (info.not_alive_count_change == 1)
		{
			std::cout << "A matched DataWriter has become inactive" << std::endl;
		}
	}

	virtual void on_sample_rejected(
		DataReader* reader,
		const eprosima::fastrtps::SampleRejectedStatus& info)
	{
		(void)reader, (void)info;
		std::cout << "A received data sample was rejected" << std::endl;
	}

	virtual void on_requested_incompatible_qos(
		DataReader* /*reader*/,
		const RequestedIncompatibleQosStatus& info)
	{
		std::cout << "Found a remote Topic with incompatible QoS (QoS ID: " << info.last_policy_id <<
			")" << std::endl;
	}

	virtual void on_sample_lost(
		DataReader* reader,
		const SampleLostStatus& info)
	{
		(void)reader, (void)info;
		std::cout << "A data sample was lost and will not be received" << std::endl;
	}



};

int main(int argc, char** argv)
{

	if (argc != 2) return 0;
	HelloWorld hy;
	std::string str{argv[1]};
	std::string str1;
	std::string str2;
	if (str == "1")
	{
		str1 = "2";
		str2 = "3";
	}
	else if (str == "2")
	{
		str1 = "1";
		str2 = "3";
	}
	else if (str == "3")
	{
		str1 = "1";
		str2 = "2";
	}

	std::string file_name = "my" + str + ".xml";
	DomainParticipant* participant;
	if (DomainParticipantFactory::get_instance()->load_XML_profiles_file(file_name) == ReturnCode_t::RETCODE_OK)
	{
		std::cout << "registration profile done" << std::endl;
	}

	//participant = DomainParticipantFactory::get_instance()->create_participant_with_profile("TCP1Participant");
	participant = DomainParticipantFactory::get_instance()->create_participant( 0, PARTICIPANT_QOS_DEFAULT);
	if (!participant)
	{
		std::cout << "create participant done" << std::endl;
	}

	TypeSupport PtrSupporType = TypeSupport(new HelloWorldPubSubType());
	participant->register_type(PtrSupporType);

	Publisher* publisher = participant->create_publisher(PUBLISHER_QOS_DEFAULT, nullptr);
	Subscriber* subscriber = participant->create_subscriber(SUBSCRIBER_QOS_DEFAULT, nullptr);



	Topic* topic_pub = participant->create_topic("HW" + str, PtrSupporType.get_type_name(), TOPIC_QOS_DEFAULT, nullptr);
	Topic* topic_sub1 = participant->create_topic("HW" + str1, PtrSupporType.get_type_name(), TOPIC_QOS_DEFAULT, nullptr);
	Topic* topic_sub2 = participant->create_topic("HW" + str2, PtrSupporType.get_type_name(), TOPIC_QOS_DEFAULT, nullptr);

	CustomDataReaderListener listener;

	DataReader* reader1 = subscriber->create_datareader(topic_sub1, DATAREADER_QOS_DEFAULT, &listener);
	DataReader* reader2 = subscriber->create_datareader(topic_sub2, DATAREADER_QOS_DEFAULT, &listener);
	DataWriter* writer_ = publisher->create_datawriter(topic_pub, DATAWRITER_QOS_DEFAULT, nullptr);

	int count = 0;

	for (;;)
	{
		hy.message() = "HellowWorld-" + str + ": " + std::to_string(count);
		writer_->write(&hy);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		count++;
	}

	return 0;
}
