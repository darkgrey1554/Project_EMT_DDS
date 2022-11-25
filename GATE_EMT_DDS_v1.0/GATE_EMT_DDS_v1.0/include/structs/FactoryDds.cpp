#include <structs/FactoryDds.h>

namespace atech::srv::io
{
	namespace _dds = eprosima::fastdds::dds;

	std::shared_ptr<FactoryDDS> FactoryDDS::p_instance = nullptr;
	std::mutex FactoryDDS::singlton;

	FactoryDDS::FactoryDDS()
	{
		log = LoggerSpaceScada::GetLoggerScada(LoggerSpaceScada::TypeLogger::SPDLOG);
		return;
	};

	FactoryDDS::~FactoryDDS()
	{
		return;
	};

	std::shared_ptr<FactoryDDS> FactoryDDS::get_instance()
	{
		std::lock_guard<std::mutex> lock(singlton);

		if (p_instance == nullptr)
		{
			p_instance.reset(new atech::srv::io::FactoryDDS());
		}											 

		return p_instance;
	}

	ResultReqest FactoryDDS::init_dds()
	{
		ResultReqest result { ResultReqest::OK };
		std::lock_guard<std::mutex> lock(guarden);	

		try
		{
			result = init_participant();
			result = init_subscriber();
			result = init_publisher();
			registration_type_dds();

			if (result != ResultReqest::OK) throw 1;

			log->Debug("FactoryDDS node-{}: Init dds done", _node_id);
		}
		catch (int& e)
		{
			result = ResultReqest::ERR;
			log->Critical("FactoryDDS node-{}: Error init of dds: error {}: syserror {}", _node_id, e, 0);
		}
		catch (...)
		{
			result = ResultReqest::ERR;
			log->Critical("FactoryDDS node-{}: Error init of dds: error {}: syserror {}", _node_id, 0, 0);
		}

		return result;
	}

	ResultReqest FactoryDDS::registration_topic(const std::string& name, const TypeTopic& type_topic)
	{
		ResultReqest result{ ResultReqest::OK };
		std::lock_guard<std::mutex> lock(guarden);
		_dds::Topic* topic = nullptr;		

		try
		{
			if (!_participant) throw 1;

			if (this->map_topic.count(name) > 0) return result;

			//topic = _participant->create_topic_with_profile(name, type_name[type_topic], get_name_topic_profile(name));
			if (!topic)
			{
				topic = _participant->create_topic(name, type_name[type_topic], _dds::TOPIC_QOS_DEFAULT);
				if (!topic) throw 2;
				log->Debug("FactoryDDS node-{}: Registration of topic {} : default ", _node_id, name);
			}
			else
			{
				log->Debug("FactoryDDS node-{}: Registration of topic {}: xml profile", _node_id, name);
			}

			map_topic[name] = topic;
		}
		catch (int& e)
		{
			result = ResultReqest::ERR;
			log->Critical("FactoryDDS node-{}: Error registration of topic {} : error {} : syserror {}", _node_id, name, e, 0);
		}
		catch (...)
		{
			result = ResultReqest::ERR;
			log->Critical("FactoryDDS node-{}: Error registration of topic {} : error {} : syserror {}", _node_id, name, 0, 0);
		}

		return result;
	}
	
	ResultReqest FactoryDDS::unregistration_topic(std::string name)
	{
		ResultReqest result{ ResultReqest::OK };
		std::lock_guard<std::mutex> lock(guarden);

		try
		{
			if (!_participant) throw 1;
			if (map_topic.count(name) == 0) return result;
			if (_participant->delete_topic(map_topic[name]) != ReturnCode_t::RETCODE_OK) throw 2;
			map_topic.erase(name);
			log->Debug("FactoryDDS node-{}: unregistration of topic {} done", _node_id, name);
		}
		catch (int& e)
		{
			result = ResultReqest::ERR;
			log->Critical("FactoryDDS node-{}: Errorr unregistration of topic {}: error {} : syserror {}", _node_id, name, e, 0);
		}
		catch (...)
		{
			result = ResultReqest::ERR;
			log->Critical("FactoryDDS node-{}: Errorr unregistration of topic {}: error {} : syserror {}", _node_id, name, 0, 0);
		}

		return result;
	}

