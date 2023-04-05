#pragma once
#ifndef _ADAPTERS_HPP_
#define _ADATPTERS_HPP_

#include <string>
#include <vector>
#include <memory>

constexpr const char* OPC_UA_CONFIG = "OPC_UA";
constexpr const char* DDS_CONFIG = "DDS";
constexpr const char* SM_CONFIG = "SM";
constexpr const char* WS_CONFIG = "WS";
constexpr const char* WA_CONFIG = "WA";
constexpr const char* HS_CONFIG = "HS";
constexpr const char* FT_CONFIG = "FT";
constexpr const char* DB_CONFIG = "DB";

namespace atech 
{
    namespace common 
    {
        class Topic {
        public:
            Topic() = default;
            virtual ~Topic() = default;

        private:
            std::string topic_name;
            std::vector<size_t> tag_ids;

        public:
            const std::string& get_topic_name() const { return topic_name; }
            std::string& get_mutable_topic_name() { return topic_name; }
            void set_topic_name(const std::string& value) { this->topic_name = value; }

            const std::vector<size_t>& get_tag_ids() const { return tag_ids; }
            std::vector<size_t>& get_mutable_tag_ids() { return tag_ids; }
            void set_tag_ids(const std::vector<size_t>& value) { this->tag_ids = value; }
        };

        class Config 
        { 
            public:
            virtual ~Config(){}; 
        };

        class DdsConfig : public Config {
        public:
            DdsConfig() = default;
            virtual ~DdsConfig() = default;

        private:

            std::string topic_name;
            std::string type_name;

        public:

            const std::string& get_topic_name() const { return topic_name; }
            std::string& get_mutable_topic_name() { return topic_name; }
            void set_topic_name(const std::string& value) { this->topic_name = value; }

            const std::string& get_type_name() const { return type_name; }
            std::string& get_mutable_type_name() { return type_name; }
            void set_type_name(const std::string& value) { this->type_name = value; }
        };

        class SmConfig :public Config {
        public:
            SmConfig() = default;
            virtual ~SmConfig() = default;

        private:

            std::string name_point_sm;
            size_t size_int_data;
            size_t size_float_data;
            size_t size_double_data;
            size_t size_char_data;
            size_t size_str;

        public:
            const std::string& get_name_point_sm() const { return name_point_sm; }
            std::string& get_mutable_name_point_sm() { return name_point_sm; }
            void set_name_point_sm(const std::string& value) { this->name_point_sm = value; }

            const size_t& get_size_int_data() const { return size_int_data; }
            size_t& get_mutable_size_int_data() { return size_int_data; }
            void set_size_int_data(const size_t& value) { this->size_int_data = value; }

            const size_t& get_size_float_data() const { return size_float_data; }
            size_t& get_mutable_size_float_data() { return size_float_data; }
            void set_size_float_data(const size_t& value) { this->size_float_data = value; }

            const size_t& get_size_double_data() const { return size_double_data; }
            size_t& get_mutable_size_double_data() { return size_double_data; }
            void set_size_double_data(const size_t& value) { this->size_double_data = value; }

            const size_t& get_size_char_data() const { return size_char_data; }
            size_t& get_mutable_size_char_data() { return size_char_data; }
            void set_size_char_data(const size_t& value) { this->size_char_data = value; }

            const size_t& get_size_str() const { return size_str; }
            size_t& get_mutable_size_str() { return size_str; }
            void set_size_str(const size_t& value) { this->size_str = value; }
        };

        class UaConfig :public Config {
        public:
            UaConfig() = default;
            virtual ~UaConfig() = default;

        private:
            std::string endpoint_url;
            std::string security_mode;
            std::string security_policy;
            std::string user_name;
            std::string password;
            std::string namespace_index;

        public:
            const std::string& get_endpoint_url() const { return endpoint_url; }
            std::string& get_mutable_endpoint_url() { return endpoint_url; }
            void set_endpoint_url(const std::string& value) { this->endpoint_url = value; }

