#pragma once
#include <filesystem>
#include <LoggerScada.h>
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
#include <set>



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
		size_t namespaceindex;
		size_t partition_size;
	};

	struct AnswerOPCUAHeaderData : public IAnswer
	{

	};


	struct OPC_UA_ReadRequest
	{
		UA_ReadRequest request;

		OPC_UA_ReadRequest()
		{
			UA_ReadRequest_init(&request);
		};

		~OPC_UA_ReadRequest()
		{
			UA_ReadRequest_clear(&request);
		};
	};

	struct OPC_UA_WriteRequest
	{
		UA_WriteRequest request;

		OPC_UA_WriteRequest()
		{
			UA_WriteRequest_init(&request);
		};

		~OPC_UA_WriteRequest()
		{
			UA_WriteRequest_clear(&request);
		};
	};

	struct OPC_UA_ReadResponse
	{
		UA_ReadResponse response;

		OPC_UA_ReadResponse()
		{
		};

		~OPC_UA_ReadResponse()
		{
			UA_ReadResponse_clear(&response);
		};
	};

	struct OPC_UA_WriteResponse
	{
		UA_WriteResponse response;

		OPC_UA_WriteResponse()
		{
		};

		~OPC_UA_WriteResponse()
		{
			UA_WriteResponse_clear(&response);
		};
	};

	static std::string path_trusts_files = "trust_list_opc";

	class AdapterOPCUA : public IAdapter
	{
		std::mutex mutex_init;
		std::atomic<atech::common::Status> current_status = atech::common::Status::NONE; 
		atech::logger::ILoggerScada_ptr log; 
		ConfigAdapterOPCUA config;
		SetTags last_data{};

		UA_ByteString certificate = UA_STRING_NULL;
		UA_ByteString privateKey = UA_STRING_NULL;
		std::string path_certificate = "certificate/client_cert.der";
		std::string path_privatekey = "certificate/client_key.der";
		size_t trustListSize = 0;
		UA_ByteString* trustList = nullptr;
		std::vector<std::unique_ptr<OPC_UA_ReadRequest>> _vector_request_to_read;
		std::vector<std::unique_ptr<OPC_UA_WriteRequest>> _vector_request_to_write;
		std::vector<std::vector<scada_ate::gate::adapter::InfoTag>> _v_tags_to_read{};
		std::vector<std::vector<scada_ate::gate::adapter::LinkTags>> _v_tags_to_write{};
		UA_Client* _client;

		ResultReqest take_trust_list();
		ResultReqest take_certificate();
		UA_ByteString loadFile(const char* path);
		ResultReqest validation_security();
		ResultReqest create_client();
		UA_MessageSecurityMode get_messege_security(SecurityMode& mode);
		UA_String get_security_policyUri(SecurityPolicy& mode);

		ResultReqest establishing_connection();

		template<typename TRequest, typename TRespone>
		inline ResultReqest request_to_opc_server(TRequest& request, TRespone& respone);

		template<typename T>
		ResultReqest fill_vector_request(std::vector<std::vector<T>>& target, std::vector<T>& source, UA_ReadResponse& respone, size_t& count_read);

		template<typename T>
		InfoTag& take_tags_target(std::vector<T>& vector, size_t offset);

		template<typename T> 
		ResultReqest take_actual_vector_tags(T& source, std::vector<T>& target);

		ResultReqest create_requests_to_write();
		ResultReqest create_requests_to_read();
		ResultReqest  build_vector_requests_to_read(std::vector<std::unique_ptr<OPC_UA_ReadRequest>>& targer, 
													std::vector< std::vector<scada_ate::gate::adapter::InfoTag>>& source);
		ResultReqest  build_vector_requests_to_write(std::vector<std::unique_ptr<OPC_UA_WriteRequest>>& targer,
													 std::vector<std::vector<scada_ate::gate::adapter::LinkTags>>& source);

		UA_DataType convert_to_UA_DataType(TypeValue& type_value);
		const UA_DataType* convert_to_pUA_DataType(TypeValue& type_value);

		ResultReqest update_outdata(UA_ReadResponse& respone, std::vector<scada_ate::gate::adapter::InfoTag>& tags);
		
		void registration_value(InfoTag& tag, UA_DataValue* source);
		template<typename T>
		void registration_value(UA_DataValue* source, ValueT<T>& target);

		ResultReqest validation_data(InfoTag& tag, UA_DataValue& value);
		bool isequil_type(const TypeValue& type_scada,const UA_Int32& type_opc);
		void init_deque();
		std::variant<int, float, double, char, std::string> get_value(void* ptr_value,const UA_Int32& type_opc, size_t offset);
		char convert_UAStatus_toScadaStatus(const UA_StatusCode& status_opc);
		UA_StatusCode convert_ScadaStatus_toUAStatus(const char& status_scada);
		ResultReqest init_UA_Variant(UA_Variant* variant, InfoTag& tag);
		void update_last_data(const SetTags& data);
		
		template<class T> void update_value(ValueT<T>& target, const ValueT<T>& source, const LinkTags& link);

		int demask(const int& value, int mask_source, const int& value_target, const int& mask_target);
		void update_write_respone(UA_WriteRequest* request, std::vector<LinkTags>& tags);

		template<typename T>
		void update_write_value(UA_WriteValue* target, ValueT<T>& source)
		{

			try
			{
				target->value.status = convert_ScadaStatus_toUAStatus(source.quality);
				target->value.serverTimestamp = TimeConverter::GetTime_LLmcs() * 10LL + UA_DATETIME_UNIX_EPOCH;
				target->value.sourceTimestamp = source.time * 10LL + UA_DATETIME_UNIX_EPOCH;

				if constexpr (std::is_same_v<T,std::string>)
				{
					if (target->value.value.data) UA_String_delete((UA_String*)target->value.value.data);
					UA_String str = UA_String_fromChars(source.value.c_str());
					target->value.value.data = &str;
				}
				else
				{
					switch (target->value.value.type->typeKind)
					{
					case UA_DataTypeKind::UA_DATATYPEKIND_INT16:
						*(int16_t*)target->value.value.data = source.value;
						break;
					case UA_DataTypeKind::UA_DATATYPEKIND_UINT16:
						*(uint16_t*)target->value.value.data = source.value;
						break;
					case UA_DataTypeKind::UA_DATATYPEKIND_INT32:
						*(int32_t*)target->value.value.data = source.value;
						break;
					case UA_DataTypeKind::UA_DATATYPEKIND_UINT32:
						*(uint32_t*)target->value.value.data = source.value;
						break;
					case UA_DataTypeKind::UA_DATATYPEKIND_FLOAT:
						*(float*)target->value.value.data = source.value;
						break;
					case UA_DataTypeKind::UA_DATATYPEKIND_DOUBLE:
						*(double*)target->value.value.data = source.value;
						break;
					case UA_DataTypeKind::UA_DATATYPEKIND_BYTE:
						*(char*)target->value.value.data = source.value;
						break;
					case UA_DataTypeKind::UA_DATATYPEKIND_SBYTE:
						*(char*)target->value.value.data = source.value;
						break;
					}
				}
			}
			catch(...)
			{
				log->Critical("AdapterOPCUA id-{}: Error update_write_value: error: {}, syserror: {} ", config.id_adapter, 0, 0);
			}
			
			return;
		}

		ResultReqest init_last_data();
		ResultReqest init_write_request();
		UA_NodeId tag_to_nodeid(const InfoTag& tag);

		void log_info_config();
		std::string to_string(const SecurityPolicy& value);
		std::string to_string(const SecurityMode& value);
		std::string to_string(const Authentication& value);
		ResultReqest init_adapter();
		void destroy();

		public:

		AdapterOPCUA(std::shared_ptr<IConfigAdapter> config);
		~AdapterOPCUA();
		ResultReqest InitAdapter() override;
		ResultReqest ReadData(std::deque<SetTags>** data) override;
		ResultReqest WriteData(const std::deque<SetTags>& data) override;
		TypeAdapter GetTypeAdapter() override;
		uint32_t GetId() override;
		ResultReqest Start(std::deque<std::pair<uint32_t, atech::common::Status>>& st, uint32_t id = 0) override;
		ResultReqest Stop(std::deque<std::pair<uint32_t, atech::common::Status>>& st, uint32_t id = 0) override;
		ResultReqest ReInit(std::deque<std::pair<uint32_t, atech::common::Status>>& st, uint32_t id = 0) override;
		ResultReqest GetStatus(std::deque<std::pair<uint32_t, atech::common::Status>>& st, uint32_t id = 0) override;
		std::shared_ptr<IAnswer> GetInfo(ParamInfoAdapter param) override;
	};

	template<typename T>
	InfoTag& AdapterOPCUA::take_tags_target(std::vector<T>& vector, size_t offset)
	{
		if constexpr (std::is_same_v<T, InfoTag>)
			return vector[offset];
		else
			return vector[offset].target;
	}

	template<typename T>
	void AdapterOPCUA::registration_value(UA_DataValue * source, ValueT<T>& target)
	{
		if (source == nullptr)
		{
			target.time = 0;
			target.quality = 0;
			target.value = {};
			return;
		}

		if (source->hasSourceTimestamp)
		{
			target.time = source->sourceTimestamp - UA_DATETIME_UNIX_EPOCH;
		}
		else
		{
			target.time = 0;
		}

		if (source->hasStatus == true)
		{
			target.quality = convert_UAStatus_toScadaStatus(source->status);
		}
		else
		{
			target.quality = 0;
		}

		if constexpr (std::is_same_v<T, std::string>)
		{
			if (source->value.type->typeKind == UA_DataTypeKind::UA_DATATYPEKIND_STRING)
			{
				target.value = std::string((const char*)((UA_String*)source->value.data)->data, ((UA_String*)source->value.data)->length);
			}
			else
			{
				target.value = {};
			}
		}
		else
		{
			switch (source->value.type->typeKind)
			{
			case  UA_DataTypeKind::UA_DATATYPEKIND_INT16:
				target.value = (T)*((int16_t*)source->value.data);
				break;
			case  UA_DataTypeKind::UA_DATATYPEKIND_UINT16:
				target.value = (T)*((uint16_t*)source->value.data);
				break;
			case  UA_DataTypeKind::UA_DATATYPEKIND_INT32:
				target.value = (T)*((int32_t*)source->value.data);
				break;
			case  UA_DataTypeKind::UA_DATATYPEKIND_UINT32:
				target.value = (T)*((uint32_t*)source->value.data);
				break;
			case  UA_DataTypeKind::UA_DATATYPEKIND_FLOAT:
				target.value = *((T*)source->value.data);
				break;
			case  UA_DataTypeKind::UA_DATATYPEKIND_DOUBLE:
				target.value = *((T*)source->value.data);
				break;
			case  UA_DataTypeKind::UA_DATATYPEKIND_BYTE:
				target.value = *(T*)((uint8_t*)source->value.data);
				break;
			case  UA_DataTypeKind::UA_DATATYPEKIND_SBYTE:
				target.value = *(T*)((int8_t*)source->value.data);
				break;
			}
		}

		return;
	}

	template<typename T>
	ResultReqest AdapterOPCUA::fill_vector_request(std::vector<std::vector<T>>& target, std::vector<T>& source, UA_ReadResponse& respone, size_t& count_read)
	{
		ResultReqest result{ ResultReqest::OK };
		if (target.empty()) target.push_back({});
		std::vector<T>* vector = &target.back();
		try
		{
			for (int i = 0; i < respone.resultsSize; i++)
			{
				UA_DataValue& value = respone.results[i];
				T& tag = source[count_read * config.partition_size + i];
				if constexpr (std::is_same_v<T, InfoTag>)
				{
					if (validation_data(tag, value) == ResultReqest::OK)
					{
						vector->push_back(tag);
						if (vector->size() >= config.partition_size)
						{
							target.push_back({});
							vector = &target.back();
						}
					}
				}
				else if (std::is_same_v<T, LinkTags>)
				{
					if (validation_data(tag.target, value) == ResultReqest::OK)
					{
						vector->push_back(tag);
						if (vector->size() >= config.partition_size)
						{
							target.push_back({});
							vector = &target.back();
						}
					}
				}
				else {};
				
			}
		}
		catch (int& e)
		{
			log->Critical("AdapterOPCUA id-{}: Error fill_vector_request: error: {}, syserror: {} ", config.id_adapter, e, 0);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterOPCUA id-{}: Error fill_vector_request: error: {}, syserror: {} ", config.id_adapter, 0, 0);
			result = ResultReqest::ERR;
		}

		return result;
	}

	template<typename TRequest, typename TRespone>
	inline ResultReqest AdapterOPCUA::request_to_opc_server(TRequest& request, TRespone& response)
	{
		uint32_t syserror;
		ResultReqest result{ ResultReqest::OK };

		try
		{
			if constexpr (std::is_same_v<TRequest, UA_ReadRequest>)
			{
				response = UA_Client_Service_read(_client, request);
			}
			else if (std::is_same_v<TRequest, UA_WriteRequest>)
			{
				response = UA_Client_Service_write(_client, request);
			}
			else {}
			
			if (response.responseHeader.serviceResult != UA_STATUSCODE_GOOD)
			{
				UA_SecureChannelState _state_channel;
				UA_SessionState _state_session;
				UA_StatusCode _state_connect;
				UA_Client_getState(_client, &_state_channel, &_state_session, &_state_connect);
				if (_state_channel == UA_SecureChannelState::UA_SECURECHANNELSTATE_CLOSED || _state_channel != UA_STATUSCODE_GOOD)
				{
					syserror = _state_connect;
					current_status.store(atech::common::Status::ERROR_CONNECTING);
					throw 1;
				}
				throw 2;
			};
		}
		catch (int& e)
		{
			log->Critical("AdapterOPCUA id-{} : Error request_to_opc_server: error: {} syserror: {}", config.id_adapter, e, syserror);
			ResultReqest result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterOPCUA id-{} : Error request_to_opc_server: error: {} syserror: {}", config.id_adapter, 0, syserror);
			ResultReqest result = ResultReqest::ERR;
		}

		return result;
	}

	template<typename T>
	ResultReqest AdapterOPCUA::take_actual_vector_tags(T& source, std::vector<T>& target)
	{
		if (source.empty())
			return ResultReqest::OK;

		ResultReqest result{};
		size_t full_size_read = source.size();
		size_t counter_read = 0;
		size_t size_window = config.partition_size;
		uint32_t syserror = 0;

		try
		{
			target.clear();
			target.push_back({});

			while (full_size_read > counter_read * size_window)
			{
				std::unique_ptr ptr_request = std::make_unique<OPC_UA_ReadRequest>();

				size_t current_size_read = full_size_read - counter_read * size_window > size_window ?
					size_window : full_size_read - counter_read * size_window;

				ptr_request->request.nodesToReadSize = current_size_read;
				ptr_request->request.nodesToRead = (UA_ReadValueId*)UA_Array_new(current_size_read, &UA_TYPES[UA_TYPES_READVALUEID]);


				for (int i = 0; i < current_size_read; i++)
				{
					UA_ReadValueId& value = ptr_request->request.nodesToRead[i];
					InfoTag& tag = take_tags_target(source, i + (counter_read - 1) * size_window);;
					UA_ReadValueId_init(&value);
					value.nodeId = tag_to_nodeid(tag);
					value.attributeId = UA_ATTRIBUTEID_VALUE;
					if (tag.is_array) value.indexRange = UA_STRING_ALLOC(std::to_string(tag.offset).c_str());
				}


				for (int i = 0;; i++)
				{
					std::unique_ptr ptr_response = std::make_unique<OPC_UA_ReadResponse>();
					if (request_to_opc_server(ptr_request->request, ptr_response->response) != ResultReqest::OK)
					{
						if (i >= 2) throw 1;
						continue;
					}

					fill_vector_request(target, source, ptr_response->response, counter_read);
				}

				counter_read++;
			}
		}
		catch (int& e)
		{
			log->Warning("AdapterOPCUA id-{}: Error creat_request_to_read: error: {}, syserror: {} ", config.id_adapter, e, 0);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Warning("AdapterOPCUA id-{}: Error creat_request_to_read: error: {}, syserror: {} ", config.id_adapter, 0, 0);
			result = ResultReqest::ERR;
		}

		return result;
	}

	template<class T> void AdapterOPCUA::update_value(ValueT<T>& target, const ValueT<T>& source, const LinkTags& link)
	{
		T& val_last = target.value;
		T val_current{};

		if constexpr (std::is_same_v<int, T>)
		{
			if (link.target.mask != 0)
			{
				val_last = target.value;
				val_current = demask(source.value, link.source.mask, val_last, link.target.mask);
			}
			else
			{
				val_current = source.value;
			}
		}
		else
		{
			val_current = source.value;
		}

		if (link.type_registration == TypeRegistration::RECIVE)
		{
			target.value = val_current;
			target.quality = source.quality;
			target.time = source.time;
			return;
		}

		if (link.type_registration == TypeRegistration::UPDATE)
		{
			if (val_last == val_current && target.quality == source.quality) return;
			target.value = val_current;
			target.quality = source.quality;
			target.time = source.time;
			return;
		}

		if (link.type_registration == TypeRegistration::DELTA)
		{
			if constexpr (!std::is_same_v<T, std::string>)
			{
				if (abs(val_last - val_current) > link.delta || target.quality != source.quality)
				{
					target.value = val_current;
					target.quality = source.quality;
					target.time = source.time;
					return;
				}
			}
			else
			{
				return;
			}
		}

		return;
	};
	
}


