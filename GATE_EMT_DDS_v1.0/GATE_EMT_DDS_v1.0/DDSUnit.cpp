#include "DDSUnit.h"

/// --- SUBSCRIBER --- ///

DDSUnit_Subscriber::DDSUnit_Subscriber(ConfigDDSUnit config) : config(config)
{
	GlobalStatus.store(StatusDDSUnit::START);
	std::string helpstr;
	readerkks = new KKSReader();
	SharedMemoryUnit = new SharedMemoryDDS();

	log = LoggerSpace::Logger::getpointcontact();


	if (readerkks->ReadKKSlist(config.NameListKKS) != ResultReqest::OK)
	{
		GlobalStatus.store(StatusDDSUnit::ERROR_INIT);
	}
	else
	{
		helpstr.clear();
		helpstr += CreateNameMemoryDDS(TypeData::ANALOG, TypeDirection::EMTfromDDS, config.Domen) + "_" + config.NameMemory;
		if (SharedMemoryUnit->CreateMemory(TypeData::ANALOG, TypeDirection::EMTfromDDS, readerkks->size_analog(), helpstr) != ResultReqest::OK)
		{
			GlobalStatus.store(StatusDDSUnit::ERROR_INIT);
		}

		helpstr.clear();
		helpstr += CreateNameMemoryDDS(TypeData::DISCRETE, TypeDirection::EMTfromDDS, config.Domen) + "_" + config.NameMemory;
		if (SharedMemoryUnit->CreateMemory(TypeData::DISCRETE, TypeDirection::EMTfromDDS, readerkks->size_discrete(), helpstr) != ResultReqest::OK)
		{
			GlobalStatus.store(StatusDDSUnit::ERROR_INIT);
		}

		helpstr.clear();
		helpstr += CreateNameMemoryDDS(TypeData::BINAR, TypeDirection::EMTfromDDS, config.Domen) + "_" + config.NameMemory;
		if (SharedMemoryUnit->CreateMemory(TypeData::DISCRETE, TypeDirection::EMTfromDDS, readerkks->size_discrete(), helpstr) != ResultReqest::OK)
		{
			GlobalStatus.store(StatusDDSUnit::ERROR_INIT);
		}

		/*ParticipantAttributes PParam;
		PParam.rtps.setName("Subscriber_unit");
		participant_ = Domain::createParticipant(PParam);
		if (participant_ == nullptr)
		{
			GlobalStatus.store(StatusDDSUnit::ERROR_INIT);
			log->WriteLogWARNING("ERROR_INIT_PARTICIANT_SUBSCRIBER", 0, 0);
		}*/

		{
			std::vector<uint32_t> vec;
			vec.resize(readerkks->size_analog());
			DynamicType_ptr base_type = DynamicTypeBuilderFactory::get_instance()->create_float32_type();
			DynamicTypeBuilder_ptr builder_type = DynamicTypeBuilderFactory::get_instance()->create_array_builder(base_type, vec);
			builder_type->set_name("analog_data");
			base_type_array_analog = builder_type->build();
			m_DynType_analog.SetDynamicType(base_type_array_analog);
		}

		{
			std::vector<uint32_t> vec;
			vec.resize(readerkks->size_discrete());
			DynamicType_ptr base_type = DynamicTypeBuilderFactory::get_instance()->create_int32_type();
			DynamicTypeBuilder_ptr builder_type = DynamicTypeBuilderFactory::get_instance()->create_array_builder(base_type, vec);
			builder_type->set_name("discrete_data");
			base_type_array_discrete = builder_type->build();
			m_DynType_discrete.SetDynamicType(base_type_array_discrete);
		}

		{
			std::vector<uint32_t> vec;
			vec.resize(readerkks->size_binar());
			DynamicType_ptr base_type = DynamicTypeBuilderFactory::get_instance()->create_float32_type();
			DynamicTypeBuilder_ptr builder_type = DynamicTypeBuilderFactory::get_instance()->create_array_builder(base_type, vec);
			builder_type->set_name("binar_data");
			base_type_array_binar = builder_type->build();
			m_DynType_binar.SetDynamicType(base_type_array_binar);
		}
		
		DomainParticipantQos participantQos;
		participantQos.name("Participant_publisher");
		participant_ = DomainParticipantFactory::get_instance()->create_participant(config.Domen, participantQos);
		if (participant_ == nullptr)
		{
			GlobalStatus.store(StatusDDSUnit::ERROR_INIT);
			log->WriteLogWARNING("ERROR_INIT_PARTICIANT_PUBLISHER", 0, 0);
		}

		if (participant_->register_type(m_DynType_analog) != ReturnCode_t::RETCODE_OK)
		{
			log->WriteLogWARNING("ERROR_REGISTER_TYPE_ANALOG");
		}
		if (participant_->register_type(m_DynType_discrete) != ReturnCode_t::RETCODE_OK)
		{
			log->WriteLogWARNING("ERROR_REGISTER_TYPE_DISCRETE");
		}
		if (participant_->register_type(m_DynType_binar) != ReturnCode_t::RETCODE_OK)
		{
			log->WriteLogWARNING("ERROR_REGISTER_TYPE_BINAR");
		}

		helpstr.clear();
		helpstr += config.NameMemory + "Analog_Data";
		topic_analog = participant_->create_topic(helpstr, "analog_data", TOPIC_QOS_DEFAULT);
		if (topic_analog == nullptr)
		{
			log->WriteLogWARNING("ERROR CREATE TOPIC ANALOG (SUBSCRIBER)", 0, 0);
		}

		helpstr.clear();
		helpstr += config.NameMemory + "Discrete_Data";
		topic_discrete = participant_->create_topic(helpstr, "discrete_data", TOPIC_QOS_DEFAULT);
		if (topic_discrete == nullptr)
		{
			log->WriteLogWARNING("ERROR CREATE TOPIC DISCRETE (SUBSCRIBER)", 0, 0);
		}

		helpstr.clear();
		helpstr += config.NameMemory + "Binar_Data";
		topic_binar = participant_->create_topic(helpstr, "binar_data", TOPIC_QOS_DEFAULT);
		if (topic_binar == nullptr)
		{
			log->WriteLogWARNING("ERROR CREATE TOPIC BINAR (SUBSCRIBER)", 0, 0);
		}

		std::thread t(&thread_transmite, TypeData::ANALOG);
		t.detach();
		t = std::thread(&thread_transmite, TypeData::DISCRETE);
		t.detach();
		t = std::thread(&thread_transmite, TypeData::BINAR);
		t.detach();

	}
}

