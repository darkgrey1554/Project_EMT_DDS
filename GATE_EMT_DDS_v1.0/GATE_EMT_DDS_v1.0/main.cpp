#include <iostream>
#include "Config_Reader.h"
#include <memory>
#include <ctime>
#include <chrono>
#include "KKS_Reader.h"
#include "DDSUnit.h"
#include "logger.h"

char* c =new char(10);

std::unique_ptr<void*> f()
{
	
	std::unique_ptr<void*> v = std::make_unique<void*>((void*)c);
	return std::move(v);
}


int main()
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

	
	DDSUnit* sub = CreateDDSUnit(TypeDDSUnit::SUBSCRIBER, config_sub);
	DDSUnit* pub = CreateDDSUnit(TypeDDSUnit::PUBLISHER, config_pub);
	
	int asd = 0;

	while (1)
	{
		asd++;
		std::this_thread::sleep_for(1000ms);
	}

	for (int i = 0; i < 10; i++)
	{
		c[i] = i+0x30;
	}

	std::unique_ptr<void*> a = f();

	char* s = (char*)*a.get();

	std::cout << *s << std::endl;
	std::cout << *(s+1) << std::endl;


	ConfigReaderDDS* conf = new ConfigReaderDDS();
	conf->ReadConfigDDS();

	std::cout << "Hello" <<std::endl;

}