            const std::string& get_security_mode() const { return security_mode; }
            std::string& get_mutable_security_mode() { return security_mode; }
            void set_security_mode(const std::string& value) { this->security_mode = value; }

            const std::string& get_security_policy() const { return security_policy; }
            std::string& get_mutable_security_policy() { return security_policy; }
            void set_security_policy(const std::string& value) { this->security_policy = value; }

            const std::string& get_user_name() const { return user_name; }
            std::string& get_mutable_user_name() { return user_name; }
            void set_user_name(const std::string& value) { this->user_name = value; }

            const std::string& get_password() const { return password; }
            std::string& get_mutable_password() { return password; }
            void set_password(const std::string& value) { this->password = value; }

            const std::string& get_namespace_index() const { return namespace_index; }
            std::string& get_mutable_namespace_index() { return namespace_index; }
            void set_namespace_index(const std::string& value) { this->namespace_index = value; }
        };

        class WsConfig :public Config {
        public:
            WsConfig() = default;
            virtual ~WsConfig() = default;

        private:
            std::string host;
            int64_t port;
            int64_t period;
            bool is_ipv4;
            bool send_changes_only;
            bool is_binary_format;
            std::vector<Topic> topics;

        public:


            const std::string& get_host() const { return host; }
            std::string& get_mutable_host() { return host; }
            void set_host(const std::string& value) { this->host = value; }

            const int64_t& get_port() const { return port; }
            int64_t& get_mutable_port() { return port; }
            void set_port(const int64_t& value) { this->port = value; }

            const int64_t& get_period() const { return period; }
            int64_t& get_mutable_period() { return period; }
            void set_period(const int64_t& value) { this->period = value; }

            const bool& get_is_ipv4() const { return is_ipv4; }
            bool& get_mutable_is_ipv4() { return is_ipv4; }
            void set_is_ipv4(const bool& value) { this->is_ipv4 = value; }

            const bool& get_send_changes_only() const { return send_changes_only; }
            bool& get_mutable_send_changes_only() { return send_changes_only; }
            void set_send_changes_only(const bool& value) { this->send_changes_only = value; }

            const bool& get_is_binary_format() const { return is_binary_format; }
            bool& get_mutable_is_binary_format() { return is_binary_format; }
            void set_is_binary_format(const bool& value) { this->is_binary_format = value; }

            const std::vector<Topic>& get_topics() const { return topics; }
            std::vector<Topic>& get_mutable_topics() { return topics; }
            void set_topics(const std::vector<Topic>& value) { this->topics = value; }
        };

        class FtConfig:public Config {
        public:
            FtConfig() = default;
            virtual ~FtConfig() = default;

        private:
            std::string host;
            size_t port;
            std::string driver;
            std::string database;
            std::string schema;
            std::string user;
            std::string password;
            size_t maxconnections;
            size_t timeout;
            std::string encrypt;
            size_t monitor_period;
            size_t check_period;
            size_t health_period;
            size_t cusage_interval;

        public:
            const std::string& get_host() const { return host; }
            void set_host(const std::string& value) { this->host = value; }

            const size_t& get_port() const { return port; }
            void set_port(const size_t& value) { this->port = value; }

            const std::string& get_driver() const { return driver; }
            void set_driver(const std::string& value) { this->driver = value; }

            const std::string& get_database() const { return database; }
            void set_database(const std::string& value) { this->database = value; }

            const std::string& get_schema() const { return schema; }
            void set_schema(const std::string& value) { this->schema = value; }

            const std::string& get_user() const { return user; }
            void set_user(const std::string& value) { this->user = value; }

            const std::string& get_password() const { return password; }
            void set_password(const std::string& value) { this->password = value; }

            const size_t& get_maxconnections() const { return maxconnections; }
            void set_maxconnections(const size_t& value) { this->maxconnections = value; }

