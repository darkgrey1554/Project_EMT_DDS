#include <iostream>
#include "Config_Reader.h"
#include <memory>
#include <ctime>
#include <chrono>
#include "KKS_Reader.h"
#include "DDSUnit.h"
#include "logger.h"

#include "HelloWorldPublisher.h"
#include "HelloWorldSubscriber.h"

//#include <fastrtps/log/Log.h>

//using eprosima::fastdds::dds::Log;

/*int main(int argc, char** argv)
{
	std::cout << "Starting " << std::endl;
	int type = 1;
	int count = 5;
	long sleep = 100;
	if (argc > 1)
	{
		if (strcmp(argv[1], "publisher") == 0)
		{
			type = 1;
			if (argc >= 3)
			{
				count = atoi(argv[2]);
				if (argc == 4)
				{
					sleep = atoi(argv[3]);
				}
			}
		}
		else if (strcmp(argv[1], "subscriber") == 0)
			type = 2;
	}
	else
	{
		std::cout << "publisher OR subscriber argument needed" << std::endl;
		Log::Reset();
		return 0;
	}

	switch (type)
	{
	case 1:
	{
		HelloWorldPublisher mypub;
		if (mypub.init())
		{
			mypub.run(count, sleep);
		}
		break;
	}
	case 2:
	{
		HelloWorldSubscriber mysub;
		if (mysub.init())
		{
			mysub.run();
		}
		break;
	}
	}
	Log::Reset();
	return 0;
}*/


int main(int argc, char** argv)
{
	LoggerSpace::Logger* log = LoggerSpace::Logger::getpointcontact();

	log->TurnOnLog();

	ConfigDDSUnit config_sub;
	ConfigDDSUnit config_pub;

	config_pub.Domen = 0;
	config_pub.IP_MAIN =  "127.0.0.1";
	config_pub.IP_RESERVE = "127.0.0.1";
	config_pub.NameListKKS = "List_KKS_Out.txt";
	config_pub.NameMemory = "sm001";
	config_pub.Port_MAIN = 30000;
	config_pub.Port_RESERVE = 30001;

	config_sub.Domen = 0;
	config_sub.IP_MAIN = "127.0.0.1";
	config_sub.IP_RESERVE = "127.0.0.1";
	config_sub.NameListKKS = "List_KKS_In.txt";
	config_sub.NameMemory = "sm001";
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
	

	ConfigReaderDDS* conf = new ConfigReaderDDS();
	conf->ReadConfigDDS();

	std::cout << "Hello" <<std::endl;

}
