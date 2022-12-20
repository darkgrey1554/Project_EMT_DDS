#include<Module_IO/UnitTransfer/adapters/OPCUA_Adapter/AdapterOPCUA.h>

namespace  scada_ate::gate::adapter::opc
{
	AdapterOPCUA::AdapterOPCUA(std::shared_ptr<IConfigAdapter> config)
	{
		std::shared_ptr<ConfigAdapterOPCUA> config_point = std::reinterpret_pointer_cast<ConfigAdapterOPCUA>(config);

		if (config_point != nullptr && config_point->type_adapter == TypeAdapter::OPC_UA)
		{
			this->config = *config_point;
		}

		UA_ReadRequest_init(&_read_request);
		UA_WriteRequest_init(&_write_request);
		data.clear();

		log = std::make_shared<atech::logger::LoggerScadaSpdDds>();
	};

	AdapterOPCUA::~AdapterOPCUA() 
	{
		const std::lock_guard<std::mutex> lock_init(mutex_init);
		destroy();
	};

	ResultReqest AdapterOPCUA::InitAdapter()
	{
		const std::lock_guard<std::mutex> lock_init(mutex_init);
		return init_adapter();
	}

	UA_ByteString AdapterOPCUA::loadFile(const char* const path) 
	{

		UA_ByteString fileContents = UA_STRING_NULL;
		std::ifstream file;
		file.open(path, std::ios::binary | std::ios::ate);
		if (!file.is_open())
		{
			return fileContents;
		}

		fileContents.length = file.tellg();
		fileContents.data = (UA_Byte*)UA_malloc(fileContents.length * sizeof(UA_Byte));
		if (fileContents.data) {
			file.seekg(0);
			file.read((char*)fileContents.data, fileContents.length);
		}
		else
		{
			fileContents.length = 0;
		}

		file.close();

		return fileContents;
	}

	void AdapterOPCUA::log_info_config()
	{
		log->Debug("AdapterOPCUA id-{}: Config: Endpoint_url : {}, SecurityMode: {}, SecurityPolicy: {}, Authentication: {}, User: {}, Password: {}",
			config.id_adapter, 
			config.endpoint_url, 
			to_string(config.security_mode), 
			to_string(config.security_policy), 
			to_string(config.authentication), 
			config.user_name, config.password);
	}

	ResultReqest AdapterOPCUA::take_trust_list()
	{
		ResultReqest result = ResultReqest::OK;

		try
		{
			const std::filesystem::path box{ path_trusts_files };
			std::filesystem::create_directory(box);
			std::list<std::filesystem::path> _list_files;
			for (auto const& dir_entry : std::filesystem::directory_iterator{box})
			{
				if (dir_entry.is_regular_file())
				{
					std::string str = dir_entry.path().filename().string();
					if (std::strcmp(&str[str.length() - 4], ".der") == 0)
					{
						_list_files.push_back(dir_entry.path());
					}
				}
			}

			trustListSize = _list_files.size();
			if (trustListSize == 0) throw 1;
			trustList = (UA_ByteString*)UA_malloc(trustListSize * sizeof(UA_ByteString));
			if (trustList == nullptr) throw 2;

			size_t count_iter = 0;
			for (auto& iter : _list_files)
			{
				*(trustList+count_iter) = loadFile(iter.string().c_str());
				count_iter++;
			}
		}			  
		catch (int& e)
		{
			log->Warning("AdapterOPCUA id-{}: error read_trust_list: error: {}, syserror: {} ", config.id_adapter, e, 0);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterOPCUA id-{}: error read_trust_list: error: {}, syserror: {} ", config.id_adapter,0,0);
			result = ResultReqest::ERR;
		}

		return result;
	} 

	ResultReqest  AdapterOPCUA::validation_security()
	{
		if (config.security_mode != SecurityMode::None) return ResultReqest::ERR;
		if (config.security_policy != SecurityPolicy::None) return ResultReqest::ERR;
		return ResultReqest::OK;
	}

	ResultReqest AdapterOPCUA::take_certificate()
	{
		ResultReqest result{ ResultReqest::OK };

		try
		{
			/*if (!std::filesystem::exists("certificate"))
			{
				std::filesystem::create_directory("certificate");
				throw 1;
			}*/
			this->certificate = loadFile(path_certificate.c_str());
			this->privateKey = loadFile(path_privatekey.c_str());

			if (certificate.length == 0 || certificate.data == nullptr) throw 2;
			if (privateKey.length == 0 || privateKey.data == nullptr) throw 3;
		}
		catch (int& e)
		{
			log->Warning("AdapterOPCUA id-{}: error take_certificate: error: {}, syserror: {} ", config.id_adapter, e, 0);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Warning("AdapterOPCUA id-{}: error take_certificate: error: {}, syserror: {} ", config.id_adapter, 0, 0);
			result = ResultReqest::ERR;
		}

		return result;
	}

