#include "Module_CTRL.hpp"

namespace atech::srv::io::ctrl
{
	Module_CTRL::Module_CTRL()
	{
		log = std::make_shared<atech::logger::LoggerScadaSpdDds>();
		return;
	};

	Module_CTRL::~Module_CTRL()
	{
		DdsCommand command;
		
	};

	ResultReqest Module_CTRL::LoadConfigService()
	{
		ResultReqest result{ ResultReqest::OK };
		try
		{
			std::fstream file;
			std::stringstream stream;
			file.open("config.json", std::ios::in);
			if (!file.is_open()) throw 1;

			config_str.clear();
			stream << file.rdbuf();
			config_str = stream.str();

			file.close();

			nlohmann::json config = nlohmann::json::parse(config_str);
			if (config.count("node_target") == 0) throw 2;
			if (!config["node_target"].is_number_unsigned()) throw 3;
			_node_id = config["node_target"];

			if (config.count("dds") == 0 && config.count("adapters") == 0) throw 4;

		}
		catch (int& e)
		{
			log->Critical("Module_CTRL node-{}: Error load config file: error {}: syserror {}", _node_id, e, 0);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("Module_CTRL node-{}: Error load config file: error {}: syserror {}", _node_id, 0, 0);
			result = ResultReqest::ERR;
		}

		return result;
	}

	ResultReqest Module_CTRL::TakeConfigLogger(atech::logger::ConfigLogger& config)
	{
		ResultReqest result{ ResultReqest::OK };

		return result;
	}

	ResultReqest Module_CTRL::InitDDSLayer()
	{
		ResultReqest result{ ResultReqest::OK };
		result = init_dds_layer(config_str);
		return result;
	}

