#include "LoggerScadaInterface.h"
#include "LoggerScadaNone.h"

#if defined(LOGGER_ATECH_SCADA_SPDLOG_DDS) 
	#include "LoggerScadaSpdDDS.h"
#endif

namespace atech::logger
{
	std::atomic<TypeLogger> ILoggerScada::default_type_logger{ TypeLogger::SPDDDS };

	ResultRequest ILoggerScada::SetTypeDefaultLogger(TypeLogger type)
	{
		ResultRequest result{ ResultRequest::OK };

		if (type == TypeLogger::NONE)
		{
			result = ResultRequest::ERR;
		}
		else
		{
			default_type_logger.store(type);
		}

		return ResultRequest::OK;
	}

	TypeLogger ILoggerScada::GetTypeDefaultLogger()
	{
		return default_type_logger.load();
	}

	ILoggerScada_ptr ILoggerScada::GetInstance(TypeLogger type)
	{
		if (type == TypeLogger::NONE) type = ILoggerScada::GetTypeDefaultLogger();
		if (type == TypeLogger::SPDDDS)
		{
			#if defined(LOGGER_ATECH_SCADA_SPDLOG_DDS) 
			return std::make_shared<LoggerScadaSpdDds>();
			#endif
		}

		return std::make_shared<LoggerScadaNone>();
	}
}