#pragma once
#include <filesystem>

#include <LoggerScada.hpp>
#include <Module_IO/UnitTransfer/adapters/Adapters.hpp>
#include <structs/TimeConverter.hpp>

#include <open62541/client_config_default.h>
#include <open62541/client_highlevel.h>
#include <open62541/plugin/log_stdout.h>
#include <open62541/plugin/securitypolicy.h>
#include <open62541/plugin/securitypolicy_default.h>
#include <open62541/plugin/pki_default.h>
#include <stdlib.h>



namespace scada_ate::gate::adapter::opc
{

	enum class SecurityMode
	{
		None,
		Sign,
		SignEndEncrypt
	};

	enum class SecurityPolicy
	{
		None,
		Basic256,
		Basic256Sha256,
		Basic128Rsa15,
		Aes128Sha256RsaOaep,
		Aes256Sha256RsaPss
	};

	enum class Authentication
	{
		Anonymous,
		User,
		Certificate
	};

	struct ConfigAdapterOPCUA : public IConfigAdapter
	{
		std::string endpoint_url;
		SecurityMode security_mode;
		SecurityPolicy security_policy;
		Authentication authentication;
		std::string user_name;
		std::string password;
	};

	struct AnswerOPCUAHeaderData : public IAnswer
	{

	};

	static std::string path_trusts_files = "trust_list_opc";

	class AdapterOPCUA : public IAdapter
	{
		std::mutex mutex_init;
		std::atomic<StatusAdapter> current_status = StatusAdapter::Null; /// переменная статуса адаптера 
		std::shared_ptr<LoggerSpaceScada::ILoggerScada> log; /// логгер
		ConfigAdapterOPCUA config;

		UA_ByteString certificate = UA_STRING_NULL;
		UA_ByteString privateKey = UA_STRING_NULL;
		std::string path_certificate = "certificate/client_cert.der";
		std::string path_privatekey = "certificate/client_key.der";
		size_t trustListSize = 0;
		UA_ByteString* trustList = nullptr;

		ResultReqest take_trust_list();
		UA_ByteString loadFile(const char* const path);
		ResultReqest validation_security();
		ResultReqest take_certificate();

		void log_info_config();
		std::string& to_string(const SecurityPolicy& value);
		std::string& to_string(const SecurityMode& value);
		std::string& to_string(const Authentication& value);

		public:

		AdapterOPCUA(std::shared_ptr<IConfigAdapter> config);
		~AdapterOPCUA();
		ResultReqest InitAdapter() override;
		ResultReqest ReadData(std::deque<SetTags>** data) override;
		ResultReqest WriteData(const std::deque<SetTags>& data) override;
		TypeAdapter GetTypeAdapter() override;
		StatusAdapter GetStatusAdapter() override;
		std::shared_ptr<IAnswer> GetInfoAdapter(ParamInfoAdapter param) override;
	};
}


