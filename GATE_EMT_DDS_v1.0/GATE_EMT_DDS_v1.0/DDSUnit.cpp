#include "DDSUnit.h"

/// --- SUBSCRIBER --- ///

DDSUnit* CreateDDSUnit(TypeDDSUnit type, ConfigDDSUnit config)
{
	DDSUnit* p = NULL;
	switch (type)
	{
	case TypeDDSUnit::SUBSCRIBER:
		p = new DDSUnit_Subscriber(config);
		break;
	case TypeDDSUnit::PUBLISHER:
		p = new DDSUnit_Publisher(config);
		break;
	default:
		p = NULL;
	}
	return p;
}

DDSUnit_Subscriber::DDSUnit_Subscriber(ConfigDDSUnit config) : config(config)
{
	GlobalStatus.store(StatusDDSUnit::START);
	std::string helpstr;
	readerkks = new KKSReader();
	SharedMemoryUnit = new SharedMemoryDDS();
	ReturnCode_t res;
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
		if (SharedMemoryUnit->CreateMemory(TypeData::BINAR, TypeDirection::EMTfromDDS, readerkks->size_discrete(), helpstr) != ResultReqest::OK)
		{
			GlobalStatus.store(StatusDDSUnit::ERROR_INIT);
		}

		uint32_t length = readerkks->size_analog();
		DynamicTypeBuilder_ptr base_type = DynamicTypeBuilderFactory::get_instance()->create_float32_builder();
		DynamicTypeBuilder_ptr builder = DynamicTypeBuilderFactory::get_instance()->create_sequence_builder(base_type.get(), length);
		builder->set_name(config.NameMemory + "_analog_data");
		DynamicType_ptr sequence_type = DynamicTypeBuilderFactory::get_instance()->create_type(builder.get()); //DynamicType_ptr sequence_type = builder->build();
		DynamicData_ptr data(DynamicDataFactory::get_instance()->create_data(sequence_type));
		TypeSupport m_type(new eprosima::fastrtps::types::DynamicPubSubType(sequence_type));
		
		/*uint32_t length = readerkks->size_analog();
		DynamicType_ptr base_type = DynamicTypeBuilderFactory::get_instance()->create_float32_type();
		DynamicTypeBuilder_ptr builder = DynamicTypeBuilderFactory::get_instance()->create_sequence_builder(base_type, length);
		builder->set_name(config.NameMemory + "analog_data");
		DynamicType_ptr sequence_type = builder->build();
		res = m_DynType_analog.SetDynamicType(sequence_type);*/


			/*std::vector<uint32_t> vec;
			vec.resize(readerkks->size_analog());
			DynamicType_ptr base_type = DynamicTypeBuilderFactory::get_instance()->create_float32_type();
			DynamicTypeBuilder_ptr builder_type = DynamicTypeBuilderFactory::get_instance()->create_array_builder(base_type, vec);
			res = builder_type->set_name("analog_data");
			base_type_array_analog = builder_type->build();
			res = m_DynType_analog.SetDynamicType(base_type_array_analog);*/
		
			
			/*DynamicTypeBuilder_ptr struct_type_builder(DynamicTypeBuilderFactory::get_instance()->create_struct_builder());

			// Add members to the struct.
			struct_type_builder->add_member(0, "index", DynamicTypeBuilderFactory::get_instance()->create_uint32_type());
			struct_type_builder->add_member(1, "message", DynamicTypeBuilderFactory::get_instance()->create_string_type());
			struct_type_builder->set_name("HelloWorld");

			DynamicType_ptr dynType = struct_type_builder->build();
			m_DynType_analog.SetDynamicType(dynType);
			//eprosima::fastrtps::types::DynamicData* m_DynHello = DynamicDataFactory::get_instance()->create_data(dynType);
			//m_DynHello->set_uint32_value(0, 0);
			//m_DynHello->set_string_value("HelloWorld", 1);*/
			



		/*{
			std::vector<uint32_t> vec;
			vec.resize(readerkks->size_discrete());
			DynamicType_ptr base_type = DynamicTypeBuilderFactory::get_instance()->create_int32_type();
			DynamicTypeBuilder_ptr builder_type = DynamicTypeBuilderFactory::get_instance()->create_array_builder(base_type, vec);
			res = builder_type->set_name("discrete_data");
			base_type_array_discrete = builder_type->build();
			res = m_DynType_discrete.SetDynamicType(base_type_array_discrete);
		}

		{
			std::vector<uint32_t> vec;
			vec.resize(readerkks->size_binar());
			DynamicType_ptr base_type = DynamicTypeBuilderFactory::get_instance()->create_float32_type();
			DynamicTypeBuilder_ptr builder_type = DynamicTypeBuilderFactory::get_instance()->create_array_builder(base_type, vec);
			res = builder_type->set_name("binar_data");
			base_type_array_binar = builder_type->build();
			res = m_DynType_binar.SetDynamicType(base_type_array_binar);
		}*/
		
		DomainParticipantQos participantQos;
		participantQos.name("Participant_subscriber");
		participant_ = DomainParticipantFactory::get_instance()->create_participant(config.Domen, participantQos);
		if (participant_ == nullptr)
		{
			GlobalStatus.store(StatusDDSUnit::ERROR_INIT);
			log->WriteLogWARNING("ERROR_INIT_PARTICIANT_PUBLISHER", 0, 0);
		}

		try 
		{
			m_type.get()->auto_fill_type_information(false);
			m_type.get()->auto_fill_type_object(true);

			res = m_type.register_type(participant_);
		}
		catch (...)
		{

		}

		if ( res != ReturnCode_t::RETCODE_OK)
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

		std::thread t;
		t = std::thread(&DDSUnit_Subscriber::thread_transmite, this, TypeData::ANALOG);
		t.detach();
		/*t = std::thread(&DDSUnit_Subscriber::thread_transmite, this, TypeData::DISCRETE);
		t.detach();
		t = std::thread(&DDSUnit_Subscriber::thread_transmite, this, TypeData::BINAR);
		t.detach();*/

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


DDSUnit_Subscriber::~DDSUnit_Subscriber()
{
}

ResultReqest DDSUnit_Subscriber::Stop()
{
	return ResultReqest::ERR;
};

ResultReqest DDSUnit_Subscriber::Start()
{
	return ResultReqest::ERR;
};

StatusDDSUnit DDSUnit_Subscriber::GetCurrentStatus()
{
	StatusDDSUnit a = StatusDDSUnit::EMPTY;
	return a;
};

ConfigDDSUnit DDSUnit_Subscriber::GetConfig()
{
	ConfigDDSUnit a;
	return a;
};

ResultReqest DDSUnit_Subscriber::SetConfig()
{
	return ResultReqest::ERR;
};

ResultReqest DDSUnit_Subscriber::Restart()
{
	return ResultReqest::ERR;
};

void DDSUnit_Subscriber::Delete()
{
	return;
};

TypeDDSUnit DDSUnit_Subscriber::MyType()
{
	TypeDDSUnit a = TypeDDSUnit::SUBSCRIBER;
	return a;
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
		if (SharedMemoryUnit->CreateMemory(TypeData::BINAR, TypeDirection::EMTtoDDS, readerkks->size_discrete(), helpstr) != ResultReqest::OK)
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

		std::thread t;
		t = std::thread(&DDSUnit_Publisher::thread_transmite, this, TypeData::ANALOG);
		t.detach();
		/*t = std::thread(&DDSUnit_Publisher::thread_transmite, this, TypeData::DISCRETE);
		t.detach();
		t = std::thread(&DDSUnit_Publisher::thread_transmite, this, TypeData::BINAR);
		t.detach();*/
	    
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

DDSUnit_Publisher::~DDSUnit_Publisher()
{
}

ResultReqest DDSUnit_Publisher::Stop()
{
	return ResultReqest::ERR;
};

ResultReqest DDSUnit_Publisher::Start()
{
	return ResultReqest::ERR;
};

StatusDDSUnit DDSUnit_Publisher::GetCurrentStatus()
{
	StatusDDSUnit a = StatusDDSUnit::EMPTY;
	return a;
};

ConfigDDSUnit DDSUnit_Publisher::GetConfig()
{
	ConfigDDSUnit a;
	return a;
};

ResultReqest DDSUnit_Publisher::SetConfig()
{
	return ResultReqest::ERR;
};

ResultReqest DDSUnit_Publisher::Restart()
{
	return ResultReqest::ERR;
};

void DDSUnit_Publisher::Delete()
{
	return;
};

TypeDDSUnit DDSUnit_Publisher::MyType()
{
	TypeDDSUnit a = TypeDDSUnit::PUBLISHER;
	return a;
};