	eprosima::fastdds::dds::DataWriter* FactoryDDS::get_datawriter(std::string name_topic)
	{
		_dds::DataWriter* writer = nullptr;
		std::lock_guard<std::mutex> lock(guarden);

		try
		{
			if (map_topic.count(name_topic) == 0) throw 1;
			if (!_publisher) throw 2;
			//writer = _publisher->create_datawriter_with_profile(map_topic[name_topic], get_name_datawriter_profile(name_topic));
			if (!writer)
			{
				writer = _publisher->create_datawriter(map_topic[name_topic], _dds::DATAWRITER_QOS_DEFAULT);
				if (!writer) throw 3;
				log->Debug("FactoryDDS node-{}: Registration of datawriter topic {}: default", _node_id, name_topic);
			}
			else
			{
				log->Debug("FactoryDDS node-{}: Registration of datawriter topic {}: xml", _node_id, name_topic);
			}			
		}
		catch(int& e)
		{
			writer = nullptr;
			log->Critical("FactoryDDS node-{}: Error registration of datawriter topic {}", _node_id, name_topic);
		}
		catch(...)
		{
			writer = nullptr;
			log->Critical("FactoryDDS node-{}: Error registration of datawriter topic {}", _node_id, name_topic);
		}

		return writer;
	}

	eprosima::fastdds::dds::DataReader* FactoryDDS::get_datareader(std::string name_topic)
	{
		_dds::DataReader* reader = nullptr;
		std::lock_guard<std::mutex> lock(guarden);

		try
		{
			if (map_topic.count(name_topic) == 0) throw 1;
			if (!_subscriber) throw 2;
			//reader = _subscriber->create_datareader_with_profile(map_topic[name_topic], get_name_datareader_profile(name_topic));
			if (!reader)
			{
				reader = _subscriber->create_datareader(map_topic[name_topic], _dds::DATAREADER_QOS_DEFAULT);
				if (!reader) throw 3;
				log->Debug("FactoryDDS node-{}: Registration of datareader topic {}: default", _node_id, name_topic);
			}
			else
			{
				log->Debug("FactoryDDS node-{}: Registration of datareader topic {}: xml", _node_id, name_topic);
			}
		}
		catch (int& e)
		{
			reader = nullptr;
			log->Critical("FactoryDDS node-{}: Error registration of datareader topic {}", _node_id, name_topic);
		}
		catch (...)
		{
			reader = nullptr;
			log->Critical("FactoryDDS node-{}: Error registration of datareader topic {}", _node_id, name_topic);
		}

		return reader;
	}

	ResultReqest FactoryDDS::delete_datawriter(eprosima::fastdds::dds::DataWriter* writer)
	{
		ResultReqest result{ ResultReqest::OK };
		std::lock_guard<std::mutex> lock(guarden);
		std::string name_topic;

		try
		{
			if (!writer) throw 1;
			name_topic = writer->get_topic()->get_name();
			if (!_publisher) throw 2;
			if (_publisher->delete_datawriter(writer) != ReturnCode_t::RETCODE_OK) throw 3;
			log->Debug("FactoryDDS node-{}: Unregistration of datarwriter {} done", _node_id, name_topic);
		}
		catch(int& e)
		{ 
			result = ResultReqest::ERR;
			log->Critical("FactoryDDS node-{}: Error unregistration of datarwriter {}: error {} : syserror {}", _node_id, name_topic, e, 0);
		}
		catch(...)
		{
			result = ResultReqest::ERR;
			log->Critical("FactoryDDS node-{}: Error unregistration of datarwriter {}: error {} : syserror {}", _node_id, name_topic, 0, 0);
		}  

		return result;
	}

	ResultReqest FactoryDDS::delete_datareader(eprosima::fastdds::dds::DataReader* reader)
	{
		ResultReqest result{ ResultReqest::OK };
		std::lock_guard<std::mutex> lock(guarden);
		std::string name_topic;

		try
		{
			if (!reader) throw 1;
			name_topic = reader->get_topicdescription()->get_name();
			if (!_subscriber) throw 2;
			if (_subscriber->delete_datareader(reader) != ReturnCode_t::RETCODE_OK) throw 3;
			log->Debug("FactoryDDS node-{}: Unregistration of datareader {} done", _node_id, name_topic);
		}
		catch (int& e)
		{
			result = ResultReqest::ERR;
			log->Critical("FactoryDDS node-{}: Error unregistration of datareader {}: error {} : syserror {}", _node_id, name_topic, e, 0);
		}
		catch (...)
		{
			result = ResultReqest::ERR;
			log->Critical("FactoryDDS node-{}: Error unregistration of datreader {}: error {} : syserror {}", _node_id, name_topic, 0, 0);
		}

		return result;
	};

