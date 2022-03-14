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
#include "DataDds.h"
#include "DataDdsPubSubTypes.h"

#include "DDSData.h"
#include "DDSDataPubSubTypes.h"


using namespace eprosima::fastdds::dds;
using namespace eprosima::fastrtps::types;
using namespace std::chrono_literals;

unsigned int size { 100000 };

static int num = 10000;

class CustomDataReaderListener : public DataReaderListener
{
	std::shared_ptr<DataDDS> data = std::make_shared<DataDDS>();

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
		reader->take_next_sample(data.get(), &info);
		
		{
			std::vector<int>& vec_i = data->vec_datai();
			std::vector<float>& vec_f = data->vec_dataf();

			std::cout << "size int: " << vec_i.size() << std::endl;
			std::cout << "vectorI[first]: " << *vec_i.begin() << std::endl;
			std::cout << "vectorI[last]: " << *vec_i.rbegin() << std::endl;
			std::cout << "size int: " << vec_f.size() << std::endl;
			std::cout << "vectorF[first]: " << *vec_f.begin() << std::endl;
			std::cout << "vectorF[last]: " << *vec_f.rbegin() << std::endl << std::endl;
		}

	
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

void test_pub()
{
	
	DomainParticipant* participant_pub;
	participant_pub = DomainParticipantFactory::get_instance()->create_participant(0, PARTICIPANT_QOS_DEFAULT);

	Publisher* publisher_ = participant_pub->create_publisher(PUBLISHER_QOS_DEFAULT, nullptr);

	TypeSupport PtrSupporType = TypeSupport(new DataDDSPubSubType());
	participant_pub->register_type(PtrSupporType);
	std::shared_ptr<DataDDS> plc_ = std::make_shared<DataDDS>();
	Topic* topic_pub = participant_pub->create_topic("test_1", PtrSupporType.get_type_name(), TOPIC_QOS_DEFAULT, nullptr);

	DataWriter* writer_ = publisher_->create_datawriter(topic_pub, DATAWRITER_QOS_DEFAULT, nullptr);

	int count = 0;

	std::vector<std::pair<int, int>> test{ {100,100}, {50000,40000}, {60000,60000}, {65000,65000}, {65400,65400}, {67000,67000}, {70000,70000}, {80000,80000}, {90000,90000} , {100000,100000} };
	int num_test = 0;
	int count_cycle = 0;

	plc_->vec_datai().resize(test[num_test].first);
	plc_->vec_dataf().resize(test[num_test].second);


	for (;;)
	{	
		{
			std::vector<int>& vec_i = plc_->vec_datai();
			for (int i = 0; i < vec_i.size(); i++)
			{
				vec_i[i] = count + i;
			}

			std::vector<float>& vec_f = plc_->vec_dataf();
			for (int i = 0; i < vec_f.size(); i++)
			{
				vec_f[i] = (count + i)*0.1;
			}
		}

		std::cout << "result write: " << writer_->write(plc_.get()) << std::endl;
		count += 1;
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		count_cycle++;
		if (count_cycle > 5)
		{
			count_cycle = 0;
			num_test++;
			if (num_test >= test.size()) num_test = 0;
			plc_->vec_datai().resize(test[num_test].first);
			plc_->vec_dataf().resize(test[num_test].second);
		}
	}

}



void test_sub()
{
	CustomDataReaderListener* listener_ = new CustomDataReaderListener();
	DomainParticipant* participant_sub;
	participant_sub = DomainParticipantFactory::get_instance()->create_participant(0, PARTICIPANT_QOS_DEFAULT);

	Subscriber* subcriber_ = participant_sub->create_subscriber(SUBSCRIBER_QOS_DEFAULT, nullptr);

	TypeSupport PtrSupporType = TypeSupport(new DataDDSPubSubType());
	participant_sub->register_type(PtrSupporType);

	Topic* topic_sub = participant_sub->create_topic("test_1", PtrSupporType.get_type_name(), TOPIC_QOS_DEFAULT, nullptr);

	DataReader* reader_ = subcriber_->create_datareader(topic_sub, DATAREADER_QOS_DEFAULT, listener_);

	int count = 0;
	SampleInfo info;
	for (;;)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

}

int main(int argc, char** argv)
{
	std::string pub = "pub";
	std::string sub = "sub";

	setsizeint(500000);
	setsizefloat(500000);
	std::cout << DataDDS::getMaxCdrSerializedSize() << std::endl;

	if (argc < 2) return 0;

	if (pub.compare(argv[1]) == 0)
	{
		test_pub();
	}

	if (sub.compare(argv[1]) == 0)
	{
		test_sub();
	}

	return 0;
}

