#include "DDSUnit.h"

/// --- SUBSCRIBER --- ///

DDSUnit_Subcriber::DDSUnit_Subcriber(ConfigDDSUnit config) : config(config)
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
			DynamicType_ptr base_type_array = builder_type->build();
			m_DynType_analog.SetDynamicType(base_type_array);
			data_analog = DynamicDataFactory::get_instance()->create_data(base_type_array);
			Domain::registerDynamicType(participant_ , &m_DynType_analog);
		}

		{
			std::vector<uint32_t> vec;
			vec.resize(readerkks->size_discrete());
			DynamicType_ptr base_type = DynamicTypeBuilderFactory::get_instance()->create_int32_type();
			DynamicTypeBuilder_ptr builder_type = DynamicTypeBuilderFactory::get_instance()->create_array_builder(base_type, vec);
			DynamicType_ptr base_type_array = builder_type->build();
			builder_type->set_name("discrete_data");
			m_DynType_discrete.SetDynamicType(base_type_array);
			data_discrete = DynamicDataFactory::get_instance()->create_data(base_type_array);
			Domain::registerDynamicType(participant_, &m_DynType_discrete);
		}

		{
			std::vector<uint32_t> vec;
			vec.resize(readerkks->size_binar());
			DynamicType_ptr base_type = DynamicTypeBuilderFactory::get_instance()->create_float32_type();
			DynamicTypeBuilder_ptr builder_type = DynamicTypeBuilderFactory::get_instance()->create_array_builder(base_type, vec);
			DynamicType_ptr base_type_array = builder_type->build();
			builder_type->set_name("binar_data");
			m_DynType_binar.SetDynamicType(base_type_array);
			data_binar = DynamicDataFactory::get_instance()->create_data(base_type_array);
			Domain::registerDynamicType(participant_, &m_DynType_binar);
		}
		
		{
			helpstr.clear();
			helpstr += "ANALOG" + config.NameMemory;
			SubscriberAttributes Rparam;
			Rparam.topic.topicKind = NO_KEY;
			Rparam.topic.topicDataType = "analog_data";
			Rparam.topic.topicName = helpstr;
			subscriber_ = participant_
		}

	}
	



}















/// --- PUBLISHER --- ///

DDSUnit_Publisher::DDSUnit_Publisher(ConfigDDSUnit config) : config(config)
{
	GlobalStatus.store(StatusDDSUnit::START);
	std::string helpstr;
	readerkks = new KKSReader();
	SharedMemoryUnit = new SharedMemoryDDS();
	eprosima::

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
			DynamicType_ptr base_type_array = builder_type->build();
			m_DynType_analog.SetDynamicType(base_type_array);
			data_analog = DynamicDataFactory::get_instance()->create_data(base_type_array);
			//Domain::registerDynamicType(participant_, &m_DynType_analog);
		}

		{
			std::vector<uint32_t> vec;
			vec.resize(readerkks->size_discrete());
			DynamicType_ptr base_type = DynamicTypeBuilderFactory::get_instance()->create_int32_type();
			DynamicTypeBuilder_ptr builder_type = DynamicTypeBuilderFactory::get_instance()->create_array_builder(base_type, vec);
			DynamicType_ptr base_type_array = builder_type->build();
			builder_type->set_name("discrete_data");
			m_DynType_discrete.SetDynamicType(base_type_array);
			data_discrete = DynamicDataFactory::get_instance()->create_data(base_type_array);
			//Domain::registerDynamicType(participant_, &m_DynType_discrete);
		}

		{
			std::vector<uint32_t> vec;
			vec.resize(readerkks->size_binar());
			DynamicType_ptr base_type = DynamicTypeBuilderFactory::get_instance()->create_float32_type();
			DynamicTypeBuilder_ptr builder_type = DynamicTypeBuilderFactory::get_instance()->create_array_builder(base_type, vec);
			DynamicType_ptr base_type_array = builder_type->build();
			builder_type->set_name("binar_data");
			m_DynType_binar.SetDynamicType(base_type_array);
			data_binar = DynamicDataFactory::get_instance()->create_data(base_type_array);
			//Domain::registerDynamicType(participant_, &m_DynType_binar);
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
		}

		participant_->register_type(m_DynType_analog);
		participant_->register_type(m_DynType_discrete);
		participant_->register_type(m_DynType_binar);

		topic_analog = participant_->

	    
	}





}