	ResultReqest FactoryDDS::delete_dds()
	{
		ResultReqest result{ ResultReqest::OK };
		std::lock_guard<std::mutex> lock(guarden);
		try
		{
			if (!_subscriber)
			{
				if (_subscriber->delete_contained_entities() != ReturnCode_t::RETCODE_OK)
				{
					log->Critical("FactoryDDS node-{}: Error clear of subscribers", _node_id);
				}
				_subscriber = nullptr;
			}

			if (!_publisher)
			{
				if (_publisher->delete_contained_entities() != ReturnCode_t::RETCODE_OK)
				{
					log->Critical("FactoryDDS node-{}: Error clear of publisher", _node_id);
				}
				_publisher = nullptr;
			}

			if (!_participant)
			{
				if (_participant->delete_contained_entities() != ReturnCode_t::RETCODE_OK)
				{
					log->Critical("FactoryDDS node-{}: Error clear of participant", _node_id);
				}
				if (_dds::DomainParticipantFactory::get_instance()->delete_participant(_participant) != ReturnCode_t::RETCODE_OK) throw 1;
				_participant = nullptr;
			}
		}
		catch (int& e)
		{
			result = ResultReqest::ERR;
			log->Critical("FactoryDDS node-{}: Error delete of dds: error {} : syserror {}", _node_id, e, 0);
		}
		catch (...)
		{
			result = ResultReqest::ERR;
			log->Critical("FactoryDDS node-{}: Error delete of dds: error {} : syserror {}", _node_id, 0, 0);
		}

		return result;
	}

	ResultReqest FactoryDDS::install_config_xml(const std::string& str_xml)
	{
		ResultReqest result{ ResultReqest::OK };
		std::lock_guard<std::mutex> lock(guarden);

		try
		{
			if (!_participant) throw 1;
			if (_dds::DomainParticipantFactory::get_instance()->load_XML_profiles_string(str_xml.c_str(), str_xml.size()) != ReturnCode_t::RETCODE_OK)
				throw 2;

			log->Critical("FactoryDDS node-{}: Registration of config done", _node_id);
		}
		catch(int& e)
		{
			result = ResultReqest::ERR;
			log->Critical("FactoryDDS node-{}: Error registration of config: error {} : syserror {}", _node_id, e, 0);
		}
		catch(...)
		{
			result = ResultReqest::ERR;
			log->Critical("FactoryDDS node-{}: Error registration of config: error {} : syserror {}", _node_id, 0, 0);
		} 

		return result;
	}

	ResultReqest FactoryDDS::registration_type_config()
	{
		ResultReqest result{ ResultReqest::OK };
		std::lock_guard<std::mutex> lock(guarden);

		try
		{
			_dds::TypeSupport SupporTypeDDSConfig(new DdsConfigPubSubType());
			if (!_participant) throw 1;
			if (SupporTypeDDSConfig.register_type(_participant) != ReturnCode_t::RETCODE_OK) throw 2;
			type_name[atech::srv::io::TypeTopic::DDSConfig] = SupporTypeDDSConfig.get_type_name();
		}
		catch (int& e)
		{
			result = ResultReqest::ERR;
			log->Critical("FactoryDDS node-{}: Error registration of config type: error {} : syserror {}", _node_id, e, 0);
		}
		catch (...)
		{
			result = ResultReqest::ERR;
			log->Critical("FactoryDDS node-{}: Error registration of config type: error {} : syserror {}", _node_id, 0, 0);
		} 	
		
		return result;
	}

