#include "UnitAid_DDS.hpp"
#include <Module_CTRL/Module_CTRL.hpp>

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
			if (!config.manager) throw 0;

			_factory_dds = atech::srv::io::FactoryDDS::get_instance();
			if (!_factory_dds) throw 1;

			if (create_topics() != ResultReqest::OK) throw 3;
			if (init_subscriber() != ResultReqest::OK) throw 4;
			if (init_publisher() != ResultReqest::OK) throw 5;

			log->Info("UnitAdi_DDS node_id-{} : Initialization done",config.node_id);
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

	ResultReqest UnitAid_DDS::TakeServiceConfig(size_t size_data, std::string& str)
	{
		ResultReqest result{ ResultReqest::OK };
		_dds::DataReader* data_reader = nullptr;
		DdsConfig config;
		_dds::SampleInfo info;

		try
		{
			if (!_factory_dds) throw 1;
			atech::common::SizeTopics::SetMaxSizeDdsConfig(size_data);
			if (_factory_dds->registration_type_config() != ResultReqest::OK)
				throw 2;

			if (_factory_dds->registration_topic(topic_name_config, atech::srv::io::TypeTopic::DDSConfig) != ResultReqest::OK)
				throw 3;

			data_reader = _factory_dds->get_datareader(topic_name_config);
			if (!data_reader) throw 4;

			for (int i = 0; i < 10; i++)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				if (data_reader->take_next_sample(&config, &info) != ReturnCode_t::RETCODE_OK) continue;
				if (!info.valid_data) continue;
				if (config.id_target() != this->config.node_id) continue;
				break;
			}

			if (config.id_target() != this->config.node_id) throw 6;

			std::vector<char>& cf = config.conf_subject();
			str = std::string(&cf[0], cf.size());

			log->Info("UnitAid node-{}: Received new config", this->config.node_id);
		}
		catch(int& e)
		{
			log->Critical("UnitAid_DDS node-{}: Error receive new config: error {}: syserror {}", this->config.node_id , e, 0);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("UnitAid_DDS node-{}: Error receive new config: error {}: syserror {}", this->config.node_id, 0, 0);
			result = ResultReqest::ERR;
		}

		try
		{
			_factory_dds->delete_datareader(data_reader);
			_factory_dds->unregistration_topic(topic_name_config);
			_factory_dds->unregistration_type_config();
		}
		catch (...)
		{	
			log->Critical("UnitAid_DDS node-{}: Error delete topic of config: error {}: syserror {}", this->config.node_id, 0, 0);
		}
		
		return result;
	};

	ResultReqest UnitAid_DDS::clear_properties()
	{
		ReturnCode_t res;
		ResultReqest result{ ResultReqest::OK };

		try
		{
			if (_reader_command != nullptr)
			{
				_factory_dds->delete_datareader(_reader_command);
				_reader_command = nullptr;
			}

			if (_responder != nullptr)
			{
				_factory_dds->delete_datawriter(_responder);
				_responder = nullptr;
			}

			_factory_dds->unregistration_topic(get_name_TopicCommand());
			_factory_dds->unregistration_topic(get_name_TopicRespond());
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

	ResultReqest UnitAid_DDS::init_subscriber()
	{
		ResultReqest result = ResultReqest::OK;

		try
		{
			if (!_reader_command)
			{
				//_reader_command = _subscriber->create_datareader(topic_command, _dds::DATAREADER_QOS_DEFAULT, _listener.get());
				_reader_command = _factory_dds->get_datareader(get_name_TopicCommand());
				if (!_reader_command) throw 1;
				_reader_command->set_listener(_listener.get());
			}
			log->Debug("UnitAid_DDS node-{}: Init datareader done", this->config.node_id);
		}
		catch (int& e)
		{
			log->Critical("UnitAid_DDS node-{}: Error init datareader : error: {}", this->config.node_id, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("UnitAid_DDS node-{}: Error init datareader : error: {}", this->config.node_id, 0);
			result = ResultReqest::ERR;
		}

		return result;
	};

	ResultReqest UnitAid_DDS::init_publisher()
	{
		ResultReqest result = ResultReqest::OK;

		try
		{
			if (!_responder)
			{
				_responder = _factory_dds->get_datawriter(get_name_TopicRespond());
				if (!_responder) throw 1;
			}
			log->Debug("UnitAid_DDS node-{}: Init datawriter done", this->config.node_id);
		}
		catch (int& e)
		{
			log->Critical("UnitAid_DDS node-{}: Error init datawriter : error: {}", this->config.node_id, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("UnitAid_DDS node-{}: Error init datawriter : error: {}", this->config.node_id, 0);
			result = ResultReqest::ERR;
		}

		return result;
	};

	ResultReqest UnitAid_DDS::create_topics()
	{
		ResultReqest result = ResultReqest::OK;

		try
		{
			if (!_factory_dds)
			{
				throw 1;
			}

			if (_factory_dds->registration_topic(get_name_TopicCommand(), atech::srv::io::TypeTopic::DDSCommand) != ResultReqest::OK)
			{
				log->Critical("UnitAid_DDS node-{}: Error init topic of command", this->config.node_id);
			}
			else
			{
				log->Debug("UnitAid_DDS node-{}: Init topic of command done", this->config.node_id);
			}

			if (_factory_dds->registration_topic(get_name_TopicRespond(), atech::srv::io::TypeTopic::DDSStatus) != ResultReqest::OK)
			{
				log->Critical("UnitAid_DDS node-{}: Error init topic of status done", this->config.node_id);
			}
			else
			{
				log->Debug("UnitAid_DDS node-{}: Init topic of status done", this->config.node_id);
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
			_master->log->Info("UnitAid_DDS node-{}: Subscriber mathed",_master->config.node_id);
		}
		else if (info.current_count_change == -1)
		{
			_master->log->Info("UnitAid_DDS node-{}: Subscriber unmathed", _master->config.node_id);
		}

		return;
	};

	void UnitAid_DDS::SubListener::on_data_available(_dds::DataReader* reader)
	{
		_dds::SampleInfo info;
		DdsCommand data;
		DdsStatus  status;

		try
		{
			if (reader->take_next_sample(&data, &info) != ReturnCode_t::RETCODE_OK) return;
			if (!info.valid_data)
			{
				_master->log->Warning("UnitAid_DDS node-{}, Error receive command", _master->config.node_id);
				return;
			}

			if (_master->verification_node_id(data.id_target()) != ResultReqest::OK) return;

			_master->log->Info("UnitAid_DDS node-{}, Receive command {}", _master->config.node_id, data.cmd_code());
			status = _master->broadcast_command(data);

			status.id_source(_master->config.node_id);
			status.id_target(data.id_source());
			status.cmd_code(data.cmd_code());
			status.st_time(TimeConverter::GetTime_LLmcs());
			
			if (status.st_code() != (uint32_t)atech::common::Status::NONE) _master->_responder->write(&status);
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

	ResultReqest UnitAid_DDS::RespondStatus(DdsStatus& status)
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
		return "dds_command";
	}

	std::string UnitAid_DDS::get_name_TopicRespond()
	{
		return "dds_status";
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

	std::string UnitAid_DDS::get_name_topic_config_profile()
	{
		return topic_name_config + "_profile";
	};

	std::string UnitAid_DDS::get_name_datareader_config_profile()
	{
		return topic_name_config + "datareader_profile";
	};

	StatusUnitAid UnitAid_DDS::GetStatus()
	{
		return _status.load();
	}

	DdsStatus UnitAid_DDS::broadcast_command(DdsCommand& cmd)
	{
		return config.manager->function_processing(cmd);
	};

	ResultReqest UnitAid_DDS::verification_node_id(uint32_t node_target)
	{
		uint32_t node = (node_target & 0xFFFF0000) >> 16;
		uint32_t type = (node_target & 0x0000FF00) >> 8;
		uint32_t id = (node_target & 0x000000FF);

		uint32_t node_this = (config.node_id & 0xFFFF0000) >> 16;
		uint32_t type_this = (config.node_id & 0x0000FF00) >> 8;
		uint32_t id_this = (config.node_id & 0x000000FF);

		if (type != 0 && type != type_this) return ResultReqest::ERR;
		if (node != 0 && node != node_this)	return ResultReqest::ERR;
		if (id != 0 && id != id_this)	return ResultReqest::ERR;

		return 	 ResultReqest::OK;
	};
}