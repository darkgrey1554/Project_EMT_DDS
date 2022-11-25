#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>

#include "ddsformat/DDSData/DDSData.h"
#include "ddsformat/DDSData/DDSDataPubSubTypes.h"
#include "ddsformat/SizeTopics.h"


using namespace eprosima::fastdds::dds;

DataReader* data_reader;
static int cmd = 0;

void thread_read()
{
	DDSData data;
	SampleInfo info;

	while (cmd == 0)
	{
		data_reader->take_next_sample(&data, &info);
		std::vector<int32_t>& v_data = data.data_int().value();
		if (!info.valid_data)
		{
			std::cout << "no data" << std::endl;
		}
		if (!v_data.empty())
		{
			std::cout << "----------------------------" << std::endl;
			std::cout << "data[0] = " << v_data[0] << std::endl;
			std::cout << "data[" << v_data.size()-1 << "] = "   << v_data[v_data.size()-1] << std::endl;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}


int main(int arg, char* argc[])
{
	if (arg < 2) return - 1;

	std::string topic_name(argc[1]);

	atech::common::SizeTopics::SetMaxSizeDataCollectionInt(100);
	atech::common::SizeTopics::SetMaxSizeDataCollectionFloat(100);
	atech::common::SizeTopics::SetMaxSizeDataCollectionDouble(100);
	atech::common::SizeTopics::SetMaxSizeDataCollectionChar(100);
	atech::common::SizeTopics::SetMaxSizeDataChar(100);

	std::cout << "Subscribe topic: " << topic_name << std::endl;

	DomainParticipant* participant_pub;
	participant_pub = DomainParticipantFactory::get_instance()->create_participant(0, PARTICIPANT_QOS_DEFAULT);

	TypeSupport type = TypeSupport(new DDSDataPubSubType());
	type.register_type(participant_pub);
	Topic* topic = participant_pub->create_topic(topic_name, type.get_type_name(), TOPIC_QOS_DEFAULT);

	Subscriber* sub = participant_pub->create_subscriber(SUBSCRIBER_QOS_DEFAULT);

	data_reader = sub->create_datareader(topic, DATAREADER_QOS_DEFAULT);

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));	
	
	std::thread tr{ thread_read };

	char c = 'a';
	while (c!='q')
	{
		std::cin >> c;
	}

	cmd = 1;

	tr.join();

	sub->delete_datareader(data_reader);
	participant_pub->delete_subscriber(sub);
	participant_pub->delete_topic(topic);

	return 0;
}