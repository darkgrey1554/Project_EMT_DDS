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
#include <fstream>;



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
		int namespaceindex;
	};

	struct AnswerOPCUAHeaderData : public IAnswer
	{

	};

	struct OPCTagHash {
		std::size_t operator()(const InfoTag& k) const
		{
			std::size_t h1 = std::hash<std::string>{}(k.tag);
			std::size_t h2 = std::hash<int>{}(k.id_tag);
			return h1 ^ (h2 << 1);
		}
	};

	struct OPCTagEqual {
		bool operator()(const InfoTag& lhs, const InfoTag& rhs) const
		{
			if (lhs.id_tag != rhs.id_tag) return false;
			if (lhs.tag != rhs.tag) return false;
			return true;
		}
	};

	static std::string path_trusts_files = "trust_list_opc";

	class AdapterOPCUA : public IAdapter
	{
		std::mutex mutex_init;
		std::atomic<StatusAdapter> current_status = StatusAdapter::Null; /// ���������� ������� �������� 
		std::shared_ptr<LoggerSpaceScada::ILoggerScada> log; /// ������
		ConfigAdapterOPCUA config;
		SetTags last_data{};

		UA_ByteString certificate = UA_STRING_NULL;
		UA_ByteString privateKey = UA_STRING_NULL;
		std::string path_certificate = "certificate/client_cert.der";
		std::string path_privatekey = "certificate/client_key.der";
		size_t trustListSize = 0;
		UA_ByteString* trustList = nullptr;
		UA_ReadRequest _read_request;
		UA_WriteRequest _write_request;
		UA_Client* _client;
		std::unordered_map<InfoTag, size_t, OPCTagHash, OPCTagEqual> map_index_request_read;
		
		

		ResultReqest take_trust_list();
		ResultReqest take_certificate();
		UA_ByteString loadFile(const char* const path);
		ResultReqest validation_security();
		ResultReqest create_client();
		UA_MessageSecurityMode get_messege_security(SecurityMode& mode);
		UA_String get_security_policyUri(SecurityPolicy& mode);

		ResultReqest establishing_connection();

		ResultReqest create_request_to_read();
		ResultReqest create_request_to_write();
		UA_DataType convert_to_UA_DataType(TypeValue& type_value);
		const UA_DataType* convert_to_pUA_DataType(TypeValue& type_value);

		ResultReqest validation_data(InfoTag& tag, UA_DataValue& value);
		bool isequil_type(const TypeValue& type_scada,const UA_Int32& type_opc);
		void init_deque();
		Value init_value(const TypeValue& type);
		Value get_opc_value(InfoTag& tag, UA_DataValue& value_opc);
		std::variant<int, float, double, char, std::string> get_value(void* ptr_value,const UA_Int32& type_opc, size_t offset);
		char convert_UAStatus_toScadaStatus(const UA_StatusCode& status_opc);

		void init_last_data();
		ResultReqest init_write_request();
		UA_NodeId tag_to_nodeid(const InfoTag& tag);
		void* set_value(InfoTag& tag);


		void log_info_config();
		std::string to_string(const SecurityPolicy& value);
		std::string to_string(const SecurityMode& value);
		std::string to_string(const Authentication& value);

		void destroy();

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


