#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>

#include "ddsformat/TypeTopics.h"
#include "ddsformat/SizeTopics.h"

namespace _dds = eprosima::fastdds::dds;

char c = '1';

class Listener : public _dds::DataReaderListener
{
	void on_data_available(_dds::DataReader* reader) override
	{
		DDSData data;
		_dds::SampleInfo info;
		if (reader->take_next_sample(&data, &info) != ReturnCode_t::RETCODE_OK) return;
		if (!info.valid_data) return;
		std::cout << "----- Receive data ----\n";

		if (data.data_int().value().size() != 0)
		{
			std::cout << "data_int[0] = " << data.data_int().value()[0] << "\n";
			std::cout << "data_int["<< data.data_int().value().size() / 2 <<"] = " << data.data_int().value()[data.data_int().value().size()/2] << "\n";
			std::cout << "data_int[" << data.data_int().value().size() -1 <<"] = " << data.data_int().value()[data.data_int().value().size()-1] << "\n";
			std::cout << std::endl;
		}

		if (data.data_float().value().size() != 0)
		{
			std::cout << "data_float[0] = " << data.data_float().value()[0] << "\n";
			std::cout << "data_float["<< data.data_float().value().size() / 2 <<"] = " << data.data_float().value()[data.data_float().value().size() / 2] << "\n";
			std::cout << "data_float["<< data.data_float().value().size() - 1 <<"] = " << data.data_float().value()[data.data_float().value().size() - 1] << "\n";
			std::cout << std::endl;
		}
	}

	void on_subscription_matched( _dds::DataReader* reader, const _dds::SubscriptionMatchedStatus& info) override
	{
		(void)reader;
		if (info.current_count_change == 1)
		{
			std::cout << "Matched" << std::endl;
		}
		else if (info.current_count_change == -1)
		{
			std::cout << "Unmatched" << std::endl;
		}
	}
};

void func_write(_dds::DataWriter* writer, size_t s_i, size_t s_f)
{
	DDSData data;
	data.data_int().value().resize(s_i);
	data.data_int().quality().resize(s_i);
	data.data_float().value().resize(s_f);
	data.data_float().quality().resize(s_f);

	while (c != 'q')
	{
		std::vector<int>& vec_int = data.data_int().value();
		std::vector<int>& vec_float = data.data_int().value();

		writer->write(&data);

		for (auto& iter : vec_int)
		{
			if (iter > 100) { iter = 0; }
			else { iter += 1; };
		}

		for (auto& iter : vec_float)
		{
			if (iter > 100) { iter = 0; }
			else { iter += 0.1; };
		}


		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
};

int main(int argv, char* argc[])
{
	if (argv != 5) return -1;

	std::string command = argc[1];
	std::string name_topic = argc[2];
	size_t size_int = std::atoi(argc[3]);
	size_t size_float = std::atoi(argc[4]);

	_dds::DomainParticipant* participant = nullptr;
	_dds::Subscriber* sub = nullptr;
	_dds::Publisher* pub = nullptr;
	_dds::Topic* topic = nullptr;
	_dds::DataReader* reader = nullptr;
	_dds::DataWriter* writer = nullptr;
	Listener lisneter{};
	std::thread t;

	atech::common::SizeTopics::SetMaxSizeDataCollectionInt(size_int);
	atech::common::SizeTopics::SetMaxSizeDataCollectionFloat(size_float);

	for (;;)
	{
		participant = _dds::DomainParticipantFactory::get_instance()->create_participant(0, _dds::PARTICIPANT_QOS_DEFAULT);
		if (!participant)
		{
			std::cout << "Error create participant" << std::endl;
			break;
		}

		_dds::TypeSupport type_datadds(new DDSDataPubSubType());
		if (participant->register_type(type_datadds) != ReturnCode_t::RETCODE_OK)
		{
			std::cout << "Error registration type" << std::endl;
			break;
		}

		topic = participant->create_topic(name_topic, type_datadds->getName(), _dds::TOPIC_QOS_DEFAULT);
		if (!topic)
		{
			std::cout << "Error create topic" << std::endl;
			break;
		}

		if (command == "r")
		{
			sub = participant->create_subscriber(_dds::SUBSCRIBER_QOS_DEFAULT);
			if (!sub)
			{
				std::cout << "Error create subscriber" << std::endl;
				break;
			}
			
			reader = sub->create_datareader(topic, _dds::DATAREADER_QOS_DEFAULT, &lisneter);
			if (!reader)
			{
				std::cout << "Error create datareader" << std::endl;
				break;
			}
		}
		else if (command == "w")
		{
			pub = participant->create_publisher(_dds::PUBLISHER_QOS_DEFAULT);
			if (!pub)
			{
				std::cout << "Error create subscriber" << std::endl;
				break;
			}

			writer = pub->create_datawriter(topic, _dds::DATAWRITER_QOS_DEFAULT);
			if (!reader)
			{
				std::cout << "Error create datareader" << std::endl;
				break;
			}

			t = std::thread(func_write, writer, size_int, size_float);
		}
		

		std::cout << "START" << std::endl;

		
		while (c != 'q')
		{
			std::cin >> c;
		}

		if (t.joinable()) t.join();

		break;
	}

	if (writer)
	{
		pub->delete_datawriter(writer);
	}

	if (reader)
	{
		sub->delete_datareader(reader);
	}
	
	if (sub)
	{
		participant->delete_subscriber(sub);
	}

	if (pub)
	{
		participant->delete_publisher(pub);
	}

	if (topic)
	{
		participant->delete_topic(topic);
	}

	if (participant)
	{
		_dds::DomainParticipantFactory::get_instance()->delete_participant(participant);
	}
	return 0;
}