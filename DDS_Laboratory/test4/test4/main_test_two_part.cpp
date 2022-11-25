#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantListener.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>

#include "ddsformat/DDSData/DDSData.h"
#include "ddsformat/DDSData/DDSDataPubSubTypes.h"
#include "ddsformat/SizeTopics.h"

#include <ddsformat/DdsCommand/DdsCommand.h>
#include <ddsformat/DdsStatus/DdsStatus.h>
#include <ddsformat/DdsConfig/DdsConfig.h>

#include <ddsformat/DdsCommand/DdsCommandPubSubTypes.h>
#include <ddsformat/DdsStatus/DdsStatusPubSubTypes.h>
#include <ddsformat/DdsConfig/DdsConfigPubSubTypes.h>
#include <fastdds/rtps/transport/TCPv4TransportDescriptor.h>
#include <fastdds/rtps/transport/TCPTransportDescriptor.h>
#include <fastdds/rtps/transport/shared_mem/SharedMemTransportDescriptor.h>
#include <fastrtps/utils/IPLocator.h>
#include <ServiceType.h>

using namespace eprosima::fastdds::dds;
using namespace eprosima::fastdds::rtps;
using namespace eprosima::fastrtps::rtps;

class Listen_ : public DomainParticipantListener
{
	void on_subscriber_discovery(DomainParticipant*, eprosima::fastrtps::rtps::ReaderDiscoveryInfo&& info) override 
	{
		if (info.status == eprosima::fastrtps::rtps::ReaderDiscoveryInfo::DISCOVERED_READER)
		{
			std::cout << "New subscriber discovered" << std::endl;
		}
		else if (info.status == eprosima::fastrtps::rtps::ReaderDiscoveryInfo::REMOVED_READER)
		{
			std::cout << "New subscriber lost" << std::endl;
		}
	}
};

