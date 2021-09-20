#pragma once

#include "structs.h"
#include <vector>
#include "logger.h"
#include <iofstream>
#include <> 

template <class T>
class ConfigRader
{
	public
		virtual ResultReqest ReadConfigFile() = 0;
		virtual ResultReqest ReadConfigSpecial(void* info) = 0;
		virtual ResultReqest GetResult(std::vector<T>& vector_result) = 0;
		virtual ResultReqest SetNameConfigFile(std::string name) = 0;
		virtual ResultReqest WriteConfigFile(std::vector<T>& vector_data) = 0;
};

class ConfigReaderDDS : ConfigRader<ConfigDDSUnit>
{
	
	LoggerSpace::Logger* log = LoggerSpace::Logger::getpointcontact();
	std::vector<ConfigDDSUnit> vector_data;
	std::string name_config;

public:

	ResultReqest ReadConfigFile();
	ResultReqest ReadConfigSpecial(void* info);
	ResultReqest GetResult(std::vector<ConfigDDSUnit>& vector_result);
	ResultReqest SetNameConfigFile(std::string name);

	//ResultReqest ReadConfigDDS(std::string file_name = "ConfigDDS.txt");
	//ResultReqest ReadConfigDDS_Subscribers(std::string file_name = "ConfigDDS.txt");
	//ResultReqest ReadConfigDDS_Publishers(std::string file_name = "ConfigDDS.txt");
};

