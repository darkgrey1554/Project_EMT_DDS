#pragma once

#include "structs.h"
#include <vector>
#include "logger.h"

class ConfigReaderDDS
{
	
	LoggerSpace::Logger* log = LoggerSpace::Logger::getpointcontact();

public:

	std::vector<ConfigSubscriber> SubscribersDDS;
	std::vector<ConfigPublisher> PublishersDDS;



	ResultReqest ReadConfigDDS(std::string file_name = "ConfigDDS.txt");
	ResultReqest ReadConfigDDS_Subscribers(std::string file_name = "ConfigDDS.txt");
	ResultReqest ReadConfigDDS_Publishers(std::string file_name = "ConfigDDS.txt");
	//ResultReqest ReadKKSListIn(std::string file_name = "ListKKSIn.txt");
	//ResultReqest ReadKKSListOut(std::string file_name = "ListKKSOut.txt");

};