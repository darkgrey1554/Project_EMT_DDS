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
 * @file HelloWorldPublisher.cpp
 *
 */

#include "HelloWorldPublisher.h"
#include <atomic>
#include <vector>
#include <thread>
#include <chrono>






HelloWorldPublisher::HelloWorldPublisher()
    : mp_participant(nullptr)
    , mp_publisher(nullptr)
{
}

bool HelloWorldPublisher::init()
{
    
    {
        std::vector<uint32_t> lengths = { 1,4 };
        DynamicType_ptr base_type = DynamicTypeBuilderFactory::get_instance()->create_uint32_type();
        DynamicTypeBuilder_ptr builder = DynamicTypeBuilderFactory::get_instance()->create_array_builder(base_type, lengths);
        DynamicType_ptr array_type = builder->build();

        DynamicTypeBuilder_ptr struct_type_builder(DynamicTypeBuilderFactory::get_instance()->create_struct_builder());
        struct_type_builder->add_member(0, "index", DynamicTypeBuilderFactory::get_instance()->create_uint32_type());
        struct_type_builder->add_member(1, "message", DynamicTypeBuilderFactory::get_instance()->create_string_type());
        struct_type_builder->add_member(2, "array", array_type);
        struct_type_builder->set_name("HelloWorld_1");
        dyn_type1 = struct_type_builder->build();
    }

    {
        std::vector<uint32_t> lengths = { 1,10 };
        DynamicType_ptr base_type = DynamicTypeBuilderFactory::get_instance()->create_uint32_type();
        DynamicTypeBuilder_ptr builder = DynamicTypeBuilderFactory::get_instance()->create_array_builder(base_type, lengths);
        DynamicType_ptr array_type = builder->build();

        DynamicTypeBuilder_ptr struct_type_builder(DynamicTypeBuilderFactory::get_instance()->create_struct_builder());
        struct_type_builder->add_member(0, "index", DynamicTypeBuilderFactory::get_instance()->create_uint32_type());
        struct_type_builder->add_member(1, "message", DynamicTypeBuilderFactory::get_instance()->create_string_type());
        struct_type_builder->add_member(2, "array", array_type);
        struct_type_builder->set_name("HelloWorld_2");
        dyn_type2 = struct_type_builder->build();
    }


    TypeSupport m_type1(new eprosima::fastrtps::types::DynamicPubSubType(dyn_type1));
    TypeSupport m_type2(new eprosima::fastrtps::types::DynamicPubSubType(dyn_type2));

    /*m_Hello->set_string_value("Hello DDS Dynamic World", 0);
    m_Hello->set_uint32_value(0, 1);
    eprosima::fastrtps::types::DynamicData* array = m_Hello->loan_value(2);
    array->set_uint32_value(10, array->get_array_index({0, 0}));
    array->set_uint32_value(20, array->get_array_index({1, 0}));
    array->set_uint32_value(30, array->get_array_index({2, 0}));
    array->set_uint32_value(40, array->get_array_index({3, 0}));
    array->set_uint32_value(50, array->get_array_index({4, 0}));
    array->set_uint32_value(60, array->get_array_index({0, 1}));
    array->set_uint32_value(70, array->get_array_index({1, 1}));
    array->set_uint32_value(80, array->get_array_index({2, 1}));
    array->set_uint32_value(90, array->get_array_index({3, 1}));
    array->set_uint32_value(100, array->get_array_index({4, 1}));
    m_Hello->return_loaned_value(array);*/

    DomainParticipantQos pqos;
    pqos.name("Participant_pub");
    mp_participant = DomainParticipantFactory::get_instance()->create_participant(0, pqos);

    if (mp_participant == nullptr)
    {
        return false;
    }

    //REGISTER THE TYPE
    m_type1.get()->auto_fill_type_information(false);
    m_type1.get()->auto_fill_type_object(true);
    m_type1.register_type(mp_participant);

    m_type2.get()->auto_fill_type_information(false);
    m_type2.get()->auto_fill_type_object(true);
    m_type2.register_type(mp_participant);

    //CREATE THE PUBLISHER
    mp_publisher = mp_participant->create_publisher(PUBLISHER_QOS_DEFAULT, nullptr);

    if (mp_publisher == nullptr)
    {
        return false;
    }

    /*topic_1 = mp_participant->create_topic("DDSDynHelloWorldTopic", "HelloWorld", TOPIC_QOS_DEFAULT);

    if (topic_1 == nullptr)
    {
        return false;
    }

    // CREATE THE WRITER
    writer_ = mp_publisher->create_datawriter(topic_, DATAWRITER_QOS_DEFAULT, &m_listener);

    if (writer_ == nullptr)
    {
        return false;
    }

    return true;*/

}

HelloWorldPublisher::~HelloWorldPublisher()
{
    if (writer_1 != nullptr)
    {
        mp_publisher->delete_datawriter(writer_1);
    }
    if (mp_publisher != nullptr)
    {
        mp_participant->delete_publisher(mp_publisher);
    }
    if (topic_1 != nullptr)
    {
        mp_participant->delete_topic(topic_1);
    }

    if (writer_2 != nullptr)
    {
        mp_publisher->delete_datawriter(writer_2);
    }
    if (mp_publisher != nullptr)
    {
        mp_participant->delete_publisher(mp_publisher);
    }
    if (topic_2 != nullptr)
    {
        mp_participant->delete_topic(topic_1);
    }

    DomainParticipantFactory::get_instance()->delete_participant(mp_participant);
}

