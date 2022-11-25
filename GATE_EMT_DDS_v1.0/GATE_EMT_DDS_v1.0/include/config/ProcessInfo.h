#pragma once
#ifndef _PROCESS_INFO_HPP_
#define _PROCESS_INFO_HPP_


#include <string>
#include <vector>
#include <memory>


namespace atech {
    namespace common {

        class ProcessInfo {
        public:
            ProcessInfo() = default;
            virtual ~ProcessInfo() = default;

        private:
        
            std::string hname;
            int64_t pid;
            std::string pname;
            std::string pparam;
            int64_t pmemory;
            double pcpu;
             int64_t state;
            std::string pinfo;

        public:
            const std::string& get_hname() const { return hname; }
            std::string& get_mutable_hname() { return hname; }
            void set_hname(const std::string& value) { this->hname = value; }

            const int64_t& get_pid() const { return pid; }
            int64_t& get_mutable_pid() { return pid; }
            void set_pid(const int64_t& value) { this->pid = value; }

            const std::string& get_pname() const { return pname; }
            std::string& get_mutable_pname() { return pname; }
            void set_pname(const std::string& value) { this->pname = value; }
                    

            const std::string& get_pparam() const { return pparam; }
            std::string& get_mutable_pparam() { return pparam; }
            void set_pparam(const std::string& value) { this->pparam = value; }

            const int64_t& get_pmemory() const { return pmemory; }
            int64_t& get_mutable_pmemory() { return pmemory; }
            void set_pmemory(const int64_t& value) { this->pmemory = value; }

            const double& get_pcpu() const { return pcpu; }
            double& get_mutable_pcpu() { return pcpu; }
            void set_pcpu(const double& value) { this->pcpu = value; }
                      
            const int64_t& get_state() const { return state; }
            int64_t& get_mutable_state() { return state; }
            void set_state(const int64_t& value) { this->state = value; }

            const std::string& get_pinfo() const { return pinfo; }
            std::string& get_mutable_pinfo() { return pinfo; }
            void set_pinfo(const std::string& value) { this->pinfo = value; }
        };

    } //namespace common

} //namespace atech

#endif // _PROCESS_INFO_HPP_

