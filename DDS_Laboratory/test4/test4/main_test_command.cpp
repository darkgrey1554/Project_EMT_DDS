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

#include <ddsformat/DdsCommand/DdsCommand.h>
#include <ddsformat/DdsStatus/DdsStatus.h>
#include <ddsformat/DdsConfig/DdsConfig.h>

#include <ddsformat/DdsCommand/DdsCommandPubSubTypes.h>
#include <ddsformat/DdsStatus/DdsStatusPubSubTypes.h>
#include <ddsformat/DdsConfig/DdsConfigPubSubTypes.h>
#include <ServiceType.h>

#include <deque>
#include "json.hpp"
#include <fstream>
#include <istream>

using namespace eprosima::fastdds::dds;

atech::common::Command str_to_cmd(std::string str)
{
	atech::common::Command cmd{ atech::common::Command::NONE };

	if (str == "SEND_STATUS")
	{
		cmd = atech::common::Command::SEND_STATUS;
	}
	else if (str == "START")
	{
		cmd = atech::common::Command::START;
	}
	else if (str == "STOP")
	{
		cmd = atech::common::Command::STOP;
	}
	else if (str == "APPLY_CONFIG")
	{
		cmd = atech::common::Command::APPLY_CONFIG;
	}
	else if (str == "RECEIVE_CONFIG")
	{
		cmd = atech::common::Command::RECEIVE_CONFIG;
	}
	else if (str == "RESET")
	{
		cmd = atech::common::Command::RESET;
	}
	else if (str == "SEND_PROCESS_INFO")
	{
		cmd = atech::common::Command::SEND_PROCESS_INFO;
	}
	else if (str == "SEND_VERSION")
	{
		cmd = atech::common::Command::SEND_VERSION;
	}
	else if (str == "SET_LOGGING_LEVEL")
	{
		cmd = atech::common::Command::SET_LOGGING_LEVEL;
	}

	return cmd;
}

std::string create_str_cmd1(std::vector<uint32_t>& vect)
{
	nlohmann::json json;
	json["id"] = vect;
	return json.dump();
}

std::string create_str_cmd2(uint32_t size)
{
	nlohmann::json json;
	json["size"] = size;
	return json.dump();
}


class Listener : public DataReaderListener
{
	public :
	Listener() {};
	~Listener() {};
	void on_subscription_matched(DataReader*, const SubscriptionMatchedStatus& info) override;
	void on_data_available(DataReader* reader) override;
};

void Listener::on_subscription_matched(DataReader*, const SubscriptionMatchedStatus& info)
{
	if (info.current_count_change == 1)
	{
		std::cout <<  "Subscriber mathed" << std::endl;
	}
	else if (info.current_count_change == -1)
	{
		std::cout << "Subscriber unmathed" << std::endl;
	}

	return;
}

void Listener::on_data_available(DataReader* reader)
{
	DdsStatus data;
	SampleInfo info;
	reader->take_next_sample(&data, &info);
	if (info.valid_data)
	{
		std::cout << "Message:" << std::endl;
		std::cout << "id_source: " << data.id_source() << std::endl;
		std::cout << "id_target: " << data.id_target() << std::endl;
		std::cout << "code: " << data.cmd_code() << std::endl;
		std::cout << "status: " << data.st_code() << std::endl;
		std::cout << "parametrs:\n" << (const char*)&data.st_desc()[0] << std::endl;
	}		

	return;
}

//void thread_read(DataReader* reader)
//{
//	
//}

