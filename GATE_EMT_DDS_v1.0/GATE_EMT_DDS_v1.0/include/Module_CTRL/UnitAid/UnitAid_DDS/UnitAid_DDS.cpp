#include "UnitAid_DDS.hpp"

namespace atech::srv::io::ctrl
{

	UnitAid_DDS::UnitAid_DDS(std::shared_ptr<IConfigUnitAid> config)
	{
		this->config = *std::reinterpret_pointer_cast<ConfigUnitAid_DDS>(config);
		log = LoggerSpaceScada::GetLoggerScada(LoggerSpaceScada::TypeLogger::SPDLOG);
	}

	UnitAid_DDS::~UnitAid_DDS()
	{
		clear_properties();
		return;
	}

	ResultReqest UnitAid_DDS::InitUnitAid()
	{

		ResultReqest result{ ResultReqest::OK };
		const std::lock_guard<std::mutex> lock_init(_mutex_init);

		StatusUnitAid status = _status.load(std::memory_order::memory_order_relaxed);
		if (status == StatusUnitAid::INITIALIZATION || status == StatusUnitAid::OK)
		{
			result = ResultReqest::IGNOR;
			return result;
		}
		_status.store(StatusUnitAid::INITIALIZATION, std::memory_order_relaxed);

		try
		{
			if (!config.func) throw 0;

			if (init_participant() != ResultReqest::OK) throw 1;
			if (registration_types() != ResultReqest::OK) throw 2;
			if (create_topics() != ResultReqest::OK) throw 3;
			if (init_subscriber() != ResultReqest::OK) throw 4;
			if (init_publisher() != ResultReqest::OK) {};

			log->Info("UnitAdi_DDS node_id-{} : Initialization done");
			_status.store(StatusUnitAid::OK);
		}
		catch (int& e)
		{
			log->Critical("UnitAdi_DDS node_id-{} : Error initialization : error {} syserror: {}", config.node_id, e, 0);
			_status.store(StatusUnitAid::ERROR_INIT);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("UnitAdi_DDS node_id-{} : Error initialization : error {} syserror: {}", config.node_id, 0, 0);
			_status.store(StatusUnitAid::ERROR_INIT);
			result = ResultReqest::ERR;
		}

		return result;
	}

	ResultReqest UnitAid_DDS::clear_properties()
	{
		ReturnCode_t res;
		ResultReqest result{ ResultReqest::OK };

		try
		{
			if (_reader_command != nullptr)
			{
				res = _subscriber->delete_datareader(_reader_command);
				if (res != ReturnCode_t::RETCODE_OK) throw 1;
				_reader_command = nullptr;
			}

			if (_responder != nullptr)
			{
				res = _publisher->delete_datawriter(_responder);
				if (res != ReturnCode_t::RETCODE_OK) throw 2;
				_responder = nullptr;
			}

			if (_subscriber != nullptr)
			{
				res = _participant->delete_subscriber(_subscriber);
				if (res != ReturnCode_t::RETCODE_OK) throw 3;
				_subscriber = nullptr;
			}

			if (_publisher != nullptr)
			{
				res = _participant->delete_publisher(_publisher);
				if (res != ReturnCode_t::RETCODE_OK) throw 4;
				_publisher = nullptr;
			}

			if (topic_command != nullptr)
			{
				res = _participant->delete_topic(topic_command);
				if (res != ReturnCode_t::RETCODE_OK) throw 5;
				topic_command = nullptr;
			}

			if (topic_respond != nullptr)
			{
				res = _participant->delete_topic(topic_respond);
				if (res != ReturnCode_t::RETCODE_OK) throw 6;
				topic_respond = nullptr;
			}

			if (_participant != nullptr)
			{
				_participant->unregister_type(type_support_topic_command.get_type_name());
				_participant->unregister_type(type_support_topic_respond.get_type_name());
				res = _dds::DomainParticipantFactory::get_instance()->delete_participant(_participant);
				if (res != ReturnCode_t::RETCODE_OK) throw 7;
				_participant = nullptr;
			}
		}
		catch (int& e)
		{
			log->Critical("UnitAdi_DDS node_id-{} : Error clear_properties : error {} syserror: {}", config.node_id, e, res());
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("UnitAdi_DDS node_id-{} : Error clear_properties : error {} syserror: {}", config.node_id, 0, 0);
			result = ResultReqest::ERR;
		}

		return result;
	};