	ResultReqest FactoryDDS::unregistration_type_config()
	{
		ResultReqest result{ ResultReqest::OK };
		std::lock_guard<std::mutex> lock(guarden);

		try
		{
			if (!_participant) throw 1;
			if (type_name.count(atech::srv::io::TypeTopic::DDSConfig) == 0) throw 2;
			_participant->unregister_type(type_name[atech::srv::io::TypeTopic::DDSConfig]);
			type_name.erase(atech::srv::io::TypeTopic::DDSConfig);
		}
		catch (int& e)
		{
			result = ResultReqest::ERR;
			log->Critical("FactoryDDS node-{}: Error unregistration of config type: error {} : syserror {}", _node_id, e, 0);
		}
		catch (...)
		{
			result = ResultReqest::ERR;
			log->Critical("FactoryDDS node-{}: Error unregistration of config type: error {} : syserror {}", _node_id, 0, 0);
		}
	}




	
	ResultReqest FactoryDDS::init_participant()
	{
		ResultReqest result{ ResultReqest::OK };

		try
		{
			if (!_participant)
			{
				//_participant = _dds::DomainParticipantFactory::get_instance()->create_participant_with_profile(get_name_participant_profile());
				if (!_participant)
				{
					_participant = _dds::DomainParticipantFactory::get_instance()->
						create_participant(0, _dds::PARTICIPANT_QOS_DEFAULT);
					if (!_participant)
					{
						throw 1;
					}

					log->Debug("FactoryDDS node-{}: Init participant done : defualt", _node_id);
				}
				else
				{
					log->Debug("FactoryDDS node-{}: Init participant done : XML-file", _node_id);
				}
			}
		}
		catch (int& e)
		{
			result = ResultReqest::ERR;
			log->Critical("FactoryDDS node-{}: Error init participant : error {} : syserror {}", _node_id, e, 0);
		}
		catch (...)
		{
			result = ResultReqest::ERR;
			log->Critical("FactoryDDS node-{}: Error init participant : error {} : syserror {}", _node_id, 0, 0);
		}

		return result;
	};

	ResultReqest FactoryDDS::init_subscriber()
	{
		ResultReqest result{ ResultReqest::OK };

		try
		{
			if (!_participant) throw 1;
			if (!_subscriber)
			{
				//_subscriber = _participant->create_subscriber_with_profile(get_name_subscriber_profile());
				if (!_subscriber)
				{
					_subscriber = _participant->create_subscriber(_dds::SUBSCRIBER_QOS_DEFAULT);
					if (!_subscriber)
					{
						throw 2;
					}

					log->Debug("FactoryDDS node-{}: Init subscriber done : defualt", _node_id);
				}
				else
				{
					log->Debug("FactoryDDS node-{}: Init subscriber done : XML-file", _node_id);
				}
			}
		}
		catch (int& e)
		{
			result = ResultReqest::ERR;
			log->Critical("FactoryDDS node-{}: Error init subscriber : error {} : syserror {}", _node_id, e, 0);
		}
		catch (...)
		{
			result = ResultReqest::ERR;
			log->Critical("FactoryDDS node-{}: Error init subscriber : error {} : syserror {}", _node_id, 0, 0);
		}

		return result;
	};

	ResultReqest FactoryDDS::init_publisher()
	{
		ResultReqest result{ ResultReqest::OK };

		try
		{
			if (!_participant) throw 1;
			if (!_publisher)
			{
				//_publisher = _participant->create_publisher_with_profile(get_name_publisher_profile());
				if (!_publisher)
				{
					_publisher = _participant->create_publisher(_dds::PUBLISHER_QOS_DEFAULT);
					if (!_publisher)
					{
						throw 2;
					}

					log->Debug("FactoryDDS node-{}: Init publisher done : defualt", _node_id);
				}
				else
				{
					log->Debug("FactoryDDS node-{}: Init publisher done : XML-file", _node_id);
				}
			}
		}
		catch (int& e)
		{
			result = ResultReqest::ERR;
			log->Critical("FactoryDDS node-{}: Error init publisher : error {} : syserror {}", _node_id, e, 0);
		}
		catch (...)
		{
			result = ResultReqest::ERR;
			log->Critical("FactoryDDS node-{}: Error init publisher : error {} : syserror {}", _node_id, 0, 0);
		}

		return result;
	}
	
