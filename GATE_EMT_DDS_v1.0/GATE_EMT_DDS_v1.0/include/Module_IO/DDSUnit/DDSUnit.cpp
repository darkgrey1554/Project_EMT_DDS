#include "DDSUnit.hpp"


namespace scada_ate::gate::ddsunit
{

	std::shared_ptr<IDDSUnit> CreateDDSUnit(ConfigDDSUnit config)
	{
		std::shared_ptr<IDDSUnit> p = nullptr;
		if (config.TypeUnit == TypeDDSUnit::SUBSCRIBER)
		{
			if (config.Typeinfo == adapter::TypeInfo::Data && config.Typedata == adapter::TypeData::Base)
			{
				p = std::make_shared<DDSUnit_Subscriber<DDSData>>(config);
			}
			else if (config.Typeinfo == adapter::TypeInfo::Data && config.Typedata == adapter::TypeData::Extended)
			{
				p = std::make_shared<DDSUnit_Subscriber<DDSDataEx>>(config);
			}

		}
		else if (config.TypeUnit == TypeDDSUnit::PUBLISHER)
		{
			if (config.Typeinfo == adapter::TypeInfo::Data && config.Typedata == adapter::TypeData::Base)
			{
				p = std::make_shared<DDSUnit_Publisher<DDSData>>(config);
			}
			else if (config.Typeinfo == adapter::TypeInfo::Data && config.Typedata == adapter::TypeData::Extended)
			{
				p = std::make_shared<DDSUnit_Publisher<DDSDataEx>>(config);
			}

		}

		return p;
	}
	/// --- SUBSCRIBER --- ///
	template<class TKind> DDSUnit_Subscriber<TKind>::DDSUnit_Subscriber(ConfigDDSUnit config) : start_config(config)
	{
		SetStatus(StatusDDSUnit::EMPTY);
		log = LoggerSpaceScada::GetLoggerScada(LoggerSpaceScada::TypeLogger::SPDLOG);
		name_unit = CreateNameUnit(start_config.PointName);
		log->Debug("DDSUnit_Subscriber : Create unit name: {}", name_unit);
	};

