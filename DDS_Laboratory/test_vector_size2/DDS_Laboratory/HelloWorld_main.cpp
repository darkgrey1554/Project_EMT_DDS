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
#include "DDSData.h"
#include "DDSDataPubSubTypes.h"
#include "TimeConverter.hpp"

#include "DDSData.h"
#include "DDSDataPubSubTypes.h"

#include "DDSDataEx.h"
#include "DDSDataExPubSubTypes.h"

#include <fastdds/rtps/transport/TCPv4TransportDescriptor.h>
#include <fastrtps/utils/IPLocator.h>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>

#include <fstream>;

using namespace eprosima::fastdds::dds;
using namespace eprosima::fastrtps::types;
using namespace std::chrono_literals;

unsigned int size { 100000 };

int num = 10000;
int count_points = 3000;


class CustomDataReaderListener : public DataReaderListener
{
	std::shared_ptr<DDSData> data = std::make_shared<DDSData>();
	std::vector<long long> time_reg;
	int count_test = 0;
	int count_ = 0;
	long long t = 0;
	int last_size_ = 0;

public:

	CustomDataReaderListener()
		: DataReaderListener()
	{
		time_reg.resize(count_points);
	}

	virtual ~CustomDataReaderListener()
	{
	}

	virtual void on_data_available(
		DataReader* reader)
	{
		SampleInfo info;
		int size_ = 0;
		reader->take_next_sample(data.get(), &info);
		t = TimeConverter::GetTime_LLmcs();

		size_ = data->data_int().value().size()*2;
		if (last_size_ != size_)
		{
			count_ = 0;
			last_size_ = size_;
			std::cout << " TEST size = " << size_ << std::endl;
			return;
		}

		if (count_ % (count_points/100) == 0)
		{
			std::cout << count_ / (count_points/100) << "%" << std::endl;
		}

		if (count_ < count_points)
		{
			time_reg[count_] = t - data->time_service();
			count_++;
		}


		if (count_ == count_points)
		{
			std::cout << 100 << "%" << std::endl;
			std::cout << "Write " << std::endl;
			std::fstream file;
			file.open("test_"+ std::to_string(count_test) +"_timetransmite_" + std::to_string(data->data_int().value().size()*2) + ".txt", std::ios_base::out);
			for (int i = 0; i < time_reg.size(); i++)
			{
				file << time_reg[i] << "\n";
			}
			count_++;
			count_test++;
			file.close();
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

		count_ = 0;
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

	DomainParticipantQos qos = PARTICIPANT_QOS_DEFAULT;

	qos.transport().send_socket_buffer_size = 10000000;
	qos.transport().listen_socket_buffer_size = 10000000;

	auto tcp_transport = std::make_shared<eprosima::fastdds::rtps::TCPv4TransportDescriptor>();
	tcp_transport->sendBufferSize = 10000000;
	tcp_transport->receiveBufferSize = 10000000;
	tcp_transport->add_listener_port(40000);
	tcp_transport->set_WAN_address("192.168.0.174");

	qos.transport().user_transports.push_back(tcp_transport);
	qos.transport().use_builtin_transports = false;


	
	DomainParticipant* participant_pub;
	participant_pub = DomainParticipantFactory::get_instance()->create_participant(0, qos);

	Publisher* publisher_ = participant_pub->create_publisher(PUBLISHER_QOS_DEFAULT, nullptr);

	TypeSupport PtrSupporType = TypeSupport(new DDSDataPubSubType());
	participant_pub->register_type(PtrSupporType);
	std::shared_ptr<DDSData> plc_ = std::make_shared<DDSData>();
	Topic* topic_pub = participant_pub->create_topic("test_1", PtrSupporType.get_type_name(), TOPIC_QOS_DEFAULT, nullptr);

	DataWriter* writer_ = publisher_->create_datawriter(topic_pub, DATAWRITER_QOS_DEFAULT, nullptr);

	int count = 0;

	plc_->data_int().value().resize(num/2);
	plc_->data_int().quality().resize(num / 2);
	plc_->data_float().value().resize(num / 2);
	plc_->data_float().quality().resize(num / 2);

	long long time = TimeConverter::GetTime_LLmcs();
	long long end = TimeConverter::GetTime_LLmcs();

	std::vector<int> test_{ 100,200,300,400,500,600,700,800,900,
						   1000,2000,3000,4000,5000,6000,7000,8000,9000,
						   10000,20000,30000,40000,50000,60000,70000,80000,90000,
						   100000,200000,300000,400000,500000,600000,700000,800000,900000,1000000, 100, 200 };

	long long t;

	for (int i=0; i< test_.size();i++)
	{	
		plc_->data_int().value().resize(test_[i]/2);
		plc_->data_int().quality().resize(test_[i] / 2);
		plc_->data_float().value().resize(test_[i] / 2);
		plc_->data_float().quality().resize(test_[i] / 2);
		count = 0;

		std::cout << "START TEST " << test_[i] << std::endl;
		for (;;)
		{
			end = TimeConverter::GetTime_LLmcs();
			if ((end - time) / 1000 < 80)
			{
				std::this_thread::sleep_for(1ms);
				continue;
			}
			time = end;

			plc_->time_service() = TimeConverter::GetTime_LLmcs();

			writer_->write(plc_.get());
			count += 1;
			if (count > 4000) break;
		}
	}
	while (1);

}



void test_sub()
{

	DomainParticipantQos qos = PARTICIPANT_QOS_DEFAULT;

	qos.transport().send_socket_buffer_size = 1000000;
	qos.transport().listen_socket_buffer_size = 1000000;

	/// --- инициализация политик --- ///
	///--------------------------------------------///
	//////////////////////////////////////////////////

	qos.transport().use_builtin_transports = false;

	auto tcp_transport = std::make_shared<eprosima::fastdds::rtps::TCPv4TransportDescriptor>();
	tcp_transport->sendBufferSize = 1000000;
	tcp_transport->receiveBufferSize = 1000000;

	qos.transport().user_transports.push_back(tcp_transport);

	eprosima::fastrtps::rtps::Locator_t initial_peer_locator;
	initial_peer_locator.kind = LOCATOR_KIND_TCPv4;
	eprosima::fastrtps::rtps::IPLocator::setIPv4(initial_peer_locator, "192.168.0.174");
	initial_peer_locator.port = 40000;

	qos.wire_protocol().builtin.initialPeersList.push_back(initial_peer_locator);
	qos.transport().use_builtin_transports = false;


	CustomDataReaderListener* listener_ = new CustomDataReaderListener();
	DomainParticipant* participant_sub;
	participant_sub = DomainParticipantFactory::get_instance()->create_participant(0, qos);

	Subscriber* subcriber_ = participant_sub->create_subscriber(SUBSCRIBER_QOS_DEFAULT, nullptr);

	TypeSupport PtrSupporType = TypeSupport(new DDSDataPubSubType());
	participant_sub->register_type(PtrSupporType);

	Topic* topic_sub = participant_sub->create_topic("test_1", PtrSupporType.get_type_name(), TOPIC_QOS_DEFAULT, nullptr);

	DataReader* reader_ = subcriber_->create_datareader(topic_sub, DATAREADER_QOS_DEFAULT, listener_);

	for (;;)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

}

int main(int argc, char** argv)
{
	std::string pub = "pub";
	std::string sub = "sub";



	if (argc < 3) return 0;

	num = std::atoi(argv[2]);

	scada_ate::typetopics::SetMaxSizeDataCollectionInt(10000);
	scada_ate::typetopics::SetMaxSizeDataCollectionFloat(10000);
	scada_ate::typetopics::SetMaxSizeDataCollectionInt(10000);
	scada_ate::typetopics::SetMaxSizeDataCollectionDouble(10000);
	scada_ate::typetopics::SetMaxSizeDataCollectionChar(10000);
	scada_ate::typetopics::SetMaxSizeDataChar(10000 / 5);

	scada_ate::typetopics::SetMaxSizeDDSDataExVectorInt(10000);
	scada_ate::typetopics::SetMaxSizeDDSDataExVectorFloat(10000);
	scada_ate::typetopics::SetMaxSizeDDSDataExVectorDouble(10000);
	scada_ate::typetopics::SetMaxSizeDDSDataExVectorChar(10000);
	scada_ate::typetopics::SetMaxSizeDataExVectorChar(10000 / 5);


	DDSDataEx data_ex;

	DDSData data_;

	size_t size_ = 10000;

	data_ex.data_int().resize(size_);
	data_ex.data_float().resize(size_);
	data_ex.data_double().resize(size_);
	data_ex.data_char().resize(size_);
	
	for (int i = 0; i < size_; i++)
	{
		data_ex.data_char()[i].value().resize(size_ /5);
	}

	data_.data_int().value().resize(size_);
	data_.data_float().value().resize(size_);
	data_.data_double().value().resize(size_);
	data_.data_char().value().resize(size_);

	for (int i = 0; i < size_; i++)
	{
		data_.data_char().value()[i].value().resize(10000/5);
	}

	data_.data_int().quality().resize(size_);
	data_.data_float().quality().resize(size_);
	data_.data_double().quality().resize(size_);
	data_.data_char().quality().resize(size_);


	std::cout << "data_ex: " << DDSDataEx::getCdrSerializedSize(data_ex) << std::endl;
	std::cout << "data_: " << DDSData::getCdrSerializedSize(data_) << std::endl;

	std::thread tr;

	if (pub.compare(argv[1]) == 0)
	{
		tr = std::thread(test_pub);
	}

	if (sub.compare(argv[1]) == 0)
	{
		tr = std::thread(test_sub);
	}


	for (;;)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	return 0;
}

