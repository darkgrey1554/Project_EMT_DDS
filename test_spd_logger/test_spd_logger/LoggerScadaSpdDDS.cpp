#if defined(LOGGER_ATECH_SCADA_SPDLOG_DDS)

#include "LoggerScadaSpdDDS.h"

namespace atech::logger
{
	std::shared_ptr<spdlog::logger> LoggerScadaSpdDds::_logger{};
	std::mutex 	LoggerScadaSpdDds::_guarden{};

	template<typename Mutex> void dds_sink<Mutex>::sink_it_(const spdlog::details::log_msg& msg)
	{
		DdsStatus _status;
		if (_datawriter)
		{
			_status.id_source(_node_id);
			_status.id_target(static_cast<uint32_t>(atech::common::Service::HS) << 4);
			_status.cmd_code(static_cast<uint32_t>(atech::common::Command::LOGGING));
			_status.st_code(static_cast<uint32_t>(loglevel_to_status_code(msg.level)));
			_status.st_time(std::chrono::time_point_cast<std::chrono::microseconds>(msg.time).time_since_epoch().count());
			auto& array = _status.st_desc();
			auto& str = msg.payload;
			//std::strncpy(&(*array.begin()), &(*msg.payload.begin()), array.size() < msg.payload.size() ? array.size() : msg.payload.size());

			size_t s = array.size() < msg.payload.size() ? array.size() : msg.payload.size();
			for (int i = 0; i < s; i++)
			{
				array[i] = str[i];
			}
			_datawriter->write(&_status);
		}
	}

	template<typename Mutex> void dds_sink<Mutex>::flush_()
	{
	}

	template<typename Mutex> void dds_sink<Mutex>::set_datawriter(eprosima::fastdds::dds::DataWriter* dw)
	{
		std::lock_guard<Mutex> lock(this->mutex_);
		_datawriter = dw;
	};

	template<typename Mutex> void dds_sink<Mutex>::set_node_id(const uint32_t& node)
	{
		std::lock_guard<Mutex> lock(this->mutex_);
		_node_id = node;
	}

	template<typename Mutex> eprosima::fastdds::dds::DataWriter* dds_sink<Mutex>::get_datawriter()
	{
		std::lock_guard<Mutex> lock(this->mutex_);
		return _datawriter;
	};

	template<typename Mutex> void dds_sink<Mutex>::clear_datawriter()
	{
		std::lock_guard<Mutex> lock(this->mutex_);
		_datawriter = nullptr;
	};

	template<typename Mutex> atech::common::Status dds_sink<Mutex>::loglevel_to_status_code(const spdlog::level::level_enum& level) const
	{
		atech::common::Status st;

		switch (level)
		{
		case spdlog::level::level_enum::debug:
			st = atech::common::Status::DEBUG;
			break;
		case spdlog::level::level_enum::info:
			st = atech::common::Status::INFO;
			break;
		case spdlog::level::level_enum::warn:
			st = atech::common::Status::WARNING;
			break;
		case spdlog::level::level_enum::err:
			st = atech::common::Status::ERR;
			break;
		case spdlog::level::level_enum::critical:
			st = atech::common::Status::CRITICAL;
			break;
		default:
			st = atech::common::Status::CRITICAL;
			break;
		}

		return st;
	}

	
	
	
	void LoggerScadaSpdDds::call_info_logger(std::string& str)
	{
		std::shared_ptr<spdlog::logger> log{ std::atomic_load(&_logger) };
		if (log)
		{
			log->info(str);
		}
	}

	void LoggerScadaSpdDds::call_debug_logger(std::string& str)
	{
		std::shared_ptr<spdlog::logger> log{ std::atomic_load(&_logger) };
		if (log)
		{
			log->debug(str);
		}
	}

	void LoggerScadaSpdDds::call_warning_logger(std::string& str)
	{
		std::shared_ptr<spdlog::logger> log{ std::atomic_load(&_logger) };
		if (log)
		{
			log->warn(str);
		}
	}

	void LoggerScadaSpdDds::call_critical_logger(std::string& str)
	{
		std::shared_ptr<spdlog::logger> log{ std::atomic_load(&_logger) };
		if (log)
		{
			log->critical(str);
		}
	}


	ResultRequest LoggerScadaSpdDds::SetLevel(const LevelLog& level)
	{
		ResultRequest result{ ResultRequest::OK };

		try
		{
			std::shared_ptr<spdlog::logger> log{ std::atomic_load(&_logger) };
			if (log)
			{
				log->set_level(log_to_spdlog(level));
			}
			else
			{
				result = ResultRequest::IGNOR;
			}
		}
		catch (...)
		{
			result = ResultRequest::ERR;
		}

		return result;
	}

	ResultRequest LoggerScadaSpdDds::ClearLogger()
	{
		ResultRequest result{ ResultRequest::OK };
		std::lock_guard<std::mutex> lock(_guarden);

		try
		{
			std::shared_ptr<spdlog::logger> log{ std::atomic_load(&_logger) };
			if (log)
			{
				std::shared_ptr<spdlog::logger> l = nullptr;
				std::atomic_store(&_logger, l);
				spdlog::drop(name_log);
			}
			else
			{
				result = ResultRequest::IGNOR;
			}

			//std::this_thread::sleep_for(std::chrono::seconds(1));
		}
		catch (...)
		{
			result = ResultRequest::ERR;
		} 	

		return result;
	}

