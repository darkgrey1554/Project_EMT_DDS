#pragma once
#ifndef _TOPICS_HPP_
#define _TOPICS_HPP_

#include <string>
#include <vector>
#include <memory>

namespace atech {
    namespace common {

        class TypeSize {
        public:
            TypeSize() = default;
            virtual ~TypeSize() = default;

        private:
            std::string type_name;
            size_t size;

        public:
            const std::string& get_type_name() const { return type_name; }
            std::string& get_mutable_type_name() { return type_name; }
            void set_type_name(const std::string& value) { this->type_name = value; }

            const size_t& get_size() const { return size; }
            size_t& get_mutable_size() { return size; }
            void set_size(const size_t& value) { this->size = value; }
        };


        class DdsTypeSize {
        public:
            DdsTypeSize() = default;
            virtual ~DdsTypeSize() = default;

        private:
            std::string type_name;
            std::vector<TypeSize> type_sizes;

        public:
            const std::string& get_type_name() const { return type_name; }
            std::string& get_mutable_type_name() { return type_name; }
            void set_type_name(const std::string& value) { this->type_name = value; }

            const std::vector<TypeSize>& get_type_sizes() const { return type_sizes; }
            std::vector<TypeSize>& get_mutable_type_sizes() { return type_sizes; }
            void set_type_sizes(const std::vector<TypeSize>& value) { this->type_sizes = value; }
        };


        class TopicMaxSize {
        public:
            TopicMaxSize() = default;
            virtual ~TopicMaxSize() = default;

        private:
            int64_t hash;
            std::vector<DdsTypeSize> dds_type_size;

        public:
            const int64_t& get_hash() const { return hash; }
            int64_t& get_mutable_hash() { return hash; }
            void set_hash(const int64_t& value) { this->hash = value; }

            const std::vector<DdsTypeSize>& get_dds_type_size() const { return dds_type_size; }
            std::vector<DdsTypeSize>& get_mutable_dds_type_size() { return dds_type_size; }
            void set_dds_type_size(const std::vector<DdsTypeSize>& value) { this->dds_type_size = value; }
        };

    }
}

#endif // _TOPICS_HPP_