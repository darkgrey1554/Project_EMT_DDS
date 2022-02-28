// Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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
 * @file HelloWorldSubscriber.cpp
 *
 */

#include "HelloWorldSubscriber.h"
#include <fastrtps/attributes/ParticipantAttributes.h>
#include <fastrtps/attributes/SubscriberAttributes.h>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/rtps/messages/CDRMessage.h>
#include <fastrtps/types/DynamicDataHelper.hpp>
#include <fastrtps/types/DynamicDataFactory.h>
#include <chrono>

using namespace std::chrono_literals;
using namespace eprosima::fastdds::dds;
using eprosima::fastrtps::types::ReturnCode_t;

HelloWorldSubscriber::HelloWorldSubscriber()
    : mp_participant(nullptr)
    , mp_subscriber(nullptr)
    , m_listener(this)
{
}

bool HelloWorldSubscriber::init()
{
    //Do not enable entities on creation
    DomainParticipantFactoryQos factory_qos;
    factory_qos.entity_factory().autoenable_created_entities = false;
    DomainParticipantFactory::get_instance()->set_qos(factory_qos);

    DomainParticipantQos pqos;
    pqos.name("Participant_sub");
    StatusMask par_mask = StatusMask::subscription_matched() << StatusMask::data_available();
    mp_participant = DomainParticipantFactory::get_instance()->create_participant(0, pqos, nullptr);// &m_listener, par_mask);

    if (mp_participant == nullptr)
    {
        return false;
    }
    if (mp_participant->enable() != ReturnCode_t::RETCODE_OK)
    {
        DomainParticipantFactory::get_instance()->delete_participant(mp_participant);
        return false;
    }

    // CREATE THE COMMON READER ATTRIBUTES
    //qos_ = DATAREADER_QOS_DEFAULT;
    //qos_.reliability().kind = RELIABLE_RELIABILITY_QOS;

    return true;
}

HelloWorldSubscriber::~HelloWorldSubscriber()
{
    for (const auto& it : topics_)
    {
        mp_subscriber->delete_datareader(it.first);
        mp_participant->delete_topic(it.second);
    }
    if (mp_subscriber != nullptr)
    {
        mp_participant->delete_subscriber(mp_subscriber);
    }

    DomainParticipantFactory::get_instance()->delete_participant(mp_participant);
    topics_.clear();
    readers_.clear();
    datas_.clear();
}

void HelloWorldSubscriber::SubListener::on_subscription_matched(
        DataReader*,
        const SubscriptionMatchedStatus& info)
{
    if (info.current_count_change == 1)
    {
        n_matched = info.total_count;
        std::cout << "Subscriber matched" << std::endl;
    }
    else if (info.current_count_change == -1)
    {
        n_matched = info.total_count;
        std::cout << "Subscriber unmatched" << std::endl;
    }
    else
    {
        std::cout << info.current_count_change
                  << " is not a valid value for SubscriptionMatchedStatus current count change" << std::endl;
    }
}

void HelloWorldSubscriber::SubListener::on_data_available(
        DataReader* reader)
{
    auto dit = subscriber_->datas_.find(reader);

    if (dit != subscriber_->datas_.end())
    {
        eprosima::fastrtps::types::DynamicData_ptr data = dit->second;
        SampleInfo info;
        if (reader->take_next_sample(data.get(), &info) == ReturnCode_t::RETCODE_OK)
        {
            if (info.instance_state == ALIVE_INSTANCE_STATE)
            {
                eprosima::fastrtps::types::DynamicType_ptr type = subscriber_->readers_[reader];
                this->n_samples++;
                std::cout << "Received: " << subscriber_->topics_[reader]->get_name() << " data of type: " << type->get_name() << std::endl;
                eprosima::fastrtps::types::DynamicDataHelper::print(data);
            }
        }
    }
}