	ResultReqest UnitAid_DDS::init_participant()
	{
		ResultReqest result = ResultReqest::OK;

		try
		{
			_participant = _dds::DomainParticipantFactory::get_instance()->create_participant_with_profile(get_name_participant_profile());
			if (!_participant)
			{
				_participant = _dds::DomainParticipantFactory::get_instance()->create_participant(0, _dds::PARTICIPANT_QOS_DEFAULT);
				if (!_participant)
				{
					throw 1;
				}

				log->Debug("UnitAid_DDS node-{}: Init participant done : defualt", this->config.node_id);
			}
			else
			{
				log->Debug("UnitAid_DDS node-{}: Init participant done : XML-file", this->config.node_id);
			}

		}
		catch (int& e)
		{
			log->Critical("UnitAid_DDS node-{}: Error init participant : error: {}", this->config.node_id, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("UnitAid_DDS node-{}: Error init participant : error: {}", this->config.node_id, 0);
			result = ResultReqest::ERR;
		}

		return result;
	};

	ResultReqest UnitAid_DDS::init_subscriber()
	{
		ResultReqest result = ResultReqest::OK;

		try
		{
			if (!_participant) throw 1;

			_subscriber = _participant->create_subscriber_with_profile(get_name_subscriber_profile());

			if (!_subscriber)
			{
				_subscriber = _participant->create_subscriber(_dds::SUBSCRIBER_QOS_DEFAULT);
				if (!_subscriber) throw 2;

				log->Debug("UnitAid_DDS node-{}: Init subscriber done : default", this->config.node_id);
			}
			else
			{
				log->Debug("UnitAid_DDS node-{}: Init subscriber done : XML-file", this->config.node_id);
			}

			_reader_command = _subscriber->create_datareader_with_profile(topic_command, get_name_datareader_profile(), _listener.get());

			if (!_reader_command)
			{
				_reader_command = _subscriber->create_datareader(topic_command, _dds::DATAREADER_QOS_DEFAULT, _listener.get());
				if (!_reader_command) throw 3;
				log->Debug("UnitAid_DDS node-{}: Init datareader done : default", this->config.node_id);
			}
			else
			{
				log->Debug("UnitAid_DDS node-{}: Init datareader done : XML-file", this->config.node_id);
			}

		}
		catch (int& e)
		{
			log->Critical("UnitAid_DDS node-{}: Init subscriber : error: {}", this->config.node_id, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("UnitAid_DDS node-{}: Error init subscriber : error: {}", this->config.node_id, 0);
			result = ResultReqest::ERR;
		}

		return result;
	};

	ResultReqest UnitAid_DDS::init_publisher()
	{
		ResultReqest result = ResultReqest::OK;

		try
		{
			if (!_participant) throw 1;

			_publisher = _participant->create_publisher_with_profile(get_name_publisher_profile());

			if (!_publisher)
			{
				_publisher = _participant->create_publisher(_dds::PUBLISHER_QOS_DEFAULT);
				if (!_publisher) throw 2;
				log->Debug("UnitAid_DDS node-{}: Init publisher done : default", this->config.node_id);
			}
			else
			{
				log->Debug("UnitAid_DDS node-{}: Init publisher done : XML-file", this->config.node_id);
			}

			_responder = _publisher->create_datawriter_with_profile(topic_respond, get_name_datawriter_profile());

			if (!_responder)
			{
				_responder = _publisher->create_datawriter(topic_respond, _dds::DATAWRITER_QOS_DEFAULT);
				if (!_responder)
				{
					throw 3;
				}
				else
				{
					log->Debug("UnitAid_DDS node-{}: Init datawriter done : default", this->config.node_id);
				}
			}
			else
			{
				log->Debug("UnitAid_DSS node-{}: Init datawriter done : XML-file", this->config.node_id);
			}

		}
		catch (int& e)
		{
			log->Critical("UnitAid_DSS node-{}: Init publisher : error: {}", this->config.node_id, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("UnitAid_DSS node-{}: Error Init publisher: error: {}", this->config.node_id, 0);
			result = ResultReqest::ERR;
		}

		return result;
	};

	ResultReqest UnitAid_DDS::registration_types()
	{
		ResultReqest result = ResultReqest::OK;

		try
		{
			type_support_topic_command = _dds::TypeSupport(new TopicCommandPubSubType());
			if (type_support_topic_command.register_type(_participant) != ReturnCode_t::RETCODE_OK) throw 1;
		}
		catch (...)
		{
			log->Debug("UnitAid_DDS node-{}: Error registration TopicCommand", config.node_id);
			result = ResultReqest::ERR;
		}

		try
		{
			type_support_topic_respond = _dds::TypeSupport(new TopicStatusPubSubType());
			if (type_support_topic_respond.register_type(_participant) != ReturnCode_t::RETCODE_OK) throw 1;
		}
		catch (...)
		{
			log->Debug("UnitAid_DDS node-{}: Error registration TopicStatus", config.node_id);
			result = ResultReqest::ERR;
		}

		return result;
	};

	ResultReqest UnitAid_DDS::create_topics()
	{
		ResultReqest result = ResultReqest::OK;

		try
		{
			if (!_participant)
			{
				throw 1;
			}

			topic_command = _participant->create_topic_with_profile(get_name_TopicCommand(),
				type_support_topic_command.get_type_name(),
				get_name_topic_profile_command());

			if (!topic_command)
			{
				topic_command = _participant->create_topic(get_name_TopicCommand(),
					type_support_topic_command.get_type_name(),
					_dds::TOPIC_QOS_DEFAULT);
				if (!topic_command)
				{
					throw 1;
				}

				log->Debug("UnitAid_DDS node-{}: Init topic command done : defualt", this->config.node_id);
			}
			else
			{
				log->Debug("UnitAid_DDS node-{}: Init topic done : XML-file", this->config.node_id);
			}

			topic_respond = _participant->create_topic_with_profile(get_name_TopicRespond(),
				type_support_topic_respond.get_type_name(),
				get_name_topic_profile_respond());

			if (!topic_command)
			{
				topic_command = _participant->create_topic(get_name_TopicRespond(),
					type_support_topic_respond.get_type_name(),
					_dds::TOPIC_QOS_DEFAULT);
				if (!topic_command)
				{
					throw 2;
				}

				log->Debug("UnitAid_DDS node-{}: Init topic command done : defualt", this->config.node_id);
			}
			else
			{
				log->Debug("UnitAid_DDS node-{}: Init topic done : XML-file", this->config.node_id);
			}

		}
		catch (int& e)
		{
			log->Critical("UnitAid_DDS node-{}: Error init topic : error: {}", this->config.node_id, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("UnitAid_DDS node-{}: Error init topic : error: {}", this->config.node_id, 0);
			result = ResultReqest::ERR;
		}

		return result;
	};


	void UnitAid_DDS::SubListener::on_subscription_matched(_dds::DataReader*, const _dds::SubscriptionMatchedStatus& info)
	{
		if (info.current_count_change == 1)
		{
			_master->log->Info("UnitAid_DDS node-{}: Subscriber mathed");
		}
		else if (info.current_count_change == -1)
		{
			_master->log->Info("UnitAid_DDS node-{}: Subscriber unmathed");
		}

		return;
	};

	void UnitAid_DDS::SubListener::on_data_available(_dds::DataReader* reader)
	{
		_dds::SampleInfo info;
		TopicCommand data;
		TopicStatus  status;

		try
		{
			if (reader->take_next_sample(&data, &info) != ReturnCode_t::RETCODE_OK) return;
			if (!info.valid_data)
			{
				_master->log->Warning("UnitAid_DDS node-{}, Error receive command", _master->config.node_id);
				return;
			}

			if (_master->config.node_id != data.id_target()) return;

			_master->log->Info("UnitAid_DDS node-{}, Receive command {}", _master->config.node_id, data.code_command());

			if (_master->config.func == nullptr) return;
			status = _master->config.func(data);
			_master->_responder->write(&status);
		}
		catch (int& e)
		{
			_master->log->Critical("UnitAid_DDS node-{}: Error receive command : error: {}", _master->config.node_id, e);
		}
		catch (...)
		{
			_master->log->Critical("UnitAid_DDS node-{}: Error receive command : error: {}", _master->config.node_id, 0);
		}

		return;
	};

	ResultReqest UnitAid_DDS::RespondStatus(TopicStatus& status)
	{
		ResultReqest result{ ResultReqest::OK };

		try
		{
			if (_responder) _responder->write(&status);
		}
		catch (int& e)
		{
			log->Critical("UnitAid_DDS node-{}: Error respond status : error: {}", config.node_id, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("UnitAid_DDS node-{}: Error respond status : error: {}", config.node_id, 0);
			result = ResultReqest::ERR;
		}

		return result;
	}


	std::string UnitAid_DDS::get_name_participant_profile()
	{
		return "participant_profile";
	}

	std::string UnitAid_DDS::get_name_TopicCommand()
	{
		return "topic_command_atech_scada";
	}

	std::string UnitAid_DDS::get_name_TopicRespond()
	{
		return "topic_respond_atech_scada";
	}

	std::string UnitAid_DDS::get_name_topic_profile_command()
	{
		return "topic_command_profile";
	}

	std::string UnitAid_DDS::get_name_topic_profile_respond()
	{
		return "topic_respond_profile";
	}

	std::string UnitAid_DDS::get_name_subscriber_profile()
	{
		return "topic_command_subscriber_profile";
	}

	std::string UnitAid_DDS::get_name_datawriter_profile()
	{
		return "topic_command_datawrite_profile";
	}

	std::string UnitAid_DDS::get_name_publisher_profile()
	{
		return "topic_respond_datawrite_profile";
	}

	std::string UnitAid_DDS::get_name_datareader_profile()
	{
		return "topic_respond_datawrite_profile";
	}

	StatusUnitAid UnitAid_DDS::GetStatus()
	{
		return _status.load();
	}
}