void HelloWorldPublisher::PubListener::on_publication_matched(
        eprosima::fastdds::dds::DataWriter*,
        const eprosima::fastdds::dds::PublicationMatchedStatus& info)
{
    if (info.current_count_change == 1)
    {
        n_matched = info.total_count;
        firstConnected = true;
        std::cout << "Publisher matched" << std::endl;
    }
    else if (info.current_count_change == -1)
    {
        n_matched = info.total_count;
        std::cout << "Publisher unmatched" << std::endl;
    }
    else
    {
        std::cout << info.current_count_change
                  << " is not a valid value for PublicationMatchedStatus current count change" << std::endl;
    }
}

void HelloWorldPublisher::runThread( uint32_t samples, uint32_t sleep)
{
    topic_1 = mp_participant->create_topic("topic_number_1", "HelloWorld_1", TOPIC_QOS_DEFAULT);
    topic_2 = mp_participant->create_topic("topic_number_2", "HelloWorld_2", TOPIC_QOS_DEFAULT);
    
    ReturnCode_t res;
    std::vector<InstanceHandle_t> topvec;

    DynamicData_ptr data_1; 
    DynamicData_ptr data_2; 
    data_1 = DynamicDataFactory::get_instance()->create_data(dyn_type1);
    data_2 = DynamicDataFactory::get_instance()->create_data(dyn_type2);
    eprosima::fastrtps::types::DynamicData* array;

    int count = 0;
    uint32_t index = 0;;

    for (;;)
    {

        res = mp_participant->get_discovered_topics(topvec);

        if (count == 0)
        {
            writer_1 = mp_publisher->create_datawriter(topic_1, DATAWRITER_QOS_DEFAULT, nullptr);
            data_1->set_string_value("QWEERTASDF", 1);
            data_1->set_uint32_value(0, 0);
        }
        else
        {
            if (count < 20 || count > 30)
            {
                data_1->get_uint32_value(index, 0);
                data_1->set_uint32_value(index + 1, 0);
                array = data_1->loan_value(2);

                for (uint32_t i = 0; i < 4; i++)
                {
                    array->set_uint32_value(10 * i + index, array->get_array_index({ 0, i }));
                }
                data_1->return_loaned_value(array);
                writer_1->write(data_1.get());

            }
            else if (count == 20)
            {
                mp_publisher->delete_datawriter(writer_1);
                writer_1 = nullptr;
                mp_participant->delete_topic(topic_1);
                topic_1 = nullptr;
            }
            else if (count == 30)
            {
                topic_1 = mp_participant->create_topic("topic_number_1", "HelloWorld_2", TOPIC_QOS_DEFAULT);
                writer_1 = mp_publisher->create_datawriter(topic_1, DATAWRITER_QOS_DEFAULT, nullptr);
            }
            
        }

        if (count < 10)
        {
        }
        else if (count == 10)
        {
            writer_2 = mp_publisher->create_datawriter(topic_2, DATAWRITER_QOS_DEFAULT, nullptr);
            data_2->set_string_value("QWEERTASDF", 1);
            data_2->set_uint32_value(0, 0);
        }
        else if (count > 10)
        {
            data_2->get_uint32_value(index, 0);
            data_2->set_uint32_value(index + 1, 0);
            array = data_2->loan_value(2);

            for (uint32_t i = 0; i < 10; i++)
            {
                array->set_uint32_value(10 * i + index, array->get_array_index({ 0, i }));
            }
            data_2->return_loaned_value(array);
            writer_2->write(data_2.get());
        }

        count++;
        std::this_thread::sleep_for(1000ms);
    }
}

void HelloWorldPublisher::run(
        uint32_t samples,
        uint32_t sleep)
{
    stop = false;
    std::thread thread(&HelloWorldPublisher::runThread, this, samples, sleep);
    if (samples == 0)
    {
        std::cout << "Publisher running. Please press enter to stop the Publisher at any time." << std::endl;
        //std::cin.ignore();
        //stop = true;
    }
    else
    {
        std::cout << "Publisher running " << samples << " samples." << std::endl;
    }
    thread.detach();
}

bool HelloWorldPublisher::publish(
        bool waitForListener)
{
    /*if (m_listener.firstConnected || !waitForListener || m_listener.n_matched > 0)
    {
        uint32_t index;
        m_Hello->get_uint32_value(index, 1);
        m_Hello->set_uint32_value(index + 1, 1);

        eprosima::fastrtps::types::DynamicData* array = m_Hello->loan_value(2);
        array->set_uint32_value(10 + index, array->get_array_index({0, 0}));
        array->set_uint32_value(20 + index, array->get_array_index({1, 0}));
        array->set_uint32_value(30 + index, array->get_array_index({2, 0}));
        array->set_uint32_value(40 + index, array->get_array_index({3, 0}));
        array->set_uint32_value(50 + index, array->get_array_index({4, 0}));
        array->set_uint32_value(60 + index, array->get_array_index({0, 1}));
        array->set_uint32_value(70 + index, array->get_array_index({1, 1}));
        array->set_uint32_value(80 + index, array->get_array_index({2, 1}));
        array->set_uint32_value(90 + index, array->get_array_index({3, 1}));
        array->set_uint32_value(100 + index, array->get_array_index({4, 1}));
        m_Hello->return_loaned_value(array);

        writer_->write(m_Hello.get());
        return true;
    }*/
    return false;
}
