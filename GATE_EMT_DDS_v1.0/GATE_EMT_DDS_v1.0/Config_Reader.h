#pragma once

#include "structs.h"
#include <vector>
#include "logger.h"
#include <iostream>
#include <fstream>
#include "rapidjson/document.h"

template <class T>
class ConfigReader
{
	virtual ResultReqest WriteConfigFile() = 0;
public:
	virtual ResultReqest ReadConfigFile() = 0;
	virtual ResultReqest ReadConfigSpecial() = 0;
	virtual ResultReqest GetResult(std::vector<T>& vector_result) = 0;
	virtual ResultReqest SetNameConfigFile(std::string name) = 0;
	
};


class ConfigReaderDDS : public ConfigReader<ConfigDDSUnit>
{
	ResultReqest WriteConfigFile();
	LoggerSpace::Logger* log = LoggerSpace::Logger::getpointcontact();
	ConfigGate config;
	std::vector<ConfigDDSUnit> vector_data;
	std::string name_config = "config.json";
	std::string name_configunits = "configunits.json";

public:

	ConfigReaderDDS();
	ResultReqest ReadConfigFile();
	ResultReqest ReadConfigSpecial();
	ResultReqest GetResult(std::vector<ConfigDDSUnit>& vector_result);
	ResultReqest SetNameConfigFile(std::string name);

	//ResultReqest ReadConfigDDS(std::string file_name = "ConfigDDS.txt");
	//ResultReqest ReadConfigDDS_Subscribers(std::string file_name = "ConfigDDS.txt");
	//ResultReqest ReadConfigDDS_Publishers(std::string file_name = "ConfigDDS.txt");
};

ConfigReader<ConfigDDSUnit>* CreateConfigReader(Type_Gate type);
