#pragma once
#include <LoggerScadaInterface.h>


namespace atech::logger
{
	struct ConfigLoggerNone : public ConfigLogger
	{
	};

	class LoggerScadaNone : public ILoggerScada
	{
		void call_info_logger(std::string& str) override;
		void call_debug_logger(std::string& str) override;
		void call_warning_logger(std::string& str) override;
		void call_critical_logger(std::string& str) override;

	public:

		ResultRequest SetLevel(const LevelLog& level) override;
		ResultRequest ClearLogger() override;
		ResultRequest Init(std::shared_ptr<ConfigLogger> config) override;

	};

	using LoggerScadaNone_ptr = std::shared_ptr<LoggerScadaNone>;
}