	ResultReqest FactoryDDS::registration_type_dds()
	{
		ResultReqest result{ ResultReqest::OK }; 

		try
		{
			if (!_participant) throw 1;

			_dds::TypeSupport SupporTypeDDSData(new DDSDataPubSubType());
			_dds::TypeSupport SupporTypeDDSDataEx(new DDSDataExPubSubType());
			_dds::TypeSupport SupporTypeDDSAlarm(new DDSAlarmPubSubType());
			_dds::TypeSupport SupporTypeDDSAlarmEx(new DDSAlarmExPubSubType());
			_dds::TypeSupport SupporTypeDDSCommand(new DdsCommandPubSubType());
			_dds::TypeSupport SupporTypeDDSStatus(new DdsStatusPubSubType());

			type_name[TypeTopic::DDSData] = SupporTypeDDSData.get_type_name();
			type_name[TypeTopic::DDSDataEx] = SupporTypeDDSDataEx.get_type_name();
			type_name[TypeTopic::DDSAlarm] = SupporTypeDDSAlarm.get_type_name();
			type_name[TypeTopic::DDSAlarmEx] = SupporTypeDDSAlarmEx.get_type_name();
			type_name[TypeTopic::DDSCommand] = SupporTypeDDSCommand.get_type_name();
			type_name[TypeTopic::DDSStatus] = SupporTypeDDSStatus.get_type_name();

			if (SupporTypeDDSData.register_type(_participant) != ReturnCode_t::RETCODE_OK)
			{
				log->Critical("FactoryDDS node-{}: Error registration type DDSData: error 0 : syserror 0", _node_id);
			}

			if (SupporTypeDDSDataEx.register_type(_participant) != ReturnCode_t::RETCODE_OK)
			{
				log->Critical("FactoryDDS node-{}: Error registration type DDSDataEx: error 0 : syserror 0", _node_id);
			}

			if (SupporTypeDDSAlarm.register_type(_participant) != ReturnCode_t::RETCODE_OK)
			{
				log->Critical("FactoryDDS node-{}: Error registration type DDSAlarm: error 0 : syserror 0", _node_id);
			}

			if (SupporTypeDDSAlarmEx.register_type(_participant) != ReturnCode_t::RETCODE_OK)
			{
				log->Critical("FactoryDDS node-{}: Error registration type DDSAlarmEx: error 0 : syserror 0", _node_id);
			}


			if (SupporTypeDDSCommand.register_type(_participant) != ReturnCode_t::RETCODE_OK)
			{
				log->Critical("FactoryDDS node-{}: Error registration type DDSCommand: error 0 : syserror 0", _node_id);
			}

			if (SupporTypeDDSStatus.register_type(_participant) != ReturnCode_t::RETCODE_OK)
			{
				log->Critical("FactoryDDS node-{}: Error registration type DDSStatus: error 0 : syserror 0", _node_id);
			}

			log->Debug("FactoryDDS node-{}: registration types dds done", _node_id);
		}
		catch (int& e)
		{
			result = ResultReqest::ERR;
			log->Critical("FactoryDDS node-{}: Error registation types dds: error {}: syserror {}", _node_id, e, 0);
		}
		catch (...)
		{
			result = ResultReqest::ERR;
			log->Critical("FactoryDDS node-{}: Error registation types dds: error {}: syserror {}", _node_id, 0, 0);
		} 		

		return result;
	}
	
	std::string FactoryDDS::get_name_participant_profile()
	{
		std::string str;
		str += "participant_profile";
		return std::move(str);
	};

	std::string FactoryDDS::get_name_subscriber_profile()
	{
		std::string str;
		str += "subscriber_profile";
		return std::move(str);
	}

	std::string FactoryDDS::get_name_publisher_profile()
	{
		std::string str;
		str += "publisher_profile";
		return std::move(str);
	};

	std::string FactoryDDS::get_name_topic_profile(const std::string& name)
	{
		std::string str;
		str = name + "_profile";
		return str;
	}
	
	std::string FactoryDDS::get_name_datawriter_profile(const std::string& name_topic)
	{
		std::string str;
		str = name_topic + "datawriter_profile";
		return str;
	}

	std::string FactoryDDS::get_name_datareader_profile(const std::string& name_topic)
	{
		std::string str;
		str = name_topic + "datareader_profile";
		return str;
	}




}