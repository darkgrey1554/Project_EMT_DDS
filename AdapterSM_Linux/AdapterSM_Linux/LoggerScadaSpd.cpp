#include "LoggerScadaspd.hpp"

namespace LoggerSpaceScada
{
	namespace LoggerSpd
	{
		std::mutex LoggerScadaSpd::mutex_lock;

		LoggerScadaSpd::LoggerScadaSpd(ConfigLogger config) 
		{
			try 
			{
				/// --- singleton --- ///
				spd_logger = spdlog::get(namelog);
				if (spd_logger != nullptr) return;
				std::lock_guard<std::mutex> lock(mutex_lock);
				spd_logger = spdlog::get(namelog);
				if (spd_logger != nullptr) return;

				/// --- проверка config --- ///
				if (config.file_mame.empty()) config.file_mame = "logdef";
				if (config.size_file == 0) config.size_file = 10;
				if (config.size_file > 500) config.size_file = 500;
				if (config.numbers_file == 0) config.numbers_file = 1;

				std::vector<spdlog::sink_ptr> sinks;
				std::string helpstr;

				sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());

				helpstr.clear();
				if (config.file_path.empty())
				{
					helpstr = config.file_mame;
				}
				else
				{
					helpstr = config.file_path + "\\" + config.file_mame;
				}

				size_t size_file = config.size_file * 1024 * 1024;
				sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(helpstr, size_file, config.numbers_file));

				auto combined_logger = std::make_shared<spdlog::logger>(namelog, begin(sinks), end(sinks));

				combined_logger->set_level(level_to_spdlevel(config.level));

				combined_logger->set_pattern("[%d/%m/%C %H:%M:%S.%e] [%^%l%$]\t%v");

				spdlog::register_logger(combined_logger);

				spd_logger = spdlog::get(namelog);

			}
			catch (...)
			{
			}
		};

		LoggerScadaSpd::~LoggerScadaSpd() { return; };

		void LoggerScadaSpd::call_info_logger(std::string str)
		{
			try
			{
				if (spd_logger != nullptr) spd_logger->info(str);
			}
			catch (...)
			{
			}

		}

		void LoggerScadaSpd::call_debug_logger(std::string str)
		{
			try
			{
				if (spd_logger != nullptr) spd_logger->debug(str);
			}
			catch (...)
			{
			}
		}

		void LoggerScadaSpd::call_warning_logger(std::string str)
		{
			try
			{
				if (spd_logger != nullptr) spd_logger->warn(str);
			}
			catch (...)
			{
			}
		}

		void LoggerScadaSpd::call_critical_logger(std::string str)
		{
			try
			{
				if (spd_logger != nullptr) spd_logger->critical(str);
			}
			catch (...)
			{
			}
		}

		ResultReqest LoggerScadaSpd::TurnOFF(TurnLog turn)
		{ 		
			return ResultReqest::IGNOR; 
		};

		ResultReqest LoggerScadaSpd::TurnON(TurnLog turn)
		{ 
			return ResultReqest::IGNOR; 
		};

		ResultReqest LoggerScadaSpd::SetName(std::string str) 
		{ 
			return ResultReqest::IGNOR; 
		};

		ResultReqest LoggerScadaSpd::SetPath(std::string str) 
		{ 
			return ResultReqest::IGNOR; 
		};

		ResultReqest LoggerScadaSpd::SetLevel(LevelLog level) 
		{ 
			ResultReqest res = ResultReqest::OK;
			try
			{
				spd_logger->set_level(level_to_spdlevel(level));
			}
			catch (...)
			{
				res = ResultReqest::ERR;
			}
			return res;
		};

		ResultReqest LoggerScadaSpd::SetFileSize(size_t size_MB)
		{ 
			return ResultReqest::IGNOR; 
		};

		ResultReqest LoggerScadaSpd::SetFileNumber(size_t number)
		{ 
			return ResultReqest::IGNOR; 
		};

		spdlog::level::level_enum LoggerScadaSpd::level_to_spdlevel(LevelLog level)
		{
			if (level == LevelLog::Debug) return spdlog::level::debug;
			if (level == LevelLog::Info) return spdlog::level::info;
			if (level == LevelLog::Warning) return spdlog::level::warn;
			if (level == LevelLog::Critical) return spdlog::level::critical;

			return spdlog::level::debug;
		}
	}
}