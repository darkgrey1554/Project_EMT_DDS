#include "DDSUnit.h"


namespace gate
{
	std::shared_ptr<DDSUnit> CreateDDSUnit(ConfigDDSUnit config)
	{
		std::shared_ptr<DDSUnit> p = nullptr;
		switch (config.TypeUnit)
		{
		case TypeDDSUnit::SUBSCRIBER:
			p = std::make_shared<DDSUnit_Subscriber>(config);
			break;
		case TypeDDSUnit::PUBLISHER:
			p = std::make_shared<DDSUnit_Publisher>(config);
			break;
		default:
			p = NULL;
		}
		return p;
	}

	/// --- SUBSCRIBER --- ///

	DDSUnit_Subscriber::DDSUnit_Subscriber(ConfigDDSUnit config) : config(config)
	{
		SetStatus(StatusDDSUnit::START);
		log = LoggerSpace::Logger::getpointcontact();
		std::string helpstr;

		/// --- инициализация транспортного уровня --- ///
		///--------------------------------------------///
		//////////////////////////////////////////////////

		/// --- иницализация participant --- /// 

		participant_ = std::make_shared<DomainParticipant>
			(DomainParticipantFactory::get_instance()->create_participant(this->config.Domen, PARTICIPANT_QOS_DEFAULT, nullptr));
		if (!participant_)
		{
			helpstr.clear();
			helpstr += "Error init DDSUnit: error create of participant, name units: " + this->config.PointName;
			log->WriteLogERR(helpstr.c_str(), 0, 0);
			SetStatus(StatusDDSUnit::ERROR_INIT);
			return;
		}

		/// --- инициализация subscriber --- ///

		if (this->config.Frequency > 0)
		{
			subscriber_ = std::make_shared<eprosima::fastdds::dds::Subscriber>
				(participant_->create_subscriber(SUBSCRIBER_QOS_DEFAULT, nullptr));
		}
		else
		{
			subscriber_ = std::make_shared<eprosima::fastdds::dds::Subscriber>
				(participant_->create_subscriber(SUBSCRIBER_QOS_DEFAULT, (SubscriberListener*)&listener_));
		}

		if (!subscriber_)
		{
			helpstr.clear();
			helpstr += "Error init DDSUnit: error create of subscriber, name units: " + this->config.PointName;
			log->WriteLogERR(helpstr.c_str(), 0, 0);
			SetStatus(StatusDDSUnit::ERROR_INIT);
			return;
		}

		/// --- создание динамического типа --- ///
		/* struct:
		*	typedata : uint8; (parent TypeData)
		*	TimeLastUpdate :
				h : char8;
				m : char8;
				s : char8;
				ms: unit16;
			size_data : uint32;
			count_write : uint32;
			data [size_data] : uint32/float;
		*/

		DynamicTypeBuilder_ptr created_type_typedata = DynamicTypeBuilderFactory::get_instance()->create_char8_builder();
		DynamicTypeBuilder_ptr created_type_TimeLastUpdate_h = DynamicTypeBuilderFactory::get_instance()->create_char8_builder();
		DynamicTypeBuilder_ptr created_type_TimeLastUpdate_m = DynamicTypeBuilderFactory::get_instance()->create_char8_builder();
		DynamicTypeBuilder_ptr created_type_TimeLastUpdate_s = DynamicTypeBuilderFactory::get_instance()->create_char8_builder();
		DynamicTypeBuilder_ptr created_type_TimeLastUpdate_ms = DynamicTypeBuilderFactory::get_instance()->create_uint16_builder();
		DynamicTypeBuilder_ptr created_type_size_data = DynamicTypeBuilderFactory::get_instance()->create_uint32_builder();
		DynamicTypeBuilder_ptr created_type_count_write = DynamicTypeBuilderFactory::get_instance()->create_uint32_builder();
		
		std::vector<uint32_t> lengths = { 1, this->config.Size};
		DynamicType_ptr base_type_data;
		switch (this->config.Typedata)
		{
		case TypeData::ANALOG:
			base_type_data = DynamicTypeBuilderFactory::get_instance()->create_float32_type();
			break;
		case TypeData::DISCRETE:
			base_type_data = DynamicTypeBuilderFactory::get_instance()->create_uint32_type();
			break;
		case TypeData::BINAR:
			base_type_data = DynamicTypeBuilderFactory::get_instance()->create_char8_type();
			break;
		default:
			base_type_data = DynamicTypeBuilderFactory::get_instance()->create_char8_type();
			break;
		}
		DynamicTypeBuilder_ptr builder = DynamicTypeBuilderFactory::get_instance()->create_array_builder(base_type_data, lengths);
		DynamicType_ptr array_type = builder->build();
		
		DynamicTypeBuilder_ptr struct_type_builder = DynamicTypeBuilderFactory::get_instance()->create_struct_builder();
		struct_type_builder->add_member(0, "typedata", created_type_typedata.get());
		struct_type_builder->add_member(1, "TimeLastUpdate_h", created_type_TimeLastUpdate_h.get());
		struct_type_builder->add_member(2, "TimeLastUpdate_m", created_type_TimeLastUpdate_m.get());
		struct_type_builder->add_member(3, "TimeLastUpdate_s", created_type_TimeLastUpdate_s.get());
		struct_type_builder->add_member(4, "TimeLastUpdate_ms", created_type_TimeLastUpdate_ms.get());
		struct_type_builder->add_member(5, "size_data", created_type_size_data.get());
		struct_type_builder->add_member(6, "count_write", created_type_count_write.get());
		struct_type_builder->add_member(7, "data", array_type);

		helpstr.clear();
		helpstr += "typedataDDS" + this->config.PointName;
		struct_type_builder->set_name(helpstr);
		DynamicType_ptr dynType = struct_type_builder->build();

		Domain::registerDynamicType(participant_, &dynType);








	};