            const size_t& get_timeout() const { return timeout; }
            void set_timeout(const size_t& value) { this->timeout = value; }

            const std::string& get_encrypt() const { return encrypt; }
            void set_encrypt(const std::string& value) { this->encrypt = value; }

            const size_t& get_monitor_period() const { return monitor_period; }
            void set_monitor_period(const size_t& value) { this->monitor_period = value; }

            const size_t& get_check_period() const { return check_period; }
            void set_check_period(const size_t& value) { this->check_period = value; }

            const size_t& get_health_period() const { return health_period; }
            void set_health_period(const size_t& value) { this->health_period = value; }

            const size_t& get_cusage_interval() const { return cusage_interval; }
            void set_cusage_interval(const size_t& value) { this->cusage_interval = value; }
        };

        class Putdata {
        public:
            Putdata() = default;
            virtual ~Putdata() = default;

        private:
            std::string tag;
            int64_t id_tag;
            bool is_array;
            int64_t offset;
            std::string type;
            int64_t mask;

        public:
            const std::string& get_tag() const { return tag; }
            std::string& get_mutable_tag() { return tag; }
            void set_tag(const std::string& value) { this->tag = value; }

            const int64_t& get_id_tag() const { return id_tag; }
            int64_t& get_mutable_id_tag() { return id_tag; }
            void set_id_tag(const int64_t& value) { this->id_tag = value; }

            const bool& get_is_array() const { return is_array; }
            bool& get_mutable_is_array() { return is_array; }
            void set_is_array(const bool& value) { this->is_array = value; }

            const int64_t& get_offset() const { return offset; }
            int64_t& get_mutable_offset() { return offset; }
            void set_offset(const int64_t& value) { this->offset = value; }

            const std::string& get_type() const { return type; }
            std::string& get_mutable_type() { return type; }
            void set_type(const std::string& value) { this->type = value; }

            const int64_t& get_mask() const { return mask; }
            int64_t& get_mutable_mask() { return mask; }
            void set_mask(const int64_t& value) { this->mask = value; }
        };

        class Datum {
        public:
            Datum() = default;
            virtual ~Datum() = default;

        private:
            Putdata inputdata;
            Putdata outputdata;
            std::string type_registration;
            double delta;

        public:
            const Putdata& get_inputdata() const { return inputdata; }
            Putdata& get_mutable_inputdata() { return inputdata; }
            void set_inputdata(const Putdata& value) { this->inputdata = value; }

            const Putdata& get_outputdata() const { return outputdata; }
            Putdata& get_mutable_outputdata() { return outputdata; }
            void set_outputdata(const Putdata& value) { this->outputdata = value; }

            const std::string& get_type_registration() const { return type_registration; }
            std::string& get_mutable_type_registration() { return type_registration; }
            void set_type_registration(const std::string& value) { this->type_registration = value; }

            const double& get_delta() const { return delta; }
            double& get_mutable_delta() { return delta; }
            void set_delta(const double& value) { this->delta = value; }
        };


        class Mapping {
        public:
            Mapping() = default;
            virtual ~Mapping() = default;

        private:
            uint32_t id_map;
            size_t frequency;
            std::vector<Datum> data;

        public:
            const uint32_t& get_id_map() const { return id_map; }
            uint32_t& get_mutable_id_map() { return id_map; }
            void set_id_map(const uint32_t& value) { this->id_map = value; }

            const size_t& get_frequency() const { return frequency; }
            size_t& get_mutable_frequency() { return frequency; }
            void set_frequency(const size_t& value) { this->frequency = value; }

            const std::vector<Datum>& get_data() const { return data; }
            std::vector<Datum>& get_mutable_data() { return data; }
            void set_data(const std::vector<Datum>& value) { this->data = value; }
        };

        class InputUnit {
        public:
            InputUnit() = default;
            virtual ~InputUnit() = default;

        private:
            uint32_t id;
            std::string version;
            std::string type_adapter;
            std::shared_ptr<Config> config;
            uint32_t id_map;

