#pragma once
#include <string>
#include <memory>

namespace LoggerSpaceScada
{
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

	class LoggerScada
	{
	public:

		template<typename... Args> virtual void Init(std::string str, ...) = 0;
		virtual void Info(std::string str, ...) =0;
		virtual void Debug(std::string str, ...) =0;
		virtual void Warning(std::string str, ...) =0;
		virtual void Critical(std::string str, ...) =0;

		virtual ResultReqest TurnOFF(TurnLog turn) = 0;
		virtual ResultReqest TurnON(TurnLog turn) = 0;
		virtual ResultReqest SetName(std::string str) = 0;
		virtual ResultReqest SetPath(std::string str) = 0;
		virtual ResultReqest SetLevel(LevelLog level) = 0;
		virtual ResultReqest SetFileSize(unsigned int size_MB) = 0;
		virtual ResultReqest SetFileNumber(unsigned int number) = 0;

		LoggerScada() = default;
		~LoggerScada() = default;
	};

	//std::shared_ptr<LoggerScada> GetLoggerScada();
}
