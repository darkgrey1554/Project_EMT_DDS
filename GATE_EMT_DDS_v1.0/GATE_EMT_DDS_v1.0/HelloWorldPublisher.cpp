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
#include <fastrtps/attributes/ParticipantAttributes.h>
#include <fastrtps/attributes/PublisherAttributes.h>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/publisher/qos/PublisherQos.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/qos/DataWriterQos.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>

#include <fastrtps/types/DynamicType.h>
#include <fastrtps/types/DynamicData.h>
#include <fastrtps/types/DynamicDataFactory.h>
#include <fastrtps/types/DynamicTypeBuilderFactory.h>
#include <fastrtps/types/DynamicTypeBuilderPtr.h>
#include <fastrtps/types/DynamicTypeBuilder.h>

#include <fastrtps/types/DynamicDataFactory.h>

#include <fastrtps/xmlparser/XMLProfileManager.h>

#include <thread>

using namespace eprosima::fastdds::dds;
using namespace eprosima::fastrtps::types;

HelloWorldPublisher::HelloWorldPublisher()
    : mp_participant(nullptr)
    , mp_publisher(nullptr)
{
}

bool HelloWorldPublisher::init()
{
    /*if (eprosima::fastrtps::xmlparser::XMLP_ret::XML_OK !=
            eprosima::fastrtps::xmlparser::XMLProfileManager::loadXMLFile("example_type.xml"))
    {
        std::cout << "Cannot open XML file \"example_type.xml\". Please, run the publisher from the folder "
                  << "that contatins this XML file." << std::endl;
        return false;
    }

    eprosima::fastrtps::types::DynamicType_ptr dyn_type =
            eprosima::fastrtps::xmlparser::XMLProfileManager::getDynamicTypeByName("HelloWorld")->build();
            
     TypeSupport m_type(new eprosima::fastrtps::types::DynamicPubSubType(dyn_type));
            */
    
    DynamicTypeBuilder_ptr struct_type_builder(DynamicTypeBuilderFactory::get_instance()->create_struct_builder());
    struct_type_builder->add_member(0, "message", DynamicTypeBuilderFactory::get_instance()->create_string_type());
    struct_type_builder->add_member(1, "index", DynamicTypeBuilderFactory::get_instance()->create_uint32_type());
    struct_type_builder->set_name("HelloWorld");
    DynamicType_ptr dyn_type = struct_type_builder->build();
    //base_type_array_analog = struct_type_builder->build();
    TypeSupport m_type(new eprosima::fastrtps::types::DynamicPubSubType(dyn_type));


    
    m_Hello = eprosima::fastrtps::types::DynamicDataFactory::get_instance()->create_data(dyn_type);

    m_Hello->set_string_value("Hello DDS Dynamic World", 0);
    m_Hello->set_uint32_value(0, 1);
    

    DomainParticipantQos pqos;
    pqos.name("Participant_pub");
    mp_participant = DomainParticipantFactory::get_instance()->create_participant(0, pqos);

    if (mp_participant == nullptr)
    {
        return false;
    }

    //REGISTER THE TYPE
    m_type.get()->auto_fill_type_information(false);
    m_type.get()->auto_fill_type_object(true);

    m_type.register_type(mp_participant);

    //CREATE THE PUBLISHER
    mp_publisher = mp_participant->create_publisher(PUBLISHER_QOS_DEFAULT, nullptr);

    if (mp_publisher == nullptr)
    {
        return false;
    }

    topic_ = mp_participant->create_topic("DDSDynHelloWorldTopic", "HelloWorld", TOPIC_QOS_DEFAULT);

    if (topic_ == nullptr)
    {
        return false;
    }

    // CREATE THE WRITER
    writer_ = mp_publisher->create_datawriter(topic_, DATAWRITER_QOS_DEFAULT, &m_listener);

    if (writer_ == nullptr)
    {
        return false;
    }

    return true;

}

HelloWorldPublisher::~HelloWorldPublisher()
{
    if (writer_ != nullptr)
    {
        mp_publisher->delete_datawriter(writer_);
    }
    if (mp_publisher != nullptr)
    {
        mp_participant->delete_publisher(mp_publisher);
    }
    if (topic_ != nullptr)
    {
        mp_participant->delete_topic(topic_);
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

void HelloWorldPublisher::runThread(
        uint32_t samples,
        uint32_t sleep)
{
    if (samples == 0)
    {
        while (!stop)
        {
            if (publish(false))
            {
                std::string message;
                m_Hello->get_string_value(message, 0);
                uint32_t index;
                m_Hello->get_uint32_value(index, 1);
                std::cout << "Message: " << message << " with index: " << index
                          << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
        }
    }
    else
    {
        for (uint32_t s = 0; s < samples; ++s)
        {
            if (!publish())
            {
                --s;
            }
            else
            {
                std::string message;
                m_Hello->get_string_value(message, 0);
                uint32_t index;
                m_Hello->get_uint32_value(index, 1);
                std::cout << "Message: " << message << " with index: " << index
                          << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
        }
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
        std::cin.ignore();
        stop = true;
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
    if (m_listener.firstConnected || !waitForListener || m_listener.n_matched > 0)
    {
        uint32_t index;
        m_Hello->get_uint32_value(index, 1);
        m_Hello->set_uint32_value(index + 1, 1);

        writer_->write(m_Hello.get());
        return true;
    }
    return false;
}