        public:
            const uint32_t get_id() const { return id; }
            uint32_t& get_mutable_id() { return id; }
            void set_id(const uint32_t value) { this->id = value; }

            const std::string& get_type_adapter() const { return type_adapter; }
            std::string& get_mutable_type_adapter() { return type_adapter; }
            void set_type_adapter(const std::string& value) { this->type_adapter = value; }

            const std::string& get_version() const { return version; }
            std::string& get_mutable_version() { return version; }
            void set_version(const std::string& value) { this->version = value; }

            std::shared_ptr<Config> get_config() const { return config; }
            void set_config(std::shared_ptr<Config> value) { this->config = value; }

            const uint32_t& get_id_map() const { return id_map; }
            uint32_t& get_mutable_id_map() { return id_map; }
            void set_id_map(const uint32_t& value) { this->id_map = value; }
        };


        class OutputUnit {
        public:
            OutputUnit() = default;
            virtual ~OutputUnit() = default;

        private:
            uint32_t id;
            std::string version;
            std::string type_adapter;
            std::shared_ptr<Config> config;
            uint32_t id_map;

        public:
            const uint32_t& get_id() const { return id; }
            uint32_t& get_mutable_id() { return id; }
            void set_id(const uint32_t& value) { this->id = value; }

            const std::string& get_version() const { return version; }
            std::string& get_mutable_version() { return version; }
            void set_version(const std::string& value) { this->version = value; }

            const std::string& get_type_adapter() const { return type_adapter; }
            std::string& get_mutable_type_adapter() { return type_adapter; }
            void set_type_adapter(const std::string& value) { this->type_adapter = value; }

            std::shared_ptr<Config> get_config() const { return config; }
            void set_config(std::shared_ptr<Config> value) { this->config = value; }

            const uint32_t& get_id_map() const { return id_map; }
            uint32_t& get_mutable_id_map() { return id_map; }
            void set_id_map(const uint32_t& value) { this->id_map = value; }
        };


        class Unit {
        public:
            Unit() = default;
            virtual ~Unit() = default;

        private:
            uint32_t id;
            size_t frequency;
            std::vector<InputUnit> input_units;
            std::vector<OutputUnit> output_units;
            std::vector<Mapping> mapping;

        public:
            const uint32_t& get_id() const { return id; }
            uint32_t& get_mutable_id() { return id; }
            void set_id(const uint32_t& value) { this->id = value; }

            const size_t& get_frequency() const { return frequency; }
            size_t& get_mutable_frequency() { return frequency; }
            void set_frequency(const size_t& value) { this->frequency = value; }

            const std::vector<InputUnit>& get_input_units() const { return input_units; }
            std::vector<InputUnit>& get_mutable_input_units() { return input_units; }
            void set_input_units(const  std::vector<InputUnit>& value) { this->input_units = value; }

            const std::vector<OutputUnit>& get_output_units() const { return output_units; }
            std::vector<OutputUnit>& get_mutable_output_units() { return output_units; }
            void set_output_units(const std::vector<OutputUnit>& value) { this->output_units = value; }

            const std::vector<Mapping>& get_mapping() const { return mapping; }
            std::vector<Mapping>& get_mutable_mapping() { return mapping; }
            void set_mapping(const std::vector<Mapping>& value) { this->mapping = value; }
        };

        class Adapters {
        public:
            Adapters() = default;
            virtual ~Adapters() = default;

        private:
            int64_t hash;
            std::vector<Unit> units;

        public:
            const int64_t& get_hash() const { return hash; }
            int64_t& get_mutable_hash() { return hash; }
            void set_hash(const int64_t& value) { this->hash = value; }

            const std::vector<Unit>& get_units() const { return units; }
            std::vector<Unit>& get_mutable_units() { return units; }
            void set_units(const std::vector<Unit>& value) { this->units = value; }
        };
    }
}

#endif // _ADAPTERS_HPP_
