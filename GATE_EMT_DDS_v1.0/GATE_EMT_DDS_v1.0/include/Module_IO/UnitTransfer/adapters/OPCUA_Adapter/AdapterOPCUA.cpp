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

		log = LoggerSpaceScada::GetLoggerScada(LoggerSpaceScada::TypeLogger::SPDLOG);
	};

	AdapterOPCUA::~AdapterOPCUA() {};

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



			

		}
		catch (int& e)
		{
			
		}
		catch (...)
		{
		}
	}

	UA_ByteString AdapterOPCUA::loadFile(const char* const path) 
	{
		UA_ByteString fileContents = UA_STRING_NULL;

		FILE* fp = fopen(path, "rb");
		if (!fp) {
			errno = 0;
			return fileContents;
		}

		fseek(fp, 0, SEEK_END);
		fileContents.length = (size_t)ftell(fp);
		fileContents.data = (UA_Byte*)UA_malloc(fileContents.length * sizeof(UA_Byte));
		if (fileContents.data) {
			fseek(fp, 0, SEEK_SET);
			size_t read = fread(fileContents.data, sizeof(UA_Byte), fileContents.length, fp);
			if (read != fileContents.length)
				UA_ByteString_clear(&fileContents);
		}
		else {
			fileContents.length = 0;
		}
		fclose(fp);

		return fileContents;
	}

	void AdapterOPCUA::log_info_config()
	{
		log->Debug("AdapterOPCUA id-{}: Config: Endpoint_url : {}, SecurityMode: {}, SecurityPolicy: {}, Authentication: {}, User: {}, Password: {})",config.id_adapter, 
			config.endpoint_url, to_string(config.security_mode), to_string(config.security_policy), to_string(config.authentication), config.user_name, config.password);
	}

	std::string& AdapterOPCUA::to_string(const SecurityPolicy& value)
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

	std::string& AdapterOPCUA::to_string(const SecurityMode& value)
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

	std::string& AdapterOPCUA::to_string(const Authentication& value)
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
			trustList = (UA_ByteString*)UA_alloca(sizeof(UA_ByteString) * trustListSize);
			if (trustList == nullptr) throw 2;

			size_t count_iter = 0;
			for (auto& iter : _list_files)
			{
				trustList[count_iter] = loadFile(iter.string().c_str());
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

		return;
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
			certificate = loadFile(path_certificate.c_str());
			privateKey = loadFile(path_privatekey.c_str());

			if (certificate.length == 0 || certificate.data == nullptr) throw 1;
			if (privateKey.length == 0 || privateKey.data == nullptr) throw 2;
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




	ResultReqest AdapterOPCUA::ReadData(std::deque<SetTags>** data) { return ResultReqest::IGNOR; };
	ResultReqest AdapterOPCUA::WriteData(const std::deque<SetTags>& data) { return ResultReqest::IGNOR; };
	TypeAdapter AdapterOPCUA::GetTypeAdapter() { return TypeAdapter::OPC_UA; };
	StatusAdapter AdapterOPCUA::GetStatusAdapter() { return StatusAdapter::Null; };
	std::shared_ptr<IAnswer> AdapterOPCUA::GetInfoAdapter(ParamInfoAdapter param) { return nullptr; };

};																					  