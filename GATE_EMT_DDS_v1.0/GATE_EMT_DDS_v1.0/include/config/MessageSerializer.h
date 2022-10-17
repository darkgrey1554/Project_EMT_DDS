#pragma once

#ifndef _MESSAGE_SERIALIZER_HPP_
#define _MESSAGE_SERIALIZER_HPP_


#include <string>
#include <fstream>
#include "json.hpp"
#include "adapters.h"
#include "topics.h"
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

    template <> struct adl_serializer<scd::common::TypeSize> {
        static void to_json(ordered_json& message, const scd::common::TypeSize& data) {
            try {
                message["type_name"] = data.get_type_name();
                message["size"] = data.get_size();
            }
            catch (json::exception& e) {

                throw scd::common::Error("Error in to_json of TypeSize: " + (std::string)e.what());
            }
        }

        static void from_json(const ordered_json& message, scd::common::TypeSize& data) {
            try {
                data.set_type_name(message.at("type_name").get<std::string>());
                data.set_size(message.at("size").get<int64_t>());
            }
            catch (json::exception& e) {

                throw scd::common::Error("Error in from_json of TypeSize: " + (std::string)e.what());
            }
        }
    };

    template <> struct adl_serializer<scd::common::DdsTypeSize> {
        static void to_json(ordered_json& message, const scd::common::DdsTypeSize& data) {
            try {
                message["type_name"] = data.get_type_name();
                std::vector<scd::common::TypeSize> inner_data = data.get_type_sizes();
                message["type_sizes"] = inner_data;
            }
            catch (json::exception& e) {

                throw scd::common::Error("Error in to_json of DdsTypeSize: " + (std::string)e.what());
            }
        }

        static void from_json(const ordered_json& message, scd::common::DdsTypeSize& data) {
            try {
                data.set_type_name(message.at("type_name").get<std::string>());
                data.set_type_sizes(message.at("type_sizes").get<std::vector<scd::common::TypeSize>>());
            }
            catch (json::exception& e) {

                throw scd::common::Error("Error in from_json of DdsTypeSize: " + (std::string)e.what());
            }

        }
    };

    template <> struct adl_serializer<scd::common::TopicMaxSize> {
        static void to_json(ordered_json& message, const scd::common::TopicMaxSize& data) {
            try {
                message["hash"] = data.get_hash();
                std::vector<scd::common::DdsTypeSize> inner_data = data.get_dds_type_size();
                message["dds_type_size"] = inner_data;
            }
            catch (json::exception& e) {

                throw scd::common::Error("Error in to_json of TopicMAxSize: " + (std::string)e.what());
            }
        }

        static void from_json(const ordered_json& message, scd::common::TopicMaxSize& data) {
            try {
                data.set_hash(message.at("hash").get<int64_t>());
                data.set_dds_type_size(message.at("dds_type_size").get<std::vector<scd::common::DdsTypeSize>>());
            }
            catch (json::exception& e) {

                throw scd::common::Error("Error in from_json of TopicMaxSize: " + (std::string)e.what());
            }

        }
    };

    template <> struct adl_serializer<scd::common::Topic> {
        static void to_json(ordered_json& message, const scd::common::Topic& data) {
            try {
                message["topic_name"] = data.get_topic_name();
                std::vector<int64_t> inner_data = data.get_tag_ids();
                message["tag_ids"] = inner_data;
            }
            catch (json::exception& e) {

                throw scd::common::Error("Error in to_json of Topic: " + (std::string)e.what());
            }
        }

        static void from_json(const ordered_json& message, scd::common::Topic& data) {
            try {
                data.set_topic_name(message.at("topic_name").get<std::string>());
                data.set_tag_ids(message.at("tag_ids").get<std::vector<int64_t>>());
            }
            catch (json::exception& e) {

                throw scd::common::Error("Error in from_json of Topic: " + (std::string)e.what());
            }

        }
    };


    template <> struct adl_serializer<scd::common::Putdata> {
        static void to_json(ordered_json& message, const scd::common::Putdata& data) {
            try {
                message["id_tag"] = data.get_id_tag();
                message["tag"] = data.get_tag();
                message["type"] = data.get_type();
                message["is_array"] = data.get_is_array();
                message["offset"] = data.get_offset();
                message["mask"] = data.get_mask();
            }
            catch (json::exception& e) {

                throw scd::common::Error("Error in to_json of Pudata: " + (std::string)e.what());
            }

        }

        static void from_json(const ordered_json& message, scd::common::Putdata& data) {
            try {
                data.set_id_tag(message.at("id_tag").get<int64_t>());
                data.set_tag(message.at("tag").get<std::string>());
                data.set_type(message.at("type").get<std::string>());
                data.set_is_array(message.at("is_array").get<bool>());
                data.set_offset(message.at("offset").get<int64_t>());
                data.set_mask(message.at("mask").get<int64_t>());
            }
            catch (json::exception& e) {

                throw scd::common::Error("Error in from_json of Pudata: " + (std::string)e.what());
            }

        }
    };

    template <> struct adl_serializer<scd::common::Datum> {
        static void to_json(ordered_json& message, const scd::common::Datum& data) {
            try {
                message["inputdata"] = data.get_inputdata();
                message["outputdata"] = data.get_outputdata();
                message["type_registration"] = data.get_type_registration();
                message["delta"] = data.get_delta();
            }
            catch (json::exception& e) {

                throw scd::common::Error("Error in to_json of Data: " + (std::string)e.what());
            }
        }

        static void from_json(const ordered_json& message, scd::common::Datum& data) {
            try {
                data.set_inputdata(message.at("inputdata").get<scd::common::Putdata>());
                data.set_outputdata(message.at("outputdata").get<scd::common::Putdata>());
                data.set_type_registration(message.at("type_registration").get<std::string>());
                data.set_delta(message.at("delta").get<double>());
            }
            catch (json::exception& e) {

                throw scd::common::Error("Error in from_json of Data: " + (std::string)e.what());
            }
        }
    };

    template <> struct adl_serializer<scd::common::Mapping> {
        static void to_json(ordered_json& message, const scd::common::Mapping& data) {
            try {
                message["id_map"] = data.get_id_map();
                message["frequency"] = data.get_frequency();
                std::vector<scd::common::Datum> inner_data = data.get_data();
                message["data"] = inner_data;
            }
            catch (json::exception& e) {

                throw scd::common::Error("Error in to_json of Mapping: " + (std::string)e.what());
            }
        }

        static void from_json(const ordered_json& message, scd::common::Mapping& data) {
            try {
                data.set_id_map(message.at("id_map").get<int64_t>());
                data.set_frequency(message.at("frequency").get<int64_t>());
                data.set_data(message.at("data").get<std::vector<scd::common::Datum>>());
            }
            catch (json::exception& e) {

                throw scd::common::Error("Error in from_json of Mapping: " + (std::string)e.what());
            }

        }
    };

    template <> struct adl_serializer<scd::common::Config> {
        static void to_json(ordered_json& message, const scd::common::Config& data) {}

        static void from_json(const ordered_json& message, scd::common::Config& data) {}
    };

    template <> struct adl_serializer<scd::common::SmConfig> {
        static void to_json(ordered_json& message, const scd::common::SmConfig& data) {
            try {
                message["name_point_sm"] = data.get_name_point_sm();
                message["size_float_data"] = data.get_size_float_data();
                message["size_int_data"] = data.get_size_int_data();
                message["size_double_data"] = data.get_size_double_data();
                message["size_char_data"] = data.get_size_char_data();
                message["size_str"] = data.get_size_str();
            }
            catch (json::exception& e) {

                throw scd::common::Error("Error in to_json of SmConfig: " + (std::string)e.what());
            }
        }

        static void from_json(const ordered_json& message, scd::common::SmConfig& data) {
            try {
                data.set_name_point_sm(message.at("name_point_sm").get<std::string>());
                data.set_size_float_data(message.at("size_float_data").get<int64_t>());
                data.set_size_double_data(message.at("size_double_data").get<int64_t>());
                data.set_size_char_data(message.at("size_char_data").get<int64_t>());
                data.set_size_str(message.at("size_str").get<int64_t>());
            }
            catch (json::exception& e) {

                throw scd::common::Error("Error in from_json of SmConfig: " + (std::string)e.what());
            }
        }
    };

    template <> struct adl_serializer<scd::common::UaConfig> {
        static void to_json(ordered_json& message, const scd::common::UaConfig& data) {
            try {
                message["endpoint_url"] = data.get_endpoint_url();
                message["security_mode"] = data.get_security_mode();
                message["security_policy"] = data.get_security_policy();
                message["user_name"] = data.get_user_name();
                message["password"] = data.get_password();
                message["namespace_index"] = data.get_namespace_index();
            }
            catch (json::exception& e) {

                throw scd::common::Error("Error in to_json of UaConfig: " + (std::string)e.what());
            }

        }

        static void from_json(const ordered_json& message, scd::common::UaConfig& data) {
            try {
                data.set_endpoint_url(message.at("endpoint_url").get<std::string>());
                data.set_security_mode(message.at("security_mode").get<std::string>());
                data.set_security_policy(message.at("security_policy").get<std::string>());
                data.set_user_name(message.at("user_name").get<std::string>());
                data.set_password(message.at("password").get<std::string>());
                data.set_namespace_index(message.at("namespace_index").get<std::string>());
            }
            catch (json::exception& e) {

                throw scd::common::Error("Error in from_json of UaConfig: " + (std::string)e.what());
            }
        }
    };

    template <> struct adl_serializer<scd::common::DdsConfig> {
        static void to_json(ordered_json& message, const scd::common::DdsConfig& data) {
            try {
                message["topic_name"] = data.get_topic_name();
                message["type_name"] = data.get_type_name();
            }
            catch (json::exception& e) {

                throw scd::common::Error("Error in to_json of DdsConfig: " + (std::string)e.what());
            }

        }

        static void from_json(const ordered_json& message, scd::common::DdsConfig& data) {
            try {
                data.set_topic_name(message.at("topic_name").get<std::string>());
                data.set_type_name(message.at("type_name").get<std::string>());
            }
            catch (json::exception& e) {

                throw scd::common::Error("Error in from_json of DdsConfig: " + (std::string)e.what());
            }


        }
    };

    template <> struct adl_serializer<scd::common::WsConfig> {
        static void to_json(ordered_json& message, const scd::common::WsConfig& data) {
            try {
                message["host"] = data.get_host();
                message["port"] = data.get_port();
                message["period"] = data.get_period();
                message["is_ipv4"] = data.get_is_ipv4();
                message["send_changes_only"] = data.get_send_changes_only();
                message["is_binary_format"] = data.get_is_binary_format();
                std::vector<scd::common::Topic> inner_data = data.get_topics();
                message["topics"] = inner_data;
            }
            catch (json::exception& e) {

                throw scd::common::Error("Error in to_json of WsConfig: " + (std::string)e.what());
            }
        }

        static void from_json(const ordered_json& message, scd::common::WsConfig& data) {
            try {
                data.set_host(message.at("host").get<std::string>());
                data.set_port(message.at("port").get<int64_t>());
                data.set_period(message.at("period").get<int64_t>());
                data.set_is_ipv4(message.at("is_ipv4").get<bool>());
                data.set_send_changes_only(message.at("send_changes_only").get<bool>());
                data.set_is_binary_format(message.at("is_binary_format").get<bool>());
                data.set_topics(message.at("topics").get<std::vector<scd::common::Topic>>());
            }
            catch (json::exception& e) {

                throw scd::common::Error("Error in from_json of WsConfig: " + (std::string)e.what());
            }

        }
    };

    template <> struct adl_serializer<scd::common::InputUnit> {
        static void to_json(ordered_json& message, const scd::common::InputUnit& data) {
            try {
                message["id"] = data.get_id();
                message["type_adapter"] = data.get_type_adapter();
                message["config"] = data.get_config();
                message["id_map"] = data.get_id_map();
            }
            catch (json::exception& e) {

                throw scd::common::Error("Error in to_json of InputUnit: " + (std::string)e.what());
            }

        }

        static void from_json(const ordered_json& message, scd::common::InputUnit& data) {
            try {
                data.set_id(message.at("id").get<int64_t>());
                data.set_type_adapter(message.at("type_adapter").get<std::string>());
                data.set_id_map(message.at("id_map").get<int64_t>());
                if (data.get_type_adapter() == "DDS")
                    data.set_config(message.at("config").get<std::shared_ptr<scd::common::DdsConfig>>());
                if (data.get_type_adapter() == "SM")
                    data.set_config(message.at("config").get<std::shared_ptr<scd::common::SmConfig>>());
                if (data.get_type_adapter() == "OPC_UA")
                    data.set_config(message.at("config").get<std::shared_ptr<scd::common::UaConfig>>());
                if (data.get_type_adapter() == "WS")
                    data.set_config(message.at("config").get<std::shared_ptr<scd::common::WsConfig>>());
            }
            catch (json::exception& e) {

                throw scd::common::Error("Error in from_json of InputUnit: " + (std::string)e.what());
            }

        }
    };

    template <> struct adl_serializer<scd::common::OutputUnit> {
        static void to_json(ordered_json& message, const scd::common::OutputUnit& data) {
            try {
                message["id"] = data.get_id();
                message["type_adapter"] = data.get_type_adapter();
                message["id_map"] = data.get_id_map();
                message["config"] = data.get_config();
            }
            catch (json::exception& e) {

                throw scd::common::Error("Error in to_json of OutputUnit: " + (std::string)e.what());
            }

        }

        static void from_json(const ordered_json& message, scd::common::OutputUnit& data) {
            try {
                data.set_id(message.at("id").get<int64_t>());
                data.set_type_adapter(message.at("type_adapter").get<std::string>());
                data.set_id_map(message.at("id_map").get<int64_t>());
                if (data.get_type_adapter() == "DDS")
                    data.set_config(message.at("config").get<std::shared_ptr<scd::common::DdsConfig>>());
                if (data.get_type_adapter() == "SM")
                    data.set_config(message.at("config").get<std::shared_ptr<scd::common::SmConfig>>());
                if (data.get_type_adapter() == "OPC_UA")
                    data.set_config(message.at("config").get<std::shared_ptr<scd::common::UaConfig>>());
            }
            catch (json::exception& e) {

                throw scd::common::Error("Error in from_json of Outputunit: " + (std::string)e.what());
            }

        }
    };

    template <> struct adl_serializer<scd::common::Unit> {
        static void to_json(ordered_json& message, const scd::common::Unit& data) {
            try {
                message["frequency"] = data.get_frequency();
                message["id"] = data.get_id();
                message["input_units"] = data.get_input_units();
                message["output_units"] = data.get_output_units();
                // std::shared_ptr<std::vector<Mapping>> inner_data = data.get_mapping();
                message["mapping"] = data.get_mapping();
            }
            catch (json::exception& e) {

                throw scd::common::Error("Error in to_json of Unit: " + (std::string)e.what());
            }
        }

        static void from_json(const ordered_json& message, scd::common::Unit& data) {
            try {
                data.set_frequency(message.at("frequency").get<int64_t>());
                data.set_id(message.at("id").get<int64_t>());
                data.set_input_units(message.at("input_units").get<std::vector<scd::common::InputUnit>>());
                data.set_output_units(message.at("output_units").get<std::vector<scd::common::OutputUnit>>());
                data.set_mapping(message.at("mapping").get<std::vector<scd::common::Mapping>>());
            }
            catch (json::exception& e) {

                throw scd::common::Error("Error in from_json of Unit: " + (std::string)e.what());
            }

        }
    };


    template <> struct adl_serializer<scd::common::Adapters> {
        static void to_json(ordered_json& message, const scd::common::Adapters& data) {
            try {
                message["hash"] = data.get_hash();
                message["units"] = data.get_units();
            }
            catch (json::exception& e) {

                throw scd::common::Error("Error in to_json of Adapters: " + (std::string)e.what());
            }
        }

        static void from_json(const ordered_json& message, scd::common::Adapters& data) {
            try {
                data.set_hash(message.at("hash").get<int64_t>());
                data.set_units(message.at("units").get<std::vector<scd::common::Unit>>());
            }
            catch (json::exception& e) {

                throw scd::common::Error("Error in from_json of Adapters: " + (std::string)e.what());
            }

        }
    };
}





#endif // _MESSAGE_SERIALIZER_HPP_