void DDSUnit_Subscriber::thread_transmite(TypeData type_data_thread)
{
	DataReader* reader = nullptr;
	DynamicData_ptr data;
	float iterf = 0.0;
	unsigned int iteri = 0;
	char iterc = 0;
	SampleInfo info;
	ReturnCode_t res;


	switch (type_data_thread)
	{
	case TypeData::ZERO:
		break;
	case TypeData::ANALOG:

		reader = subscriber_->create_datareader(topic_analog, DATAREADER_QOS_DEFAULT);
		data = DynamicDataFactory::get_instance()->create_data(base_type_array_analog);

		break;
	case TypeData::DISCRETE:

		reader = subscriber_->create_datareader(topic_discrete, DATAREADER_QOS_DEFAULT);
		data = DynamicDataFactory::get_instance()->create_data(base_type_array_discrete);

		break;
	case TypeData::BINAR:

		reader = subscriber_->create_datareader(topic_binar, DATAREADER_QOS_DEFAULT);
		data = DynamicDataFactory::get_instance()->create_data(base_type_array_binar);

		break;
	default:
		break;
	}

	if (reader == nullptr) return;

	for (;;)
	{

		res = reader->take_next_sample(&data, &info);

		for (int i = 0; i < 10; i++)
		{
			

			switch (type_data_thread)
			{
			case TypeData::ZERO:
				break;
			case TypeData::ANALOG:

				iterf = data->get_float32_value(i);
				std::cout << "ANALOG_DATA::" << iterf <<std::endl;
				break;
			case TypeData::DISCRETE:

				iteri = data->get_int32_value(i);
				std::cout << "DISCRETE_DATA::" << iteri << std::endl;
				break;
			case TypeData::BINAR:
				iterc = data->get_char8_value(i);
				std::cout << "BINAR_DATA::" << iterc << std::endl;
				break;
			default:
				break;
			}

			
		}

		std::this_thread::sleep_for(1000ms);

	}

	reader->~DataReader();
	return;

};


/// --- PUBLISHER --- ///