	ResultReqest AdapterOPCUA::create_client()
	{
		ResultReqest result{ ResultReqest::OK };

		try
		{
			_client = UA_Client_new();
			UA_ClientConfig* config_client = UA_Client_getConfig(_client);

			config_client->securityMode = get_messege_security(this->config.security_mode); /* require encryption */
			config_client->securityPolicyUri = get_security_policyUri(this->config.security_policy);
			UA_StatusCode retVal = UA_ClientConfig_setDefaultEncryption(config_client, certificate, privateKey,
				trustList, trustListSize, NULL, 0);
			UA_SecurityPolicy_None(config_client->securityPolicies, certificate, &config_client->logger);
			config_client->clientDescription.applicationUri = UA_STRING_ALLOC("urn:open62541.client.application");
		}
		catch(int& e)
		{
			log->Warning("AdapterOPCUA id-{}: error create_client: error: {}, syserror: {} ", config.id_adapter, e, 0);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Warning("AdapterOPCUA id-{}: error create_client: error: {}, syserror: {} ", config.id_adapter, 0, 0);
			result = ResultReqest::ERR;
		}

		return result;
	};

	ResultReqest AdapterOPCUA::establishing_connection()
	{
		ResultReqest result{ ResultReqest::OK };
		UA_StatusCode retval;

		try
		{
			if (config.authentication == Authentication::User)
			{
				retval = UA_Client_connectUsername(_client, this->config.endpoint_url.c_str(), config.user_name.c_str(), config.password.c_str());
				if (retval != UA_STATUSCODE_GOOD) throw 1;
			}
			else if (config.authentication == Authentication::Anonymous)
			{
				retval = UA_Client_connect(_client, this->config.endpoint_url.c_str());
				if (retval != UA_STATUSCODE_GOOD) throw 2;
			}
			else if (config.authentication == Authentication::Certificate)
			{
				throw 3;
			}			
		}
		catch (int& e)
		{
			log->Warning("AdapterOPCUA id-{}: error establishing_connection: error: {}, syserror: {} ", config.id_adapter, e, retval);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Warning("AdapterOPCUA id-{}: error establishing_connection: error: {}, syserror: {} ", config.id_adapter, 0, 0);
			result = ResultReqest::ERR;
		}  

		return result;
	}

	UA_MessageSecurityMode AdapterOPCUA::get_messege_security(SecurityMode& mode)
	{
		UA_MessageSecurityMode mode_out{ UA_MessageSecurityMode::UA_MESSAGESECURITYMODE_INVALID };
		if (mode == SecurityMode::None) mode_out = UA_MESSAGESECURITYMODE_NONE;
		if (mode == SecurityMode::Sign) mode_out = UA_MESSAGESECURITYMODE_SIGN;
		if (mode == SecurityMode::SignEndEncrypt) mode_out = UA_MESSAGESECURITYMODE_SIGNANDENCRYPT;

		return mode_out;
	}

	UA_String AdapterOPCUA::get_security_policyUri(SecurityPolicy& mode)
	{
		UA_String str;
		if (mode == SecurityPolicy::Aes128Sha256RsaOaep)
		{
			str = UA_STRING_ALLOC("http://opcfoundation.org/UA/SecurityPolicy#Aes128Sha256RsaOaep");
		}
		else if (mode == SecurityPolicy::Aes256Sha256RsaPss)
		{
			str = UA_STRING_ALLOC("http://opcfoundation.org/UA/SecurityPolicy#Aes256Sha256RsaPss");
		}
		else if (mode == SecurityPolicy::Basic128Rsa15)
		{
			str = UA_STRING_ALLOC("http://opcfoundation.org/UA/SecurityPolicy#Basic128Rsa15");
		}
		else if (mode == SecurityPolicy::Basic256)
		{
			str = UA_STRING_ALLOC("http://opcfoundation.org/UA/SecurityPolicy#Basic256");
		}
		else if (mode == SecurityPolicy::Basic256Sha256)
		{
			str = UA_STRING_ALLOC("http://opcfoundation.org/UA/SecurityPolicy#Basic256Sha256");
		}
		else
		{
			str = UA_STRING_ALLOC("http://opcfoundation.org/UA/SecurityPolicy#None");
		}

		return str;
	}

