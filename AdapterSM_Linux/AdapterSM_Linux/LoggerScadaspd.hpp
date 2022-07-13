#include "LoggerScada.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"


namespace LoggerSpaceScada
{
	namespace LoggerSpd
	{
		class LoggerScadaSpd : public ILoggerScada
		{
		protected:

			void call_info_logger(std::string str);
			void call_debug_logger(std::string str);
			void call_warning_logger(std::string str);
			void call_critical_logger(std::string str);		

			LoggerScadaSpd() = delete;			

			std::string namelog = "LoggerScadaSpd";
			static std::mutex mutex_lock;
			std::shared_ptr<spdlog::logger> spd_logger;
			spdlog::level::level_enum level_to_spdlevel(LevelLog level);

		public:

			LoggerScadaSpd(ConfigLogger config);
			~LoggerScadaSpd();

			ResultReqest TurnOFF(TurnLog turn) override;
			ResultReqest TurnON(TurnLog turn) override;
			ResultReqest SetName(std::string str) override;
			ResultReqest SetPath(std::string str) override;
			ResultReqest SetLevel(LevelLog level) override;
			ResultReqest SetFileSize(size_t size_MB) override;
			ResultReqest SetFileNumber(size_t number) override;
		
		};
	}
}