DDSUnit_Publisher::DDSUnit_Publisher(ConfigDDSUnit config) : config(config)
{
	GlobalStatus.store(StatusDDSUnit::START);
	std::string helpstr;
	readerkks = new KKSReader();
	SharedMemoryUnit = new SharedMemoryDDS();
	ReturnCode_t result_eprosima;

	log = LoggerSpace::Logger::getpointcontact();


	if (readerkks->ReadKKSlist(config.NameListKKS) != ResultReqest::OK)
	{
		GlobalStatus.store(StatusDDSUnit::ERROR_INIT);
	}
	else
	{
		helpstr.clear();
		helpstr += CreateNameMemoryDDS(TypeData::ANALOG, TypeDirection::EMTtoDDS, config.Domen) + "_" + config.NameMemory;
		if (SharedMemoryUnit->CreateMemory(TypeData::ANALOG, TypeDirection::EMTtoDDS, readerkks->size_analog(), helpstr) != ResultReqest::OK)
		{
			GlobalStatus.store(StatusDDSUnit::ERROR_INIT);
		}

		helpstr.clear();
		helpstr += CreateNameMemoryDDS(TypeData::DISCRETE, TypeDirection::EMTtoDDS, config.Domen) + "_" + config.NameMemory;
		if (SharedMemoryUnit->CreateMemory(TypeData::DISCRETE, TypeDirection::EMTtoDDS, readerkks->size_discrete(), helpstr) != ResultReqest::OK)
		{
			GlobalStatus.store(StatusDDSUnit::ERROR_INIT);
		}

		helpstr.clear();
		helpstr += CreateNameMemoryDDS(TypeData::BINAR, TypeDirection::EMTtoDDS, config.Domen) + "_" + config.NameMemory;
		if (SharedMemoryUnit->CreateMemory(TypeData::DISCRETE, TypeDirection::EMTtoDDS, readerkks->size_discrete(), helpstr) != ResultReqest::OK)
		{
			GlobalStatus.store(StatusDDSUnit::ERROR_INIT);
		}

		/*ParticipantAttributes PParam;
		PParam.rtps.setName("Subscriber_unit");
		participant_ = Domain::createParticipant(PParam);
		if (participant_ == nullptr)
		{
			GlobalStatus.store(StatusDDSUnit::ERROR_INIT);
			log->WriteLogWARNING("ERROR_INIT_PARTICIANT_SUBSCRIBER", 0, 0);
		}*/

		{
			std::vector<uint32_t> vec;
			vec.resize(readerkks->size_analog());
			DynamicType_ptr base_type = DynamicTypeBuilderFactory::get_instance()->create_float32_type();
			DynamicTypeBuilder_ptr builder_type = DynamicTypeBuilderFactory::get_instance()->create_array_builder(base_type, vec);
			builder_type->set_name("analog_data");
			base_type_array_analog = builder_type->build();
			m_DynType_analog.SetDynamicType(base_type_array_analog);
			
			//Domain::registerDynamicType(participant_, &m_DynType_analog);
		}

		{
			std::vector<uint32_t> vec;
			vec.resize(readerkks->size_discrete());
			DynamicType_ptr base_type = DynamicTypeBuilderFactory::get_instance()->create_int32_type();
			DynamicTypeBuilder_ptr builder_type = DynamicTypeBuilderFactory::get_instance()->create_array_builder(base_type, vec);
			builder_type->set_name("discrete_data");
			base_type_array_discrete = builder_type->build();
			m_DynType_discrete.SetDynamicType(base_type_array_discrete);
			//Domain::registerDynamicType(participant_, &m_DynType_discrete);
		}

		{
			std::vector<uint32_t> vec;
			vec.resize(readerkks->size_binar());
			DynamicType_ptr base_type = DynamicTypeBuilderFactory::get_instance()->create_float32_type();
			DynamicTypeBuilder_ptr builder_type = DynamicTypeBuilderFactory::get_instance()->create_array_builder(base_type, vec);
			builder_type->set_name("binar_data");
			base_type_array_binar = builder_type->build();
			m_DynType_binar.SetDynamicType(base_type_array_binar);			
		}

		/*{
			helpstr.clear();
			helpstr += "ANALOG" + config.NameMemory;
			SubscriberAttributes Rparam;
			Rparam.topic.topicKind = NO_KEY;
			Rparam.topic.topicDataType = "analog_data";
			Rparam.topic.topicName = helpstr;
			subscriber_ = participant_
		}*/

		DomainParticipantQos participantQos;
		participantQos.name("Participant_publisher");
		participant_ = DomainParticipantFactory::get_instance()->create_participant(config.Domen, participantQos);
		if (participant_ == nullptr)
		{
			GlobalStatus.store(StatusDDSUnit::ERROR_INIT);
			log->WriteLogWARNING("ERROR_INIT_PARTICIANT_PUBLISHER", 0, 0);
			return;
		}

		if (participant_->register_type(m_DynType_analog) != ReturnCode_t::RETCODE_OK)
		{
			log->WriteLogWARNING("ERROR_REGISTER_TYPE_ANALOG");
		}
		if (participant_->register_type(m_DynType_discrete) != ReturnCode_t::RETCODE_OK)
		{
			log->WriteLogWARNING("ERROR_REGISTER_TYPE_ANALOG");
		}
		if (participant_->register_type(m_DynType_discrete) != ReturnCode_t::RETCODE_OK)
		{
			log->WriteLogWARNING("ERROR_REGISTER_TYPE_ANALOG");
		}

		helpstr.clear();
		helpstr += config.NameMemory + "Analog_Data";
		topic_analog = participant_->create_topic(helpstr,"analog_data", TOPIC_QOS_DEFAULT);
		if (topic_analog == nullptr)
		{
			log->WriteLogWARNING("ERROR CREATE TOPIC ANALOG (PUBLISHER)", 0, 0);
		}

		helpstr.clear();
		helpstr += config.NameMemory + "Discrete_Data";
		topic_discrete = participant_->create_topic(helpstr, "discrete_data", TOPIC_QOS_DEFAULT);
		if (topic_discrete == nullptr)
		{
			log->WriteLogWARNING("ERROR CREATE TOPIC DISCRETE (PUBLISHER)", 0, 0);
		}

		helpstr.clear();
		helpstr += config.NameMemory + "Binar_Data";
		topic_binar = participant_->create_topic(helpstr, "binar_data", TOPIC_QOS_DEFAULT);
		if (topic_binar == nullptr)
		{
			log->WriteLogWARNING("ERROR CREATE TOPIC BINAR (PUBLISHER)", 0, 0);
		}

		publisher_ = participant_->create_publisher(PUBLISHER_QOS_DEFAULT, nullptr);

		if (publisher_ == nullptr)
		{
			log->WriteLogWARNING("ERROR INIT PUBSISHER", 0, 0);
			return;
		}

		std::thread t(&thread_transmite, TypeData::ANALOG);
		t.detach();
		t = std::thread(&thread_transmite, TypeData::DISCRETE);
		t.detach();
		t = std::thread(&thread_transmite, TypeData::BINAR);
		t.detach();
	    
	}

}

