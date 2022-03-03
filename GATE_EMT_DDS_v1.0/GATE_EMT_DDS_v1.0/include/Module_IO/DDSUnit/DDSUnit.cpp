#include "DDSUnit.hpp"


namespace scada_ate::gate::ddsunit
{
	std::shared_ptr<IDDSUnit> CreateDDSUnit(ConfigDDSUnit config)
	{
		std::shared_ptr<IDDSUnit> p = nullptr;
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
		log = LoggerSpaceScada::GetLoggerScada(LoggerSpaceScada::TypeLogger::SPDLOG);
		name_unit = CreateNameUnit(start_config.PointName);
		log->Debug("DDSUnit_Subscriber : Create unit name: {}", name_unit);
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
			helpstr += "Error DDSUnit: Initialization already done: name units: " + this->name_unit;
			log->Warning("DDSUnit {}: Initialization already done", name_unit);
			return ResultReqest::IGNOR;
		}

		try 
		{

			config = start_config;
			name_unit = CreateNameUnit(config.PointName);

			/// --- иницализация participant --- /// 
			result_command = init_participant();
			if (result_command != ResultReqest::OK)
			{
				throw 1;
			}

			/// --- инициализация subscriber --- ///
			result_command = init_subscriber();
			if (result_command != ResultReqest::OK)
			{
				throw 2;
			}

			/// --- регистрация типа ---- ///
			result_command = register_type();
			if (result_command != ResultReqest::OK)
			{
				throw 4;
			}

			/// --- регистрация топика --- ///
			result_command = register_topic();
			if (result_command != ResultReqest::OK)
			{
				throw 5;
			}

			/// --- создание адаптера --- /// 
			result_command = init_adapter();
			if (result_command != ResultReqest::OK)
			{
				throw 6;
			}

			/// --- регистрация DataReader --- /// 

			result_command = init_reader_data();
			if (result_command != ResultReqest::OK)
			{
				throw 7;
			}

		}
		catch (int& e)
		{
			log->Critical("DDSUnit {}: Error Initialization: error: {}", name_unit, e);
			SetStatus(StatusDDSUnit::ERROR_INIT);
			return ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("DDSUnit {}: Error Initialization: error: {}", name_unit, 0);
			SetStatus(StatusDDSUnit::ERROR_INIT);
			return ResultReqest::ERR;
		}		