	ResultReqest AdapterOPCUA::create_request_to_read()
	{
		ResultReqest result{ ResultReqest::OK };

		try
		{			
			if (config.vec_tags_source.empty()) return result;

			map_index_request_read.clear();
			size_t counter_iter = 0;
			for (auto& iter : config.vec_tags_source)
			{
				if (map_index_request_read.count(iter) > 0) continue;

				map_index_request_read[iter] = counter_iter;
				counter_iter++;
			}

			_read_request.nodesToReadSize = map_index_request_read.size();
			_read_request.nodesToRead = (UA_ReadValueId*)UA_Array_new(map_index_request_read.size(), &UA_TYPES[UA_TYPES_READVALUEID]);

			for (auto& iter : map_index_request_read)
			{
				UA_ReadValueId& value = _read_request.nodesToRead[iter.second];
				UA_ReadValueId_init(&value);
				value.nodeId = tag_to_nodeid(iter.first);
				value.attributeId = UA_ATTRIBUTEID_VALUE;
			}
		}
		catch (int& e)
		{
			log->Warning("AdapterOPCUA id-{}: error creat_request_to_read: error: {}, syserror: {} ", config.id_adapter, e, 0);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Warning("AdapterOPCUA id-{}: error creat_request_to_read: error: {}, syserror: {} ", config.id_adapter, 0, 0);
			result = ResultReqest::ERR;
		}

		return result;
	}

	ResultReqest AdapterOPCUA::create_request_to_write()
	{
		ResultReqest result{ ResultReqest::OK };
		
		try
		{
			if (config.vec_link_tags.empty()) return result;

			if (init_last_data() != ResultReqest::OK) throw 1;
			if (init_write_request() != ResultReqest::OK) throw 2;

		}
		catch (int& e)
		{
			log->Warning("AdapterOPCUA id-{}: error create_request_to_write: error: {}, syserror: {} ", config.id_adapter, e, 0);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Warning("AdapterOPCUA id-{}: error create_request_to_write: error: {}, syserror: {} ", config.id_adapter, 0, 0);
			result = ResultReqest::ERR;
		}

		return result;
	};

	void AdapterOPCUA::init_deque()
	{
		data.clear();
		data.push_back({});
		auto _data_unit = data.begin();
		for (auto& itag : config.vec_tags_source)
		{
			_data_unit->map_data[itag] = init_value(itag.type);
		}
	}

	void AdapterOPCUA::destroy()
	{
		try
		{
			log->Debug("AdapterOPCUA id-{}: Start destroy", config.id_adapter);
			UA_ByteString_clear(&certificate);
			UA_ByteString_clear(&privateKey);
			for (size_t i = 0; i < trustListSize; i++)
			{
				UA_ByteString_clear(&trustList[i]);
			}
			UA_ReadRequest_clear(&_read_request);
			UA_WriteRequest_clear(&_write_request);
			UA_Client_delete(_client);
			_client = nullptr;
			current_status.store(atech::common::Status::NONE);
			log->Debug("AdapterOPCUA id-{}: Destroy done", this->config.id_adapter);
		}
		catch (int& e)
		{
			log->Critical("AdapterOPCUA id-{}: Error destroy: error {}: syserror {} ", config.id_adapter, e, 0);
		}
		catch (...)
		{
			log->Critical("AdapterOPCUA id-{}: Error destroy: error {}: syserror {} ", config.id_adapter, 0, 0);
		} 	
		
		return;
	}

	ResultReqest AdapterOPCUA::init_adapter()
	{
		ResultReqest res = ResultReqest::OK;
		unsigned int result = 0;
		unsigned long sys_error = 0;

		atech::common::Status status = current_status.load(std::memory_order::memory_order_relaxed);
		if (status == atech::common::Status::INIT || status == atech::common::Status::OK)
		{
			ResultReqest res = ResultReqest::IGNOR;
			return res;
		}

		try
		{
			if (config.type_adapter != TypeAdapter::OPC_UA) throw 1;
			log->Debug("AdapterOPCUA id-{}: Start init", config.id_adapter);

			log_info_config();

			init_deque();

			if (take_trust_list() == ResultReqest::ERR)
			{
				if (validation_security() != ResultReqest::OK) throw 2;
			}

			if (take_certificate() == ResultReqest::ERR)
			{
				if (validation_security() != ResultReqest::OK) throw 3;
			}

			if (create_client() != ResultReqest::OK) throw 4;

			if (establishing_connection() == ResultReqest::ERR) throw 5;

			if (create_request_to_read() == ResultReqest::ERR) {};

			if (create_request_to_write() == ResultReqest::ERR) {};

			current_status.store(atech::common::Status::OK);
			log->Debug("AdapterOPCUA id-{}: Init DONE", config.id_adapter);
		}
		catch (int& e)
		{
			log->Critical("AdapterOPCUA id-{}: Error initiation: error: {} syserror: {}", config.id_adapter, e, 0);
			res = ResultReqest::ERR;
			current_status.store(atech::common::Status::ERROR_INIT);
		}
		catch (...)
		{
			log->Critical("AdapterOPCUA id-{}: Error initiation: error: {} syserror: {}", config.id_adapter, 0, 0);
			res = ResultReqest::ERR;
			current_status.store(atech::common::Status::ERROR_INIT);
		}

		return res;
	}



