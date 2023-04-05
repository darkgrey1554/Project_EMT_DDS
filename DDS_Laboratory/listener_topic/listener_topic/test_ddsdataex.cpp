#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/DataWriterListener.hpp>]

#include "ddsformat/TypeTopics.h"
#include "ddsformat/SizeTopics.h"
#include <TimeConverter.hpp>

namespace _dds = eprosima::fastdds::dds;

char c = '1';

class Listener : public _dds::DataReaderListener
{
	void on_data_available(_dds::DataReader* reader) override
	{
		DDSDataEx data;
		_dds::SampleInfo info;
		if (reader->take_next_sample(&data, &info) != ReturnCode_t::RETCODE_OK) return;
		if (!info.valid_data) return;
		std::cout << "----- Receive data ----\n";

		if (data.data_int().size() != 0)
		{
			std::cout << "DATA INT, size : " << data.data_int().size() << "\n";
			std::cout << "data_int[0] = \n";
			std::cout << "\tid_tad: " << data.data_int()[0].id_tag() << "\n";
			std::cout << "\tvalue: " << data.data_int()[0].value() << "\n";
			std::cout << "data_int[" << data.data_int().size() - 1 << "] = \n";
			std::cout << "\tid_tad: " << data.data_int()[data.data_int().size() - 1].id_tag() << "\n";
			std::cout << "\tvalue: " << data.data_int()[data.data_int().size() - 1].value() << "\n";
			std::cout << std::endl;
		}

		if (data.data_float().size() != 0)
		{
			std::cout << "DATA FLOAT, size : " << data.data_float().size() << "\n";
			std::cout << "data_float[0] = \n";
			std::cout << "\tid_tad: " << data.data_float()[0].id_tag() << "\n";
			std::cout << "\tvalue: " << data.data_float()[0].value() << "\n";
			std::cout << "data_float[" << data.data_float().size() - 1 << "] = \n";
			std::cout << "\tid_tad: " << data.data_float()[data.data_float().size() - 1].id_tag() << "\n";
			std::cout << "\tvalue: " << data.data_float()[data.data_float().size() - 1].value() << "\n";
			std::cout << std::endl;
		}
	}

	void on_subscription_matched(_dds::DataReader* reader, const _dds::SubscriptionMatchedStatus& info) override
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

class ListenerW : public _dds::DataWriterListener
{

	void on_publication_matched(_dds::DataWriter* reader, const _dds::PublicationMatchedStatus& info) override
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
	DDSDataEx data;
	data.data_int().resize(s_i);
	data.data_float().resize(s_f);

	std::vector<DataExInt>& vec_int = data.data_int();
	std::vector<DataExFloat>& vec_float = data.data_float();

	for (int i = 0; i < s_i; i++)
	{
		vec_int[i].id_tag()=i;
		vec_int[i].value()=i;
		vec_int[i].quality() = 0;
		vec_int[i].time_source() = TimeConverter::GetTime_LLmcs();
	}

	for (int i = 0; i < s_f; i++)
	{
		vec_float[i].id_tag() = s_i+i;
		vec_float[i].value() = i;
		vec_float[i].quality() = 0;
		vec_float[i].time_source() = TimeConverter::GetTime_LLmcs();
	}

	int counter = 1;
	int cont = 1;
	while (c != 'q')
	{

		writer->write(&data); 

		data.data_int().resize(s_i/counter);
		data.data_float().resize(s_f/counter);

		for (int i = 0; i < s_i; i += counter)
		{
			vec_int[i].id_tag() = i;
			vec_int[i].value() = cont+1+i;
			vec_int[i].quality() = 0;
			vec_int[i].time_source() = TimeConverter::GetTime_LLmcs();
		}

		for (int i = 0; i < s_f; i += counter)
		{
			vec_float[i].id_tag() = s_i+i;
			vec_float[i].value() = cont+i+0.1;
			vec_float[i].quality() = 0;
			vec_float[i].time_source() = TimeConverter::GetTime_LLmcs();
		}

		counter = counter % 2 + 1;
		cont++;
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
	ListenerW lisneterw{};
	std::thread t;

	atech::common::SizeTopics::SetMaxSizeDataCollectionInt(size_int);
	atech::common::SizeTopics::SetMaxSizeDataCollectionFloat(size_float);
	atech::common::SizeTopics::SetMaxSizeDDSDataExVectorInt(size_int);
	atech::common::SizeTopics::SetMaxSizeDDSDataExVectorFloat(size_float);

	for (;;)
	{
		participant = _dds::DomainParticipantFactory::get_instance()->create_participant(0, _dds::PARTICIPANT_QOS_DEFAULT);
		if (!participant)
		{
			std::cout << "Error create participant" << std::endl;
			break;
		}

		_dds::TypeSupport type_datadds(new DDSDataExPubSubType());
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

			writer = pub->create_datawriter(topic, _dds::DATAWRITER_QOS_DEFAULT, &lisneterw);
			if (!writer)
			{
				std::cout << "Error create datawrite" << std::endl;
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