		log->Debug("DDSUnit {}: Initialization done", name_unit);
		SetStatus(StatusDDSUnit::WORK);
		return ResultReqest::OK;
	}

	ResultReqest DDSUnit_Subscriber::init_participant()
	{	

		std::string helpstr;
		ResultReqest result{ ResultReqest::OK };
		DomainParticipantQos qos = PARTICIPANT_QOS_DEFAULT;

		/// --- инициализация политик --- ///
		///--------------------------------------------///
		//////////////////////////////////////////////////

		/// --- иницализация participant --- /// 	

		try
		{
			participant_ =
				DomainParticipantFactory::get_instance()->create_participant(this->config.Domen, qos, nullptr);
			if (!participant_) throw - 1;
		}
		catch (...)
		{
			log->Critical("DDSUnit {}: Error Init_participant");
			result = ResultReqest::ERR;
		}

		return result;
	}

	ResultReqest DDSUnit_Subscriber::init_subscriber()
	{
		std::string helpstr;
		ResultReqest result{ ResultReqest::OK };
		try
		{
			subscriber_ = participant_->create_subscriber(SUBSCRIBER_QOS_DEFAULT, nullptr);
			if (!subscriber_) throw 1;
		}
		catch (int& e)
		{
			log->Critical("DDSUnit {}: Error Init_subscriber: error {} ", this->name_unit, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("DDSUnit {}: Error Init_subscriber: error {} ", this->name_unit, 0);
			result = ResultReqest::ERR;
		}

		return result;
	}

	ResultReqest DDSUnit_Subscriber::register_type()
	{
		ResultReqest result{ ResultReqest::OK };

		try
		{
			TypeSupport PtrSupporType(new DDSDataPubSubType());
			TypeSupport PtrSupporTypeEx(new DDSDataExPubSubType());
			if (PtrSupporType.register_type(participant_) != ReturnCode_t::RETCODE_OK) throw 1;
			if (PtrSupporTypeEx.register_type(participant_) != ReturnCode_t::RETCODE_OK) throw 2;
		}
		catch (int& e)
		{
			log->Critical("DDSUnit {}: Error register_type: error {}", this->name_unit, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("DDSUnit {}: Error register_type: error {}", this->name_unit, 0);
			result = ResultReqest::ERR;
		}

		return result;
	}

	ResultReqest DDSUnit_Subscriber::register_topic()
	{
		ResultReqest result{ ResultReqest::OK };
		TypeSupport type_;

		try
		{
			if (config.Typedata == adapter::TypeData::Base)
			{
				type_ = TypeSupport(new DDSDataPubSubType());
				data_point = std::make_shared<DDSData>();
			}
			else if (config.Typedata == adapter::TypeData::Extended)
			{
				type_ = TypeSupport(new DDSDataExPubSubType());
				data_point = std::make_shared<DDSDataEx>();
			}

			topic_data = participant_->create_topic( CreateNameTopic(), type_.get_type_name(), TOPIC_QOS_DEFAULT);
			if (topic_data == nullptr) throw 1;

		}
		catch (int& e)
		{
			log->Critical("DDSUnit {}: Error registration of topic: {}", this->name_unit, e);
			result = ResultReqest::OK;
		}
		catch (...)
		{
			log->Critical("DDSUnit {}: Error registration of topic: {}", this->name_unit, 0);
			result = ResultReqest::OK;
		}
		return result;
	};

	ResultReqest DDSUnit_Subscriber::init_reader_data()
	{
		std::string helpstr;
		ResultReqest result{ ResultReqest::OK };

		try
		{
			if (this->config.Frequency <= 0)
			{
				reader_data = subscriber_->create_datareader(topic_data, DATAREADER_QOS_DEFAULT, listener_.get());
				if (reader_data == nullptr) throw 1;
				listener_->Start();
			}
			else
			{
				reader_data = subscriber_->create_datareader(topic_data, DATAREADER_QOS_DEFAULT, nullptr);
				if (reader_data == nullptr) throw 2;
				thread_transmite = std::jthread(&DDSUnit_Subscriber::function_thread_transmite, this);
			}
		}
		catch (int& e)
		{
			log->Critical("DDSUnit {}: Error initialization reader data: error {}", this->name_unit, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("DDSUnit {}: Error initialization reader data: error {}", this->name_unit, 0);
			result = ResultReqest::ERR;
		}

		return result;
	};

	ResultReqest DDSUnit_Subscriber::init_adapter()
	{
		std::string helpstr;
		ResultReqest result{ResultReqest::OK};

		try
		{
			if (AdapterUnit != nullptr) throw 3;

			AdapterUnit = CreateAdapter(this->config.Adapter);
			if (AdapterUnit == nullptr) throw 1;
			result = AdapterUnit->InitAdapter(config.conf_adapter);
			if (result != ResultReqest::OK) throw 2;
		}
		catch (int& e_int)
		{
			log->Critical("DDSUnit {}: Error initialization of Adapter: error {}", this->name_unit, e_int);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("DDSUnit {}: Error initialization of Adapter: error {}", this->name_unit, 0);
			result = ResultReqest::ERR;
		}

		return result;
	}

	void DDSUnit_Subscriber::function_thread_transmite()
	{
		std::chrono::steady_clock::time_point start, end;
		std::chrono::milliseconds delta_ms;
		start = std::chrono::steady_clock::now();
		eprosima::fastrtps::types::DynamicData* array = nullptr;
		SampleInfo info;
		ReturnCode_t res;
		std::string helpstr;
		std::stop_token stoper;
		status_thread.store(StatusThreadDSSUnit::WORK, std::memory_order_relaxed);

		try
		{

			stoper = thread_transmite.get_stop_token();
			if (!stoper.stop_possible()) throw 1;

			while (1)
			{
				if (stoper.stop_requested()) break;

				end = std::chrono::steady_clock::now();
				delta_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
				if (delta_ms.count() < config.Frequency - scatter_frequency)
				{ 
					std::this_thread::sleep_for(std::chrono::microseconds(1)); 
					continue; 
				}
				else if (delta_ms.count() > config.Frequency)
				{
					log->Warning("DDSUnit {}: Time out transfer data: {} ", this->name_unit, delta_ms.count());
				}
				start = std::chrono::steady_clock::now();


				if (reader_data->take_next_sample(data_point.get(), &info) != ReturnCode_t::RETCODE_OK) //throw 2;
				{
					log->Warning("DDSUnit {}: Error read data in thread of thransfer", this->name_unit);
					continue;
				}

				if (AdapterUnit->WriteData(std::static_pointer_cast<DDSData>(data_point)) != ResultReqest::OK) throw 4;
				log->Debug("DDSUnit {}: Thread of thransfer: read done", this->name_unit);
			}
		}
		catch (int& e)
		{
			log->Critical("DDSUnit {}: Error in thread of thransfer: error {}", this->name_unit, e);
			status_thread.store(StatusThreadDSSUnit::FAIL, std::memory_order_relaxed);
			return;
		}
		catch (...)
		{
			log->Critical("DDSUnit {}: Error in thread of thransfer: error {}", this->name_unit, 0);
			status_thread.store(StatusThreadDSSUnit::FAIL, std::memory_order_relaxed);
			return;
		}

		status_thread.store(StatusThreadDSSUnit::TERMINATE, std::memory_order_relaxed);
		return;
	};

	void DDSUnit_Subscriber::SubListener::on_subscription_matched(DataReader*, const SubscriptionMatchedStatus& info)
	{
		//////////
		//////////
		//////////
	}

	void DDSUnit_Subscriber::SubListener::on_data_available(DataReader* reader)
	{
		SampleInfo info;
		eprosima::fastrtps::types::DynamicData* array = nullptr;
		std::string helpstr;
		if ( status.load(std::memory_order_relaxed) != CommandListenerSubscriber::START) return;

		try
		{
			if (reader->take_next_sample(master->data_point.get(), &info) != ReturnCode_t::RETCODE_OK) //throw 2;
			{
				master->log->Warning("DDSUnit {}: Error read data in thread of thransfer", master->name_unit);
			}

			if (master->AdapterUnit->WriteData(std::static_pointer_cast<DDSData>(master->data_point)) != ResultReqest::OK) throw 4;
			master->log->Debug("DDSUnit {}: Thread of thransfer: read done", master->name_unit);

		}
		catch (int& e)
		{
			master->log->Critical("DDSUnit {}: Error in listener_: error ", this->master->name_unit, e);
			return;
		}
		catch (...)
		{
			master->log->Critical("DDSUnit {}: Error in listener_: error ", this->master->name_unit, 0);
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
		ResultReqest result{ ResultReqest::OK };
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
			log->Debug("DDSUnit {}: Stop Command done", this->name_unit);
		}
		catch (int& e)
		{
			log->Critical("DDSUnit {}: Error stop command: error {}", this->name_unit, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("DDSUnit {}: Error stop command: error {}", this->name_unit, 0);
			result = ResultReqest::ERR;
		}

		return result;
	};

	ResultReqest DDSUnit_Subscriber::Start()
	{
		std::string helpstr;
		ResultReqest result = ResultReqest::OK;

		try
		{
			if (GetCurrentStatus() != StatusDDSUnit::STOP) throw 1;

			if (this->config.Frequency <= 0)
			{
				listener_->Start();
			}
			else
			{
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
			}

			SetStatus(StatusDDSUnit::WORK);

			log->Debug("DDSUnit {}: command Start done", this->name_unit);
		}
		catch (int& e)
		{
			log->Critical("DDSUnit {}: Error command Start: error {}", this->name_unit, e);
		}
		catch (...)
		{
			log->Critical("DDSUnit {}: Error command Start: error {}", this->name_unit, 0);
		}

		return result;
	};

	ResultReqest DDSUnit_Subscriber::Delete()
	{
		std::string helpstr;
		ResultReqest result = { ResultReqest::OK };

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

			log->Debug("DDSUnit {}: Delete done", this->name_unit);
		}
		catch (int& e)
		{
			log->Critical("DDSUnit {}: Error command delete: error {}", this->name_unit, e);
			SetStatus(StatusDDSUnit::ERROR_DESTROYED);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("DDSUnit {}: Error command delete: error {}", this->name_unit, 0);
			SetStatus(StatusDDSUnit::ERROR_DESTROYED);
			result = ResultReqest::ERR;
		}

		return result;
	};

	ResultReqest DDSUnit_Subscriber::Restart()
	{
		std::string helpstr;
		ResultReqest result = { ResultReqest::OK };

		try
		{
			if (Delete() != ResultReqest::OK) throw 1;
			if (Initialization() != ResultReqest::OK) throw 2;
			log->Debug("DDSUnit {}: Restart done");
		}
		catch (int& e)
		{
			log->Critical("DDSUnit {}: Error command Restart: error {}", this->name_unit, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("DDSUnit {}: Error command Restart: error {}", this->name_unit, 0);
			result = ResultReqest::ERR;
		}

		return result;
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
			log->Critical("DDSUnit {}: Error Destructor", this->name_unit);
		}
	}

	void DDSUnit_Subscriber::SetStatus(StatusDDSUnit status)
	{
		GlobalStatus.store(status, std::memory_order_relaxed);
	};

	std::string DDSUnit_Subscriber::CreateNameTopic()
	{
		std::string str;
		if (config.Typedata == adapter::TypeData::Base) str += "TopicDDSData_";
		str += config.PointName;
		return str;
	}

	std::string DDSUnit_Subscriber::CreateNameUnit(std::string short_name)
	{
		return "Subscriber_" + short_name;
	}


	/////////////////////////////////////////////////
	/// -------------- PUBLISHER --------------- ///
	////////////////////////////////////////////////

	DDSUnit_Publisher::DDSUnit_Publisher(ConfigDDSUnit config) : start_config(config)
	{
		SetStatus(StatusDDSUnit::EMPTY);
		log = LoggerSpaceScada::GetLoggerScada(LoggerSpaceScada::TypeLogger::SPDLOG);
		name_unit = CreateNameUnit(start_config.PointName);
	}

	ResultReqest DDSUnit_Publisher::Initialization()
	{
		std::string helpstr;
		ResultReqest result {ResultReqest::OK};
		StatusDDSUnit status_unit = GetCurrentStatus();

		try
		{
		}
		catch(int& e)
		{
			if (status_unit != StatusDDSUnit::EMPTY &&
				status_unit != StatusDDSUnit::ERROR_INIT &&
				status_unit != StatusDDSUnit::DESTROYED)
			{
				log->Warning("DDSUnit {}: Initialization already done", this->name_unit);
				return ResultReqest::IGNOR;
			}

			config = start_config;
			name_unit = CreateNameUnit(config.PointName);

			/// --- иницализация participant --- /// 

			if (init_participant() != ResultReqest::OK)
			{
				SetStatus(StatusDDSUnit::ERROR_INIT);
				throw 1;
			}

			/// --- инициализация subscriber --- ///

			if (init_publisher() != ResultReqest::OK)
			{
				SetStatus(StatusDDSUnit::ERROR_INIT);
				throw 2;
			}

			/// --- создание динамического типа --- ///

			if (create_dynamic_data_type() != ResultReqest::OK)
			{
				SetStatus(StatusDDSUnit::ERROR_INIT);
				throw 3;
			}

			/// --- регистрация типа ---- ///

			if (register_type() != ResultReqest::OK)
			{
				SetStatus(StatusDDSUnit::ERROR_INIT);
				throw 4;
			}

			/// --- регистрация топика --- ///

			if (register_topic() != ResultReqest::OK)
			{
				SetStatus(StatusDDSUnit::ERROR_INIT);
				throw 5;
			}

			/// --- создание адаптера --- /// 

			if (init_adapter() != ResultReqest::OK)
			{
				SetStatus(StatusDDSUnit::ERROR_INIT);
				throw 6;
			}

			/// --- регистрация DataReader --- /// 

			if (init_writer_data() != ResultReqest::OK)
			{
				SetStatus(StatusDDSUnit::ERROR_INIT);
				throw 7;
			}

			SetStatus(StatusDDSUnit::WORK);
		}
		catch (int& e)
		{
			log->Critical("DDSUnit {}: Error Initialization: error {}", this->name_unit, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("DDSUnit {}: Error Initialization: error {}", this->name_unit, 0);
			result = ResultReqest::ERR;
		}		

		return result;
	}

	ResultReqest DDSUnit_Publisher::init_participant()
	{

		std::string helpstr;
		ResultReqest result{ ResultReqest::OK };

		/// --- инициализация транспортного уровня --- ///
		///--------------------------------------------///
		//////////////////////////////////////////////////

		/// --- иницализация participant --- /// 	

		try
		{
			participant_ =
				DomainParticipantFactory::get_instance()->create_participant(this->config.Domen, PARTICIPANT_QOS_DEFAULT, nullptr);
			if (!participant_) throw 1;
		}
		catch (int& e)
		{
			log->Critical("DDSUnit {}: Error create of partocipant: error {}", this->name_unit, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("DDSUnit {}: Error create of partocipant: error {}", this->name_unit, 0);
			result = ResultReqest::ERR;
		}

		return result;
	}

	ResultReqest DDSUnit_Publisher::init_publisher()
	{
		std::string helpstr;
		ResultReqest result{ ResultReqest::OK };

		try
		{
			publisher_ = participant_->create_publisher(PUBLISHER_QOS_DEFAULT, nullptr);
			if (!publisher_) throw 1;
		}
		catch (int& e)
		{
			log->Critical("DDSUnit {}: Error create of partocipant: error {}", this->name_unit, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("DDSUnit {}: Error create of partocipant: error {}", this->name_unit, 0);
			result = ResultReqest::ERR;
		}

		return result;
	}

	ResultReqest DDSUnit_Publisher::register_type()
	{

		ResultReqest result{ ResultReqest::OK };

		try
		{
			TypeSupport PtrSupporType = eprosima::fastrtps::types::DynamicPubSubType(type_data);
			PtrSupporType.get()->auto_fill_type_information(false);
			PtrSupporType.get()->auto_fill_type_object(true);
			if (PtrSupporType.register_type(participant_) != ReturnCode_t::RETCODE_OK) throw 1;
		}
		catch (int& e)
		{
			log->Critical("DDSUnit {}: Error regisration of type: error {}", this->name_unit, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("DDSUnit {}: Error regisration of type: error {}", this->name_unit, 0);
			result = ResultReqest::ERR;
		}

		return result;
	}

	ResultReqest DDSUnit_Publisher::register_topic()
	{
		ResultReqest result { ResultReqest::OK };

		try
		{
			topic_data = participant_->create_topic(CreateNameTopic(this->config.PointName), CreateNameType(this->config.PointName), TOPIC_QOS_DEFAULT);
			if (topic_data == nullptr) throw 1;
		}
		catch (int& e)
		{
			log->Critical("DDSUnit {}: Error registration of topic: error {}", this->name_unit, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("DDSUnit {}: Error registration of topic: error {}", this->name_unit, 0);
			result = ResultReqest::ERR;
		}

		return result;
	};

	ResultReqest DDSUnit_Publisher::init_adapter()
	{
		ResultReqest result { ResultReqest::OK };

		try
		{
			if (AdapterUnit != nullptr) throw 3;

			AdapterUnit = CreateAdapter(this->config.Adapter);
			if (AdapterUnit == nullptr) throw 1;
			std::shared_ptr<IConfigAdapter> conf_adater = create_config_adapter();
			if (AdapterUnit->InitAdapter(conf_adater) != ResultReqest::OK) throw 2;
		}
		catch (int& e_int)
		{
			log->Critical("DDSUnit {}: Error initialization of adapter: error {}", this->name_unit, e_int);
			return ResultReqest::ERopeR;
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

	ResultReqest DDSUnit_Publisher::init_writer_data()
	{
		std::string helpstr;

		try
		{
			writer_data = publisher_->create_datawriter(topic_data, DATAWRITER_QOS_DEFAULT);
			if (writer_data == nullptr) throw -1;		
			thread_transmite = std::jthread(&DDSUnit_Publisher::function_thread_transmite,this);
		}
		catch (...)
		{
			helpstr.clear();
			helpstr += "Error init DDSUnit: Error create of writer_data: name units: " + this->name_unit;
			log->WriteLogERR(helpstr.c_str(), 0, 0);
			return ResultReqest::ERR;
		}

		return ResultReqest::OK;
	}

	void DDSUnit_Publisher::function_thread_transmite()
	{
		std::chrono::steady_clock::time_point start, end;
		std::chrono::milliseconds delta_ms;
		start = std::chrono::steady_clock::now();
		eprosima::fastrtps::types::DynamicData* array = nullptr;
		SampleInfo info;
		ResultReqest res;
		std::string helpstr;
		std::stop_token stoper;

		std::time_t time_p;
		std::tm* time_now;
		std::chrono::system_clock::time_point time;
		std::chrono::milliseconds msec;
		unsigned int count_write = 0;

		status_thread.store(StatusThreadDSSUnit::WORK, std::memory_order_relaxed);

		int size_type_data = size_type_data_baits(config.Typedata);
		std::shared_ptr<char> mass_data(new char[size_type_data * config.Size], std::default_delete<char[]>());
		for (int i = 0; i < size_type_data * config.Size; i++) *(mass_data.get() + i) = 0;

		try
		{
			data->set_char8_value((char)config.Typedata, 0);
			data->set_uint32_value(config.Size, 5);
			stoper = thread_transmite.get_stop_token();
			if (!stoper.stop_possible()) throw 1;

			while (1)
			{
				if (stoper.stop_requested()) break;

				end = std::chrono::steady_clock::now();
				delta_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
				if (delta_ms.count() < config.Frequency - frequency_scatter)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
					continue;
				}
				start = std::chrono::steady_clock::now();

				array = data->loan_value(7);
				if (array == nullptr) throw 3;

				if (AdapterUnit->ReadData(mass_data.get(), config.Size) != ResultReqest::OK)
				{
					data->return_loaned_value(array);
					throw 4;
				}

				for (int i = 0; i < config.Size; i++)
				{
					mirror_data_to_DDS(mass_data.get(), array, i);
				}

				data->return_loaned_value(array);

				time = std::chrono::system_clock::now();
				time_p = std::chrono::system_clock::to_time_t(time);
				time_now = std::localtime(&time_p);
				msec = std::chrono::duration_cast<std::chrono::milliseconds>(time.time_since_epoch())
					- std::chrono::duration_cast<std::chrono::milliseconds>
					(std::chrono::duration_cast<std::chrono::seconds>(time.time_since_epoch()));
				count_write++;

				data->set_char8_value((char)time_now->tm_hour, 1);
				data->set_char8_value((char)time_now->tm_min, 2);
				data->set_char8_value((char)time_now->tm_sec, 3);
				data->set_uint16_value(msec.count(), 4);
				data->set_uint32_value(count_write, 6);

				if (!writer_data->write(data.get())) throw 5;
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
			return;
		}
			
		status_thread.store(StatusThreadDSSUnit::TERMINATE, std::memory_order_relaxed);
		return;
	};

	inline void  DDSUnit_Publisher::mirror_data_to_DDS(void* buf, eprosima::fastrtps::types::DynamicData* array_dds, unsigned int i)
	{
		float val;
		if (config.Typedata == TypeData::ANALOG)
		{
			array_dds->set_float32_value(*(reinterpret_cast<float*>(buf) + i), array_dds->get_array_index({ 0, i }));
		}
		else if (config.Typedata == TypeData::DISCRETE)
		{
			array_dds->set_int32_value(*(reinterpret_cast<int*>(buf) + i), array_dds->get_array_index({ 0, i }));
		}
		else if (config.Typedata == TypeData::BINAR)
		{
			array_dds->set_char8_value(*(reinterpret_cast<char*>(buf) + i), array_dds->get_array_index({ 0, i }));
		}
	}

	DDSUnit_Publisher::~DDSUnit_Publisher()
	{
		std::string helpstr;

		if (Delete() == ResultReqest::ERR)
		{
			helpstr.clear();
			helpstr += "Error DDSUnit: Error Destructor : name units: " + config.PointName;
			log->WriteLogERR(helpstr.c_str(), 0, 0);
		}
	}

	ResultReqest DDSUnit_Publisher::Stop()
	{
		std::string helpstr;
		try
		{
			if (GetCurrentStatus() != StatusDDSUnit::WORK) throw 1;

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

	ResultReqest DDSUnit_Publisher::Start()
	{
		std::string helpstr;
		try
		{
			if (GetCurrentStatus() != StatusDDSUnit::STOP) throw 1;

			if (thread_transmite.joinable())
			{
				if (status_thread.load(std::memory_order_relaxed) != StatusThreadDSSUnit::WORK)
				{
					thread_transmite.join();
					thread_transmite = std::jthread(&DDSUnit_Publisher::function_thread_transmite, this);
				}
			}
			else
			{
				thread_transmite = std::jthread(&DDSUnit_Publisher::function_thread_transmite, this);
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

	StatusDDSUnit DDSUnit_Publisher::GetCurrentStatus() const
	{
		return GlobalStatus.load(std::memory_order_relaxed);
	};

	ConfigDDSUnit DDSUnit_Publisher::GetConfig() const
	{
		return config;
	};

	ResultReqest DDSUnit_Publisher::SetNewConfig(ConfigDDSUnit conf)
	{
		start_config = conf;
		return ResultReqest::OK;
	};

	ResultReqest DDSUnit_Publisher::Restart()
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

	ResultReqest DDSUnit_Publisher::Delete()
	{
		std::string helpstr;

		try
		{
			if (thread_transmite.joinable())
			{
				thread_transmite.request_stop();
				thread_transmite.join();
			}

			if (writer_data != nullptr)
			{
				if (publisher_->delete_datawriter(writer_data) != ReturnCode_t::RETCODE_OK) throw 2;
				writer_data = nullptr;
			}
			if (topic_data != nullptr)
			{
				if (participant_->delete_topic(topic_data) != ReturnCode_t::RETCODE_OK) throw 3;
				topic_data = nullptr;
			}
			if (publisher_ != nullptr)
			{
				if (participant_->delete_publisher(publisher_) != ReturnCode_t::RETCODE_OK) throw 4;
				publisher_ = nullptr;
			}
			
			if (!participant_)
			{
				if (DomainParticipantFactory::get_instance()->delete_participant(participant_) != ReturnCode_t::RETCODE_OK) throw 5;
				participant_ = nullptr;
			}


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

	TypeDDSUnit DDSUnit_Publisher::GetType() const
	{
		return TypeDDSUnit::PUBLISHER;
	}

	void DDSUnit_Publisher::SetStatus(StatusDDSUnit status)
	{
		GlobalStatus.store(status, std::memory_order_relaxed);
	};

	std::string DDSUnit_Publisher::CreateNameTopic(std::string short_name)
	{
		return "TopicdataDDS";
		//return "TopicdataDDS_" + short_name;
	}

	std::string DDSUnit_Publisher::CreateNameType(std::string short_name)
	{
		return "TypedataDDS_" + short_name;
	}

	std::string DDSUnit_Publisher::CreateNameUnit(std::string short_name)
	{
		return "Publisher_" + short_name;
	}


}
