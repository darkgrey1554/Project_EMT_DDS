#pragma once
#ifndef _LOGGER_CONFIG_HPP_
#define _LOGGER_CONFIG_HPP_


#include <string>
#include <vector>
#include <memory>


namespace atech {
    namespace common {

        class LoggerConfig {
        public:
            LoggerConfig() = default;
            virtual ~LoggerConfig() = default;

        private:
            std::string file_name;
            std::string file_path;
            size_t file_size;
            size_t file_number;
            size_t log_level;
            bool log_dds;
            size_t log_level_dds;

        public:
            const std::string& get_file_name() const { return file_name; }
            std::string& get_mutable_file_name() { return file_name; }
            void set_file_name(const std::string& value) { this->file_name = value; }

            const std::string& get_file_path() const { return file_path; }
            std::string& get_mutable_file_path() { return file_path; }
            void set_file_path(const std::string& value) { this->file_path = value; }

            const size_t& get_file_size() const { return file_size; }
            size_t& get_mutable_file_size() { return file_size; }
            void set_file_size(const size_t& value) { this->file_size = value; }

            const size_t& get_file_number() const { return file_number; }
            size_t& get_mutable_file_number() { return file_number; }
            void set_file_number(const size_t& value) { this->file_number = value; }

            const size_t& get_log_level() const { return log_level; }
            size_t& get_mutable_log_level() { return log_level; }
            void set_log_level(const size_t& value) { this->log_level = value; }

            const bool& get_log_dds() const { return log_dds; }
            bool& get_mutable_log_dds() { return log_dds; }
            void set_log_dds(const bool& value) { this->log_dds = value; }

            const size_t& get_log_level_dds() const { return log_level_dds; }
            size_t& get_mutable_log_level_dds() { return log_level_dds; }
            void set_log_level_dds(const size_t& value) { this->log_level_dds = value; }
        };


       
    } //namespace common

} //namespace atech

#endif // _LOGGER_CONFIG_HPP_

