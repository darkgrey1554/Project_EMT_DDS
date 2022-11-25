#pragma once

#ifndef _MESSAGE_SERIALIZER_HPP_
#define _MESSAGE_SERIALIZER_HPP_


#include <string>
#include <fstream>
#include "json.hpp"
#include "adapters.h"
#include "topics.h"
#include "LoggerConfig.h"
#include "ProcessInfo.h"
#include "Exception.hpp"






namespace nlohmann {
     	
    template <typename T>
    struct adl_serializer<std::unique_ptr<T>> {
        static void to_json(ordered_json& j, const std::unique_ptr<T>& ptr) {
            if (ptr.get()) {
                j = *ptr;
            }
            else {
                j = nullptr;
            }
        }
        static void from_json(const ordered_json& j, std::unique_ptr<T>& ptr) {
            if (j.is_null()) {
                ptr == nullptr;
            }
            else {
                ptr = std::make_unique<T>(j.get<T>());
            }
        }
    };

    template <typename T> struct adl_serializer<std::shared_ptr<T>> {
        static void from_json(const ordered_json& j, std::shared_ptr<T>& ptr) {
            if (j.is_null()) {
                ptr == nullptr;
            }
            else {
                ptr = std::make_shared<T>(j.get<T>());
            }
        }
        static void to_json(ordered_json& j, const std::shared_ptr<T>& ptr) {
            if (ptr.get()) {
                j = *ptr;
            }
            else {
                j = nullptr;
            }
        }
    };

    template <> struct adl_serializer<atech::common::TypeSize> {
        static void to_json(ordered_json& message, const atech::common::TypeSize& data) {
            try {
                message["type_name"] = data.get_type_name();
                message["size"] = data.get_size();
            }
            catch (json::exception& e) {

                throw atech::common::Error("Error in to_json of TypeSize: " + (std::string)e.what());
            }
        }

        static void from_json(const ordered_json& message, atech::common::TypeSize& data) {
            try {
                data.set_type_name(message.at("type_name").get<std::string>());
                data.set_size(message.at("size").get<size_t>());
            }
            catch (json::exception& e) {

                throw atech::common::Error("Error in from_json of TypeSize: " + (std::string)e.what());
            }
        }
    };

    template <> struct adl_serializer<atech::common::DdsTypeSize> {
        static void to_json(ordered_json& message, const atech::common::DdsTypeSize& data) {
            try {
                message["type_name"] = data.get_type_name();
                std::vector<atech::common::TypeSize> inner_data = data.get_type_sizes();
                message["type_sizes"] = inner_data;
            }
            catch (json::exception& e) {

                throw atech::common::Error("Error in to_json of DdsTypeSize: " + (std::string)e.what());
            }
        }

        static void from_json(const ordered_json& message, atech::common::DdsTypeSize& data) {
            try {
                data.set_type_name(message.at("type_name").get<std::string>());
                data.set_type_sizes(message.at("type_sizes").get<std::vector<atech::common::TypeSize>>());
            }
            catch (json::exception& e) {

                throw atech::common::Error("Error in from_json of DdsTypeSize: " + (std::string)e.what());
            }
            
        }
    };

    template <> struct adl_serializer<atech::common::TopicMaxSize> {
        static void to_json(ordered_json& message, const atech::common::TopicMaxSize& data) {
            try {
                message["hash"] = data.get_hash();
                std::vector<atech::common::DdsTypeSize> inner_data = data.get_dds_type_size();
                message["dds_type_size"] = inner_data;
            }
            catch (json::exception& e) {

                throw atech::common::Error("Error in to_json of TopicMAxSize: " + (std::string)e.what());
            }
        }

        static void from_json(const ordered_json& message, atech::common::TopicMaxSize& data) {
            try {
                data.set_hash(message.at("hash").get<int64_t>());
                data.set_dds_type_size(message.at("dds_type_size").get<std::vector<atech::common::DdsTypeSize>>());
            }
            catch (json::exception& e) {

                throw atech::common::Error("Error in from_json of TopicMaxSize: " + (std::string)e.what());
            }
            
        }
    };

