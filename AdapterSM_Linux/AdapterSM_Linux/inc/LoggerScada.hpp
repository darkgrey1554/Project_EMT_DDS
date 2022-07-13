#pragma once
#include <string>
#include <memory>
#include <string_view>
//#include <format>
#include <iostream>
#include "spdlog/fmt/fmt.h"

namespace LoggerSpaceScada
{
	enum class TypeLogger
	{
		SPDLOG,
		LOGMy
	};

	enum class ResultReqest
	{
		OK,
		ERR,
		IGNOR
	};

	enum class LevelLog
	{
		Info,
		Debug,
		Warning,
		Critical
	};

	enum class TurnLog
	{
		ON,
		OFF
	};

	struct ConfigLogger
	{
		std::string file_mame;
		std::string file_path;
		size_t size_file;
		size_t numbers_file;
		LevelLog level;
	};

	class ILoggerScada
	{
	protected:

		virtual void call_info_logger(std::string str) = 0;
		virtual void call_debug_logger(std::string str) = 0;
		virtual void call_warning_logger(std::string str) = 0;
		virtual void call_critical_logger(std::string str) = 0;

		ILoggerScada();
		~ILoggerScada();

	public:

		template<typename... Args> void Info(std::string_view str, Args&&... args)
		{
			//std::string helpstr = std::vformat(str, std::make_format_args(args...));
			std::string helpstr = fmt::format(str,args...);
			call_info_logger(helpstr);
			return;
		}

		template<typename... Args> void Debug(std::string_view str, Args&&... args)
		{
			//std::string helpstr = std::vformat(str, std::make_format_args(args...));
			std::string helpstr = fmt::format(str, args...);
			call_debug_logger(helpstr);
			return;
		}

		template<typename... Args> void Warning(std::string_view str, Args&&... args)
		{
			//std::string helpstr = std::vformat(str, std::make_format_args(args...));
			std::string helpstr = fmt::format(str, args...);
			call_warning_logger(helpstr);
			return;
		}

		template<typename... Args> void Critical(std::string_view str, Args&&... args)
		{
			//std::string helpstr = std::vformat(str, std::make_format_args(args...));
			std::string helpstr = fmt::format(str, args...);
			call_critical_logger(helpstr);
			return;
		}

		virtual ResultReqest TurnOFF(TurnLog turn) = 0;
		virtual ResultReqest TurnON(TurnLog turn) = 0;
		virtual ResultReqest SetName(std::string str) = 0;
		virtual ResultReqest SetPath(std::string str) = 0;
		virtual ResultReqest SetLevel(LevelLog level) = 0;
		virtual ResultReqest SetFileSize(size_t size_MB) = 0;
		virtual ResultReqest SetFileNumber(size_t number) = 0;


	};

	std::shared_ptr<ILoggerScada> GetLoggerScada(TypeLogger type, ConfigLogger config = {});

}