	ResultReqest AdapterOPCUA::ReadData(std::deque<SetTags>** data) 
	{
		UA_ReadResponse _respone;
		auto data_opc = this->data.begin();
		ResultReqest result = ResultReqest::OK;
		uint32_t syserror = 0;

		if (current_status.load(std::memory_order_relaxed) != atech::common::Status::OK)
		{
			log->Debug("AdapterOPCUA id-{} : ReadData IGNOR", config.id_adapter);
			result = ResultReqest::IGNOR;
			return result;
		}
		log->Debug("AdapterOPCUA id-{} : ReadData START", config.id_adapter);

		try
		{
			_respone = UA_Client_Service_read(_client, _read_request);
			if (_respone.responseHeader.serviceResult != UA_STATUSCODE_GOOD)
			{
				UA_ReadResponse_clear(&_respone);
				UA_SecureChannelState _state_channel;
				UA_SessionState _state_session;
				UA_StatusCode _state_connect;
				UA_Client_getState(_client, &_state_channel, &_state_session, &_state_connect);
				if (_state_channel == UA_SecureChannelState::UA_SECURECHANNELSTATE_CLOSED || _state_channel != UA_STATUSCODE_GOOD)
				{
					syserror = _state_connect;
					current_status.store(atech::common::Status::ERROR_CONNECTING);
					//destroy();
					throw 1;
				}
				throw 2;
			};

			for (auto& itag : config.vec_tags_source)
			{
				auto index_tag = map_index_request_read.find(itag);
				UA_DataValue& value = _respone.results[index_tag->second];
				if (validation_data(itag, value) != ResultReqest::OK)
				{
					data_opc->map_data[itag] = init_value(itag.type);
					continue;
				};

				data_opc->map_data[itag] = get_opc_value(itag, value);
				itag.status = StatusTag::OK;
			}
			UA_ReadResponse_clear(&_respone);
		}
		catch (int& e)
		{
			log->Critical("AdapterOPCUA id-{} : Error ReadData: error: {} syserror: {}", config.id_adapter, e, syserror);
			ResultReqest result = ResultReqest::ERR;
			*data = nullptr;
		}
		catch (...)
		{
			log->Critical("AdapterOPCUA id-{} : Error ReadData: error: {} syserror: {}", config.id_adapter, 0, 0);
			ResultReqest result = ResultReqest::ERR;
			*data = nullptr;
		}

		*data = &this->data;

		return result;
	};

	ResultReqest AdapterOPCUA::validation_data(InfoTag& tag, UA_DataValue& value)
	{
		ResultReqest result{ ResultReqest::OK };

		if (value.status == UA_STATUSCODE_BADNODEIDUNKNOWN || value.hasValue == false)
		{
			if (tag.status != StatusTag::NOTDETECTED)
			{
				log->Warning("AdapterOPCUA id-{} : tag {} {} not detected", config.id_adapter, tag.tag, tag.id_tag);
				tag.status = StatusTag::NOTDETECTED;
			}
			return ResultReqest::ERR;
		}

		if (!isequil_type(tag.type, value.value.type->typeKind))
		{
			if (tag.status != StatusTag::BADTYPE)
			{
				log->Warning("AdapterOPCUA id-{} : tag {} {} bad type", config.id_adapter, tag.tag, tag.id_tag);
				tag.status = StatusTag::BADTYPE;
			}
			return ResultReqest::ERR;
		}

		if (tag.is_array != (value.value.arrayLength > 0))
		{
			if (tag.status != StatusTag::BADDIMENSION)
			{
				log->Warning("AdapterOPCUA id-{} : tag {} {} bad dimension", config.id_adapter, tag.tag, tag.id_tag);
				tag.status = StatusTag::BADDIMENSION;
			}
			return ResultReqest::ERR;
		}
		
		if (tag.offset > value.value.arrayLength)
		{
			if (tag.status != StatusTag::BADINDEX)
			{
				log->Warning("AdapterOPCUA id-{} : tag {} {} bad index", config.id_adapter, tag.tag, tag.id_tag);
				tag.status = StatusTag::BADINDEX;
			}
			return ResultReqest::ERR;
		} 

		return result;
	}