    template <> struct adl_serializer<atech::common::Topic> {
        static void to_json(ordered_json& message, const atech::common::Topic& data) {
            try {
                message["topic_name"] = data.get_topic_name();
                std::vector<size_t> inner_data = data.get_tag_ids();
                message["tag_ids"] = inner_data;
            }
            catch (json::exception& e) {

                throw atech::common::Error("Error in to_json of Topic: " + (std::string)e.what());
            }
        }

        static void from_json(const ordered_json& message, atech::common::Topic& data) {
            try {
                data.set_topic_name(message.at("topic_name").get<std::string>());
                data.set_tag_ids(message.at("tag_ids").get<std::vector<size_t>>());
            }
            catch (json::exception& e) {

                throw atech::common::Error("Error in from_json of Topic: " + (std::string)e.what());
            }
            
        }
    };
 

    template <> struct adl_serializer<atech::common::Putdata> {
        static void to_json(ordered_json& message, const atech::common::Putdata& data) {
            try {
                message["id_tag"] = data.get_id_tag();
                message["tag"] = data.get_tag();
                message["type"] = data.get_type();
                message["is_array"] = data.get_is_array();
                message["offset"] = data.get_offset();
                message["mask"] = data.get_mask();
            }
            catch (json::exception& e) {

                throw atech::common::Error("Error in to_json of Pudata: " + (std::string)e.what());
            }
          
        }

        static void from_json(const ordered_json& message, atech::common::Putdata& data) {
            try {
                data.set_id_tag(message.at("id_tag").get<int64_t>());
                data.set_tag(message.at("tag").get<std::string>());
                data.set_type(message.at("type").get<std::string>());
                data.set_is_array(message.at("is_array").get<bool>());
                data.set_offset(message.at("offset").get<int64_t>());
                data.set_mask(message.at("mask").get<int64_t>());
            }
            catch (json::exception& e) {

                throw atech::common::Error("Error in from_json of Pudata: " + (std::string)e.what());
            }
         
        }
    };

    template <> struct adl_serializer<atech::common::Datum> {
        static void to_json(ordered_json& message, const atech::common::Datum& data) {
            try {
                message["inputdata"] = data.get_inputdata();
                message["outputdata"] = data.get_outputdata();
                message["type_registration"] = data.get_type_registration();
                message["delta"] = data.get_delta();
            }
            catch (json::exception& e) {

                throw atech::common::Error("Error in to_json of Data: " + (std::string)e.what());
            }
        }

        static void from_json(const ordered_json& message, atech::common::Datum& data) {
            try {
                data.set_inputdata(message.at("inputdata").get<atech::common::Putdata>());
                data.set_outputdata(message.at("outputdata").get<atech::common::Putdata>());
                data.set_type_registration(message.at("type_registration").get<std::string>());
                data.set_delta(message.at("delta").get<double>());
            }
            catch (json::exception& e) {

                throw atech::common::Error("Error in from_json of Data: " + (std::string)e.what());
            }
        }
    };

    template <> struct adl_serializer<atech::common::Mapping> {
        static void to_json(ordered_json& message, const atech::common::Mapping& data) {
            try {
                message["id_map"] = data.get_id_map();
                message["frequency"] = data.get_frequency();
                std::vector<atech::common::Datum> inner_data = data.get_data();
                message["data"] = inner_data;
            }
            catch (json::exception& e) {

                throw atech::common::Error("Error in to_json of Mapping: " + (std::string)e.what());
            }
        }

        static void from_json(const ordered_json& message, atech::common::Mapping& data) {
            try {
                data.set_id_map(message.at("id_map").get<uint32_t>());
                data.set_frequency(message.at("frequency").get<size_t>());
                data.set_data(message.at("data").get<std::vector<atech::common::Datum>>());
            }
            catch (json::exception& e) {

                throw atech::common::Error("Error in from_json of Mapping: " + (std::string)e.what());
            }
            
        }
    };