	ResultRequest LoggerScadaSpdDds::Init(std::shared_ptr<ConfigLogger> config)
	{
		ResultRequest result = ResultRequest::OK;
		std::lock_guard<std::mutex> lock(_guarden);	

		try
		{
			auto config_spddds = std::dynamic_pointer_cast<ConfigLoggerSpdDds>(config);
			if (!config_spddds)	throw 1;
			if (std::atomic_load(&_logger)) throw 2;
			
			if (config_spddds->file_name.empty()) config_spddds->file_name = "logdef";
			if (config_spddds->file_size == 0) config_spddds->file_size = 10;
			if (config_spddds->file_size > 100) config_spddds->file_size = 100;
			if (config_spddds->file_number > 10) config_spddds->file_number = 10;

			std::string helpstr;
			helpstr.clear();
			if (!config_spddds->file_path.empty())
			{
				#if defined(_WIN32)
					helpstr = config_spddds->file_path + "\\";
				#elif defined(__linux__)
					helpstr = config_spddds->file_path + "/";
				#endif				
			}
			helpstr += config_spddds->file_name;

			std::vector<spdlog::sink_ptr> sinks;
			//sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
			sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(helpstr, config_spddds->file_size*1024*1024, config_spddds->file_number));
			sinks.push_back(std::make_shared<dds_sink_mt>(config_spddds->datawriter_ptr));
			

			auto combined_logger = std::make_shared<spdlog::logger>(name_log, begin(sinks), end(sinks));
			combined_logger->set_level(log_to_spdlog(config_spddds->level));
			combined_logger->set_pattern("[%d/%m/%C %H:%M:%S.%e] [%^%l%$]\t%v");
			spdlog::register_logger(combined_logger);
			std::atomic_store(&_logger, spdlog::get(name_log));
		}
		catch (...)
		{
			result = ResultRequest::ERR;
		}

		return result;
	}

	ResultRequest LoggerScadaSpdDds::SetDataWriterDDS(eprosima::fastdds::dds::DataWriter* dw)
	{
		ResultRequest result{ ResultRequest::OK };

		try
		{
			std::shared_ptr<spdlog::logger> log{ std::atomic_load(&_logger) };
			if (log)
			{
				auto& asd = *log->sinks().rbegin();
				auto dds_log = std::dynamic_pointer_cast<dds_sink_mt>(asd);
				if (dds_log)
				{
					dds_log->set_datawriter(dw);
				}
				else 
				{
					throw 1;
				} 				
			}
			else
			{
				result = ResultRequest::IGNOR;
			}
		}
		catch (...)
		{
			result = ResultRequest::ERR;
		}

		return result;
	}

	eprosima::fastdds::dds::DataWriter* LoggerScadaSpdDds::GetDataWriterDDS() const
	{
		eprosima::fastdds::dds::DataWriter* result{nullptr};

		try
		{
			std::shared_ptr<spdlog::logger> log{ std::atomic_load(&_logger) };
			if (log)
			{
				auto& asd = *log->sinks().rbegin();
				auto dds_log = std::dynamic_pointer_cast<dds_sink_mt>(asd);
				if (dds_log)
				{
					result = dds_log->get_datawriter();
				}
			}
		}
		catch (...)
		{
		}

		return result;
	}

	ResultRequest  LoggerScadaSpdDds::ClearDataWriterDDS()
	{
		ResultRequest result{ ResultRequest::OK };

		try
		{
			std::shared_ptr<spdlog::logger> log{ std::atomic_load(&_logger) };
			if (log)
			{
				auto& asd = *log->sinks().rbegin();
				auto dds_log = std::dynamic_pointer_cast<dds_sink_mt>(asd);
				if (dds_log)
				{
					dds_log->set_datawriter(nullptr);
				}
			}
			else
			{
				result = ResultRequest::IGNOR;
			}
		}
		catch (...)
		{
			result = ResultRequest::ERR;
		}

		return result;
	}

	spdlog::level::level_enum LoggerScadaSpdDds::log_to_spdlog(const LevelLog& level)
	{
		spdlog::level::level_enum lspd{ spdlog::level::level_enum::debug};
		switch (level)
		{
		case LevelLog::DEBUG:
			lspd = spdlog::level::level_enum::debug;
			break;
		case LevelLog::INFO:
			lspd = spdlog::level::level_enum::info;
			break;
		case LevelLog::WARNING:
			lspd = spdlog::level::level_enum::warn;
			break;
		case LevelLog::ERR:
			lspd = spdlog::level::level_enum::err;
			break;
		case LevelLog::CRITICAL:
			lspd = spdlog::level::level_enum::critical;
			break;
		default:
			lspd = spdlog::level::level_enum::debug;
			break;			 
		}

		return lspd;
	}
}	

#endif