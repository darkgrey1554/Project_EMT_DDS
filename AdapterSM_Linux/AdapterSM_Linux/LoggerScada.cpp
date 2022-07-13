#include "LoggerScada.hpp"
#include "LoggerScadaspd.hpp"

namespace LoggerSpaceScada
{
	ILoggerScada::ILoggerScada() { return; };
	ILoggerScada::~ILoggerScada() { return; };	

	std::shared_ptr<ILoggerScada> GetLoggerScada(TypeLogger type, ConfigLogger config)
	{
		if (type == TypeLogger::SPDLOG)
		{
			return std::make_shared<LoggerSpd::LoggerScadaSpd>(LoggerSpaceScada::LoggerSpd::LoggerScadaSpd(config));
		}
		
		return nullptr;
	}
}