    template <> struct adl_serializer<atech::common::Config> {
        static void to_json(ordered_json& message, const atech::common::Config& data) {}

        static void from_json(const ordered_json& message, atech::common::Config& data) {}
    };

    template <> struct adl_serializer<atech::common::SmConfig> {
        static void to_json(ordered_json& message, const atech::common::SmConfig& data) {
            try {
                message["name_point_sm"] = data.get_name_point_sm();
                message["size_float_data"] = data.get_size_float_data();
                message["size_int_data"] = data.get_size_int_data();
                message["size_double_data"] = data.get_size_double_data();
                message["size_char_data"] = data.get_size_char_data();
                message["size_str"] = data.get_size_str();
            }
            catch (json::exception& e) {

                throw atech::common::Error("Error in to_json of SmConfig: " + (std::string)e.what());
            }
        }

        static void from_json(const ordered_json& message, atech::common::SmConfig& data) {
            try {
                data.set_name_point_sm(message.at("name_point_sm").get<std::string>());
                data.set_size_float_data(message.at("size_float_data").get<size_t>());
                data.set_size_double_data(message.at("size_double_data").get<size_t>());
                data.set_size_char_data(message.at("size_char_data").get<size_t>());
                data.set_size_str(message.at("size_str").get<size_t>());
            }
            catch (json::exception& e) {

                throw atech::common::Error("Error in from_json of SmConfig: " + (std::string)e.what());
            }
        }
    };

    template <> struct adl_serializer<atech::common::UaConfig> {
        static void to_json(ordered_json& message, const atech::common::UaConfig& data) {
            try {
                message["endpoint_url"] = data.get_endpoint_url();
                message["security_mode"] = data.get_security_mode();
                message["security_policy"] = data.get_security_policy();
                message["user_name"] = data.get_user_name();
                message["password"] = data.get_password();
                message["namespace_index"] = data.get_namespace_index();
            }
            catch (json::exception& e) {

                throw atech::common::Error("Error in to_json of UaConfig: " + (std::string)e.what());
            }

        }

        static void from_json(const ordered_json& message, atech::common::UaConfig& data) {
            try {
                data.set_endpoint_url(message.at("endpoint_url").get<std::string>());
                data.set_security_mode(message.at("security_mode").get<std::string>());
                data.set_security_policy(message.at("security_policy").get<std::string>());
                data.set_user_name(message.at("user_name").get<std::string>());
                data.set_password(message.at("password").get<std::string>());
                data.set_namespace_index(message.at("namespace_index").get<std::string>());
            }
            catch (json::exception& e) {

                throw atech::common::Error("Error in from_json of UaConfig: " + (std::string)e.what());
            }
        }
    };

    template <> struct adl_serializer<atech::common::DdsConfig> {
        static void to_json(ordered_json& message, const atech::common::DdsConfig& data) {
            try {
                message["topic_name"] = data.get_topic_name();
                message["type_name"] = data.get_type_name();
            }
            catch (json::exception& e) {

                throw atech::common::Error("Error in to_json of DdsConfig: " + (std::string)e.what());
            }

        }

        static void from_json(const ordered_json& message, atech::common::DdsConfig& data) {
            try {
                data.set_topic_name(message.at("topic_name").get<std::string>());
                data.set_type_name(message.at("type_name").get<std::string>());
            }
            catch (json::exception& e) {

                throw atech::common::Error("Error in from_json of DdsConfig: " + (std::string)e.what());
            }

            
        }
    };