	bool AdapterOPCUA::isequil_type(const TypeValue& type_scada,const UA_Int32& type_opc)
	{
		bool result = false;

		switch (type_scada)						  
		{
			case TypeValue::INT:
				if (type_opc == UA_DataTypeKind::UA_DATATYPEKIND_INT16 ||
					type_opc == UA_DataTypeKind::UA_DATATYPEKIND_INT32 ||
					type_opc == UA_DataTypeKind::UA_DATATYPEKIND_UINT16 ||
					type_opc == UA_DataTypeKind::UA_DATATYPEKIND_UINT32) result = true;
			break;
			case TypeValue::FLOAT:
				if (type_opc == UA_DataTypeKind::UA_DATATYPEKIND_FLOAT) result = true;
			break;
			case TypeValue::DOUBLE:
				if (type_opc == UA_DataTypeKind::UA_DATATYPEKIND_DOUBLE) result = true;
			break;
			case TypeValue::CHAR:
				if (type_opc == UA_DataTypeKind::UA_DATATYPEKIND_BYTE ||
					type_opc == UA_DataTypeKind::UA_DATATYPEKIND_SBYTE) result = true;
			break;
			case TypeValue::STRING:
				if (type_opc == UA_DataTypeKind::UA_DATATYPEKIND_STRING) result = true;
			break;
			default:
				result = false;
			break;
		}

		return result;
	}

	Value  AdapterOPCUA::init_value(const TypeValue& type)
	{
		Value value;
		value.quality = 0;
		value.time = 0;
		if (type == TypeValue::INT) { value.value = 0; }
		else if (type == TypeValue::FLOAT) { value.value = (float).0; }
		else if (type == TypeValue::DOUBLE) { value.value = (double).0; }
		else if (type == TypeValue::CHAR) { value.value = (char)0; }
		else if (type == TypeValue::STRING) { value.value = std::string{}; }
		return value;
	}

	Value  AdapterOPCUA::get_opc_value(InfoTag& tag, UA_DataValue& value_opc)
	{
		Value value;

		if (value_opc.hasSourceTimestamp)
		{
			value.time = value_opc.sourceTimestamp - UA_DATETIME_UNIX_EPOCH;
		}
		else
		{
			value.time = 0;
		}

		if (value_opc.hasStatus == true)
		{
			value.quality = convert_UAStatus_toScadaStatus(value_opc.status);
		}
		else
		{
			value.quality = 0;
		}

		if (tag.is_array)
		{
			value.value = get_value(value_opc.value.data, value_opc.value.type->typeKind,tag.offset);
		}
		else
		{
			value.value = get_value(value_opc.value.data, value_opc.value.type->typeKind,0);
		}

		return value;
	}

	std::variant<int, float, double, char, std::string> AdapterOPCUA::get_value(void* ptr_value, const UA_Int32& type_opc, size_t offset)
	{
		std::variant<int, float, double, char, std::string> value;

		switch (type_opc)
		{
		case  UA_DataTypeKind::UA_DATATYPEKIND_INT16:
			value = (int)*((int16_t*)ptr_value + offset);
			break;
		case  UA_DataTypeKind::UA_DATATYPEKIND_UINT16:
			value = (int)*((uint16_t*)ptr_value + offset);
			break;
		case  UA_DataTypeKind::UA_DATATYPEKIND_INT32:
			value = (int)*((int32_t*)ptr_value + offset);
			break;
		case  UA_DataTypeKind::UA_DATATYPEKIND_UINT32:
			value = (int)*((uint32_t*)ptr_value + offset);
			break;
		case  UA_DataTypeKind::UA_DATATYPEKIND_FLOAT:
			value = *((float*)ptr_value + offset);
			break;
		case  UA_DataTypeKind::UA_DATATYPEKIND_DOUBLE:
			value = *((double*)ptr_value + offset);
			break;
		case  UA_DataTypeKind::UA_DATATYPEKIND_BYTE:
			value = *(char*)((uint8_t*)ptr_value + offset);
			break;
		case  UA_DataTypeKind::UA_DATATYPEKIND_SBYTE:
			value = *(char*)((int8_t*)ptr_value + offset);
			break;
		case  UA_DataTypeKind::UA_DATATYPEKIND_STRING:
			value = std::string((const char*)((UA_String*)ptr_value + offset)->data, ((UA_String*)ptr_value + offset)->length);
			break;
		default:
			break;
		}

		return std::move(value);
	};



