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


using namespace eprosima::fastdds::dds;
using namespace eprosima::fastrtps::types;
using namespace std::chrono_literals;

const int num = 100000;

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
		std::cout << "Received new data" << std::endl;
		reader->take_next_sample(data.get(), &info);

		std::vector<int>& vec_i = data->vec_datai();
		std::cout << "size vector int: " << vec_i.size() << std::endl;
		std::cout << "data_vector[first] = " << *vec_i.begin() << std::endl;
		std::cout << "data_vector[last] = " << *vec_i.rbegin() << std::endl;
		//std::vector<float>& vec_f = data->vec_dataf();
		//std::cout << "size vector int: " << vec_f.size() << std::endl;
		//std::cout << "data_vector[first] = " << *vec_f.begin() << std::endl;
		//std::cout << "data_vector[last] = " << *vec_f.rbegin() << std::endl << std::endl;
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
	std::shared_ptr<DataDDS> plc_ = std::make_shared<DataDDS>();

	PtrSupporType.get()->auto_fill_type_information(false);
	PtrSupporType.get()->auto_fill_type_object(true);
	participant_pub->register_type(PtrSupporType);

	Topic* topic_pub = participant_pub->create_topic("test_1", PtrSupporType.get_type_name(), TOPIC_QOS_DEFAULT, nullptr);

	DataWriter* writer_ = publisher_->create_datawriter(topic_pub, DATAWRITER_QOS_DEFAULT, nullptr);

	int count = 0;
	int iter = 0;
	

	std::vector<uint32_t> test{ 100, 200, 500, 1000, 3000, 5000, 10000, 20000, 30000 };
	int num_test = test.size();
	int count_test = 0;
	int number_cycle = 20;
	int count_cycle = 0;
	int time_cycle = 500;
	plc_->vec_datai().resize(test[0]);
	//plc_->vec_dataf().resize(test[0]);
	for (;;)
	{	
		std::vector<int>& vec_i = plc_->vec_datai();
		//std::vector<float>& vec_f = plc_->vec_dataf();
		iter = 0;
		for (auto i = vec_i.begin(); i<vec_i.end(); i++)
		{
			*i = count + iter;
			iter++;
		}

		/*for (auto i = vec_f.begin(); i < vec_f.end(); i++)
		{
			*i = count + iter*0.1;
			iter++;
		}*/

		std::cout << "result transfer: " << writer_->write(plc_.get()) << std::endl;
		std::cout << "data_vector_i[first] = " << *vec_i.begin() << std::endl;
		std::cout << "data_vector_i[last] = " << *vec_i.rbegin() << std::endl << std::endl;
		//std::cout << "data_vector_f[first] = " << *vec_f.begin() << std::endl;
		//std::cout << "data_vector_f[last] = " << *vec_f.rbegin() << std::endl << std::endl;
		count++;
		std::this_thread::sleep_for(std::chrono::milliseconds(time_cycle));
		count_cycle++;
		if (count_cycle > number_cycle)
		{
			count_cycle = 0;
			count_test++;
			if (count_test > test.size() - 1) count_test = 0;
			plc_->vec_datai().resize(test[count_test]);
			//plc_->vec_dataf().resize(test[count_test]);
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
	std::shared_ptr<DataDDS> plc_ = nullptr;

	PtrSupporType.get()->auto_fill_type_information(false);
	PtrSupporType.get()->auto_fill_type_object(true);
	participant_sub->register_type(PtrSupporType);

	Topic* topic_sub = participant_sub->create_topic("test_1", PtrSupporType.get_type_name(), TOPIC_QOS_DEFAULT, nullptr);

	DataReader* reader_ = subcriber_->create_datareader(topic_sub, DATAREADER_QOS_DEFAULT, listener_);

	int count = 0;
	SampleInfo info;
	for (;;)
	{
		/*reader_->take_next_sample(plc_.get(), &info);
		if (plc_ == nullptr) continue;
		std::cout << "value = "<< plc_->sensor_array()[10].v() << std::endl;*/
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

}

int main(int argc, char** argv)
{
	//DataDDS::SetMaxSizeF(100000);
	//DataDDS::SetMaxSizeI(100000);

	std::string pub = "pub";
	std::string sub = "sub";

	if (pub.compare(argv[1]) == 0)
	{
		//if (argc < 2) return 0;
		//test_pub(std::atoi(argv[2]));
		test_pub();
	}
	else if (sub.compare(argv[1]) == 0)
	{
		test_sub();
	}

	return 0;
}