	void DDSUnit_Subscriber::thread_transmite(TypeData type_data_thread)
	{

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

	TypeDDSUnit DDSUnit_Subscriber::GetType()
	{
		TypeDDSUnit a = TypeDDSUnit::SUBSCRIBER;
		return a;
	};

	void DDSUnit_Subscriber::SetStatus(StatusDDSUnit status)
	{
		GlobalStatus.store(status, std::memory_order_relaxed);
	};



		/// --- PUBLISHER --- ///

		DDSUnit_Publisher::DDSUnit_Publisher(ConfigDDSUnit config) : config(config)
	{
		GlobalStatus.store(StatusDDSUnit::START);
		std::string helpstr;

		log = LoggerSpace::Logger::getpointcontact();


		/*if (readerkks->ReadKKSlist(config.NameListKKS) != ResultReqest::OK)
		{
			GlobalStatus.store(StatusDDSUnit::ERROR_INIT);
		}*/
		//else
		{
			helpstr.clear();
			helpstr += CreateNameMemoryDDS(TypeData::ANALOG, TypeDirection::EMTtoDDS, config.Domen) + "_";
			if (SharedMemoryUnit->CreateMemory(TypeData::ANALOG, TypeDirection::EMTtoDDS, readerkks->size_analog(), helpstr) != ResultReqest::OK)
			{
				GlobalStatus.store(StatusDDSUnit::ERROR_INIT);
			}

			helpstr.clear();
			helpstr += CreateNameMemoryDDS(TypeData::DISCRETE, TypeDirection::EMTtoDDS, config.Domen) + "_";
			if (SharedMemoryUnit->CreateMemory(TypeData::DISCRETE, TypeDirection::EMTtoDDS, readerkks->size_discrete(), helpstr) != ResultReqest::OK)
			{
				GlobalStatus.store(StatusDDSUnit::ERROR_INIT);
			}

			helpstr.clear();
			helpstr += CreateNameMemoryDDS(TypeData::BINAR, TypeDirection::EMTtoDDS, config.Domen) + "_";
			if (SharedMemoryUnit->CreateMemory(TypeData::BINAR, TypeDirection::EMTtoDDS, readerkks->size_discrete(), helpstr) != ResultReqest::OK)
			{
				GlobalStatus.store(StatusDDSUnit::ERROR_INIT);
			}

			std::vector<uint32_t> lengths = { 1 , 10 };
			DynamicType_ptr base_type = DynamicTypeBuilderFactory::get_instance()->create_uint32_type();
			DynamicTypeBuilder_ptr builder = DynamicTypeBuilderFactory::get_instance()->create_array_builder(base_type, lengths);
			DynamicType_ptr array_type = builder->build();

			DynamicTypeBuilder_ptr struct_type_builder(DynamicTypeBuilderFactory::get_instance()->create_struct_builder());
			struct_type_builder->add_member(0, "index", DynamicTypeBuilderFactory::get_instance()->create_uint32_type());
			struct_type_builder->add_member(1, "message", DynamicTypeBuilderFactory::get_instance()->create_string_type());
			struct_type_builder->add_member(2, "array", array_type);
			struct_type_builder->set_name("HelloWorld");
			DynamicType_ptr dynType = struct_type_builder->build();
			base_type_array_analog = struct_type_builder->build();
			TypeSupport m_type(new eprosima::fastrtps::types::DynamicPubSubType(dynType));

			DomainParticipantQos participantQos;
			participantQos.name("Participant_publisher");
			participant_ = DomainParticipantFactory::get_instance()->create_participant(config.Domen, participantQos);
			if (participant_ == nullptr)
			{
				GlobalStatus.store(StatusDDSUnit::ERROR_INIT);
				log->WriteLogWARNING("ERROR_INIT_PARTICIANT_PUBLISHER", 0, 0);
				return;
			}

			m_type.get()->auto_fill_type_information(false);
			m_type.get()->auto_fill_type_object(true);
			m_type.register_type(participant_);

			helpstr.clear();
			helpstr += "Analog_Data";
			topic_analog = participant_->create_topic(helpstr, "HelloWorld", TOPIC_QOS_DEFAULT);
			if (topic_analog == nullptr)
			{
				log->WriteLogWARNING("ERROR CREATE TOPIC ANALOG (PUBLISHER)", 0, 0);
			}

			/*helpstr.clear();
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
			}*/

			publisher_ = participant_->create_publisher(PUBLISHER_QOS_DEFAULT, nullptr);

			if (publisher_ == nullptr)
			{
				log->WriteLogWARNING("ERROR INIT PUBSISHER", 0, 0);
				return;
			}

			writerr = publisher_->create_datawriter(topic_analog, DATAWRITER_QOS_DEFAULT, nullptr);

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
		ReturnCode_t res;
		bool res_write = 0;
		eprosima::fastrtps::types::DynamicData* array;


		switch (type_data_thread)
		{
		case TypeData::ZERO:
			break;
		case TypeData::ANALOG:

			writer = publisher_->create_datawriter(topic_analog, DATAWRITER_QOS_DEFAULT, nullptr);
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

		/*eprosima::fastrtps::types::DynamicData* array = m_Hello->loan_value(2);
		array->set_uint32_value(10, array->get_array_index({ 0, 0 }));
		array->set_uint32_value(20, array->get_array_index({ 1, 0 }));
		array->set_uint32_value(30, array->get_array_index({ 2, 0 }));
		array->set_uint32_value(40, array->get_array_index({ 3, 0 }));
		array->set_uint32_value(50, array->get_array_index({ 4, 0 }));
		array->set_uint32_value(60, array->get_array_index({ 0, 1 }));
		array->set_uint32_value(70, array->get_array_index({ 1, 1 }));
		array->set_uint32_value(80, array->get_array_index({ 2, 1 }));
		array->set_uint32_value(90, array->get_array_index({ 3, 1 }));
		array->set_uint32_value(100, array->get_array_index({ 4, 1 }));
		m_Hello->return_loaned_value(array);*/

		std::string str = "AZAZAZAZA";
		data->set_string_value(str, 1);
		res = data->set_uint32_value(iteri, 0);
		array = data->loan_value(2);
		for (unsigned int i = 0; i < 10; i++)
		{
			array->set_uint32_value(iteri + 10 * i, array->get_array_index({ 0, i }));
		}
		data->return_loaned_value(array);

		for (;;)
		{
			res = data->get_uint32_value(iteri, 0);
			res = data->set_uint32_value(iteri + 1, 0);

			array = data->loan_value(2);
			for (unsigned int i = 0; i < 10; i++)
			{
				array->set_uint32_value(iteri + 10 * i, array->get_array_index({ 0, i }));
			}
			data->return_loaned_value(array);

			res_write = writerr->write(data.get());

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
}







/*
* DDSUnit_Subscriber::DDSUnit_Subscriber(ConfigDDSUnit config) : config(config)
{
	GlobalStatus.store(StatusDDSUnit::START);
	std::string helpstr;
	readerkks = new KKSReader();
	SharedMemoryUnit = new SharedMemoryDDS();
	ReturnCode_t res;
	log = LoggerSpace::Logger::getpointcontact();


	/*if (readerkks->ReadKKSlist(config.NameListKKS) != ResultReqest::OK)
	{
		GlobalStatus.store(StatusDDSUnit::ERROR_INIT);
	}
	//else
{
	helpstr.clear();
	helpstr += CreateNameMemoryDDS(TypeData::ANALOG, TypeDirection::EMTfromDDS, config.Domen) + "_";
	if (SharedMemoryUnit->CreateMemory(TypeData::ANALOG, TypeDirection::EMTfromDDS, readerkks->size_analog(), helpstr) != ResultReqest::OK)
	{
		GlobalStatus.store(StatusDDSUnit::ERROR_INIT);
	}

	helpstr.clear();
	helpstr += CreateNameMemoryDDS(TypeData::DISCRETE, TypeDirection::EMTfromDDS, config.Domen) + "_";
	if (SharedMemoryUnit->CreateMemory(TypeData::DISCRETE, TypeDirection::EMTfromDDS, readerkks->size_discrete(), helpstr) != ResultReqest::OK)
	{
		GlobalStatus.store(StatusDDSUnit::ERROR_INIT);
	}

	helpstr.clear();
	helpstr += CreateNameMemoryDDS(TypeData::BINAR, TypeDirection::EMTfromDDS, config.Domen) + "_";
	if (SharedMemoryUnit->CreateMemory(TypeData::BINAR, TypeDirection::EMTfromDDS, readerkks->size_discrete(), helpstr) != ResultReqest::OK)
	{
		GlobalStatus.store(StatusDDSUnit::ERROR_INIT);
	}


	std::vector<uint32_t> lengths = { 1,10 };
	DynamicType_ptr base_type = DynamicTypeBuilderFactory::get_instance()->create_uint32_type();
	DynamicTypeBuilder_ptr builder = DynamicTypeBuilderFactory::get_instance()->create_array_builder(base_type, lengths);
	DynamicType_ptr array_type = builder->build();

	DynamicTypeBuilder_ptr struct_type_builder(DynamicTypeBuilderFactory::get_instance()->create_struct_builder());
	struct_type_builder->add_member(0, "index", DynamicTypeBuilderFactory::get_instance()->create_uint32_type());
	struct_type_builder->add_member(1, "message", DynamicTypeBuilderFactory::get_instance()->create_string_type());
	struct_type_builder->add_member(2, "array", array_type);
	struct_type_builder->set_name("HelloWorld");
	DynamicType_ptr dynType = struct_type_builder->build();
	base_type_array_analog = struct_type_builder->build();
	TypeSupport m_type(new eprosima::fastrtps::types::DynamicPubSubType(dynType));


	DomainParticipantQos participantQos;
	participantQos.name("Participant_subscriber");
	participant_ = DomainParticipantFactory::get_instance()->create_participant(config.Domen, participantQos);
	if (participant_ == nullptr)
	{
		GlobalStatus.store(StatusDDSUnit::ERROR_INIT);
		log->WriteLogWARNING("ERROR_INIT_PARTICIANT_PUBLISHER", 0, 0);
	}

	m_type.get()->auto_fill_type_information(false);
	m_type.get()->auto_fill_type_object(true);
	res = m_type.register_type(participant_);

	helpstr.clear();
	helpstr += "Analog_Data";
	topic_analog = participant_->create_topic(helpstr, "HelloWorld", TOPIC_QOS_DEFAULT);
	if (topic_analog == nullptr)
	{
		log->WriteLogWARNING("ERROR CREATE TOPIC ANALOG (SUBSCRIBER)", 0, 0);
	}

	subscriber_ = participant_->create_subscriber(SUBSCRIBER_QOS_DEFAULT, nullptr);
	listener_.hello_ = DynamicDataFactory::get_instance()->create_data(base_type_array_analog);
	readerr = subscriber_->create_datareader(topic_analog, DATAREADER_QOS_DEFAULT, nullptr);// &listener_);

	std::thread t(&DDSUnit_Subscriber::thread_transmite, this, TypeData::ANALOG);
	t.detach();
}
}
*/