	ResultReqest AdapterOPCUA::WriteData(const std::deque<SetTags>& data)
	{
		ResultReqest result = ResultReqest::OK;
		uint32_t syserror = 0;

		if (current_status.load(std::memory_order_relaxed) != atech::common::Status::OK)
		{
			log->Debug("AdapterOPCUA id-{} : WriteData IGNOR", config.id_adapter);
			result = ResultReqest::IGNOR;
			return result;
		}
		log->Debug("AdapterOPCUA id-{} : WriteData START", config.id_adapter);

		try
		{
			for (auto& it_data : data)
			{
				update_last_data(it_data);
				update_write_respone();
				{
					UA_WriteResponse respone;
					respone = UA_Client_Service_write(_client, _write_request);
					if (respone.responseHeader.serviceResult != UA_STATUSCODE_GOOD)
					{
						UA_WriteResponse_clear(&respone);
						UA_SecureChannelState _state_channel;
						UA_SessionState _state_session;
						UA_StatusCode _state_connect;
						UA_Client_getState(_client, &_state_channel, &_state_session, &_state_connect);
						if (_state_channel == UA_SecureChannelState::UA_SECURECHANNELSTATE_CLOSED || _state_channel != UA_STATUSCODE_GOOD)
						{
							syserror = _state_connect;
							current_status.store(atech::common::Status::ERROR_CONNECTING);
							//destroy();
							throw 1;
						}
						throw 2;
					}
					UA_WriteResponse_clear(&respone);
				}
			};	
		}
		catch (int& e)
		{
			log->Critical("AdapterOPCUA id-{} : Error ReadData: error: {} syserror: {}", config.id_adapter, e, syserror);
			ResultReqest result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterOPCUA id-{} : Error ReadData: error: {} syserror: {}", config.id_adapter, 0, 0);
			ResultReqest result = ResultReqest::ERR;
		}

		return result;
	};

	void AdapterOPCUA::update_last_data(const SetTags& data)
	{
		for (auto& it : config.vec_link_tags)
		{
			Value& last = last_data.map_data[it.target];
			if (data.map_data.count(it.source) == 0) continue;
			const Value& current = data.map_data.at(it.source);
			switch (it.target.type)
			{
			case TypeValue::INT:
				update_value<int>(last, current, it);
				break;
			case TypeValue::FLOAT:
				update_value<float>(last, current, it);
				break;
			case TypeValue::DOUBLE:
				update_value<double>(last, current, it);
				break;
			case TypeValue::CHAR:
				update_value<char>(last, current, it);
				break;
			case TypeValue::STRING:
				update_value<std::string>(last, current, it);
				break;
			default:
				break;
			}
		}

		last_data.time_source = data.time_source;
	}

	void AdapterOPCUA::update_write_respone()
	{
		int counter = 0;
		for (auto& it : config.vec_link_tags)
		{
			const Value& value = last_data.map_data[it.target];
			UA_WriteValue* wv = _write_request.nodesToWrite + counter;
			wv->value.status = convert_ScadaStatus_toUAStatus(value.quality);
			wv->value.serverTimestamp = TimeConverter::GetTime_LLmcs()*10LL + UA_DATETIME_UNIX_EPOCH;
			wv->value.sourceTimestamp = value.time*10LL + UA_DATETIME_UNIX_EPOCH;

			switch (it.target.type)
			{
			case TypeValue::INT:
				*(int*)wv->value.value.data = std::get<int>(value.value);
				break;
			case TypeValue::FLOAT:
				*(float*)wv->value.value.data = std::get<float>(value.value);
				break;
			case TypeValue::DOUBLE:
				*(double*)wv->value.value.data = std::get<double>(value.value);
				break;
			case TypeValue::CHAR:
				*(char*)wv->value.value.data = std::get<char>(value.value);
				break;
			case TypeValue::STRING:
				UA_String_delete((UA_String*)wv->value.value.data);
				UA_String str = UA_String_fromChars(std::get<std::string>(value.value).c_str());
				wv->value.value.data = &str;
				break;
			default:
				break;
			} 
			counter++;
		}
	}

	ResultReqest AdapterOPCUA::init_last_data()
	{
		ResultReqest result{ ResultReqest::OK };

		last_data.map_data.clear();
		for (auto it : this->config.vec_link_tags)
		{
			last_data.map_data[it.target] = init_value(it.target.type);
		};

		return result;
	}

	ResultReqest AdapterOPCUA::init_write_request()
	{
		ResultReqest result = ResultReqest::OK;

		try 
		{
			if (this->config.vec_link_tags.empty()) return result;

			UA_WriteValue* wv = (UA_WriteValue*)UA_Array_new(config.vec_link_tags.size(), &UA_TYPES[UA_TYPES_WRITEVALUE]);

			size_t counter = 0;
			for (auto it : config.vec_link_tags)
			{
				UA_WriteValue* pointer = wv + counter;

				pointer->nodeId = tag_to_nodeid(it.target);
				pointer->attributeId = UA_ATTRIBUTEID_VALUE;
				if (it.target.is_array) pointer->indexRange = UA_STRING_ALLOC(std::to_string(it.target.offset).c_str());
				pointer->value.hasValue = UA_TRUE;
				pointer->value.hasStatus = UA_TRUE;
				//pointer->value.hasSourceTimestamp = true;	// €блоко раздора 
				pointer->value.status = UA_STATUSCODE_BAD;

				UA_Variant* variant = UA_Variant_new();
				UA_Variant_init(variant);
				init_UA_Variant(variant, it.target);
				variant->storageType = UA_VARIANT_DATA_NODELETE;
				pointer->value.value = *variant;
				counter++;
			}
			_write_request.nodesToWriteSize = config.vec_link_tags.size();
			_write_request.nodesToWrite = wv;
		}
		catch (int& e)
		{
			log->Warning("AdapterOPCUA id-{}: error init_write_request: error: {}, syserror: {} ", config.id_adapter, e, 0);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterOPCUA id-{}: error init_write_request: error: {}, syserror: {} ", config.id_adapter, 0, 0);
			result = ResultReqest::ERR;
		}

		return result;
	}