    template <> struct adl_serializer<atech::common::WsConfig> {
        static void to_json(ordered_json& message, const atech::common::WsConfig& data) {
            try {
                message["host"] = data.get_host();
                message["port"] = data.get_port();
                message["period_ms"] = data.get_period();
                message["is_ipv4"] = data.get_is_ipv4();
                message["send_changes_only"] = data.get_send_changes_only();
                message["is_binary_format"] = data.get_is_binary_format();
                std::vector<atech::common::Topic> inner_data = data.get_topics();
                message["topics"] = inner_data;
            }
            catch (json::exception& e) {

                throw atech::common::Error("Error in to_json of WsConfig: " + (std::string)e.what());
            }
        }

        static void from_json(const ordered_json& message, atech::common::WsConfig& data) {
            try {
                data.set_host(message.at("host").get<std::string>());
                data.set_port(message.at("port").get<int64_t>());
                data.set_period(message.at("period_ms").get<int64_t>());
                data.set_is_ipv4(message.at("is_ipv4").get<bool>());
                data.set_send_changes_only(message.at("send_changes_only").get<bool>());
                data.set_is_binary_format(message.at("is_binary_format").get<bool>());
                data.set_topics(message.at("topics").get<std::vector<atech::common::Topic>>());
            }
            catch (json::exception& e) {

                throw atech::common::Error("Error in from_json of WsConfig: " + (std::string)e.what());
            }

        }
    };

    template <> struct adl_serializer<atech::common::FtConfig> {
        static void to_json(ordered_json& message, const atech::common::FtConfig& data) {
            try {
                message["host"] = data.get_host();
                message["port"] = data.get_port();
                message["driver"] = data.get_driver();
                message["database"] = data.get_database();
                message["schema"] = data.get_schema();
                message["user"] = data.get_user();
                message["password"] = data.get_password();
                message["maxconnections"] = data.get_maxconnections();
                message["timeout"] = data.get_timeout();
                message["encrypt"] = data.get_encrypt();
                message["monitor_period"] = data.get_monitor_period();
                message["check_period"] = data.get_check_period();
                message["health_period"] = data.get_health_period();
                message["cusage_interval"] = data.get_cusage_interval();
           
            }
            catch (json::exception& e) {

                throw atech::common::Error("Error in to_json of WsConfig: " + (std::string)e.what());
            }
        }

        static void from_json(const ordered_json& message, atech::common::FtConfig& data) {
            try {
                data.set_host(message.at("host").get<std::string>());
                data.set_port(message.at("port").get<size_t>());
                data.set_driver(message.at("driver").get<std::string>());
                data.set_database(message.at("database").get<std::string>());
                data.set_schema(message.at("schema").get<std::string>());
                data.set_user(message.at("user").get<std::string>());
                data.set_password(message.at("password").get<std::string>());
                data.set_maxconnections(message.at("maxconnections").get<size_t>());
                data.set_timeout(message.at("timeout").get<size_t>());
                data.set_encrypt(message.at("encrypt").get<std::string>());
                data.set_monitor_period(message.at("monitor_period").get<size_t>());
                data.set_check_period(message.at("check_period").get<size_t>());
                data.set_health_period(message.at("health_period").get<size_t>());
                data.set_cusage_interval(message.at("cusage_interval").get<size_t>());
             
            }
            catch (json::exception& e) {

                throw atech::common::Error("Error in from_json of WsConfig: " + (std::string)e.what());
            }

        }
    };

