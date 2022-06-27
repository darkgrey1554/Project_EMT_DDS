#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>

#include <TypeTopicDDS/DDSData/DDSDataPubSubTypes.h>
#include <TypeTopicDDS/DDSDataEx/DDSDataExPubSubTypes.h>

using namespace eprosima::fastdds::dds;
using namespace eprosima::fastdds::rtps;
using namespace eprosima::fastrtps::types;

int main()
{
	DomainParticipant* partic;
	DataWriter* data_writer;
	Publisher* pub;
	eprosima::fastdds::dds::Topic* tt;
	ReturnCode_t code;

	DomainParticipantQos qos = PARTICIPANT_QOS_DEFAULT;
	DomainParticipant* participant_pub;
	participant_pub = DomainParticipantFactory::get_instance()->create_participant(0, qos);

	return 0;
}