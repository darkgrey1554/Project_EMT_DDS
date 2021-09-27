#include <iostream>
#include "Config_Reader.h"
#include <memory>
#include <ctime>
#include <chrono>
#include "KKS_Reader.h"
#include "DDSUnit.h"
#include "logger.h"
#include "Config_Reader.h"


int main(int argc, char** argv)
{
	LoggerSpace::Logger* log = LoggerSpace::Logger::getpointcontact();
	log->TurnOnLog();

	ConfigReaderDDS* ConfReader = new ConfigReaderDDS();



	ConfigGate r;
	std::vector<ConfigDDSUnit> rr;

	ConfReader->ReadConfigGate(r);
	ConfReader->ReadConfigTransferUnits(rr);

	ConfigDDSUnit config_sub;
	ConfigDDSUnit config_pub;

	config_pub.Domen = 0;
	config_pub.IP_MAIN =  "127.0.0.1";
	config_pub.IP_RESERVE = "127.0.0.1";
	
	
	config_pub.Port_MAIN = 30000;
	config_pub.Port_RESERVE = 30001;

	config_sub.Domen = 0;
	config_sub.IP_MAIN = "127.0.0.1";
	config_sub.IP_RESERVE = "127.0.0.1";
	
	
	config_sub.Port_MAIN = 40000;
	config_sub.Port_RESERVE = 40001;

	DDSUnit* pub = CreateDDSUnit(TypeDDSUnit::PUBLISHER, config_pub);
	DDSUnit* sub = CreateDDSUnit(TypeDDSUnit::SUBSCRIBER, config_sub);
	
	/*HelloWorldPublisher mypub;
	if (mypub.init())
	{
		mypub.run(10, 100);
	}*/

	/*HelloWorldSubscriber mysub;
	if (mysub.init())
	{
		mysub.run();
	}*/

	/*if (argc > 1)
	{
		HelloWorldPublisher mypub;
		if (mypub.init())
		{
			mypub.run(10, 100);
		}
	}
	else
	{
		HelloWorldSubscriber mysub;
		if (mysub.init())
		{
			mysub.run();
		}
	}*/
		
	int asd = 0;

	while (1)
	{
		asd++;
		std::this_thread::sleep_for(1000ms);
	}
	

	//ConfigReaderDDS* conf = new ConfigReaderDDS();
	//conf->ReadConfigDDS();

	std::cout << "Hello" <<std::endl;

}
