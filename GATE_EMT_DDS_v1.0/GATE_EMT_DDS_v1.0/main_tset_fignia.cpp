#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <ddsformat/DDSData/DDSDataPubSubTypes.h>
#include <ddsformat/DDSData/DDSData.h>


int main()
{
	atech::common::SizeTopics::SetMaxSizeDataCollectionInt(100);
	
	eprosima::fastdds::dds::DomainParticipant* _part = 
		eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->create_participant(1, eprosima::fastdds::dds::PARTICIPANT_QOS_DEFAULT);
	auto type_support_topic_command = eprosima::fastdds::dds::TypeSupport(new DDSDataPubSubType());
	if (type_support_topic_command.register_type(_part) != ReturnCode_t::RETCODE_OK) throw 1;
	
	auto topic = _part->create_topic("asd", type_support_topic_command.get_type_name(), eprosima::fastdds::dds::TOPIC_QOS_DEFAULT);

	auto _pub = _part->create_publisher(eprosima::fastdds::dds::PUBLISHER_QOS_DEFAULT);
	auto _sub = _part->create_subscriber(eprosima::fastdds::dds::SUBSCRIBER_QOS_DEFAULT);

	auto _wr = _pub->create_datawriter(topic, eprosima::fastdds::dds::DATAWRITER_QOS_DEFAULT);
	auto _rd = _sub->create_datareader(topic, eprosima::fastdds::dds::DATAREADER_QOS_DEFAULT);

	DDSData data_out;
	DDSData data_in;
	data_out.data_int().value().resize(1);	  
	data_out.data_int().value()[0] = 10;

	_wr->write(&data_out);

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	eprosima::fastdds::dds::SampleInfo info;
	_rd->take_next_sample(&data_in, &info);

	std::cout << data_in.data_int().value()[0] << std::endl;
	

	std::cout << "OK" << std::endl;

	return 0;
}