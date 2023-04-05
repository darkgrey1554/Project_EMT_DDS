#pragma once
#include <string>
#include <memory>
#include <fmt/fmt.h>

namespace atech::logger
{	
	enum class TypeLogger
	{
		NONE,
		SPDDDS
	};

	enum class ResultRequest
	{
		OK,
		ERR,
		IGNOR
	};

	enum class LevelLog
	{
		INFO,
		DEBUG,
		WARNING,
		ERR,
		CRITICAL
	};

	struct ConfigLogger
	{
		LevelLog level;
		virtual ~ConfigLogger() = default;
	};

	class ILoggerScada;

	using ILoggerScada_ptr = std::shared_ptr<ILoggerScada>;

	class ILoggerScada
	{
		protected:

		static std::atomic<TypeLogger> default_type_logger;
		virtual void call_info_logger(std::string& str) = 0;
		virtual void call_debug_logger(std::string& str) = 0;
		virtual void call_warning_logger(std::string& str) = 0;
		virtual void call_critical_logger(std::string& str) = 0;

		ILoggerScada() = default;
		virtual ~ILoggerScada() = default;

		public:

		template<typename... Args> void Info(std::string_view str, Args&&... args)
		{
			std::string helpstr = fmt::format(str, args...);
			call_info_logger(helpstr);
			return;
		}

		template<typename... Args> void Debug(std::string_view str, Args&&... args)
		{
			std::string helpstr = fmt::format(str, args...);
			call_debug_logger(helpstr);
			return;
		}

		template<typename... Args> void Warning(std::string_view str, Args&&... args)
		{
			std::string helpstr = fmt::format(str, args...);
			call_warning_logger(helpstr);
			return;
		}

		template<typename... Args> void Error(std::string_view str, Args&&... args)
		{
			std::string helpstr = fmt::format(str, args...);
			call_critical_logger(helpstr);
			return;
		}

		template<typename... Args> void Critical(std::string_view str, Args&&... args)
		{
			std::string helpstr = fmt::format(str, args...);
			call_critical_logger(helpstr);
			return;
		}

		static ResultRequest SetTypeDefaultLogger(TypeLogger type);
		static TypeLogger GetTypeDefaultLogger();
		static ILoggerScada_ptr GetInstance(TypeLogger type = TypeLogger::NONE);

		virtual ResultRequest SetLevel(const LevelLog& level) = 0;
		virtual ResultRequest ClearLogger() = 0;
		virtual ResultRequest Init(std::shared_ptr<ConfigLogger> config) = 0;
	};

	
}