void HelloWorldSubscriber::SubListener::on_type_discovery(
        DomainParticipant*,
        const eprosima::fastrtps::rtps::SampleIdentity&,
        const eprosima::fastrtps::string_255& topic_name,
        const eprosima::fastrtps::types::TypeIdentifier*,
        const eprosima::fastrtps::types::TypeObject*,
        eprosima::fastrtps::types::DynamicType_ptr dyn_type)
{
    TypeSupport m_type(new eprosima::fastrtps::types::DynamicPubSubType(dyn_type));
    m_type.register_type(subscriber_->mp_participant);

    std::string helpstr = topic_name.to_string();

    std::cout << "Discovered type: " << m_type->getName() << " from topic " << helpstr << std::endl;

    if (subscriber_->mp_subscriber == nullptr)
    {
        //eprosima::fastrtps::SubscriberAttributes Rparam;
        //Rparam = subscriber_->att_;
        //Rparam.topic = subscriber_->topic_;
        //Rparam.topic.topicName = topic;
        //Rparam.qos = subscriber_->qos_;
        subscriber_->mp_subscriber = subscriber_->mp_participant->create_subscriber(
            SUBSCRIBER_QOS_DEFAULT, nullptr);

        if (subscriber_->mp_subscriber == nullptr)
        {
            return;
        }
    }

    //CREATE THE TOPIC
    Topic* topic = subscriber_->mp_participant->create_topic(
        helpstr,
        m_type->getName(),
        TOPIC_QOS_DEFAULT);

    if (topic == nullptr)
    {
        return;
    }

    StatusMask sub_mask = StatusMask::subscription_matched() << StatusMask::data_available();
    DataReader* reader = subscriber_->mp_subscriber->create_datareader(
        topic,
        subscriber_->qos_,
        &subscriber_->m_listener,
        sub_mask);

    subscriber_->topics_[reader] = topic;
    subscriber_->readers_[reader] = dyn_type;
    eprosima::fastrtps::types::DynamicData_ptr data(
        eprosima::fastrtps::types::DynamicDataFactory::get_instance()->create_data(dyn_type));
    subscriber_->datas_[reader] = data;
}

void HelloWorldSubscriber::run()
{
    std::cout << "Subscriber running. Please press enter to stop the Subscriber" << std::endl;
    std::thread t(&HelloWorldSubscriber::run_thread, this);
    t.detach();
    //std::cin.ignore();
}

void HelloWorldSubscriber::run(
        uint32_t number)
{
    std::cout << "Subscriber running until " << number << "samples have been received" << std::endl;
    while (number > this->m_listener.n_samples)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void HelloWorldSubscriber::run_thread()
{
    std::vector<InstanceHandle_t> vec_topic;
    ReturnCode_t res;
    eprosima::fastdds::dds::builtin::TopicBuiltinTopicData topic_data;
    Topic* topic_1;
    DataReader* reader;
    eprosima::fastdds::dds::Subscriber* sub;

    std::vector<uint32_t> lengths = { 1,10 };
    DynamicType_ptr base_type = DynamicTypeBuilderFactory::get_instance()->create_uint32_type();
    DynamicTypeBuilder_ptr builder = DynamicTypeBuilderFactory::get_instance()->create_array_builder(base_type, lengths);
    DynamicType_ptr array_type = builder->build();

    DynamicTypeBuilder_ptr struct_type_builder(DynamicTypeBuilderFactory::get_instance()->create_struct_builder());
    struct_type_builder->add_member(0, "index", DynamicTypeBuilderFactory::get_instance()->create_uint32_type());
    struct_type_builder->add_member(1, "message", DynamicTypeBuilderFactory::get_instance()->create_string_type());
    struct_type_builder->add_member(2, "array", array_type);
    struct_type_builder->set_name("HelloWorld_2");

    DynamicType_ptr dynType = struct_type_builder->build();
    TypeSupport m_type(new eprosima::fastrtps::types::DynamicPubSubType(dynType));

    DynamicData_ptr data;
    DynamicData* array;
    data = DynamicDataFactory::get_instance()->create_data(dynType);

    res = mp_participant->register_type(m_type);
    topic_1 = mp_participant->create_topic("topic2", "HelloWorld_2", TOPIC_QOS_DEFAULT);
    sub = mp_participant->create_subscriber(SUBSCRIBER_QOS_DEFAULT);
    reader = sub->create_datareader(topic_1, DATAREADER_QOS_DEFAULT);

    for (;;)
    {
        SampleInfo info;
        reader->take_next_sample(data.get(),&info);
        int val;

        std::cout << "count:" << data->get_uint32_value(0) << std::endl;
        array = data->loan_value(2);
        for (unsigned int i = 0; i < 10; i++)
        {
            val = array->get_uint32_value(array->get_array_index({ 0, i }));
            std::cout << "data[" << i << "]:"<< val << std::endl;
        }
        data->return_loaned_value(array);
        std::cout << "///////////////////////////////" << std::endl;
        std::this_thread::sleep_for(800ms);
    }
}