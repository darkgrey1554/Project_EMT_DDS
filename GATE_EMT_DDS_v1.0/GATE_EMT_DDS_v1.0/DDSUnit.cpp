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
			p = nullptr;
		}
		return p;
	}

	/// --- SUBSCRIBER --- ///

	DDSUnit_Subscriber::DDSUnit_Subscriber(ConfigDDSUnit config) : config(config)
	{
		SetStatus(StatusDDSUnit::START);
		log = LoggerSpace::Logger::getpointcontact();
		std::string helpstr;
		ReturnCode_t res_dds;
		ResultReqest result_command;

		/// --- иницализация participant --- /// 
		result_command = init_participant();
		if (result_command != ResultReqest::OK)
		{
			SetStatus(StatusDDSUnit::ERROR_INIT);
			return;
		}

		/// --- инициализация subscriber --- ///

		result_command = init_subscriber();
		if (result_command != ResultReqest::OK)
		{
			SetStatus(StatusDDSUnit::ERROR_INIT);
			return;
		}

		/// --- создание динамического типа --- ///
		
		result_command = create_dynamic_data_type();
		if (result_command != ResultReqest::OK)
		{
			SetStatus(StatusDDSUnit::ERROR_INIT);
			return;
		}

		/// --- регистрация типа ---- ///

		result_command = register_type();
		if (result_command != ResultReqest::OK)
		{
			SetStatus(StatusDDSUnit::ERROR_INIT);
			return;
		}		

		/// --- регистрация топика --- ///

		result_command = register_topic();
		if (result_command != ResultReqest::OK)
		{
			SetStatus(StatusDDSUnit::ERROR_INIT);
			return;
		}

		/// --- создание адаптера --- /// 

		result_command = init_adapter();
		if (result_command != ResultReqest::OK)
		{
			SetStatus(StatusDDSUnit::ERROR_INIT);
			return;
		}

		/// --- регистрация DataReader --- /// 

		result_command = init_reader_data();
		if (result_command != ResultReqest::OK)
		{
			SetStatus(StatusDDSUnit::ERROR_INIT);
			return;
		}



		SetStatus(StatusDDSUnit::WORK);
		return;
	};

	ResultReqest DDSUnit_Subscriber::init_participant()
	{	

		std::string helpstr;

		/// --- инициализация транспортного уровня --- ///
		///--------------------------------------------///
		//////////////////////////////////////////////////

		/// --- иницализация participant --- /// 	

		try
		{
			participant_ =
				DomainParticipantFactory::get_instance()->create_participant(this->config.Domen, PARTICIPANT_QOS_DEFAULT, nullptr);
			if (!participant_) throw - 1;
		}
		catch (...)
		{
			helpstr.clear();
			helpstr += "Error init DDSUnit: Error create of participant: name units: " + this->config.PointName;
			log->WriteLogERR(helpstr.c_str(), 0, 0);
			return ResultReqest::ERR;
		}

		return ResultReqest::OK;
	}

	ResultReqest DDSUnit_Subscriber::init_subscriber()
	{
		std::string helpstr;

		try
		{
			subscriber_ = participant_->create_subscriber(SUBSCRIBER_QOS_DEFAULT, nullptr);
			if (!subscriber_) throw - 1;
		}
		catch (...)
		{
			helpstr.clear();
			helpstr += "Error init DDSUnit: Error create of subscriber: name units: " + this->config.PointName;
			log->WriteLogERR(helpstr.c_str(), 0, 0);
			return ResultReqest::ERR;
		}

		return ResultReqest::OK;
	}

	ResultReqest DDSUnit_Subscriber::create_dynamic_data_type()
	{
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
		std::string helpstr;

		try
		{
			
			DynamicTypeBuilder_ptr created_type_typedata;
			DynamicTypeBuilder_ptr created_type_TimeLastUpdate_h;
			DynamicTypeBuilder_ptr created_type_TimeLastUpdate_m;
			DynamicTypeBuilder_ptr created_type_TimeLastUpdate_s;
			DynamicTypeBuilder_ptr created_type_TimeLastUpdate_ms;
			DynamicTypeBuilder_ptr created_type_size_data;
			DynamicTypeBuilder_ptr created_type_count_write;
			DynamicType_ptr base_type_array_data;
			DynamicTypeBuilder_ptr builder;
			DynamicType_ptr array_type;
			DynamicTypeBuilder_ptr struct_type_builder;

			try
			{
				created_type_typedata = DynamicTypeBuilderFactory::get_instance()->create_char8_builder();
				created_type_TimeLastUpdate_h = DynamicTypeBuilderFactory::get_instance()->create_char8_builder();
				created_type_TimeLastUpdate_m = DynamicTypeBuilderFactory::get_instance()->create_char8_builder();
				created_type_TimeLastUpdate_s = DynamicTypeBuilderFactory::get_instance()->create_char8_builder();
				created_type_TimeLastUpdate_ms = DynamicTypeBuilderFactory::get_instance()->create_uint16_builder();
				created_type_size_data = DynamicTypeBuilderFactory::get_instance()->create_uint32_builder();
				created_type_count_write = DynamicTypeBuilderFactory::get_instance()->create_uint32_builder();

				std::vector<uint32_t> lengths = { 1, this->config.Size };
				switch (this->config.Typedata)
				{
				case TypeData::ANALOG:
					base_type_array_data = DynamicTypeBuilderFactory::get_instance()->create_float32_type();
					break;
				case TypeData::DISCRETE:
					base_type_array_data = DynamicTypeBuilderFactory::get_instance()->create_int32_type();
					break;
				case TypeData::BINAR:
					base_type_array_data = DynamicTypeBuilderFactory::get_instance()->create_char8_type();
					break;
				default:
					base_type_array_data = DynamicTypeBuilderFactory::get_instance()->create_char8_type();
					break;
				}

				builder = DynamicTypeBuilderFactory::get_instance()->create_array_builder(base_type_array_data, lengths);
				array_type = builder->build();
			}
			catch (...)
			{
				throw 1;
			}			

			try
			{
				struct_type_builder = DynamicTypeBuilderFactory::get_instance()->create_struct_builder();
				struct_type_builder->add_member(0, "typedata", created_type_typedata.get());
				struct_type_builder->add_member(1, "TimeLastUpdate_h", created_type_TimeLastUpdate_h.get());
				struct_type_builder->add_member(2, "TimeLastUpdate_m", created_type_TimeLastUpdate_m.get());
				struct_type_builder->add_member(3, "TimeLastUpdate_s", created_type_TimeLastUpdate_s.get());
				struct_type_builder->add_member(4, "TimeLastUpdate_ms", created_type_TimeLastUpdate_ms.get());
				struct_type_builder->add_member(5, "size_data", created_type_size_data.get());
				struct_type_builder->add_member(6, "count_write", created_type_count_write.get());
				struct_type_builder->add_member(7, "data", array_type);
				helpstr.clear();
				helpstr += "typedataDDS_" + this->config.PointName;
				struct_type_builder->set_name(helpstr);
			}
			catch (...)
			{
				throw 2;
			}

			try
			{
				type_data = struct_type_builder->build();
			}
			catch(...)
			{
				throw 3;
			}			
		}
		catch (const int& e_int)
		{
			helpstr.clear();
			helpstr = "Error init DDSUnit : Error create dynamic type : name units : " + this->config.PointName;
			log->WriteLogERR(helpstr.c_str(), e_int, 0);
			return ResultReqest::ERR;
		}
		catch (...)
		{
			helpstr.clear();
			helpstr = "Error init DDSUnit : Error create dynamic type : name units : " + this->config.PointName;
			log->WriteLogERR(helpstr.c_str(), 0, 0);
			return ResultReqest::ERR;
		}

		return ResultReqest::OK;
	}

	ResultReqest DDSUnit_Subscriber::register_type()
	{
		try
		{
			TypeSupport PtrSupporType = eprosima::fastrtps::types::DynamicPubSubType(type_data);
			PtrSupporType.get()->auto_fill_type_information(false);
			PtrSupporType.get()->auto_fill_type_object(true);
			if (PtrSupporType.register_type(participant_) != ReturnCode_t::RETCODE_OK) throw - 1;
		}
		catch (...)
		{
			std::string helpstr;
			helpstr.clear();
			helpstr += "Error init DDSUnit: Error registration of type: name units: " + this->config.PointName;
			log->WriteLogERR(helpstr.c_str(), 0, 0);
			return ResultReqest::ERR;
		}		

		return ResultReqest::OK;
	}

	ResultReqest DDSUnit_Subscriber::register_topic()
	{
		try
		{
			topic_data = participant_->create_topic(CreateNameTopic(this->config.PointName), CreateNameType(this->config.PointName), TOPIC_QOS_DEFAULT);
			if (topic_data == nullptr) throw - 1;
		}
		catch (...)
		{
			std::string helpstr;
			helpstr.clear();
			helpstr += "Error init DDSUnit: Error registration of topic, name units: " + this->config.PointName;
			log->WriteLogERR(helpstr.c_str(), 0, 0);
			return ResultReqest::ERR;
		}

		return ResultReqest::OK;
	};

	ResultReqest DDSUnit_Subscriber::init_reader_data()
	{
		std::string helpstr;

		try
		{
			if (this->config.Frequency <= 0)
			{
				reader_data = subscriber_->create_datareader(topic_data, DATAREADER_QOS_DEFAULT, &listener_);
				if (reader_data == nullptr) throw - 1;
			}
			else
			{
				reader_data = subscriber_->create_datareader(topic_data, DATAREADER_QOS_DEFAULT, nullptr);
				if (reader_data == nullptr) throw - 1;
				thread_transmite = std::jthread(&DDSUnit_Subscriber::function_thread_transmite, this);
			}
		}
		catch (...)
		{
			helpstr.clear();
			helpstr += "Error init DDSUnit: Error create of reader_data: name units: " + this->config.PointName;
			log->WriteLogERR(helpstr.c_str(), 0, 0);
			return ResultReqest::ERR;
		}

		return ResultReqest::OK;
	};

	std::shared_ptr<ConfigAdapter>  DDSUnit_Subscriber::create_config_adapter()
	{

		if (config.Adapter == TypeAdapter::SharedMemory)
		{
			std::shared_ptr<ConfigSharedMemoryAdapter> config_sm = std::make_shared<ConfigSharedMemoryAdapter>();
			
			config_sm->type_adapter = TypeAdapter::SharedMemory;
			config_sm->NameMemory = this->config.PointName;
			config_sm->size = this->config.Size;
			config_sm->DataType = this->config.Typedata;

			return config_sm;
		}

		return nullptr;
	};

	ResultReqest DDSUnit_Subscriber::init_adapter()
	{
		std::string helpstr;
		ResultReqest res;

		try
		{
			AdapterUnit = CreateAdapter(this->config.Adapter);
			if (AdapterUnit == nullptr) throw 1;
			std::shared_ptr<ConfigAdapter> conf_adater = create_config_adapter();
			res = AdapterUnit->InitAdapter(conf_adater);
			if (res != ResultReqest::OK) throw 2;
		}
		catch (int& e_int)
		{
			helpstr.clear();
			helpstr += "Error init DDSUnit: Error initional adapter, name units: " + this->config.PointName;
			log->WriteLogERR(helpstr.c_str(), e_int, 0);
			return ResultReqest::ERR;
		}
		catch (...)
		{
			helpstr.clear();
			helpstr += "Error init DDSUnit: Error initional adapter, name units: " + this->config.PointName;
			log->WriteLogERR(helpstr.c_str(), 0, 0);
			return ResultReqest::ERR;
		}

		return ResultReqest::OK;
	}

	inline unsigned char DDSUnit::size_type_data_baits(TypeData type)
	{
		unsigned char result = 0;
		if (type == TypeData::ANALOG) { result = sizeof(float); }
		else if (type == TypeData::DISCRETE) { result = sizeof(int); }
		else if (type == TypeData::BINAR) { result = sizeof(char); }

		return result;
	};

	inline void  DDSUnit_Subscriber::mirror_data_form_DDS(void* buf, eprosima::fastrtps::types::DynamicData* array_dds, unsigned int i)
	{
		if (config.Typedata == TypeData::ANALOG)
		{
			*(reinterpret_cast<float*>(buf)+i) = array_dds->get_float32_value(array_dds->get_array_index({ 0, i }));
		}
		else if (config.Typedata == TypeData::DISCRETE)
		{
			*(reinterpret_cast<int*>(buf) + i) = array_dds->get_int32_value(array_dds->get_array_index({ 0, i }));
		}
		else if (config.Typedata == TypeData::BINAR)
		{
			*(reinterpret_cast<char*>(buf) + i) = array_dds->get_char8_value(array_dds->get_array_index({ 0, i }));
		}		
	}

	void DDSUnit_Subscriber::function_thread_transmite(std::stop_token stop_token)
	{
		std::chrono::steady_clock::time_point start, end;
		std::chrono::milliseconds delta_ms;
		start = std::chrono::steady_clock::now();
		eprosima::fastrtps::types::DynamicData* array = nullptr;
		SampleInfo info;
		ResultReqest res;
		std::string helpstr;

		status_thread.store(StatusThreadDSSUnit::WORK, std::memory_order_relaxed);

		try
		{
			try
			{
				data = DynamicDataFactory::get_instance()->create_data(type_data);
			}
			catch (...)
			{
				throw 1;
			}

			int size_type_data = size_type_data_baits(config.Typedata);
			std::shared_ptr<char> mass_data(new char[size_type_data * config.Size], std::default_delete<char[]>());
			for (int i = 0; i < size_type_data * config.Size; i++) *(mass_data.get() + i) = 0;

			while (1)
			{
				if (stop_token.stop_requested()) break;

				end = std::chrono::steady_clock::now();
				delta_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
				if (delta_ms.count() < config.Frequency - frequency_scatter) continue;

				if (reader_data->take_next_sample(data.get(), &info) != ReturnCode_t::RETCODE_OK) throw 2;
				array = data->loan_value(7);
				if (array == nullptr) throw 3;

				/// --- тутуту тут надо подумать (пока тупо в лоб)--- /// 

				for (int i = 0; i < config.Size; i++)
				{
					mirror_data_form_DDS(mass_data.get(), array, i);
				}
				if (data->return_loaned_value(array) != ReturnCode_t::RETCODE_OK) throw 5;
				if (AdapterUnit->WriteData(mass_data.get(), config.Size) != ResultReqest::OK) throw 4;

				start = std::chrono::steady_clock::now();
			}
		}
		catch (int& e)
		{
			helpstr.clear();
			helpstr += "Error init DDSUnit: Error in thread of thransfer: name units: " + this->config.PointName;
			log->WriteLogERR(helpstr.c_str(), e, 0);
			status_thread.store(StatusThreadDSSUnit::FAIL, std::memory_order_relaxed);
			return;
		}
		catch (...)
		{
			helpstr.clear();
			helpstr += "Error init DDSUnit: Error in thread of thransfer: name units: " + this->config.PointName;
			log->WriteLogERR(helpstr.c_str(), 0, 0);
			status_thread.store(StatusThreadDSSUnit::FAIL, std::memory_order_relaxed);
			data->return_loaned_value(array);
			return;
		}

		status_thread.store(StatusThreadDSSUnit::TERMINATE, std::memory_order_relaxed);
	};

	void DDSUnit_Subscriber::SubListener::on_subscription_matched(DataReader*, const SubscriptionMatchedStatus& info)
	{
		////////////////////////
		////////////////////////
		////////////////////////
	}

	void DDSUnit_Subscriber::SubListener::on_data_available(DataReader* reader)
	{

	}


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

	StatusDDSUnit DDSUnit_Subscriber::GetCurrentStatus() const
	{
		StatusDDSUnit a = StatusDDSUnit::EMPTY;
		return a;
	};

	ConfigDDSUnit DDSUnit_Subscriber::GetConfig() const
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

	TypeDDSUnit DDSUnit_Subscriber::GetType() const
	{
		TypeDDSUnit a = TypeDDSUnit::SUBSCRIBER;
		return a;
	};

	void DDSUnit_Subscriber::SetStatus(StatusDDSUnit status)
	{
		GlobalStatus.store(status, std::memory_order_relaxed);
	};

	std::string DDSUnit_Subscriber::CreateNameTopic(std::string short_name)
	{
		return "TopicdataDDS_" + short_name;
	}

	std::string DDSUnit_Subscriber::CreateNameType(std::string short_name)
	{
		return "TypedataDDS_" + short_name;
	}



		/// --- PUBLISHER --- ///

	DDSUnit_Publisher::DDSUnit_Publisher(ConfigDDSUnit config) : config(config)
	{
		
	}

	void DDSUnit_Publisher::thread_transmite(TypeData type_data_thread)
	{

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