	ResultReqest Module_CTRL::InitModuleIO()
	{
		ResultReqest result{ ResultReqest::OK };
		std::vector<scada_ate::gate::adapter::ConfigUnitTransfer> vec_units;

		try
		{
			if (config_str.empty()) throw 1;
			if (create_vector_config_unitstreansfer(vec_units, config_str) != ResultReqest::OK) throw 2;
			if (addunits_module_io(vec_units) != ResultReqest::OK) throw 3;
		}
		catch(int& e)
		{
			log->Critical("Module_CTRL node-{}: Error InitModuleIO: error {}", _node_id, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("Module_CTRL node-{}: Error InitModuleIO: error {}", _node_id, 0);
			result = ResultReqest::ERR;
		}															   

		return result;
	} 

	ResultReqest Module_CTRL::InitUnitAid(std::shared_ptr<IConfigUnitAid> config)
	{
		ResultReqest result{ ResultReqest::OK };
		_config_unitaid = config;
		_config_unitaid->manager = this;
		_config_unitaid->node_id = _node_id;
		_unitaid = atech::srv::io::ctrl::CreateUnit_CP(config);
		result = _unitaid->InitUnitAid();
		return result;
	};


	ResultReqest Module_CTRL::registration_size_topics(atech::common::TopicMaxSize& default_size)
	{
		ResultReqest result{ ResultReqest::OK };

		try
		{
			for (auto& topic : default_size.get_dds_type_size())
			{
				if (topic.get_type_name() == "DDSData")
				{																			   
					for (auto& data : topic.get_type_sizes())
					{
						if (data.get_type_name() == "int")
						{
							atech::common::SizeTopics::SetMaxSizeDataCollectionInt(data.get_size());
						}
						else if (data.get_type_name() == "float")
						{
							atech::common::SizeTopics::SetMaxSizeDataCollectionFloat(data.get_size());
						}
						else if (data.get_type_name() == "double")
						{
							atech::common::SizeTopics::SetMaxSizeDataCollectionDouble(data.get_size());
						}
						else if (data.get_type_name() == "char")
						{
							atech::common::SizeTopics::SetMaxSizeDataChar(data.get_size());
						}
						else if (data.get_type_name() == "char_vector")
						{
							atech::common::SizeTopics::SetMaxSizeDataCollectionChar(data.get_size());
						}
					}
				}
				else if (topic.get_type_name() == "DDSDataEx")
				{
					for (auto& data : topic.get_type_sizes())
					{
						if (data.get_type_name() == "int")
						{
							atech::common::SizeTopics::SetMaxSizeDDSDataExVectorInt(data.get_size());
						}
						else if (data.get_type_name() == "float")
						{
							atech::common::SizeTopics::SetMaxSizeDDSDataExVectorFloat(data.get_size());
						}
						else if (data.get_type_name() == "double")
						{
							atech::common::SizeTopics::SetMaxSizeDDSDataExVectorDouble(data.get_size());
						}
						else if (data.get_type_name() == "char")
						{
							atech::common::SizeTopics::SetMaxSizeDataExVectorChar(data.get_size());
						}
						else if (data.get_type_name() == "char_vector")
						{
							atech::common::SizeTopics::SetMaxSizeDDSDataExVectorChar(data.get_size());
						}
					}
				}
				else if (topic.get_type_name() == "DDSAlarm")
				{
					for (auto& data : topic.get_type_sizes())
					{
						if (data.get_type_name() == "long")
						{
							atech::common::SizeTopics::SetMaxSizeDDSAlarmVectorAlarms(data.get_size());
						}
					}
				}
				else if (topic.get_type_name() == "DDSAlarmEx")
				{
					for (auto& data : topic.get_type_sizes())
					{
						if (data.get_type_name() == "char")
						{
							atech::common::SizeTopics::SetMaxSizeDDSAlarmExVectorAlarms(data.get_size());
						}
					}
				}
			}
		}
		catch (...)
		{
			log->Warning("Module_CTRL node-{}: Error registration size max topics: error {}", _node_id, 0);
			result = ResultReqest::ERR;
		}

		return result;
	}

	ResultReqest Module_CTRL::registration_dds_profiles(nlohmann::json& json)
	{
		ResultReqest result{ ResultReqest::OK };

		try
		{
			if (json["dds"].count("profiles"))
			{
				std::string profiles;
				try
				{
					profiles = atech::common::json2xml(json["dds"]["profiles"]);
				}
				catch (...)
				{
					throw 2;
				}

				if (_factory_dds->install_config_xml(profiles) != ResultReqest::OK)
					log->Warning("Module_CTRL node-{}: Error registration profiles dds layer", _node_id);
			}
			else
			{
				throw 1;
			}
		}
		catch (int& e)
		{
			log->Warning("Module_CTRL node-{}: Error registration dds layer: error {}", _node_id, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Warning("Module_CTRL node-{}: Error registration dds layer: error {}", _node_id, 0);
			result = ResultReqest::ERR;
		}

		return result;
	}

	ResultReqest Module_CTRL::addunits_module_io(std::vector<scada_ate::gate::adapter::ConfigUnitTransfer>& vect_config_units)
	{
		ResultReqest result = ResultReqest::OK;
		std::deque<std::pair<uint32_t, atech::common::Status>> st;
		
		_module_io_ptr.reset();
		_module_io_ptr = std::make_shared<scada_ate::gate::Module_IO>();
		_module_io_ptr->SetNodeID(_node_id);

		for (auto& unit : vect_config_units)
		{
			if (_module_io_ptr->AddUnit(unit) != ResultReqest::OK)
			{
				log->Critical("Module_CTRL node-{}: error add unit transfer id-{}", _node_id, unit.id);
				continue;
			};
			
			if (_module_io_ptr->Start(st, unit.id) != ResultReqest::OK)
			{
				log->Critical("Module_CTRL node-{}: error start unit transfer id-{}", _node_id, unit.id);
			}
		}

		return result;
	};

	ResultReqest Module_CTRL::create_vector_config_unitstreansfer(std::vector<scada_ate::gate::adapter::ConfigUnitTransfer>& vect_config_units, std::string& stream)
	{
		ResultReqest result{ ResultReqest::OK };  		
		vect_config_units.clear();

		try
		{
			nlohmann::json json = nlohmann::json::parse(stream);
			if (json.count("adapters") == 0) throw 1;
			atech::common::Adapters adapters = json["adapters"].get<atech::common::Adapters>();

			std::vector<scada_ate::gate::adapter::ConfigUnitTransfer> config_units;
			auto& units_source = adapters.get_units();
			config_units.reserve(units_source.size());
			for (auto unit_source : units_source)
			{
				scada_ate::gate::adapter::ConfigUnitTransfer unit_target;
				unit_target.frequency = unit_source.get_frequency();
				unit_target.id = unit_source.get_id();
				unit_target.config_input_unit.reserve(unit_source.get_input_units().size());
				unit_target.config_output_unit.reserve(unit_source.get_output_units().size());
				unit_target.mapping.reserve(unit_source.get_mapping().size());

				for (auto& source : unit_source.get_input_units())
				{
					scada_ate::gate::adapter::IConfigAdapter_ptr target = nullptr;
					target = fill_config_adapter(source);
					/////// crutch
					if (target->type_adapter == scada_ate::gate::adapter::TypeAdapter::DDS)
					{
						((scada_ate::gate::adapter::dds::ConfigAdapterDDS*)target.get())->type_transfer = 
							scada_ate::gate::adapter::dds::TypeTransfer::SUBSCRIBER;
					}
					///////
					unit_target.config_input_unit.push_back(std::move(target));
				}

				for (auto& source : unit_source.get_output_units())
				{
					scada_ate::gate::adapter::IConfigAdapter_ptr target = nullptr;
					target = fill_config_adapter(source);
					unit_target.config_output_unit.push_back(std::move(target));
				};

				for (auto& map_source : unit_source.get_mapping())
				{
					scada_ate::gate::adapter::Mapping map_target;
					map_target.frequency = map_source.get_frequency();
					map_target.id = map_source.get_id_map();
					if (vec_datum_to_vec_links(map_target.vec_links, map_source.get_data()) != ResultReqest::OK)
					{
						log->Warning("Module_CTRL node-{}: Error registration mappin-{}", _node_id, map_source.get_id_map());
						continue;
					}
					unit_target.mapping.push_back(std::move(map_target));
				}

				config_units.push_back(std::move(unit_target));
			}

			vect_config_units = std::move(config_units);
		}
		catch (std::string& e)
		{
			result = ResultReqest::ERR;
			log->Critical("Module_CTRL node-{}: Error read config adapters: error: {}", _node_id, e);
		}
		catch (int& e)
		{
			result = ResultReqest::ERR;
			log->Critical("Module_CTRL node-{}: Error read config adapters: error: {}", _node_id, e);
		}
		catch(...)
		{
			result = ResultReqest::ERR;
			log->Critical("Module_CTRL node-{}: Error read config adapters: error: {}", _node_id, 0);
		}

		return result;
	}

	ResultReqest Module_CTRL::init_dds_layer(std::string& config)
	{
		ResultReqest result {ResultReqest::OK};

		try
		{
			if (config.empty()) throw 1;
			nlohmann::json json = nlohmann::json::parse(config);

			atech::common::TopicMaxSize default_size;
			if (json.count("topic_max_size"))
			{
				auto topic_max_size = json["topic_max_size"];
				default_size = topic_max_size.get<atech::common::TopicMaxSize>();
				this->registration_size_topics(default_size);
			}
			else
			{
				log->Warning("Module_CTRL node-{}: config TopicsMaxSize not detected", _node_id);
			}

			_factory_dds = atech::srv::io::FactoryDDS::get_instance();
			if (!_factory_dds) throw 2;

			if (json.count("dds"))
			{
				this->registration_dds_profiles(json);
			}
			else
			{
				log->Warning("Module_CTRL node-{}: config DDS profiles not detected", _node_id);
			}

			if (_factory_dds->init_dds() != ResultReqest::OK)
			{
				log->Warning("Module_CTRL node-{}: error init dds layer", _node_id);
			}

		}
		catch (int& e)
		{
			log->Critical("Module_CTRL node-{}: Error DDS layer: error {}", _node_id, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("Module_CTRL node-{}: Error DDS layer: error {}", _node_id, 0);
			result = ResultReqest::ERR;
		}

		return result;
	}
	
	ResultReqest Module_CTRL::init_module_io(std::string& config)
	{
		ResultReqest result{ ResultReqest::OK };
		std::vector<scada_ate::gate::adapter::ConfigUnitTransfer> vec_units;

		try
		{
			if (config.empty()) throw 1;
			if (create_vector_config_unitstreansfer(vec_units, config) != ResultReqest::OK) throw 2;
			if (addunits_module_io(vec_units) != ResultReqest::OK) throw 3;
		}
		catch (int& e)
		{
			log->Critical("Module_CTRL node-{}: Error init_module_io: error {}", _node_id, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("Module_CTRL node-{}: Error init_module_io: error {}", _node_id, 0);
			result = ResultReqest::ERR;
		}

		return result;
	}
	
	
	scada_ate::gate::adapter::IConfigAdapter_ptr Module_CTRL::fill_config_adapter(const atech::common::OutputUnit& out)
	{
		scada_ate::gate::adapter::IConfigAdapter_ptr adapter = nullptr;

		try
		{
			adapter = create_config_adapter(out.get_type_adapter());
			if (!adapter) throw 1;

			adapter->id_adapter = out.get_id();
			adapter->id_map = out.get_id_map();

			if (out.get_type_adapter() == "SM")
			{
				if (fill_config((scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory*)adapter.get(),
					(atech::common::SmConfig*)out.get_config().get()) != ResultReqest::OK) throw 1;
			}
			else if (out.get_type_adapter() == "DDS")
			{
				if (fill_config((scada_ate::gate::adapter::dds::ConfigAdapterDDS*)adapter.get(),
					(atech::common::DdsConfig*)out.get_config().get()) != ResultReqest::OK) throw 1;
			}
			else if (out.get_type_adapter() == "OPC_UA")
			{
				if (fill_config((scada_ate::gate::adapter::opc::ConfigAdapterOPCUA*)adapter.get(),
					(atech::common::UaConfig*)out.get_config().get()) != ResultReqest::OK) throw 1;
			}
		}
		catch (...)
		{
			log->Warning("Module_IO node-{}: Error read config adapter id-{}: error 0: syserror 0", _node_id, out.get_id());
		}		

		return std::move(adapter);
	};

	scada_ate::gate::adapter::IConfigAdapter_ptr Module_CTRL::fill_config_adapter(const atech::common::InputUnit& out)
	{
		scada_ate::gate::adapter::IConfigAdapter_ptr adapter = nullptr;

		try
		{
			adapter = create_config_adapter(out.get_type_adapter());
			if (!adapter) throw 1;

			adapter->id_adapter = out.get_id();
			adapter->id_map = out.get_id_map();

			if (out.get_type_adapter() == "SM")
			{
				if (fill_config((scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory*)adapter.get(),
					(atech::common::SmConfig*)out.get_config().get()) != ResultReqest::OK) throw 2;
			}
			else if (out.get_type_adapter() == "DDS")
			{
				if (fill_config((scada_ate::gate::adapter::dds::ConfigAdapterDDS*)adapter.get(),
					(atech::common::DdsConfig*)out.get_config().get()) != ResultReqest::OK) throw 3;
			}
			else if (out.get_type_adapter() == "OPC_UA")
			{
				if (fill_config((scada_ate::gate::adapter::opc::ConfigAdapterOPCUA*)adapter.get(),
					(atech::common::UaConfig*)out.get_config().get()) != ResultReqest::OK) throw 4;
			}
		}
		catch (...)
		{
			adapter = nullptr;
			log->Warning("Module_IO node-{}: Error read config adapter id-{}: error 0: syserror 0", _node_id, out.get_id());
		}

		return std::move(adapter);
	};

	scada_ate::gate::adapter::IConfigAdapter_ptr Module_CTRL::create_config_adapter(const std::string& type)
	{
		scada_ate::gate::adapter::IConfigAdapter_ptr ptr = nullptr;

		if (type == "SM")
		{
			ptr = std::make_shared<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory>();
			ptr->type_adapter = scada_ate::gate::adapter::TypeAdapter::SharedMemory;
		}
		else if (type == "DDS")
		{
			ptr = std::make_shared<scada_ate::gate::adapter::dds::ConfigAdapterDDS>();
			ptr->type_adapter = scada_ate::gate::adapter::TypeAdapter::DDS;
		}
		else if (type == "OPC_UA")
		{
			ptr = std::make_shared<scada_ate::gate::adapter::opc::ConfigAdapterOPCUA>();
			ptr->type_adapter = scada_ate::gate::adapter::TypeAdapter::OPC_UA;
		}

		return std::move(ptr);
	}

	ResultReqest Module_CTRL::fill_config(scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory* target, atech::common::SmConfig* source)
	{
		ResultReqest result{ ResultReqest::OK };

		try
		{
			target->NameChannel = source->get_name_point_sm();
			target->size_char_data = source->get_size_char_data();
			target->size_double_data = source->get_size_double_data();
			target->size_float_data = source->get_size_float_data();
			target->size_int_data = source->get_size_int_data();
			target->size_str = source->get_size_str();
			target->size_str_data = 100; /// nuzno popravit
		}
		catch (...)
		{
			result = ResultReqest::ERR;
		}

		return result;
	}

	ResultReqest Module_CTRL::fill_config(scada_ate::gate::adapter::dds::ConfigAdapterDDS* target, atech::common::DdsConfig* source)
	{
		ResultReqest result{ ResultReqest::OK };

		try
		{
			target->topic_name = source->get_topic_name();
			if (string_to_typeddsdata(target->type_data, source->get_type_name()) != ResultReqest::OK) throw 1;
			target->type_transfer = scada_ate::gate::adapter::dds::TypeTransfer::PUBLISHER;
			target->str_config_ddslayer.empty();
		}
		catch (...)
		{
			result = ResultReqest::ERR;
		}

		return result;
	}

	ResultReqest Module_CTRL::fill_config(scada_ate::gate::adapter::opc::ConfigAdapterOPCUA* target, atech::common::UaConfig* source)
	{
		ResultReqest result{ ResultReqest::OK };

		try
		{
			target->authentication = scada_ate::gate::adapter::opc::Authentication::Anonymous;
			target->endpoint_url = source->get_endpoint_url();
			target->namespaceindex = std::atoi(source->get_namespace_index().c_str());
			target->password = source->get_password();
			if ( string_to_security_mode_opc(target->security_mode, source->get_security_mode()) != ResultReqest::OK) throw 1;
			if (string_to_security_policy_opc(target->security_policy, source->get_security_policy()) != ResultReqest::OK) throw 1;
			target->user_name = source->get_user_name();
		}
		catch (...)
		{
			result = ResultReqest::ERR;
		}

		return result;
	}

	ResultReqest Module_CTRL::string_to_typeddsdata(scada_ate::gate::adapter::dds::TypeDDSData& type_dds, const std::string& str)
	{
		ResultReqest result{ ResultReqest::OK };

		if (str == "DDSData"){ type_dds = scada_ate::gate::adapter::dds::TypeDDSData::DDSData;}
		else if (str == "DDSDataEx") { type_dds = scada_ate::gate::adapter::dds::TypeDDSData::DDSDataEx; }
		else if (str == "DDSAlarm") { type_dds = scada_ate::gate::adapter::dds::TypeDDSData::DDSAlarm; }
		else if (str == "DDSAlarmEx") { type_dds = scada_ate::gate::adapter::dds::TypeDDSData::DDSAlarmEx; }
		else { result = ResultReqest::ERR; };

		return result;
	}

	ResultReqest Module_CTRL::string_to_security_mode_opc(scada_ate::gate::adapter::opc::SecurityMode& mode, const std::string& str)
	{
		ResultReqest result{ ResultReqest::OK };

		if (str == "None") { mode = scada_ate::gate::adapter::opc::SecurityMode::None; }
		else if (str == "Sign") { mode = scada_ate::gate::adapter::opc::SecurityMode::Sign; }
		else if (str == "SignEndEncrypt") { mode = scada_ate::gate::adapter::opc::SecurityMode::SignEndEncrypt;}
		else { result = ResultReqest::ERR; };

		return result;
	}

	ResultReqest Module_CTRL::string_to_security_policy_opc(scada_ate::gate::adapter::opc::SecurityPolicy& mode, const std::string& str)
	{
		ResultReqest result{ ResultReqest::OK };

		if (str == "None") { mode = scada_ate::gate::adapter::opc::SecurityPolicy::None; }
		else if (str == "Basic256") { mode = scada_ate::gate::adapter::opc::SecurityPolicy::Basic256; }
		else if (str == "Basic256Sha256") { mode = scada_ate::gate::adapter::opc::SecurityPolicy::Basic256Sha256; }
		else if (str == "Basic128Rsa15") { mode = scada_ate::gate::adapter::opc::SecurityPolicy::Basic128Rsa15; }
		else if (str == "Aes256Sha256RsaPss") { mode = scada_ate::gate::adapter::opc::SecurityPolicy::Aes256Sha256RsaPss; }
		else if (str == "Aes128Sha256RsaOaep") { mode = scada_ate::gate::adapter::opc::SecurityPolicy::Aes128Sha256RsaOaep; }
		else { result = ResultReqest::ERR; };

		return result;
	}

	ResultReqest Module_CTRL::vec_datum_to_vec_links(std::vector<scada_ate::gate::adapter::LinkTags>& vec_link, const std::vector<atech::common::Datum>& vec_datum)
	{
		ResultReqest result{ ResultReqest::OK };

		try
		{
			vec_link.clear();
			vec_link.reserve(vec_datum.size());

			for (auto& datum : vec_datum)
			{
				try
				{
					scada_ate::gate::adapter::LinkTags link;
					link.type_registration = datum_to_linktags_typereg(datum.get_type_registration());
					link.delta = datum.get_delta();

					link.target.tag = datum.get_outputdata().get_tag();
					link.target.id_tag = datum.get_outputdata().get_id_tag();
					link.target.is_array = datum.get_outputdata().get_is_array();
					link.target.offset = datum.get_outputdata().get_offset();
					link.target.type = datum_to_linktags_typeval(datum.get_outputdata().get_type());
					link.target.mask = datum.get_outputdata().get_mask();
					//link.target.status;

					link.source.tag = datum.get_inputdata().get_tag();
					link.source.id_tag = datum.get_inputdata().get_id_tag();
					link.source.is_array = datum.get_inputdata().get_is_array();
					link.source.offset = datum.get_inputdata().get_offset();
					link.source.type = datum_to_linktags_typeval(datum.get_inputdata().get_type());
					link.source.mask = datum.get_inputdata().get_mask();
					//link.source.status;

					vec_link.push_back(std::move(link));
				}
				catch (...)
				{
					if (!datum.get_outputdata().get_tag().empty())
					{
						log->Warning("Module_CRTL node-{}: error registration tag-{}", _node_id, datum.get_outputdata().get_tag());
					}
					else
					{
						log->Warning("Module_CRTL node-{}: error registration id_tag-{}", _node_id, datum.get_outputdata().get_id_tag());
					}
				}
			}
		}
		catch (...)
		{
			result = ResultReqest::ERR;
			vec_link.clear();
		}		

		return result;
	}

	scada_ate::gate::adapter::TypeRegistration Module_CTRL::datum_to_linktags_typereg(const std::string& str) 
	{
		if (str == "r") return scada_ate::gate::adapter::TypeRegistration::RECIVE;
		if (str == "d") return scada_ate::gate::adapter::TypeRegistration::DELTA;
		if (str == "up") return scada_ate::gate::adapter::TypeRegistration::UPDATE;
		return scada_ate::gate::adapter::TypeRegistration::RECIVE;
	}

	scada_ate::gate::adapter::TypeValue Module_CTRL::datum_to_linktags_typeval(const std::string str)
	{
		if (str == "i") return scada_ate::gate::adapter::TypeValue::INT;
		if (str == "f") return scada_ate::gate::adapter::TypeValue::FLOAT;
		if (str == "d") return scada_ate::gate::adapter::TypeValue::DOUBLE;
		if (str == "c") return scada_ate::gate::adapter::TypeValue::CHAR;
		if (str == "str") return scada_ate::gate::adapter::TypeValue::STRING;
		throw 1;
		return scada_ate::gate::adapter::TypeValue::INT;
	}


	DdsStatus Module_CTRL::function_processing(DdsCommand& cmd)
	{
		DdsStatus status;
		std::string parametr = std::string(&cmd.cmd_parameter()[0], cmd.cmd_parameter().size());

		if (cmd.cmd_code() == static_cast<uint32_t>(atech::common::Command::RECEIVE_CONFIG))
		{
			status = command_receive_new_config(parametr);
		}
		else if (cmd.cmd_code() == static_cast<uint32_t>(atech::common::Command::SEND_STATUS))
		{
			status = command_request_status(parametr);
		}
		else if (cmd.cmd_code() == static_cast<uint32_t>(atech::common::Command::START))
		{
			status = command_start(parametr);
		}
		else if (cmd.cmd_code() == static_cast<uint32_t>(atech::common::Command::STOP))
		{
			status = command_stop(parametr);
		}
		else if (cmd.cmd_code() == static_cast<uint32_t>(atech::common::Command::RESET))
		{
			status = command_reinit(parametr);
		}
		else if (cmd.cmd_code() == static_cast<uint32_t>(atech::common::Command::SEND_PROCESS_INFO))
		{
			status = command_request_process_info();
		}
		else if (cmd.cmd_code() == static_cast<uint32_t>(atech::common::Command::APPLY_CONFIG))
		{
			status = command_apply_new_config(cmd);
		}

		return status;
	}

	DdsStatus Module_CTRL::command_receive_new_config(std::string_view parametr)
	{
		DdsStatus status;
		size_t size_data;

		try
		{
			nlohmann::json json = nlohmann::json::parse(parametr);
			if (json.count("size") == 0) throw 1;
			if (!json["size"].is_number_unsigned()) throw 2;

			size_data = json["size"].get<size_t>();
			if (size_data == 0) throw 3;

			if (_unitaid->TakeServiceConfig(size_data, config_str_new) != ResultReqest::OK) throw 4;

			if (verification_config_file(config_str_new) != ResultReqest::OK) throw 5;

			status.st_code(static_cast<uint32_t>(atech::common::Status::OK));
		}
		catch (int& e)
		{
			log->Warning("Module_CTRL node-{}, Error receive new config: error {}: syserror: {}", _node_id, e, 0);
			status.st_code(static_cast<uint32_t>(atech::common::Status::ERR));
			config_str_new.clear();
		}
		catch (...)
		{
			log->Warning("Module_CTRL node-{}, Error receive new config: error {}: syserror: {}", _node_id, 0, 0);
			status.st_code(static_cast<uint32_t>(atech::common::Status::ERR));
			config_str_new.clear();
		}

		return status;
	}

	ResultReqest Module_CTRL::verification_config_file(std::string& config)
	{
		ResultReqest result {ResultReqest::OK};

		try
		{
			nlohmann::json json = nlohmann::json::parse(config);

			for (;;)
			{
				if (json.count("node_target") != 0)
				{
					if (json["node_target"].is_number_unsigned())
					{
						if (json["node_target"].get<uint32_t>() == _node_id) break;
					}
				}
				throw 1;
			}

			for (;;)
			{
				if (json.count("dds") != 0)
				{
					break;
					if (json["dds"].count("hash") != 0 && json["dds"].count("profiles") != 0)
					{
						if (json["dds"]["hash"].get<size_t>() == std::hash<nlohmann::json>{}(json["dds"]["profiles"])) break;
					}
				}
				throw 2;
			}

			for (;;)
			{
				if (json.count("topic_max_size") != 0)
				{
					break;
					if (json["topic_max_size"].count("hash") != 0 && json["topic_max_size"].count("dds_type_size") != 0)
					{
						if (json["topic_max_size"]["hash"].get<size_t>() == std::hash<nlohmann::json>{}(json["topic_max_size"]["dds_type_size"])) break;
					}
				}
				throw 3;
			}

			for (;;)
			{
				if (json.count("adapters") != 0)
				{
					break;
					if (json["adapters"].count("hash") != 0 && json["adapters"].count("units") != 0)
					{
						if (json["adapters"]["hash"].get<size_t>() == std::hash<nlohmann::json>{}(json["adapters"]["units"])) break;
					}
				}
				throw 4;
			}
		}
		catch (int& e)
		{
			log->Critical("Moduel_CTRL node-{}: Error verification new config: error {}: syserror {}", _node_id, e, 0);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("Moduel_CTRL node-{}: Error verification new config: error {}: syserror {}", _node_id, 0, 0);
			result = ResultReqest::ERR;
		}  	

		return result;
	}

	DdsStatus Module_CTRL::command_apply_new_config(DdsCommand& cmd)
	{
		DdsStatus status; 
		try 
		{
			status.st_code(static_cast<uint32_t>(atech::common::Status::NONE));

			if (config_str_new.empty()) throw 1;
			if (thread_helper.joinable())
			{
				thread_helper.join();
			}

			thread_helper = std::thread(&Module_CTRL::apply_new_config, this, cmd);
		}
		catch(int& e)
		{
			log->Warning("Module_CTRL node-{}: Error command aplply new config: error {}: syserror: {}", _node_id, e, 0);
			status.st_code(static_cast<uint32_t>(atech::common::Status::ERR));
		}
		catch (...)
		{
			log->Warning("Module_CTRL node-{}: Error command aplply new config: error {}: syserror: {}", _node_id, 0, 0);
			status.st_code(static_cast<uint32_t>(atech::common::Status::ERR));
		}																					   

		return status;
	}

	void Module_CTRL::apply_new_config(DdsCommand cmd)
	{
		DdsStatus status;
		
		try
		{
			log->Info("Module_CTRL node-{}, Start apply new config", _node_id);

			_unitaid = nullptr;
			_module_io_ptr->ClearModule();

			status.cmd_code(static_cast<uint32_t>(atech::common::Command::APPLY_CONFIG));
			status.st_code(static_cast<uint32_t>(atech::common::Status::OK));
			status.id_target(cmd.id_source());
			status.id_source(_node_id);

			init_dds_layer(config_str_new);
			init_module_io(config_str_new);
			_unitaid = atech::srv::io::ctrl::CreateUnit_CP(_config_unitaid);
			status.st_time(TimeConverter::GetTime_LLmcs());
			if (_unitaid->InitUnitAid() == ResultReqest::OK)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				_unitaid->RespondStatus(status);
			}

			log->Info("Module_CTRL node-{}, Apply new config done", _node_id);
		}
		catch (int& e)
		{
			log->Warning("Module_CTRL node-{}, Error apply new config: error {}: syserror: {}", _node_id, 0, 0);
		}
		catch (...)
		{
			log->Warning("Module_CTRL node-{}, Error apply new config: error {}: syserror: {}", _node_id, 0, 0);
		}

		return;	
	}

	ResultReqest  Module_CTRL::get_vector_id_from_json(std::vector<uint32_t>& vector, std::string_view str)
	{
		ResultReqest result { ResultReqest::OK };

		try
		{
			nlohmann::json json = nlohmann::json::parse(str);
			if (json.count("id") == 0) throw 1;
			if (!json["id"].is_array()) throw 2;
			if (!json["id"].empty()) { if (!json["id"].begin()->is_number_unsigned()) throw 3; };
			for (auto& it : json["id"])
			{
				vector.push_back(it.get<uint32_t>());
			}
		}
		catch(int& e)
		{
			log->Critical("Module_CTRL node-{}: Error get vector id: error {}", _node_id, e);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("Module_CTRL node-{}: Error get vector id: error {}", _node_id, 0);
			result = ResultReqest::ERR;
		} 	                    

		return result;
	}

	DdsStatus  Module_CTRL::command_request_status(std::string_view parametr)
	{
		std::deque<std::pair<uint32_t, atech::common::Status>> parametr_answer{};
		DdsStatus status;
		std::string str_out;
		nlohmann::json j_param;
		std::vector<uint32_t> vec_id{};

		try
		{
			if (get_vector_id_from_json(vec_id, parametr) != ResultReqest::OK) throw 1;

			j_param["success"] = std::vector<std::pair<uint32_t,atech::common::Status>>{};
			j_param["error"] = std::vector<uint32_t>{};;
			j_param["unfound"] = std::vector<uint32_t>{};;

			if (vec_id.empty())
			{
				ResultReqest res;
				res = _module_io_ptr->GetStatus(parametr_answer);
				if (res == ResultReqest::OK)
				{
					for (auto& it : parametr_answer)
					{
						nlohmann::json j;
						j["id"] = it.first;
						j["status"] = it.second;
						j_param["success"].push_back(j);
					};
				}
			}
			else
			{
				for (auto& it : vec_id)
				{
					ResultReqest res;
					parametr_answer.clear();
					res = _module_io_ptr->GetStatus(parametr_answer, it);
					if (res == ResultReqest::OK)
					{
						while (!parametr_answer.empty())
						{
							nlohmann::json j;
							j["id"] = parametr_answer.back().first;
							j["status"] = parametr_answer.back().second;
							j_param["success"].push_back(j);
							parametr_answer.pop_back();
						};
					}
					else if (res == ResultReqest::ERR)
					{
						j_param["error"].push_back(it);
					}
					else if (res == ResultReqest::IGNOR)
					{
						j_param["unfound"].push_back(it);
					}
				}
			}

			if (!j_param["error"].empty())
			{
				status.st_code(static_cast<uint32_t>(atech::common::Status::ERR));
			}
			else if (!j_param["unfound"].empty())
			{
				status.st_code(static_cast<uint32_t>(atech::common::Status::WARNING));
			}
			else
			{
				status.st_code(static_cast<uint32_t>(atech::common::Status::OK));
			}

			if (j_param.dump().size() >= status.st_desc().size())
			{
				log->Warning("Module_CTRL node-{}: command {}: description size exceeded {}", _node_id, (uint32_t)atech::common::Command::SEND_STATUS, j_param.dump());
				short i = 0;
				while (j_param.dump().size() > status.st_desc().size())
				{
					if (!j_param["successs"].empty() && i % 3 == 0)
					{
						j_param["successs"].erase(j_param["success"].size() - 1);
					}
					if (!j_param["error"].empty() && i % 3 == 1)
					{
						j_param["error"].erase(j_param["error"].size() - 1);
					}
					if (!j_param["unfound"].empty() && i == 2)
					{
						j_param["unfound"].erase(j_param["unfound"].size() - 1);
					}
					i++;
				}
			}

			auto& st_desc = status.st_desc();
			str_out = j_param.dump();
			volatile uint32_t i = 0;
			for (auto& it : str_out)
			{
				st_desc[i] = it;
				i++;
			}
		}
		catch (int& e)
		{
			log->Critical("Module_CTRL node-{}: Error command {}: error {}", _node_id, (uint32_t)atech::common::Command::SEND_STATUS, e);
			status.st_code((uint32_t)atech::common::Status::ERR);
		}
		catch (...)
		{
			log->Critical("Module_CTRL node-{}: Error command {}: error {}", _node_id, (uint32_t)atech::common::Command::SEND_STATUS, 0);
			status.st_code((uint32_t)atech::common::Status::ERR);
		}		

		return status;
	}

	DdsStatus  Module_CTRL::command_start(std::string_view parametr)
	{
		std::deque<std::pair<uint32_t, atech::common::Status>> parametr_answer;
		DdsStatus status;
		std::string str_out;
		nlohmann::json j_param;
		std::vector<uint32_t> vec_id{};

		try
		{
			if (get_vector_id_from_json(vec_id, parametr) != ResultReqest::OK) throw 1;

			j_param["success"] = std::vector<std::pair<uint32_t, atech::common::Status>>{};
			j_param["error"] = std::vector<uint32_t>{};;
			j_param["unfound"] = std::vector<uint32_t>{};;

			if (vec_id.empty())
			{
				ResultReqest res;
				res = _module_io_ptr->Start(parametr_answer);
				if (res == ResultReqest::OK)
				{
					for (auto& it : parametr_answer)
					{
						if (it.second == atech::common::Status::OK)
						{
							j_param["success"].push_back(it.first);
						}
						else
						{
							j_param["error"].push_back(it.first);
						}
					};
				}
			}
			else
			{
				for (auto& it : vec_id)
				{
					ResultReqest res;
					res = _module_io_ptr->Start(parametr_answer, it);
					if (res == ResultReqest::OK)
					{
						if (parametr_answer.back().second == atech::common::Status::OK)
						{
							j_param["success"].push_back(it);
						}
						else
						{
							j_param["error"].push_back(it);
						}
						parametr_answer.pop_back();
					}
					else if (res == ResultReqest::ERR)
					{
						j_param["error"].push_back(it);
					}
					else if (res == ResultReqest::IGNOR)
					{
						j_param["unfound"].push_back(it);
					}
				}
			}

			if (!j_param["error"].empty())
			{
				status.st_code(static_cast<uint32_t>(atech::common::Status::ERR));
			}
			else if (!j_param["unfound"].empty())
			{
				status.st_code(static_cast<uint32_t>(atech::common::Status::WARNING));
			}
			else
			{
				status.st_code(static_cast<uint32_t>(atech::common::Status::OK));
			}

			if (j_param.dump().size() >= status.st_desc().size())
			{
				log->Warning("Module_CTRL node-{}: command {}: description size exceeded {}", _node_id, (uint32_t)atech::common::Command::START, j_param.dump());
				short i = 0;
				while (j_param.dump().size() >= status.st_desc().size())
				{
					if (!j_param["successs"].empty() && i % 3 == 0)
					{
						j_param["successs"].erase(j_param["success"].size() - 1);
					}
					if (!j_param["error"].empty() && i % 3 == 1)
					{
						j_param["error"].erase(j_param["error"].size() - 1);
					}
					if (!j_param["unfound"].empty() && i == 2)
					{
						j_param["unfound"].erase(j_param["unfound"].size() - 1);
					}
					i++;
				}
			}

			auto& st_desc = status.st_desc();
			str_out = j_param.dump();
			volatile uint32_t i = 0;
			for (auto& it : str_out)
			{
				st_desc[i] = it;
				i++;
			}
		}
		catch (int& e)
		{
			log->Critical("Module_CTRL node-{}: Error command {}: error {}", _node_id, (uint32_t)atech::common::Command::START, e);
			status.st_code((uint32_t)atech::common::Status::ERR);
		}
		catch (...)
		{
			log->Critical("Module_CTRL node-{}: Error command {}: error {}", _node_id, (uint32_t)atech::common::Command::START, 0);
			status.st_code((uint32_t)atech::common::Status::ERR);
		} 		

		return status;
	}

	DdsStatus  Module_CTRL::command_stop(std::string_view parametr)
	{
		std::deque<std::pair<uint32_t, atech::common::Status>> parametr_answer;
		DdsStatus status;
		std::string str_out;
		nlohmann::json j_param;
		std::vector<uint32_t> vec_id{};			

		try
		{
			if (get_vector_id_from_json(vec_id, parametr) != ResultReqest::OK) throw 1;

			j_param["success"] = std::vector<std::pair<uint32_t, atech::common::Status>>{};
			j_param["error"] = std::vector<uint32_t>{};;
			j_param["unfound"] = std::vector<uint32_t>{};;

			if (vec_id.empty())
			{
				ResultReqest res;
				res = _module_io_ptr->Stop(parametr_answer);
				if (res == ResultReqest::OK)
				{
					for (auto& it : parametr_answer)
					{
						if (it.second == atech::common::Status::STOP)
						{
							j_param["success"].push_back(it.first);
						}
						else
						{
							j_param["error"].push_back(it.first);
						}
					};
				}
			}
			else
			{
				for (auto& it : vec_id)
				{
					ResultReqest res;
					res = _module_io_ptr->Stop(parametr_answer, it);
					if (res == ResultReqest::OK)
					{
						if (parametr_answer.back().second == atech::common::Status::STOP)
						{
							j_param["success"].push_back(it);
						}
						else
						{
							j_param["error"].push_back(it);
						}
						parametr_answer.pop_back();
					}
					else if (res == ResultReqest::ERR)
					{
						j_param["error"].push_back(it);
					}
					else if (res == ResultReqest::IGNOR)
					{
						j_param["unfound"].push_back(it);
					}
				}
			}

			if (!j_param["error"].empty())
			{
				status.st_code(static_cast<uint32_t>(atech::common::Status::ERR));
			}
			else if (!j_param["unfound"].empty())
			{
				status.st_code(static_cast<uint32_t>(atech::common::Status::WARNING));
			}
			else
			{
				status.st_code(static_cast<uint32_t>(atech::common::Status::OK));
			}

			if (j_param.dump().size() >= status.st_desc().size())
			{
				log->Warning("Module_CTRL node-{}: command {}: description size exceeded {}", _node_id, (uint32_t)atech::common::Command::START, j_param.dump());
				short i = 0;
				while (j_param.dump().size() >= status.st_desc().size())
				{
					if (!j_param["successs"].empty() && i % 3 == 0)
					{
						j_param["successs"].erase(j_param["success"].size() - 1);
					}
					if (!j_param["error"].empty() && i % 3 == 1)
					{
						j_param["error"].erase(j_param["error"].size() - 1);
					}
					if (!j_param["unfound"].empty() && i == 2)
					{
						j_param["unfound"].erase(j_param["unfound"].size() - 1);
					}
					i++;
				}
			}

			auto& st_desc = status.st_desc();
			str_out = j_param.dump();
			volatile uint32_t i = 0;
			for (auto& it : str_out)
			{
				st_desc[i] = it;
				i++;
			}
		}
		catch (int& e)
		{
			log->Critical("Module_CTRL node-{}: Error command {}: error {}", _node_id, (uint32_t)atech::common::Command::STOP, e);
			status.st_code((uint32_t)atech::common::Status::ERR);
		}
		catch (...)
		{
			log->Critical("Module_CTRL node-{}: Error command {}: error {}", _node_id, (uint32_t)atech::common::Command::STOP, 0);
			status.st_code((uint32_t)atech::common::Status::ERR);
		}

		return status;
	}

	DdsStatus  Module_CTRL::command_reinit(std::string_view parametr)
	{
		std::deque<std::pair<uint32_t, atech::common::Status>> parametr_answer;
		DdsStatus status;
		std::string str_out;
		nlohmann::json j_param;
		std::vector<uint32_t> vec_id{};

		try
		{
			if (get_vector_id_from_json(vec_id, parametr) != ResultReqest::OK) throw 1;

			j_param["success"] = std::vector<std::pair<uint32_t, atech::common::Status>>{};
			j_param["error"] = std::vector<uint32_t>{};;
			j_param["unfound"] = std::vector<uint32_t>{};;

			if (vec_id.empty())
			{
				ResultReqest res;
				res = _module_io_ptr->ReInit(parametr_answer);
				if (res == ResultReqest::OK)
				{
					for (auto& it : parametr_answer)
					{
						if (it.second == atech::common::Status::OK)
						{
							j_param["success"].push_back(it.first);
						}
						else
						{
							j_param["error"].push_back(it.first);
						}
					};
				}
			}
			else
			{
				for (auto& it : vec_id)
				{
					ResultReqest res;
					res = _module_io_ptr->ReInit(parametr_answer, it);
					if (res == ResultReqest::OK)
					{
						if (parametr_answer.back().second == atech::common::Status::OK)
						{
							j_param["success"].push_back(it);
						}
						else
						{
							j_param["error"].push_back(it);
						}
						parametr_answer.pop_back();
					}
					else if (res == ResultReqest::ERR)
					{
						j_param["error"].push_back(it);
					}
					else if (res == ResultReqest::IGNOR)
					{
						j_param["unfound"].push_back(it);
					}
				}
			}

			if (!j_param["error"].empty())
			{
				status.st_code(static_cast<uint32_t>(atech::common::Status::ERR));
			}
			else if (!j_param["unfound"].empty())
			{
				status.st_code(static_cast<uint32_t>(atech::common::Status::WARNING));
			}
			else
			{
				status.st_code(static_cast<uint32_t>(atech::common::Status::OK));
			}

			if (j_param.dump().size() >= status.st_desc().size())
			{
				log->Warning("Module_CTRL node-{}: command {}: description size exceeded {}", _node_id, (uint32_t)atech::common::Command::START, j_param.dump());
				short i = 0;
				while (j_param.dump().size() >= status.st_desc().size())
				{
					if (!j_param["successs"].empty() && i % 3 == 0)
					{
						j_param["successs"].erase(j_param["success"].size() - 1);
					}
					if (!j_param["error"].empty() && i % 3 == 1)
					{
						j_param["error"].erase(j_param["error"].size() - 1);
					}
					if (!j_param["unfound"].empty() && i == 2)
					{
						j_param["unfound"].erase(j_param["unfound"].size() - 1);
					}
					i++;
				}
			}

			auto& st_desc = status.st_desc();
			str_out = j_param.dump();
			volatile uint32_t i = 0;
			for (auto& it : str_out)
			{
				st_desc[i] = it;
				i++;
			}
		}
		catch (int& e)
		{
			log->Critical("Module_CTRL node-{}: Error command {}: error {}", _node_id, (uint32_t)atech::common::Command::STOP, e);
			status.st_code((uint32_t)atech::common::Status::ERR);
		}
		catch (...)
		{
			log->Critical("Module_CTRL node-{}: Error command {}: error {}", _node_id, (uint32_t)atech::common::Command::STOP, 0);
			status.st_code((uint32_t)atech::common::Status::ERR);
		}

		return status;
	}

	DdsStatus Module_CTRL::command_request_version(uint32_t id)
	{
		return DdsStatus{};
	}

	DdsStatus Module_CTRL::command_request_process_info()
	{
		DdsStatus answer;
		atech::common::ProcessInfo proc_info;

		answer.cmd_code((uint32_t)atech::common::Command::SEND_PROCESS_INFO);

		try
		{
			try 
			{
				proc_info.set_hname(atech::Process::GetHostName());
				proc_info.set_pcpu(atech::Process::GetProcessCpuUsage(500));
				proc_info.set_pid(atech::Process::GetProcessID());
				proc_info.set_pinfo("srv");
				proc_info.set_pmemory(atech::Process::GetProcessMemory());
				proc_info.set_pname(atech::Process::GetProcessName());
				proc_info.set_pparam(atech::Process::GetProcessParameter());
				proc_info.set_state(0);
			}
			catch(...)
			{
				throw 1;
			}
			
			nlohmann::ordered_json json;
			json["process"];
			nlohmann::adl_serializer<atech::common::ProcessInfo>::to_json(json["process"], proc_info);

			auto& mass = answer.st_desc();
			auto str = json.dump();
			if (mass.size() < str.size())
			{
				std::cout << str << std::endl;
				throw 2;
			}

			volatile int i = 0;
			for (auto& s : str)
			{
				mass[i] = s; 
				i++;
			}

			answer.st_code((uint32_t)atech::common::Status::OK);
		}
		catch (int& e)
		{
			log->Critical("Module_CTRL node-{}: Error command request process info: error {}: syserror {}", _node_id, e, 0);
			answer.st_code((uint32_t)atech::common::Status::ERR);
		}
		catch (nlohmann::json::exception& e)
		{
			log->Critical("Module_CTRL node-{}: Error command request process info: error {}: syserror {}", _node_id, e.what(), 0);
			answer.st_code((uint32_t)atech::common::Status::ERR);
		}
		catch (...)
		{
			log->Critical("Module_CTRL node-{}: Error command request process info: error {}: syserror {}", _node_id, 0, 0);
			answer.st_code((uint32_t)atech::common::Status::ERR);
		} 

		return answer;
	}

	DdsStatus command_set_log_level(std::string_view parametr)
	{
		return DdsStatus{};
	}

}


