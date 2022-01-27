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

using namespace eprosima::fastdds::dds;
using namespace eprosima::fastrtps::types;
using namespace std::chrono_literals;


void test_pub()
{
	DomainParticipant* participant_pub;
	participant_pub = DomainParticipantFactory::get_instance()->create_participant(0, PARTICIPANT_QOS_DEFAULT);

	Publisher* publisher_ = participant_pub->create_publisher(PUBLISHER_QOS_DEFAULT, nullptr);

	DynamicData_ptr data_pub;

	DynamicTypeBuilder_ptr builder = DynamicTypeBuilderFactory::get_instance()->create_struct_builder();
	builder->add_member(0, "first", DynamicTypeBuilderFactory::get_instance()->create_int32_type());
	builder->add_member(1, "other", DynamicTypeBuilderFactory::get_instance()->create_uint64_type());
	builder->set_name("test_data");
	DynamicType_ptr struct_type(builder->build());


	TypeSupport PtrSupporType = eprosima::fastrtps::types::DynamicPubSubType(struct_type);

	PtrSupporType.get()->auto_fill_type_information(false);
	PtrSupporType.get()->auto_fill_type_object(true);
	participant_pub->register_type(PtrSupporType);

	data_pub = DynamicDataFactory::get_instance()->create_data(struct_type);

	Topic* topic_pub = participant_pub->create_topic("test_1", "test_data", TOPIC_QOS_DEFAULT, nullptr);

	DataWriter* writer_ = publisher_->create_datawriter(topic_pub, DATAWRITER_QOS_DEFAULT, nullptr);

	int count = 0;

	for (;;)
	{		
		data_pub->set_int32_value(count, 0);
		writer_->write(data_pub.get());
		std::cout << "value = " << count << std::endl;
		count++;
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

}

void test_sub()
{
	DomainParticipant* participant_sub;
	participant_sub = DomainParticipantFactory::get_instance()->create_participant(0, PARTICIPANT_QOS_DEFAULT);

	Subscriber* subcriber_ = participant_sub->create_subscriber(SUBSCRIBER_QOS_DEFAULT, nullptr);

	DynamicData_ptr data_sub;

	DynamicTypeBuilder_ptr builder = DynamicTypeBuilderFactory::get_instance()->create_struct_builder();
	builder->add_member(0, "first", DynamicTypeBuilderFactory::get_instance()->create_int32_type());
	builder->add_member(1, "other", DynamicTypeBuilderFactory::get_instance()->create_uint64_type());
	builder->set_name("test_data");
	DynamicType_ptr struct_type(builder->build());


	TypeSupport PtrSupporType = eprosima::fastrtps::types::DynamicPubSubType(struct_type);

	PtrSupporType.get()->auto_fill_type_information(false);
	PtrSupporType.get()->auto_fill_type_object(true);
	participant_sub->register_type(PtrSupporType);

	data_sub = DynamicDataFactory::get_instance()->create_data(struct_type);

	Topic* topic_sub = participant_sub->create_topic("test_1", "test_data", TOPIC_QOS_DEFAULT, nullptr);

	DataReader* reader_ = subcriber_->create_datareader(topic_sub, DATAREADER_QOS_DEFAULT, nullptr);

	int count = 0;
	SampleInfo info;
	for (;;)
	{
		data_sub->get_int32_value(0);
		reader_->take_next_sample(data_sub.get(), &info);
		std::cout << "value = "<< data_sub->get_int32_value(0) << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

}

int main(int argc, char** argv)
{
	std::string pub = "pub";
	std::string sub = "sub";

	if (argc != 2) return 0;

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

/*int main(int argc, char** argv)
{
	DomainParticipant* participant_pub;
	participant_pub = DomainParticipantFactory::get_instance()->create_participant(0, PARTICIPANT_QOS_DEFAULT);
	ReturnCode_t res;

	DomainParticipant* participant_sub;
	participant_sub = DomainParticipantFactory::get_instance()->create_participant(0, PARTICIPANT_QOS_DEFAULT);

	Publisher* publisher_ = participant_pub->create_publisher(PUBLISHER_QOS_DEFAULT, nullptr);
	Subscriber* subcriber_ = participant_sub->create_subscriber(SUBSCRIBER_QOS_DEFAULT, nullptr);

	DynamicData_ptr data_pub;
	DynamicData_ptr data_sub;

	{
		DynamicTypeBuilder_ptr builder = DynamicTypeBuilderFactory::get_instance()->create_struct_builder();
		builder->add_member(0, "first", DynamicTypeBuilderFactory::get_instance()->create_int32_type());
		builder->add_member(1, "other", DynamicTypeBuilderFactory::get_instance()->create_uint64_type());
		builder->set_name("test_data");
		DynamicType_ptr struct_type(builder->build());


		TypeSupport PtrSupporType = eprosima::fastrtps::types::DynamicPubSubType(struct_type);

		PtrSupporType.get()->auto_fill_type_information(false);
		PtrSupporType.get()->auto_fill_type_object(true);
		res = participant_sub->register_type(PtrSupporType);
		res = participant_pub->register_type(PtrSupporType);

		data_pub = DynamicDataFactory::get_instance()->create_data(struct_type);
		data_sub = DynamicDataFactory::get_instance()->create_data(struct_type);
	}

	Topic* topic_pub = participant_pub->create_topic("test_1", "test_data", TOPIC_QOS_DEFAULT, nullptr);
	Topic* topic_sub = participant_sub->create_topic("test_1", "test_data", TOPIC_QOS_DEFAULT, nullptr);
	
	DataWriter* writer_ = publisher_->create_datawriter(topic_pub, DATAWRITER_QOS_DEFAULT, nullptr);

	DataReader* reader_ = subcriber_->create_datareader(topic_sub, DATAREADER_QOS_DEFAULT, nullptr);
	

	std::this_thread::sleep_for(std::chrono::seconds(4));

	publisher_->delete_datawriter(writer_);
	participant_pub->delete_topic(topic_pub);
	participant_pub->unregister_type("test_data");

	 {
		std::vector<uint32_t> lengths = { 1, 200 };
		DynamicType_ptr base_type = DynamicTypeBuilderFactory::get_instance()->create_int32_type();
		DynamicTypeBuilder_ptr builder_array = DynamicTypeBuilderFactory::get_instance()->create_array_builder(base_type, lengths);
		DynamicType_ptr array_type(builder_array->build());

		DynamicTypeBuilder_ptr builder = DynamicTypeBuilderFactory::get_instance()->create_struct_builder();
		builder->add_member(0, "first", DynamicTypeBuilderFactory::get_instance()->create_int32_type());
		builder->add_member(1, "other", DynamicTypeBuilderFactory::get_instance()->create_uint64_type());
		builder->add_member(2, "ttt", array_type);
		builder->set_name("test_data");
		DynamicType_ptr struct_type(builder->build());


		TypeSupport PtrSupporType = eprosima::fastrtps::types::DynamicPubSubType(struct_type);

		PtrSupporType.get()->auto_fill_type_information(false);
		PtrSupporType.get()->auto_fill_type_object(true);
		res = participant_pub->register_type(PtrSupporType);

		data_pub = DynamicDataFactory::get_instance()->create_data(struct_type);
	}

	topic_pub = participant_pub->create_topic("test_1", "test_data", TOPIC_QOS_DEFAULT, nullptr);

	writer_ = publisher_->create_datawriter(topic_pub, DATAWRITER_QOS_DEFAULT, nullptr);
	

	MemberId pos;
	int value = 0;
	SampleInfo info;
	DynamicData* array_;
	for(;;)
	{
		array_ = data_pub->loan_value(2);
		pos = array_->get_array_index({ 0, 0 });
		for (int i = 0; i < 100; i++)
		{
			value = array_->get_int32_value(pos + i);
			array_->set_int32_value(value + i + 1, pos + i);
		}
		data_pub->return_loaned_value(array_);
		
		writer_->write(data_pub.get());

		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		reader_->take_next_sample(data_sub.get(), &info);

		array_ = data_sub->loan_value(2);
		pos = array_->get_array_index({ 0, 0 });
		
		std::cout << "value[0] = " << array_->get_int32_value(pos) << std::endl;
		std::cout << "value[99] = " << array_->get_int32_value(pos+99) << std::endl;

		data_sub->return_loaned_value(array_);

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}*/