	template<class TKind> ResultReqest DDSUnit_Subscriber<TKind>::Initialization()
	{
		ResultReqest result_command;
		StatusDDSUnit status_unit = GetCurrentStatus();
		if (status_unit != StatusDDSUnit::EMPTY &&
			status_unit != StatusDDSUnit::ERROR_INIT &&
			status_unit != StatusDDSUnit::DESTROYED)
		{
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

		log->Info("DDSUnit {}: Initialization done", name_unit);
		SetStatus(StatusDDSUnit::WORK);
		return ResultReqest::OK;
	}

	template<class TKind> ResultReqest DDSUnit_Subscriber<TKind>::init_participant()
	{	

		std::string helpstr;
		ResultReqest result{ ResultReqest::OK };
		DomainParticipantQos qos = PARTICIPANT_QOS_DEFAULT;

		qos.transport().use_builtin_transports = false;
		auto tcp_transport = std::make_shared<eprosima::fastdds::rtps::TCPv4TransportDescriptor>();
		qos.transport().user_transports.push_back(tcp_transport);
		qos.transport().send_socket_buffer_size = 10000000;
		qos.transport().listen_socket_buffer_size = 10000000;

		eprosima::fastrtps::rtps::Locator_t initial_peer_locator;
		initial_peer_locator.kind = LOCATOR_KIND_TCPv4;
		eprosima::fastrtps::rtps::IPLocator::setIPv4(initial_peer_locator, config.IP_MAIN);
		initial_peer_locator.port = config.Port_MAIN;
		qos.wire_protocol().builtin.initialPeersList.push_back(initial_peer_locator);
		qos.transport().use_builtin_transports = false;

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

	template<class TKind> ResultReqest DDSUnit_Subscriber<TKind>::init_subscriber()
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

	template<class TKind> ResultReqest DDSUnit_Subscriber<TKind>::register_type()
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

	template<class TKind> ResultReqest DDSUnit_Subscriber<TKind>::register_topic()
	{
		ResultReqest result{ ResultReqest::OK };
		TypeSupport type_;

		try
		{
			if (typeid(TKind) == typeid(DDSData))
			{
				type_ = TypeSupport(new DDSDataPubSubType());
			}
			else if (typeid(TKind) == typeid(DDSDataEx))
			{
				type_ = TypeSupport(new DDSDataExPubSubType());
			}

			data_point = std::make_shared<TKind>();
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

	template<class TKind> ResultReqest DDSUnit_Subscriber<TKind>::init_reader_data()
	{
		ResultReqest result{ ResultReqest::OK };
		DataReaderQos rqos = DATAREADER_QOS_DEFAULT;

		try
		{
			if (config.Typerecieve == TypeRecieve::LISTEN)
			{
				reader_data = subscriber_->create_datareader(topic_data, rqos, listener_.get());
				if (reader_data == nullptr) throw 1;
				
			}
			else if (config.Typerecieve == TypeRecieve::REVIEW)
			{
				reader_data = subscriber_->create_datareader(topic_data, rqos, nullptr);
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

	template<class TKind> ResultReqest DDSUnit_Subscriber<TKind>::init_adapter()
	{
		ResultReqest result{ResultReqest::OK};

		try
		{
			if (AdapterUnit != nullptr) throw 3;
			AdapterUnit = CreateAdapter(this->config.Adapter);
			if (AdapterUnit == nullptr) throw 1;
			result = AdapterUnit->InitAdapter(this->config.conf_adapter);
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

	template<class TKind> void DDSUnit_Subscriber<TKind>::function_thread_transmite()
	{
		long long start, end;
		long delta_ms;
		SampleInfo info;
		ReturnCode_t res;
		std::string helpstr;
		std::stop_token stoper;
		status_thread.store(StatusThreadDSSUnit::WORK, std::memory_order_relaxed);
		int count_ =0;
		start = TimeConverter::GetTime_LLmcs();
		unsigned int count_timeout = 1;
		try
		{
			stoper = thread_transmite.get_stop_token();
			if (!stoper.stop_possible()) throw 1;

			while (1)
			{
				if (stoper.stop_requested()) break;

				
				while (1)
				{
					if (reader_data->take_next_sample(data_point.get(), &info) != ReturnCode_t::RETCODE_OK) //throw 2;
					{
						end = TimeConverter::GetTime_LLmcs();
						if ((end - start) / 1000 > config.Frequency*count_timeout)
						{
							count_timeout++;
							log->Warning("DDSUnit {}: TimeOut recieve data: {} ms", this->name_unit, (end - start)/1000);
						}
						std::this_thread::sleep_for(1ms);
						continue;
					}
					break;
				}

				count_timeout=1;
				end = TimeConverter::GetTime_LLmcs();
				if ((end - start) / 1000 > config.Frequency)
				{
					count_timeout++;
					log->Warning("DDSUnit {}: TimeOut recieve data: {} ms", this->name_unit, (end - start) / 1000);
				}
				start = end;

				if (AdapterUnit->WriteData(data_point) != ResultReqest::OK)
				{
					log->Warning("DDSUnit {}: Error WriteData Adapter", this->name_unit);
				}
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

	template<class TKind> void DDSUnit_Subscriber<TKind>::SubListener::on_subscription_matched(DataReader*, const SubscriptionMatchedStatus & info)
	{
		///
	}

	template<class TKind> void DDSUnit_Subscriber<TKind>::SubListener::on_data_available(DataReader* reader)
	{
		SampleInfo info;
		std::string helpstr;
		if ( status.load(std::memory_order_relaxed) != CommandListenerSubscriber::START) return;

		try
		{
			delta_trans = TimeConverter::GetTime_LLmcs();

			if ((delta_trans - delta_trans_last)/1000 > master->config.Frequency)
			{
				master->log->Warning("DDSUnit {}: TimeOut transfer data: {} ms", (delta_trans - delta_trans_last) / 1000);
			}
			
			delta_trans_last = delta_trans;

			if (reader->take_next_sample(master->data_point.get(), &info) != ReturnCode_t::RETCODE_OK)
			{
				master->log->Warning("DDSUnit {}: Error read data in thread of thransfer", master->name_unit);
			}

			if (master->AdapterUnit->WriteData(master->data_point) != ResultReqest::OK)
			{
				master->log->Warning("DDSUnit {}: Error WriteData Adapter", master->name_unit);
			}
			
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

	template<class TKind> ResultReqest DDSUnit_Subscriber<TKind>::Stop()
	{
		std::string helpstr;
		ResultReqest result{ ResultReqest::OK };
		try
		{
			if (GetCurrentStatus() != StatusDDSUnit::WORK) throw 1;

			

			if (thread_transmite.joinable())
			{
				//thread_transmite.request_stop();
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

	template<class TKind> ResultReqest DDSUnit_Subscriber<TKind>::Start()
	{
		std::string helpstr;
		ResultReqest result = ResultReqest::OK;

		try
		{
			if (GetCurrentStatus() != StatusDDSUnit::STOP) throw 1;

			if (this->config.Frequency <= 0)
			{
				
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

	template<class TKind> ResultReqest DDSUnit_Subscriber<TKind>::Delete()
	{
		std::string helpstr;
		ResultReqest result = { ResultReqest::OK };

		try
		{
			

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

	template<class TKind> ResultReqest DDSUnit_Subscriber<TKind>::Restart()
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

	template<class TKind> StatusDDSUnit DDSUnit_Subscriber<TKind>::GetCurrentStatus() const
	{
		return GlobalStatus.load(std::memory_order_relaxed);
	};

	template<class TKind> ConfigDDSUnit DDSUnit_Subscriber<TKind>::GetConfig() const
	{
		return config;
	};

	template<class TKind> ResultReqest DDSUnit_Subscriber<TKind>::SetNewConfig(ConfigDDSUnit conf)
	{
		start_config = conf;
		return ResultReqest::OK;
	};

	template<class TKind> TypeDDSUnit DDSUnit_Subscriber<TKind>::GetType() const
	{
		return TypeDDSUnit::SUBSCRIBER;
	};

	template<class TKind> DDSUnit_Subscriber<TKind>::~DDSUnit_Subscriber()
	{
		std::string helpstr;

		if (Delete() == ResultReqest::ERR)
		{
			log->Critical("DDSUnit {}: Error Destructor", this->name_unit);
		}
	}

	template<class TKind> void DDSUnit_Subscriber<TKind>::SetStatus(StatusDDSUnit status)
	{
		GlobalStatus.store(status, std::memory_order_relaxed);
	};

	template<class TKind> std::string DDSUnit_Subscriber<TKind>::CreateNameTopic()
	{
		std::string str;
		if (config.Typedata == adapter::TypeData::Base) str += "TopicDDSData_";
		str += config.PointName;
		return str;
	}

	template<class TKind> std::string DDSUnit_Subscriber<TKind>::CreateNameUnit(std::string short_name)
	{
		return "Subscriber_" + short_name;
	}


	/////////////////////////////////////////////////
	/// -------------- PUBLISHER --------------- ///
	////////////////////////////////////////////////

	template<class TKind> DDSUnit_Publisher<TKind>::DDSUnit_Publisher(ConfigDDSUnit config) : start_config(config)
	{
		SetStatus(StatusDDSUnit::EMPTY);
		log = LoggerSpaceScada::GetLoggerScada(LoggerSpaceScada::TypeLogger::SPDLOG);
		name_unit = CreateNameUnit(start_config.PointName);
	}

	template<class TKind> ResultReqest DDSUnit_Publisher<TKind>::Initialization()
	{
		std::string helpstr;
		ResultReqest result {ResultReqest::OK};
		StatusDDSUnit status_unit = GetCurrentStatus();

		try
		{
			if (status_unit != StatusDDSUnit::EMPTY &&
				status_unit != StatusDDSUnit::ERROR_INIT &&
				status_unit != StatusDDSUnit::DESTROYED)
			{
				log->Warning("DDSUnit {}: Initialization already done", this->name_unit);
				return ResultReqest::IGNOR;
			}

			log->Debug("DDSUnit {}: Start Initialization", this->name_unit);

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

		log->Info("DDSUnit {}: Initialization done", name_unit);
		SetStatus(StatusDDSUnit::WORK);
		return result;
	}

	template<class TKind> ResultReqest DDSUnit_Publisher<TKind>::init_participant()
	{

		ResultReqest result{ ResultReqest::OK };

		DomainParticipantQos qos = PARTICIPANT_QOS_DEFAULT;

		qos.transport().send_socket_buffer_size = 10000000;
		qos.transport().listen_socket_buffer_size = 10000000;

		auto tcp_transport = std::make_shared<eprosima::fastdds::rtps::TCPv4TransportDescriptor>();
		tcp_transport->sendBufferSize = 10000000;
		tcp_transport->receiveBufferSize = 10000000;
		tcp_transport->add_listener_port(config.Port_MAIN);
		tcp_transport->set_WAN_address(config.IP_MAIN);

		qos.transport().user_transports.push_back(tcp_transport);
		qos.transport().use_builtin_transports = false;

		try
		{
			participant_ =
				DomainParticipantFactory::get_instance()->create_participant(this->config.Domen, qos, nullptr);
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

	template<class TKind> ResultReqest DDSUnit_Publisher<TKind>::init_publisher()
	{
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

	template<class TKind> ResultReqest DDSUnit_Publisher<TKind>::register_type()
	{
		ResultReqest result{ ResultReqest::OK };
		TypeSupport type_;

		try
		{
			TypeSupport PtrSupporType(new DDSDataPubSubType());
			TypeSupport PtrSupporTypeEx(new DDSDataExPubSubType());
			if (PtrSupporType.register_type(participant_) != ReturnCode_t::RETCODE_OK) throw 1;
			if (PtrSupporTypeEx.register_type(participant_) != ReturnCode_t::RETCODE_OK) throw 2;
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

	template<class TKind> ResultReqest DDSUnit_Publisher<TKind>::register_topic()
	{
		ResultReqest result { ResultReqest::OK };
		TypeSupport type_;

		try
		{
			if (typeid(TKind) == typeid(DDSData))
			{
				type_ = TypeSupport(new DDSDataPubSubType());
			}
			else if (typeid(TKind) == typeid(DDSDataEx))
			{
				type_ = TypeSupport(new DDSDataExPubSubType());
			}

			data_point = std::make_shared<TKind>();
			topic_data = participant_->create_topic(CreateNameTopic(), type_.get_type_name(), TOPIC_QOS_DEFAULT);
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

	template<class TKind> ResultReqest DDSUnit_Publisher<TKind>::init_adapter()
	{
		ResultReqest result { ResultReqest::OK };

		try
		{
			if (AdapterUnit != nullptr) throw 1;
			AdapterUnit = CreateAdapter(config.Adapter);
			if (AdapterUnit->InitAdapter(config.conf_adapter) != ResultReqest::OK) throw 2;
		}
		catch (int& e_int)
		{
			log->Critical("DDSUnit {}: Error initialization of adapter: error {}", this->name_unit, e_int);
			result=ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("DDSUnit {}: Error initialization of adapter: error {}", this->name_unit, 0);
			result=ResultReqest::ERR;
		}

		return result;
	}

	template<class TKind> ResultReqest DDSUnit_Publisher<TKind>::init_writer_data()
	{

		DataWriterQos qos = DATAWRITER_QOS_DEFAULT;
		qos.history().depth = 1;
		qos.reliability().kind = BEST_EFFORT_RELIABILITY_QOS;
		qos.publish_mode().kind = ASYNCHRONOUS_PUBLISH_MODE;


		try
		{
			writer_data = publisher_->create_datawriter(topic_data, qos);
			if (writer_data == nullptr) throw 1;		
			thread_transmite = std::jthread(&DDSUnit_Publisher::function_thread_transmite,this);
		}
		catch (int& e)
		{
			log->Critical("DDSUnit {}: Error create of writer_data: error {}", this->name_unit, e);
			return ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("DDSUnit {}: Error create of writer_data: error {}", this->name_unit, 0);
			return ResultReqest::ERR;
		}

		return ResultReqest::OK;
	}

	template<class TKind> void DDSUnit_Publisher<TKind>::function_thread_transmite()
	{
		long long start, end;
		long long delta_ms;
		
		SampleInfo info;
		ResultReqest res;
		std::stop_token stoper;

		std::chrono::system_clock::time_point time;
		std::chrono::milliseconds msec;
		unsigned int count_write = 0;
		status_thread.store(StatusThreadDSSUnit::WORK, std::memory_order_relaxed);

		start = TimeConverter::GetTime_LLmcs();

		try
		{
			stoper = thread_transmite.get_stop_token();
			if (!stoper.stop_possible()) throw 1;
			start = TimeConverter::GetTime_LLmcs();

			while (1)
			{
				if (stoper.stop_requested()) break;

				end = TimeConverter::GetTime_LLmcs();
				delta_ms = (end - start)/1000;
				if (delta_ms < config.Frequency - scatter_frequency)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(5));
					continue;
				}

				start = end;

				if (delta_ms > config.Frequency)
				{
					log->Warning("Time transfer erro : {}", delta_ms);
				}				

				AdapterUnit->ReadData(data_point);			
				
				if (!writer_data->write(data_point.get()))
				{
					log->Critical("ERROR 123123  ee");
				}
			
			}
		}
		catch (int& e)
		{
			log->Critical("ERROR 123123  {}", e);
			status_thread.store(StatusThreadDSSUnit::FAIL, std::memory_order_relaxed);
			return;
		}
		catch (...)
		{
			log->Critical("ERROR 123123");
			status_thread.store(StatusThreadDSSUnit::FAIL, std::memory_order_relaxed);
			return;
		}
			
		status_thread.store(StatusThreadDSSUnit::TERMINATE, std::memory_order_relaxed);
		return;
	};


	template<class TKind> DDSUnit_Publisher<TKind>::~DDSUnit_Publisher()
	{
		std::string helpstr;

		if (Delete() == ResultReqest::ERR)
		{
			helpstr.clear();
			helpstr += "Error DDSUnit: Error Destructor : name units: " + config.PointName;
			//log->WriteLogERR(helpstr.c_str(), 0, 0);
		}
	}

	template<class TKind> ResultReqest DDSUnit_Publisher<TKind>::Stop()
	{
		std::string helpstr;
		try
		{
			if (GetCurrentStatus() != StatusDDSUnit::WORK) throw 1;

			if (thread_transmite.joinable())
			{
				//thread_transmite.request_stop();
				thread_transmite.join();
			}
			SetStatus(StatusDDSUnit::STOP);

		}
		catch (int& e)
		{
			helpstr.clear();
			helpstr += "Error DDSUnit: Stop command error: name units: " + this->name_unit;
			//log->WriteLogERR(helpstr.c_str(), e, 0);
			return ResultReqest::ERR;
		}
		catch (...)
		{
			helpstr.clear();
			helpstr += "Error DDSUnit: Stop command error: name units: " + this->name_unit;
			//log->WriteLogERR(helpstr.c_str(), 0, 0);
			return ResultReqest::ERR;
		}

		helpstr.clear();
		helpstr += "DDSUnit: Stop command done: name units: " + this->name_unit;
		//log->WriteLogINFO(helpstr.c_str(), 0, 0);
		return ResultReqest::OK;
	};

	template<class TKind> ResultReqest DDSUnit_Publisher<TKind>::Start()
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
			//log->WriteLogERR(helpstr.c_str(), e, 0);
			return ResultReqest::ERR;
		}
		catch (...)
		{
			helpstr.clear();
			helpstr += "Error DDSUnit: Start command error: name units: " + this->name_unit;
			//log->WriteLogERR(helpstr.c_str(), 0, 0);
			return ResultReqest::ERR;
		}

		helpstr.clear();
		helpstr += "DDSUnit: command Start done: name units: " + this->name_unit;
		//log->WriteLogINFO(helpstr.c_str(), 0, 0);
		return ResultReqest::OK;
	};

	template<class TKind> StatusDDSUnit DDSUnit_Publisher<TKind>::GetCurrentStatus() const
	{
		return GlobalStatus.load(std::memory_order_relaxed);
	};

	template<class TKind> ConfigDDSUnit DDSUnit_Publisher<TKind>::GetConfig() const
	{
		return config;
	};

	template<class TKind> ResultReqest DDSUnit_Publisher<TKind>::SetNewConfig(ConfigDDSUnit conf)
	{
		start_config = conf;
		return ResultReqest::OK;
	};

	template<class TKind> ResultReqest DDSUnit_Publisher<TKind>::Restart()
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
			//log->WriteLogERR(helpstr.c_str(), e, 0);
			return ResultReqest::ERR;
		}
		catch (...)
		{
			helpstr.clear();
			helpstr += "Error DDSUnit: Error command Restart: name units: " + this->name_unit;
			//log->WriteLogERR(helpstr.c_str(), 0, 0);
			return ResultReqest::ERR;
		}

		helpstr.clear();
		helpstr += "Info DDSUnit: Restart command done: name units: " + this->name_unit;
		//log->WriteLogINFO(helpstr.c_str(), 0, 0);
		return ResultReqest::OK;
	};

	template<class TKind> ResultReqest DDSUnit_Publisher<TKind>::Delete()
	{
		std::string helpstr;

		try
		{
			if (thread_transmite.joinable())
			{
				//thread_transmite.request_stop();
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
			//log->WriteLogERR(helpstr.c_str(), e, 0);
			SetStatus(StatusDDSUnit::ERROR_DESTROYED);
			return ResultReqest::ERR;
		}
		catch (...)
		{
			helpstr.clear();
			helpstr += "Error DDSUnit: Error to command Delete: name units: " + this->name_unit;
			//log->WriteLogERR(helpstr.c_str(), 0, 0);
			SetStatus(StatusDDSUnit::ERROR_DESTROYED);
			return ResultReqest::ERR;
		}

		helpstr.clear();
		helpstr += "Info DDSUnit: Delete command done: name units: " + this->name_unit;
		//log->WriteLogERR(helpstr.c_str(), 0, 0);

		return ResultReqest::OK;
	};

	template<class TKind> TypeDDSUnit DDSUnit_Publisher<TKind>::GetType() const
	{
		return TypeDDSUnit::PUBLISHER;
	}

	template<class TKind> void DDSUnit_Publisher<TKind>::SetStatus(StatusDDSUnit status)
	{
		GlobalStatus.store(status, std::memory_order_relaxed);
	};

	template<class TKind> std::string DDSUnit_Publisher<TKind>::CreateNameTopic()
	{
		std::string str;
		if (config.Typedata == adapter::TypeData::Base) str += "TopicDDSData_";
		str += config.PointName;
		return str;
	}

	template<class TKind> std::string DDSUnit_Publisher<TKind>::CreateNameUnit(std::string short_name)
	{
		return "Publisher_" + short_name;
	}
}
