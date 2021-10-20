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

	DDSUnit_Subscriber::DDSUnit_Subscriber(ConfigDDSUnit config) : start_config(config)
	{
		SetStatus(StatusDDSUnit::EMPTY);
		log = LoggerSpace::Logger::getpointcontact();
		name_unit = CreateNameUnit(start_config.PointName);
	};

	ResultReqest DDSUnit_Subscriber::Initialization()
	{
		std::string helpstr;
		ResultReqest result_command;
		StatusDDSUnit status_unit = GetCurrentStatus();



		if (status_unit != StatusDDSUnit::EMPTY &&
			status_unit != StatusDDSUnit::ERROR_INIT &&
			status_unit != StatusDDSUnit::DESTROYED)
		{
			helpstr.clear();
			helpstr += "Error DDSUnit: Initialization already done: name units: " + this->name_unit;
			log->WriteLogWARNING(helpstr.c_str(), 0, 0);
			return ResultReqest::IGNOR;
		}

		config = start_config;
		name_unit = CreateNameUnit(config.PointName);

		/// --- иницализация participant --- /// 
		result_command = init_participant();
		if (result_command != ResultReqest::OK)
		{
			SetStatus(StatusDDSUnit::ERROR_INIT);
			return ResultReqest::ERR;
		}

		/// --- инициализация subscriber --- ///

		result_command = init_subscriber();
		if (result_command != ResultReqest::OK)
		{
			SetStatus(StatusDDSUnit::ERROR_INIT);
			return ResultReqest::ERR;
		}

		/// --- создание динамического типа --- ///

		result_command = create_dynamic_data_type();
		if (result_command != ResultReqest::OK)
		{
			SetStatus(StatusDDSUnit::ERROR_INIT);
			return ResultReqest::ERR;
		}

		/// --- регистрация типа ---- ///

		result_command = register_type();
		if (result_command != ResultReqest::OK)
		{
			SetStatus(StatusDDSUnit::ERROR_INIT);
			return ResultReqest::ERR;
		}

		/// --- регистрация топика --- ///

		result_command = register_topic();
		if (result_command != ResultReqest::OK)
		{
			SetStatus(StatusDDSUnit::ERROR_INIT);
			return ResultReqest::ERR;
		}

		/// --- создание адаптера --- /// 

		result_command = init_adapter();
		if (result_command != ResultReqest::OK)
		{
			SetStatus(StatusDDSUnit::ERROR_INIT);
			return ResultReqest::ERR;
		}

		/// --- регистрация DataReader --- /// 

		result_command = init_reader_data();
		if (result_command != ResultReqest::OK)
		{
			SetStatus(StatusDDSUnit::ERROR_INIT);
			return ResultReqest::ERR;
		}

		SetStatus(StatusDDSUnit::WORK);
		helpstr.clear();
		helpstr += "Info DDSUnit: Initialization done: name units: " + this->name_unit;
		log->WriteLogINFO(helpstr.c_str(), 0, 0);
		
		return ResultReqest::OK;
	}

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
			helpstr += "Error init DDSUnit: Error create of participant: name units: " + this->name_unit;
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
			helpstr += "Error init DDSUnit: Error create of subscriber: name units: " + this->name_unit;
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

			try
			{
				data = DynamicDataFactory::get_instance()->create_data(type_data);
			}
			catch (...)
			{
				throw 4;
			}

			
		}
		catch (const int& e_int)
		{
			helpstr.clear();
			helpstr = "Error init DDSUnit : Error create dynamic type : name units : " + this->name_unit;
			log->WriteLogERR(helpstr.c_str(), e_int, 0);
			return ResultReqest::ERR;
		}
		catch (...)
		{
			helpstr.clear();
			helpstr = "Error init DDSUnit : Error create dynamic type : name units : " + this->name_unit;
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
			helpstr += "Error init DDSUnit: Error registration of type: name units: " + this->name_unit;
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
			helpstr += "Error init DDSUnit: Error registration of topic, name units: " + this->name_unit;
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
				reader_data = subscriber_->create_datareader(topic_data, DATAREADER_QOS_DEFAULT, listener_.get());
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
			helpstr += "Error init DDSUnit: Error create of reader_data: name units: " + this->name_unit;
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
			if (AdapterUnit != nullptr) throw 3;

			AdapterUnit = CreateAdapter(this->config.Adapter);
			if (AdapterUnit == nullptr) throw 1;
			std::shared_ptr<ConfigAdapter> conf_adater = create_config_adapter();
			res = AdapterUnit->InitAdapter(conf_adater);
			if (res != ResultReqest::OK) throw 2;
		}
		catch (int& e_int)
		{
			helpstr.clear();
			helpstr += "Error init DDSUnit: Error initialization of adapter, name units: " + this->name_unit;
			log->WriteLogERR(helpstr.c_str(), e_int, 0);
			return ResultReqest::ERR;
		}
		catch (...)
		{
			helpstr.clear();
			helpstr += "Error init DDSUnit: Error initialization of adapter, name units: " + this->name_unit;
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
			helpstr += "Error DDSUnit: Error in thread of thransfer: name units: " + this->name_unit;
			log->WriteLogERR(helpstr.c_str(), e, 0);
			status_thread.store(StatusThreadDSSUnit::FAIL, std::memory_order_relaxed);
			return;
		}
		catch (...)
		{
			helpstr.clear();
			helpstr += "Error DDSUnit: Error in thread of thransfer: name units: " + this->name_unit;
			log->WriteLogERR(helpstr.c_str(), 0, 0);
			status_thread.store(StatusThreadDSSUnit::FAIL, std::memory_order_relaxed);
			data->return_loaned_value(array);
			return;
		}

		status_thread.store(StatusThreadDSSUnit::TERMINATE, std::memory_order_relaxed);
		return;
	};

	void DDSUnit_Subscriber::SubListener::on_subscription_matched(DataReader*, const SubscriptionMatchedStatus& info)
	{

	}

	void DDSUnit_Subscriber::SubListener::on_data_available(DataReader* reader)
	{
		SampleInfo info;
		eprosima::fastrtps::types::DynamicData* array = nullptr;
		std::string helpstr;
		if ( status.load(std::memory_order_relaxed) != CommandListenerSubscriber::START) return;

		try
		{
			int size_type_data = master->size_type_data_baits(master->config.Typedata);
			std::shared_ptr<char> mass_data(new char[size_type_data * master->config.Size], std::default_delete<char[]>());
			for (int i = 0; i < size_type_data * master->config.Size; i++) *(mass_data.get() + i) = 0;

			if (reader->take_next_sample(master->data.get(), &info) != ReturnCode_t::RETCODE_OK) throw 2;
			array = master->data->loan_value(7);
			if (array == nullptr) throw 3;

			for (int i = 0; i < master->config.Size; i++)
			{
				master->mirror_data_form_DDS(mass_data.get(), array, i);
			}
			if (master->data->return_loaned_value(array) != ReturnCode_t::RETCODE_OK) throw 5;
			if (master->AdapterUnit->WriteData(mass_data.get(), master->config.Size) != ResultReqest::OK) throw 4;
		}
		catch (int& e)
		{
			helpstr.clear();
			helpstr += "Error init DDSUnit: Error in listener_: name units: " + this->master->name_unit;
			master->log->WriteLogERR(helpstr.c_str(), e, 0);
			return;
		}
		catch (...)
		{
			helpstr.clear();
			helpstr += "Error init DDSUnit: Error in listener_: name units: " + this->master->name_unit;
			master->log->WriteLogERR(helpstr.c_str(), 0, 0);
			master->data->return_loaned_value(array);
			return;
		}

		return;
	}

	void DDSUnit_Subscriber::SubListener::Stop()
	{
		status.store(CommandListenerSubscriber::STOP);
	}

	void DDSUnit_Subscriber::SubListener::Start()
	{
		status.store(CommandListenerSubscriber::START);
	}

	ResultReqest DDSUnit_Subscriber::Stop()
	{
		std::string helpstr;
		try
		{
			if (GetCurrentStatus() != StatusDDSUnit::WORK) throw 1;

			listener_->Stop();

			if (thread_transmite.joinable())
			{
				thread_transmite.request_stop();
				thread_transmite.join();
			}
			SetStatus(StatusDDSUnit::STOP);

		}
		catch (int& e)
		{
			helpstr.clear();
			helpstr += "Error DDSUnit: Stop command error: name units: " + this->name_unit;
			log->WriteLogERR(helpstr.c_str(), e, 0);
			return ResultReqest::ERR;
		}
		catch (...)
		{
			helpstr.clear();
			helpstr += "Error DDSUnit: Stop command error: name units: " + this->name_unit;
			log->WriteLogERR(helpstr.c_str(), 0, 0);
			return ResultReqest::ERR;
		}

		helpstr.clear();
		helpstr += "DDSUnit: Stop command done: name units: " + this->name_unit;
		log->WriteLogINFO(helpstr.c_str(), 0, 0);
		return ResultReqest::OK;
	};

	ResultReqest DDSUnit_Subscriber::Start()
	{
		std::string helpstr;
		try
		{
			if (GetCurrentStatus() != StatusDDSUnit::STOP) throw 1;

			listener_->Start();
			if (thread_transmite.joinable())
			{
				if (status_thread.load(std::memory_order_relaxed) != StatusThreadDSSUnit::WORK)
				{
					thread_transmite.join();
					thread_transmite = std::jthread(&DDSUnit_Subscriber::function_thread_transmite, this);
				}
			}
			else
			{
				thread_transmite = std::jthread(&DDSUnit_Subscriber::function_thread_transmite, this);
			}

			SetStatus(StatusDDSUnit::WORK);
		}
		catch (int& e)
		{
			helpstr.clear();
			helpstr += "Error DDSUnit: Start command error: name units: " + this->name_unit;
			log->WriteLogERR(helpstr.c_str(), e, 0);
			return ResultReqest::ERR;
		}
		catch (...)
		{
			helpstr.clear();
			helpstr += "Error DDSUnit: Start command error: name units: " + this->name_unit;
			log->WriteLogERR(helpstr.c_str(), 0, 0);
			return ResultReqest::ERR;
		}

		helpstr.clear();
		helpstr += "DDSUnit: command Start done: name units: " + this->name_unit;
		log->WriteLogINFO(helpstr.c_str(), 0, 0);
		return ResultReqest::OK;
	};

	ResultReqest DDSUnit_Subscriber::Delete()
	{
		std::string helpstr;

		try
		{
			listener_->Stop();

			if (thread_transmite.joinable())
			{
				thread_transmite.request_stop();
				thread_transmite.join();
			}

			if (reader_data != nullptr)
			{
				if (subscriber_->delete_datareader(reader_data) != ReturnCode_t::RETCODE_OK) throw 2;
				reader_data = nullptr;
			}
			if (topic_data != nullptr)
			{
				if (participant_->delete_topic(topic_data) != ReturnCode_t::RETCODE_OK) throw 3;
				topic_data = nullptr;
			}
			if (subscriber_ != nullptr)
			{
				if (participant_->delete_subscriber(subscriber_) != ReturnCode_t::RETCODE_OK) throw 4;
				subscriber_ = nullptr;
			}
			if (DomainParticipantFactory::get_instance()->delete_participant(participant_) != ReturnCode_t::RETCODE_OK) throw 5;
			participant_ = nullptr;

			try
			{
				AdapterUnit.~shared_ptr();
				AdapterUnit = nullptr;
			}
			catch (...)
			{
				throw 6;
			}

			SetStatus(StatusDDSUnit::DESTROYED);
		}
		catch (int& e)
		{
			helpstr.clear();
			helpstr += "Error DDSUnit: Error to command Delete: name units: " + this->name_unit;
			log->WriteLogERR(helpstr.c_str(), e, 0);
			SetStatus(StatusDDSUnit::ERROR_DESTROYED);
			return ResultReqest::ERR;
		}
		catch (...)
		{
			helpstr.clear();
			helpstr += "Error DDSUnit: Error to command Delete: name units: " + this->name_unit;
			log->WriteLogERR(helpstr.c_str(), 0, 0);
			SetStatus(StatusDDSUnit::ERROR_DESTROYED);
			return ResultReqest::ERR;
		}

		helpstr.clear();
		helpstr += "Info DDSUnit: Delete command done: name units: " + this->name_unit;
		log->WriteLogERR(helpstr.c_str(), 0, 0);

		return ResultReqest::OK;
	};

	ResultReqest DDSUnit_Subscriber::Restart()
	{
		std::string helpstr;

		try
		{
			if (Delete() != ResultReqest::OK) throw 1;
			if (Initialization() != ResultReqest::OK) throw 2;
		}
		catch (int& e)
		{
			helpstr.clear();
			helpstr += "Error DDSUnit: Error command Restart: name units: " + this->name_unit;
			log->WriteLogERR(helpstr.c_str(), e, 0);
			return ResultReqest::ERR;
		}
		catch (...)
		{
			helpstr.clear();
			helpstr += "Error DDSUnit: Error command Restart: name units: " + this->name_unit;
			log->WriteLogERR(helpstr.c_str(), 0, 0);
			return ResultReqest::ERR;
		}

		helpstr.clear();
		helpstr += "Info DDSUnit: Restart command done: name units: " + this->name_unit;
		log->WriteLogINFO(helpstr.c_str(), 0, 0);
		return ResultReqest::OK;
	};

	StatusDDSUnit DDSUnit_Subscriber::GetCurrentStatus() const
	{
		return GlobalStatus.load(std::memory_order_relaxed);
	};

	ConfigDDSUnit DDSUnit_Subscriber::GetConfig() const
	{
		return config;
	};

	ResultReqest DDSUnit_Subscriber::SetNewConfig(ConfigDDSUnit conf)
	{
		start_config = conf;
		return ResultReqest::OK;
	};

	TypeDDSUnit DDSUnit_Subscriber::GetType() const
	{
		return TypeDDSUnit::SUBSCRIBER;
	};

	DDSUnit_Subscriber::~DDSUnit_Subscriber()
	{
		std::string helpstr;

		if (Delete() == ResultReqest::ERR)
		{
			helpstr.clear();
			helpstr += "Error DDSUnit: Error Destructor : name units: " + config.PointName;
			log->WriteLogERR(helpstr.c_str(), 0, 0);
		}
	}



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

	std::string DDSUnit_Subscriber::CreateNameUnit(std::string short_name)
	{
		return "Subscriber_" + short_name;
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