    template <> struct adl_serializer<atech::common::LoggerConfig> {
        static void to_json(ordered_json& message, const atech::common::LoggerConfig& data) {
            try {
                message["file_name"] = data.get_file_name();
                message["file_path"] = data.get_file_path();
                message["file_size"] = data.get_file_size();
                message["file_number"] = data.get_file_number();
                message["log_level"] = data.get_log_level();
                message["log_dds"] = data.get_log_dds();
                message["log_level_dds"] = data.get_log_level_dds();
              
            }
            catch (json::exception& e) {

                throw atech::common::Error("Error in to_json of LoggerConfig: " + (std::string)e.what());
            }
        }

        static void from_json(const ordered_json& message, atech::common::LoggerConfig& data) {
            try {
                data.set_file_name(message.at("file_name").get<std::string>());
                data.set_file_path(message.at("file_path").get<std::string>());
                data.set_file_size(message.at("file_size").get<size_t>());
                data.set_file_number(message.at("file_number").get<size_t>());
                data.set_log_level(message.at("log_level").get<size_t>());
                data.set_log_dds(message.at("log_dds").get<bool>());
                data.set_log_level_dds(message.at("log_level_dds").get<size_t>());
             
            }
            catch (json::exception& e) {

                throw atech::common::Error("Error in from_json of LoggerConfig: " + (std::string)e.what());
            }

        }
    };

    template <> struct adl_serializer<atech::common::ProcessInfo> {
        static void to_json(ordered_json& message, const atech::common::ProcessInfo& data) {
            try {
                message["hname"] = data.get_hname();
                message["pid"] = data.get_pid();
                message["pname"] = data.get_pname();
                message["pparam"] = data.get_pparam();
                message["pmemory"] = data.get_pmemory();
                message["pcpu"] = data.get_pcpu();
                message["state"] = data.get_state();
                message["pinfo"] = data.get_pinfo();

            }
            catch (json::exception& e) {

                throw atech::common::Error("Error in to_json of ProcessInfo: " + (std::string)e.what());
            }
        }

        static void from_json(const ordered_json& message, atech::common::ProcessInfo& data) {
            try {
                data.set_hname(message.at("hname").get<std::string>());
                data.set_pid(message.at("pid").get<int64_t>());
                data.set_pname(message.at("pname").get<std::string>());
                data.set_pparam(message.at("pparam").get<std::string>());
                data.set_pmemory(message.at("pmemory").get<int64_t>());
                data.set_pcpu(message.at("pcpu").get<double>());
                data.set_state(message.at("state").get<int64_t>());
                data.set_pinfo(message.at("pinfo").get<std::string>());
             }
            catch (json::exception& e) {

                throw atech::common::Error("Error in from_json of ProcessInfo: " + (std::string)e.what());
            }

        }
    };

    template <> struct adl_serializer<atech::common::InputUnit> {
        static void to_json(ordered_json& message, const atech::common::InputUnit& data) {
            try {
                message["id"] = data.get_id();
                message["version"] = data.get_version();
                message["type_adapter"] = data.get_type_adapter();
                message["config"] = data.get_config();
                message["id_map"] = data.get_id_map();
            }
            catch (json::exception& e) {

                throw atech::common::Error("Error in to_json of InputUnit: " + (std::string)e.what());
            }
      
        }

        static void from_json(const ordered_json& message, atech::common::InputUnit& data) {
            try {
                data.set_id(message.at("id").get<uint32_t>());
                data.set_version(message.at("version").get<std::string>());
                data.set_type_adapter(message.at("type_adapter").get<std::string>());
                data.set_id_map(message.at("id_map").get<uint32_t>());
                if (data.get_type_adapter() == DDS_CONFIG)
                    data.set_config(message.at("config").get<std::shared_ptr<atech::common::DdsConfig>>());
                if (data.get_type_adapter() == SM_CONFIG)
                    data.set_config(message.at("config").get<std::shared_ptr<atech::common::SmConfig>>());
                if (data.get_type_adapter() == OPC_UA_CONFIG)
                    data.set_config(message.at("config").get<std::shared_ptr<atech::common::UaConfig>>());
                if (data.get_type_adapter() == WS_CONFIG)
                    data.set_config(message.at("config").get<std::shared_ptr<atech::common::WsConfig>>());
                if (data.get_type_adapter() == FT_CONFIG)
                    data.set_config(message.at("config").get<std::shared_ptr<atech::common::FtConfig>>());
            }
            catch (json::exception& e) {

                throw atech::common::Error("Error in from_json of InputUnit: " + (std::string)e.what());
            }
            
        }
    };

