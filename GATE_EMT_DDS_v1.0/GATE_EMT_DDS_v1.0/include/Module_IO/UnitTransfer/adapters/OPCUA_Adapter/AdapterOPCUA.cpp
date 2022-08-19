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

		log = LoggerSpaceScada::GetLoggerScada(LoggerSpaceScada::TypeLogger::SPDLOG);
	};

	AdapterOPCUA::~AdapterOPCUA() 
	{
		destroy();
	};

	ResultReqest AdapterOPCUA::InitAdapter()
	{
		ResultReqest res = ResultReqest::OK;
		unsigned int result = 0;
		unsigned long sys_error = 0;

		const std::lock_guard<std::mutex> lock_init(mutex_init);

		StatusAdapter status = current_status.load(std::memory_order::memory_order_relaxed);
		if (status == StatusAdapter::INITIALIZATION || status == StatusAdapter::OK)
		{
			ResultReqest res = ResultReqest::IGNOR;
			return res;
		}

		try
		{
			if (config.type_adapter != TypeAdapter::OPC_UA) throw 1;
			log->Debug("AdapterOPCUA id-{}: Init START", config.id_adapter);

			log_info_config();

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

			init_deque();
			current_status.store(StatusAdapter::OK);
			log->Debug("AdapterOPCUA id-{}: Init DONE", config.id_adapter);
		}
		catch (int& e)
		{
			log->Critical("AdapterOPCUA id-{}: Error initiation", config.id_adapter, e, 0);
			res = ResultReqest::ERR;
			current_status.store(StatusAdapter::ERROR_INIT);
		}
		catch (...)
		{
			log->Critical("AdapterOPCUA id-{}: Error initiation", config.id_adapter, 0, 0);
			res = ResultReqest::ERR;
			current_status.store(StatusAdapter::ERROR_INIT);
		}

		return res;
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
				if (iter.first.tag.empty()) { value.nodeId = UA_NODEID_NUMERIC(config.namespaceindex, iter.first.id_tag); }
				else { value.nodeId = UA_NODEID_STRING(config.namespaceindex,  (char*)iter.first.tag.c_str()); };
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
		return result;
	};

	UA_DataType AdapterOPCUA::convert_to_UA_DataType(TypeValue& type_value)
	{
		if (type_value == TypeValue::INT) return UA_TYPES[UA_TYPES_INT32];
		if (type_value == TypeValue::FLOAT) return UA_TYPES[UA_TYPES_FLOAT];
		if (type_value == TypeValue::DOUBLE) return UA_TYPES[UA_TYPES_DOUBLE];
		if (type_value == TypeValue::CHAR) return UA_TYPES[UA_TYPES_BYTE];
		if (type_value == TypeValue::STRING) return UA_TYPES[UA_TYPES_STRING];

		return UA_TYPES[UA_TYPES_BYTE];
	}

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
		UA_ByteString_clear(&certificate);
		UA_ByteString_clear(&privateKey);
		for (size_t i = 0; i < trustListSize; i++)
		{
			UA_ByteString_clear(&trustList[i]);
		}

		UA_ReadRequest_clear(&_read_request);
		UA_WriteRequest_clear(&_write_request);
		UA_Client_delete(_client);
	}


	ResultReqest AdapterOPCUA::ReadData(std::deque<SetTags>** data) 
	{
		UA_ReadResponse _respone;
		auto data_opc = this->data.begin();

		if (current_status.load(std::memory_order_relaxed) != StatusAdapter::OK)
		{
			log->Debug("AdapterOPCUA id-{} : ReadData IGNOR", config.id_adapter);
			return ResultReqest::IGNOR;
		}
		log->Debug("AdapterOPCUA id-{} : ReadData START", config.id_adapter);

		_respone = UA_Client_Service_read(_client, _read_request);
		if (_respone.responseHeader.serviceResult != UA_STATUSCODE_GOOD)
		{
			
		};

		for (auto& itag : config.vec_tags_source)
		{
			auto index_tag = map_index_request_read.find(itag);
			UA_DataValue & value = _respone.results[index_tag->second];
			if (validation_data(itag, value) != ResultReqest::OK)
			{
				data_opc->map_data[itag] = init_value(itag.type);
				continue;
			};

			data_opc->map_data[itag] = get_opc_value(itag, value);
			itag.status = StatusTag::OK;
		}
		UA_ReadResponse_clear(&_respone);
		*data = &this->data;
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

	char AdapterOPCUA::convert_UAStatus_toScadaStatus(const UA_StatusCode& status_opc)
	{
		return (char)status_opc;
	};

	std::variant<int, float, double, char, std::string> AdapterOPCUA::get_value(void* ptr_value, const UA_Int32& type_opc, size_t offset)
	{
		std::variant<int, float, double, char, std::string> value;

		switch (type_opc)
		{
		case  UA_DataTypeKind::UA_DATATYPEKIND_INT16:
			value = *(int*)((uint16_t*)ptr_value + offset);
			break;
		case  UA_DataTypeKind::UA_DATATYPEKIND_UINT16:
			value = *(int*)((uint16_t*)ptr_value + offset);
			break;
		case  UA_DataTypeKind::UA_DATATYPEKIND_INT32:
			value = *(int*)((uint16_t*)ptr_value + offset);
			break;
		case  UA_DataTypeKind::UA_DATATYPEKIND_UINT32:
			value = *(int*)((uint16_t*)ptr_value + offset);
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

		return value;
	};

	ResultReqest AdapterOPCUA::WriteData(const std::deque<SetTags>& data) { return ResultReqest::IGNOR; };
	TypeAdapter AdapterOPCUA::GetTypeAdapter() { return TypeAdapter::OPC_UA; };
	StatusAdapter AdapterOPCUA::GetStatusAdapter() { return StatusAdapter::Null; };
	std::shared_ptr<IAnswer> AdapterOPCUA::GetInfoAdapter(ParamInfoAdapter param) { return nullptr; };
}
																				  