	ResultReqest AdapterOPCUA::init_UA_Variant(UA_Variant* variant, InfoTag& tag)
	{
		ResultReqest result {ResultReqest::OK};

		int v_i = 0;
		float v_f = 0.;
		double v_d = 0;
		char v_c = 0;
		UA_String str;
		str.data = nullptr;
		str.length = 0;

		if (tag.is_array)
		{
			switch (tag.type)
			{
			case TypeValue::INT:
				UA_Variant_setArrayCopy(variant, &v_i, 1, convert_to_pUA_DataType(tag.type));
				break;
			case TypeValue::FLOAT:
				UA_Variant_setArrayCopy(variant, &v_f, 1, convert_to_pUA_DataType(tag.type));
				break;
			case TypeValue::DOUBLE:
				UA_Variant_setArrayCopy(variant, &v_d, 1, convert_to_pUA_DataType(tag.type));
				break;
			case TypeValue::CHAR:
				UA_Variant_setArrayCopy(variant, &v_c, 1, convert_to_pUA_DataType(tag.type));
				break;
			case TypeValue::STRING:
				UA_Variant_setArrayCopy(variant, &str, 1, convert_to_pUA_DataType(tag.type));
				break;
			default:
				break;
			}
			
		}
		else
		{
			switch (tag.type)
			{
			case TypeValue::INT:
				UA_Variant_setScalarCopy(variant, &v_i, convert_to_pUA_DataType(tag.type));
				break;
			case TypeValue::FLOAT:
				UA_Variant_setScalarCopy(variant, &v_f, convert_to_pUA_DataType(tag.type));
				break;
			case TypeValue::DOUBLE:
				UA_Variant_setScalarCopy(variant, &v_d, convert_to_pUA_DataType(tag.type));
				break;
			case TypeValue::CHAR:
				UA_Variant_setScalarCopy(variant, &v_c, convert_to_pUA_DataType(tag.type));
				break;
			case TypeValue::STRING:
				UA_Variant_setScalarCopy(variant, &str, convert_to_pUA_DataType(tag.type));
				break;
			default:
				break;
			}
		}

		return result;
	}



	int AdapterOPCUA::demask(const int& value, int mask_source, const int& value_target, const int& mask_target)
	{
		int val_out = value & mask_source;
		if (mask_target > mask_source)
		{
			while (mask_target != mask_source)
			{
				mask_source <<= 1;
				val_out <<= 1;
			}
		}
		else
		{
			while (mask_target != mask_source)
			{
				mask_source >>= 1;
				val_out >>= 1;
			}
		}

		val_out = value_target & (~mask_target) | val_out;
		return val_out;
	}

	char AdapterOPCUA::convert_UAStatus_toScadaStatus(const UA_StatusCode& status_opc)
	{
		if (status_opc == UA_STATUSCODE_GOOD)
		{
			return 0;
		}

		return -1;
	};

	UA_StatusCode AdapterOPCUA::convert_ScadaStatus_toUAStatus(const char& status_scada)
	{
		if (status_scada == 0)
		{
			return UA_STATUSCODE_GOOD;
		}
		return UA_STATUSCODE_BAD;
	};

	UA_NodeId AdapterOPCUA::tag_to_nodeid(const InfoTag& tag)
	{
		if (tag.tag.empty()) 
		{ 
			return UA_NODEID_NUMERIC(config.namespaceindex, tag.id_tag); 
		}
		else 
		{ 
			return UA_NODEID_STRING_ALLOC(config.namespaceindex, tag.tag.c_str()); 
		};
	}

	UA_DataType AdapterOPCUA::convert_to_UA_DataType(TypeValue& type_value)
	{
		if (type_value == TypeValue::INT) return UA_TYPES[UA_TYPES_INT32];
		if (type_value == TypeValue::FLOAT) return UA_TYPES[UA_TYPES_FLOAT];
		if (type_value == TypeValue::DOUBLE) return UA_TYPES[UA_TYPES_DOUBLE];
		if (type_value == TypeValue::CHAR) return UA_TYPES[UA_TYPES_BYTE];
		if (type_value == TypeValue::STRING) return UA_TYPES[UA_TYPES_STRING];

		return UA_TYPES[UA_TYPES_BYTE];
	}

