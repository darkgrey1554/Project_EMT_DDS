#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include "HelloWorld.h"
#include "HelloWorldPubSubTypes.h"

using namespace eprosima::fastdds::dds;



int main()
{
	DomainParticipant* participant_pub;
	participant_pub = DomainParticipantFactory::get_instance()->create_participant(0, PARTICIPANT_QOS_DEFAULT);

	TypeSupport type = TypeSupport(new HelloWorldPubSubType());
	type.register_type(participant_pub);
	Topic* topic = participant_pub->create_topic("test", type.get_type_name(), TOPIC_QOS_DEFAULT);

	Subscriber* sub = participant_pub->create_subscriber(SUBSCRIBER_QOS_DEFAULT);
	Publisher* pub = participant_pub->create_publisher(PUBLISHER_QOS_DEFAULT);

	DataReader* data_reader = sub->create_datareader(topic, DATAREADER_QOS_DEFAULT);
	DataWriter* data_writer = pub->create_datawriter(topic, DATAWRITER_QOS_DEFAULT);

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	if (participant_pub->unregister_type("add") != ReturnCode_t::RETCODE_OK)
	{
		std::cout << "err1" << std::endl;
	}
	

	HelloWorld data;
	HelloWorld in;
	SampleInfo info;
	data.message("dasd");
	data.index(1);
	data_writer->write(&data);
	for (int i = 0; i < 100; i++)
	{
		//data.message("dasd");
		//data.index(i);

		//data_writer->write(&data);

		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		if (i % 10 == 9)
		{
			data_reader->take_next_sample(&in, &info);
			std::cout << in.index() << std::endl;
			in.index(0);
		}
	}


	pub->delete_datawriter(data_writer);
	sub->delete_datareader(data_reader);

	participant_pub->delete_publisher(pub);
	participant_pub->delete_subscriber(sub);
	participant_pub->delete_topic(topic);

}