void DDSUnit_Publisher::thread_transmite(TypeData type_data_thread)
{
	DataWriter* writer = nullptr;
	DynamicData_ptr data;
	float iterf = 0.0;
	unsigned int iteri = 0;
	char iterc = 0;


	switch (type_data_thread)
	{
	case TypeData::ZERO:
		break;
	case TypeData::ANALOG:

		writer = publisher_->create_datawriter(topic_analog, DATAWRITER_QOS_DEFAULT);
		data = DynamicDataFactory::get_instance()->create_data(base_type_array_analog);

		break;
	case TypeData::DISCRETE:

		writer = publisher_->create_datawriter(topic_discrete, DATAWRITER_QOS_DEFAULT);
		data = DynamicDataFactory::get_instance()->create_data(base_type_array_discrete);

		break;
	case TypeData::BINAR:

		writer = publisher_->create_datawriter(topic_binar, DATAWRITER_QOS_DEFAULT);
		data = DynamicDataFactory::get_instance()->create_data(base_type_array_binar);

		break;
	default:
		break;
	}

	if (writer == nullptr) return;

	for (;;)
	{
		for (int i = 0; i < 10; i++)
		{
			switch (type_data_thread)
			{
			case TypeData::ZERO:
				break;
			case TypeData::ANALOG:

				data->set_float32_value(iterf, i);
				break;
			case TypeData::DISCRETE:

				data->set_int32_value(iteri, i);

				break;
			case TypeData::BINAR:

				data->set_char8_value(iterc, i);
				break;
			default:
				break;
			}
		}


		writer->write(&data);
		
		std::this_thread::sleep_for(1000ms);

	}

	writer->~DataWriter();
	return;

};