	const UA_DataType* AdapterOPCUA::convert_to_pUA_DataType(TypeValue& type_value)
	{
		if (type_value == TypeValue::INT) return &UA_TYPES[UA_TYPES_INT32];
		if (type_value == TypeValue::FLOAT) return &UA_TYPES[UA_TYPES_FLOAT];
		if (type_value == TypeValue::DOUBLE) return &UA_TYPES[UA_TYPES_DOUBLE];
		if (type_value == TypeValue::CHAR) return &UA_TYPES[UA_TYPES_BYTE];
		if (type_value == TypeValue::STRING) return &UA_TYPES[UA_TYPES_STRING];

		return &UA_TYPES[UA_TYPES_BYTE];
	}

	std::string AdapterOPCUA::to_string(const SecurityPolicy& value)
	{
		std::string str;
		switch (value)
		{
		case SecurityPolicy::None:
			str = "None";
			break;
		case SecurityPolicy::Basic256:
			str = "Basic256";
			break;
		case SecurityPolicy::Basic256Sha256:
			str = "Basic256Sha256";
			break;
		case SecurityPolicy::Basic128Rsa15:
			str = "Basic128Rsa15";
			break;
		case SecurityPolicy::Aes128Sha256RsaOaep:
			str = "Aes128Sha256RsaOaep";
			break;
		case SecurityPolicy::Aes256Sha256RsaPss:
			str = "Aes256Sha256RsaPss";
			break;
		default:
			break;
		}

		return str;
	};

	std::string AdapterOPCUA::to_string(const SecurityMode& value)
	{
		std::string str;
		switch (value)
		{
		case SecurityMode::None:
			str = "None";
			break;
		case SecurityMode::Sign:
			str = "Sign";
			break;
		case SecurityMode::SignEndEncrypt:
			str = "SignEndEncrypt";
			break;
		default:
			break;
		}

		return str;
	};

	std::string AdapterOPCUA::to_string(const Authentication& value)
	{
		std::string str;
		switch (value)
		{
		case Authentication::Anonymous:
			str = "Anonymous";
			break;
		case Authentication::Certificate:
			str = "Certificate";
			break;
		case Authentication::User:
			str = "User";
			break;
		default:
			break;
		}

		return str;
	};


	
	TypeAdapter AdapterOPCUA::GetTypeAdapter() { return TypeAdapter::OPC_UA; };

	std::shared_ptr<IAnswer> AdapterOPCUA::GetInfo(ParamInfoAdapter param) { return nullptr; };

	uint32_t AdapterOPCUA::GetId()
	{
		return config.id_adapter;
	}	 

	ResultReqest AdapterOPCUA::GetStatus(std::deque<std::pair<uint32_t, atech::common::Status>>& st, uint32_t id)
	{
		st.push_back({ this->config.id_adapter, current_status.load() });
		return ResultReqest::OK;
	}

	ResultReqest AdapterOPCUA::Start(std::deque<std::pair<uint32_t, atech::common::Status>>& st, uint32_t id)
	{
		ResultReqest result{ ResultReqest::OK };

		if (current_status.load() == atech::common::Status::STOP ||
			current_status.load() == atech::common::Status::OK)
		{
			current_status.store(atech::common::Status::OK);
			result = ResultReqest::OK;
		}
		else
		{
			result = ResultReqest::ERR;
		}

		st.push_back({ config.id_adapter, current_status.load() });
		return result;
	}

	ResultReqest AdapterOPCUA::Stop(std::deque<std::pair<uint32_t, atech::common::Status>>& st, uint32_t id)
	{
		ResultReqest result{ ResultReqest::OK };

		if (current_status.load() == atech::common::Status::STOP ||
			current_status.load() == atech::common::Status::OK)
		{
			current_status.store(atech::common::Status::STOP);
			result = ResultReqest::OK;
		}
		else
		{
			result = ResultReqest::ERR;
		}

		st.push_back({ config.id_adapter, current_status.load() });
		return result;
	}

	ResultReqest AdapterOPCUA::ReInit(std::deque<std::pair<uint32_t, atech::common::Status>>& st, uint32_t id)
	{
		ResultReqest result{ ResultReqest::OK };
		const std::lock_guard<std::mutex> lock_init(mutex_init);
		this->destroy();
		result = this->init_adapter();
		st.push_back({ config.id_adapter, current_status.load() });
		return result;
	}

}
																				  