int main()
{

	std::ifstream config;
	config.open("config.json", std::ios::in);
	nlohmann::json json = nlohmann::json::parse(config);
	atech::common::SizeTopics::SetMaxSizeDdsConfig(json.dump().size());



	DomainParticipant* participant_pub;
	participant_pub = DomainParticipantFactory::get_instance()->create_participant(0, PARTICIPANT_QOS_DEFAULT);

	TypeSupport type_command = TypeSupport(new DdsCommandPubSubType());
	type_command.register_type(participant_pub);
	TypeSupport type_status = TypeSupport(new DdsStatusPubSubType());
	type_status.register_type(participant_pub);
	TypeSupport type_config = TypeSupport(new DdsConfigPubSubType());
	type_config.register_type(participant_pub);

	Topic* topic_command = participant_pub->create_topic("dds_command", type_command.get_type_name(), TOPIC_QOS_DEFAULT);
	Topic* topic_status = participant_pub->create_topic("dds_status", type_status.get_type_name(), TOPIC_QOS_DEFAULT);
	Topic* topic_config = participant_pub->create_topic("dds_config", type_config.get_type_name(), TOPIC_QOS_DEFAULT);

	Subscriber* sub = participant_pub->create_subscriber(SUBSCRIBER_QOS_DEFAULT);
	Publisher* pub = participant_pub->create_publisher(PUBLISHER_QOS_DEFAULT);

	Listener _listener;
	DataReader* reader_status = sub->create_datareader(topic_status, DATAREADER_QOS_DEFAULT, &_listener);
	DataWriter* commander = pub->create_datawriter(topic_command, DATAWRITER_QOS_DEFAULT);
	DataWriter* writer_config = pub->create_datawriter(topic_config, DATAWRITER_QOS_DEFAULT);

	DdsConfig str_config;
	str_config.id_source(0);
	str_config.id_target(0x00010101);
	auto& vec_data = str_config.conf_subject();
	std::string sconf = json.dump();
	vec_data.resize(sconf.size());
	auto it = vec_data.begin();
	for (auto& s : sconf)
	{
		*it = s;
		it++;
	}

	

	while (1)
	{
		std::string command_str;
		atech::common::Command command;
		
		std::vector<uint32_t> parametrs{};

		std::cout << "Enter the command:";
		std::cin >> command_str;
		while (std::cin.peek() == ' ')
		{
			size_t data;
			std::cin >> data;
			parametrs.push_back(data);
		} 

		if (command_str.empty()) continue;
		if (command_str.size() == 1)
		{
			if (command_str[0] == 'q') break;
		}

		command = str_to_cmd(command_str);
		if (command == atech::common::Command::NONE)
		{
			std::cout << "Command not defined" << std::endl;
			continue;
		}

		{
			DdsCommand cmd;
			cmd.cmd_code((uint32_t)command);
			cmd.id_source(0);	 
			cmd.id_target(0x00010101);
			std::string str{};
			if (command == atech::common::Command::SEND_STATUS ||
				command == atech::common::Command::START ||
				command == atech::common::Command::STOP ||
				command == atech::common::Command::RESET)
			{
				str = create_str_cmd1(parametrs);
			}
			else if (command == atech::common::Command::RECEIVE_CONFIG)
			{
				str = create_str_cmd2(sconf.size());
				if (str.empty())
				{
					std::cout << "Error paremetrs" << std::endl;
					continue;
				}

				writer_config->write(&str_config);
			}
			else
			{
				str.clear();
			}

			if (cmd.cmd_parameter().size() < str.size())
			{
				std::cout << "Error paremetrs" << std::endl;
				continue;
			}

			{
				volatile int i = 0;
				for (auto& it : str)
				{
					cmd.cmd_parameter()[i] = it;
					i++;
				}
			}
			
			commander->write(&cmd);
		} 		

		if (command == atech::common::Command::RECEIVE_CONFIG)
		{
			for (int i = 0; i < 10; i++)
			{
				writer_config->write(&str_config);
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			}
			
		}
	}

	sub->delete_datareader(reader_status);
	pub->delete_datawriter(commander);
	pub->delete_datawriter(writer_config);
	participant_pub->delete_publisher(pub);
	participant_pub->delete_subscriber(sub);
	participant_pub->delete_topic(topic_command);
	participant_pub->delete_topic(topic_status);
	participant_pub->delete_topic(topic_config);

	return 0;
}