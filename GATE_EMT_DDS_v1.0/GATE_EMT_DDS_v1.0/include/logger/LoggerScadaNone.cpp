#include "LoggerScadaNone.h"
#include <string.h>

namespace atech::logger
{
	
	void LoggerScadaNone::call_info_logger(std::string& str)
	{
	}

	void LoggerScadaNone::call_debug_logger(std::string& str)
	{
	}

	void LoggerScadaNone::call_warning_logger(std::string& str)
	{
	}

	void LoggerScadaNone::call_critical_logger(std::string& str)
	{
	}


	ResultRequest LoggerScadaNone::SetLevel(const LevelLog& level)
	{
		ResultRequest result{ ResultRequest::OK };
		return result;
	}

	ResultRequest LoggerScadaNone::ClearLogger()
	{
		ResultRequest result{ ResultRequest::OK };
		return result;
	}

	ResultRequest LoggerScadaNone::Init(std::shared_ptr<ConfigLogger> config)
	{
		ResultRequest result = ResultRequest::OK;
		return result;
	}
}