    template <> struct adl_serializer<atech::common::OutputUnit> {
        static void to_json(ordered_json& message, const atech::common::OutputUnit& data) {
            try {
                message["id"] = data.get_id();
                message["version"] = data.get_version();
                message["type_adapter"] = data.get_type_adapter();
                message["id_map"] = data.get_id_map();
                message["config"] = data.get_config();
            }
            catch (json::exception& e) {

                throw atech::common::Error("Error in to_json of OutputUnit: " + (std::string)e.what());
            }

        }

        static void from_json(const ordered_json& message, atech::common::OutputUnit& data) {
            try {
                data.set_id(message.at("id").get<uint32_t>());
                data.set_version(message.at("version").get<std::string>());
                data.set_type_adapter(message.at("type_adapter").get<std::string>());
                data.set_id_map(message.at("id_map").get<uint32_t>());
                if (data.get_type_adapter() == DDS_CONFIG)
                    data.set_config(message.at("config").get<std::shared_ptr<atech::common::DdsConfig>>());
                if (data.get_type_adapter() == SM_CONFIG)
                    data.set_config(message.at("config").get<std::shared_ptr<atech::common::SmConfig>>());
                if (data.get_type_adapter() == OPC_UA_CONFIG)
                    data.set_config(message.at("config").get<std::shared_ptr<atech::common::UaConfig>>());
                if (data.get_type_adapter() == WS_CONFIG)
                    data.set_config(message.at("config").get<std::shared_ptr<atech::common::WsConfig>>());
                if (data.get_type_adapter() == FT_CONFIG)
                    data.set_config(message.at("config").get<std::shared_ptr<atech::common::FtConfig>>());
            }
            catch (json::exception& e) {

                throw atech::common::Error("Error in from_json of Outputunit: " + (std::string)e.what());
            }
            
        }
    };

    template <> struct adl_serializer<atech::common::Unit> {
        static void to_json(ordered_json& message, const atech::common::Unit& data) {
            try {
                message["frequency"] = data.get_frequency();
                message["id"] = data.get_id();
                message["input_units"] = data.get_input_units();
                message["output_units"] = data.get_output_units();
                // std::shared_ptr<std::vector<Mapping>> inner_data = data.get_mapping();
                message["mapping"] = data.get_mapping();
            }
            catch (json::exception& e) {

                throw atech::common::Error("Error in to_json of Unit: " + (std::string)e.what());
            }
        }

        static void from_json(const ordered_json& message, atech::common::Unit& data) {
            try {
                data.set_frequency(message.at("frequency").get<size_t>());
                data.set_id(message.at("id").get<uint32_t>());
                data.set_input_units(message.at("input_units").get<std::vector<atech::common::InputUnit>>());
                data.set_output_units(message.at("output_units").get<std::vector<atech::common::OutputUnit>>());
                data.set_mapping(message.at("mapping").get<std::vector<atech::common::Mapping>>());
            }
            catch (json::exception& e) {

                throw atech::common::Error("Error in from_json of Unit: " + (std::string)e.what());
            }
            
        }
    };


    template <> struct adl_serializer<atech::common::Adapters> {
        static void to_json(ordered_json& message, const atech::common::Adapters& data) {
            try {
                message["hash"] = data.get_hash();
                message["units"] = data.get_units();
            }
            catch (json::exception& e) {

                throw atech::common::Error("Error in to_json of Adapters: " + (std::string)e.what());
            }
        }

        static void from_json(const ordered_json& message, atech::common::Adapters& data) {
            try {
                data.set_hash(message.at("hash").get<int64_t>());
                data.set_units(message.at("units").get<std::vector<atech::common::Unit>>());
            }
            catch (json::exception& e) {

                throw atech::common::Error("Error in from_json of Adapters: " + (std::string)e.what());
            }
          
        }
    };


}

   



#endif // _MESSAGE_SERIALIZER_HPP_