void thread_writer_1()
{
	DomainParticipant* participant_pub;
	DomainParticipantQos qos;

	auto tcp_transport = std::make_shared<TCPv4TransportDescriptor>();
	tcp_transport->add_listener_port(5100);
	tcp_transport->add_listener_port(5110);
	tcp_transport->set_WAN_address("127.0.0.1");
	qos.transport().user_transports.push_back(tcp_transport);
	qos.transport().use_builtin_transports = false;
	
	//std::shared_ptr<SharedMemTransportDescriptor> shm_transport = std::make_shared<SharedMemTransportDescriptor>();
	//qos.transport().user_transports.push_back(shm_transport);
	//qos.transport().use_builtin_transports = false;

	Listen_ l;

	participant_pub = DomainParticipantFactory::get_instance()->create_participant(0, qos,&l);



	TypeSupport type_command = TypeSupport(new DdsCommandPubSubType());
	type_command.register_type(participant_pub);

	Topic* topic_command = participant_pub->create_topic("dds_command_1", type_command.get_type_name(), TOPIC_QOS_DEFAULT);
	Publisher* pub = participant_pub->create_publisher(PUBLISHER_QOS_DEFAULT);

	DataWriter* commander = pub->create_datawriter(topic_command, DATAWRITER_QOS_DEFAULT);

	volatile int i = 0;
	while (1)
	{
		DdsCommand cmd;
		i++;
		cmd.cmd_code(i);

		commander->write(&cmd);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

void thread_writer_2()
{
	DomainParticipant* participant_pub;
	DomainParticipantQos qos;

	auto tcp_transport = std::make_shared<TCPv4TransportDescriptor>();
	tcp_transport->add_listener_port(5100);
	tcp_transport->add_listener_port(5110);
	tcp_transport->set_WAN_address("127.0.0.1");
	qos.transport().user_transports.push_back(tcp_transport);
	qos.transport().use_builtin_transports = false;

	//std::shared_ptr<SharedMemTransportDescriptor> shm_transport = std::make_shared<SharedMemTransportDescriptor>();
	//qos.transport().user_transports.push_back(shm_transport);
	//qos.transport().use_builtin_transports = false;

	participant_pub = DomainParticipantFactory::get_instance()->create_participant(0, qos);

	TypeSupport type_command = TypeSupport(new DdsCommandPubSubType());
	type_command.register_type(participant_pub);

	Topic* topic_command = participant_pub->create_topic("dds_command_1", type_command.get_type_name(), TOPIC_QOS_DEFAULT);
	Publisher* pub = participant_pub->create_publisher(PUBLISHER_QOS_DEFAULT);

	DataWriter* commander = pub->create_datawriter(topic_command, DATAWRITER_QOS_DEFAULT);


	volatile int i = 100;
	while (1)
	{
		DdsCommand cmd;
		i++;
		cmd.cmd_code(i);
		commander->write(&cmd);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

void thread_reader_1()
{
	DomainParticipant* participant_pub;
	DomainParticipantQos qos;

	qos.transport().use_builtin_transports = false;
	auto tcp_transport = std::make_shared<TCPv4TransportDescriptor>();
	qos.transport().user_transports.push_back(tcp_transport);
	Locator_t initial_peer_locator;
	initial_peer_locator.kind = LOCATOR_KIND_TCPv4;
	IPLocator::setIPv4(initial_peer_locator, "127.0.0.1");
	initial_peer_locator.port = 5100;
	qos.wire_protocol().builtin.initialPeersList.push_back(initial_peer_locator);
	qos.transport().use_builtin_transports = false;

	//std::shared_ptr<SharedMemTransportDescriptor> shm_transport = std::make_shared<SharedMemTransportDescriptor>();
	//qos.transport().user_transports.push_back(shm_transport);
	//qos.transport().use_builtin_transports = false;

	

	participant_pub = DomainParticipantFactory::get_instance()->create_participant(0, qos);

	TypeSupport type_command = TypeSupport(new DdsCommandPubSubType());
	type_command.register_type(participant_pub);

	Topic* topic_command = participant_pub->create_topic("dds_command_1", type_command.get_type_name(), TOPIC_QOS_DEFAULT);
	Subscriber* pub = participant_pub->create_subscriber(SUBSCRIBER_QOS_DEFAULT);

	DataReader* commander = pub->create_datareader(topic_command, DATAREADER_QOS_DEFAULT);

	while (1)
	{
		DdsCommand cmd;
		SampleInfo info;
		commander->take_next_sample(&cmd,&info);
		std::cout << "1::" << cmd.cmd_code() << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

void thread_reader_2()
{
	DomainParticipant* participant_pub;
	DomainParticipantQos qos;

	qos.transport().use_builtin_transports = false;
	auto tcp_transport = std::make_shared<TCPv4TransportDescriptor>();
	qos.transport().user_transports.push_back(tcp_transport);
	Locator_t initial_peer_locator;
	initial_peer_locator.kind = LOCATOR_KIND_TCPv4;
	IPLocator::setIPv4(initial_peer_locator, "127.0.0.1");
	initial_peer_locator.port = 5110;
	qos.wire_protocol().builtin.initialPeersList.push_back(initial_peer_locator);
	qos.transport().use_builtin_transports = false;

	/*std::shared_ptr<SharedMemTransportDescriptor> shm_transport = std::make_shared<SharedMemTransportDescriptor>();
	qos.transport().user_transports.push_back(shm_transport);
	qos.transport().use_builtin_transports = false;*/

	participant_pub = DomainParticipantFactory::get_instance()->create_participant(0, qos);

	TypeSupport type_command = TypeSupport(new DdsCommandPubSubType());
	type_command.register_type(participant_pub);

	Topic* topic_command = participant_pub->create_topic("dds_command_2", type_command.get_type_name(), TOPIC_QOS_DEFAULT);
	Subscriber* pub = participant_pub->create_subscriber(SUBSCRIBER_QOS_DEFAULT);

	DataReader* commander = pub->create_datareader(topic_command, DATAREADER_QOS_DEFAULT);


	while (1)
	{
		DdsCommand cmd;
		SampleInfo info;
		commander->take_next_sample(&cmd, &info);
		std::cout << "2::" << cmd.cmd_code() << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

int main(int arg, char* arcg[])
{
	
	/*std::thread th1(thread_writer_1);
	std::thread th1(thread_writer_2);

	std::thread th3(thread_reader_1);
	std::thread th4(thread_reader_2);*/

	std::thread th1;
	std::thread th2;
	std::thread th3;
	std::thread th4;

	std::string str(arcg[1]);

	if (str == "p1")
	{
		th1 = std::thread(thread_writer_1);
		//std::this_thread::sleep_for(std::chrono::milliseconds(5000));
		//th2 = std::thread(thread_writer_2);
	}
	if (str == "p2")
	{
		th1 = std::thread(thread_writer_2);
	}
	else if (str == "s1")
	{
		th1 = std::thread(thread_reader_1);
	}
	else if (str == "s2")
	{
		th1 = std::thread(thread_reader_2);
	}

	char c = '0';
	while (c != 'q')
	{
		std::cin >> c;
	}
	return 0;
}