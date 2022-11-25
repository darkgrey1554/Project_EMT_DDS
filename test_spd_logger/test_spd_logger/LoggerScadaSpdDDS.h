#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/base_sink.h"
#include "spdlog/details/null_mutex.h"
#include "LoggerScada.h"
#include <mutex>
#include <atomic>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include "DdsStatus.h"
#include "ServiceType.h"
#include <string.h>

namespace atech::logger
{
	template<typename Mutex> class dds_sink;
	using dds_sink_mt = dds_sink<std::mutex>;
	using dds_sink_st = dds_sink<spdlog::details::null_mutex>;

	template<typename Mutex>
	class dds_sink : public spdlog::sinks::base_sink<Mutex>
	{

		eprosima::fastdds::dds::DataWriter* _datawriter = nullptr;
		uint32_t _node_id = 0;
        void sink_it_(const spdlog::details::log_msg& msg) override;
		void flush_() override;
		atech::common::Status loglevel_to_status_code(const spdlog::level::level_enum& level) const;

	public:
		explicit dds_sink(eprosima::fastdds::dds::DataWriter* _ptr) : _datawriter(_ptr)
		{};
		void set_datawriter(eprosima::fastdds::dds::DataWriter* dw);
		void set_node_id(const uint32_t& node);
		eprosima::fastdds::dds::DataWriter* get_datawriter();
		void clear_datawriter();
	};

	struct ConfigLoggerSpdDds : public ConfigLogger
	{
		std::string file_name;
		std::string file_path;
		size_t file_size;
		size_t file_number;
		eprosima::fastdds::dds::DataWriter* datawriter_ptr;		
	};

	class LoggerScadaSpdDds : public ILoggerScada
	{
		static std::shared_ptr<spdlog::logger> _logger;
		static std::mutex _guarden;
		std::string name_log{ "log_spd_dds" };
		
		void call_info_logger(std::string& str) override;
		void call_debug_logger(std::string& str) override;
		void call_warning_logger(std::string& str) override;
		void call_critical_logger(std::string& str) override;

		spdlog::level::level_enum log_to_spdlog(const LevelLog& level);

	public:

		virtual ResultRequest SetLevel(const LevelLog& level) override;
		virtual ResultRequest ClearLogger() override;
		virtual ResultRequest Init(std::shared_ptr<ConfigLogger> config) override;
		ResultRequest SetDataWriterDDS(eprosima::fastdds::dds::DataWriter* dw);
		eprosima::fastdds::dds::DataWriter* GetDataWriterDDS() const;
		ResultRequest ClearDataWriterDDS();
	};
}
