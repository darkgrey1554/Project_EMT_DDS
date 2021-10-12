#include <iostream>
#include "Config_Reader.h"
#include <memory>
#include <ctime>
#include <chrono>
#include "KKS_Reader.h"
#include "logger.h"
#include "Config_Reader.h"
#include "Adapters.h"
#include <memory>
#include <typeinfo>
#include <bitset>;

using namespace std::chrono_literals;

struct A
{
	int first = 0;
};

struct B : public A
{
	int second = 0;
};

std::shared_ptr<A> fun()
{
	std::shared_ptr<B> as = std::make_unique<B>();
	as->first = 10;
	as->second = 20;
	return std::move(as);
}


int main(int argc, char** argv)
{



	std::shared_ptr<B> bb = nullptr;

	{
		std::shared_ptr<A> aa = fun();
		bb = std::reinterpret_pointer_cast<B>(aa);
		std::cout << bb.use_count() << std::endl;
		std::cout << bb->first << std::endl;
		std::cout << bb->second << std::endl;

	}
	std::cout << bb.use_count() << std::endl;
	

	///////////

	LoggerSpace::Logger* log = LoggerSpace::Logger::getpointcontact();
	log->TurnOnLog();

	ConfigReader* ConfReader = new ConfigReader();

	ConfigLogger l;
	ConfigLogger* ll;
	ConfigLogger lll;
	ConfigGate r;
	std::vector<ConfigDDSUnit> rr;

	ResultReqest res;

	int data_out[100];
	int data_in[100];

	ConfigSharedMemoryAdapter confwriter;
	ConfigSharedMemoryAdapter confreader;

	confwriter.DataType = TypeData::DISCRETE;
	confwriter.NameMemory = "sm001";
	confwriter.size = 100;

	confreader.DataType = TypeData::DISCRETE;
	confreader.NameMemory = "sm001";
	confreader.size = 100;

	gate::Adapter* reader = gate::CreateAdapter(TypeAdapter::SharedMemory);
	gate::Adapter* writer = gate::CreateAdapter(TypeAdapter::SharedMemory);


	res = writer->InitAdapter(&confwriter);
	if (res == ResultReqest::OK) { std::cout << "WRITER OK" << std::endl; }
	else { std::cout << "WRITER ERR" << std::endl; }
	
	res = reader->InitAdapter(&confreader);
	if (res == ResultReqest::OK) { std::cout << "READER OK" << std::endl; }
	else { std::cout << "READER ERR" << std::endl; }

	for (int i = 0; i < 100; i++)
	{
		data_out[i] = i;
		data_in[i] = 0;
	}

	std::shared_ptr<BaseAnswer> anc = nullptr;

	while (1)
	{
		res = writer->WriteData(data_out, 100);
		if (res == ResultReqest::OK) { std::cout << "WRITER WRITE OK" << std::endl; }
		else { std::cout << "WRITER WRITE ERR" << std::endl; }

		res = reader->ReadData(data_in, 100);
		if (res == ResultReqest::OK) { std::cout << "READER READ OK" << std::endl; }
		else { std::cout << "READER READ ERR" << std::endl; }

		std::cout << "DATA_OUT:\tDATA_IN:" << std::endl;
		std::cout << data_out[0] << "\t" << data_in[0] << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(1));

		for (int i = 0; i < 100; i++)
		{
			data_out[i]++;
		}

		anc = reader->GetInfoAdapter(ParamInfoAdapter::HeaderData);

		{
			std::shared_ptr<HeaderDataAnswerSM> anc1 = std::reinterpret_pointer_cast<HeaderDataAnswerSM>(anc);
			std::cout << "-------------HEADER-------------" << std::endl;
			std::cout << "counter read: " << (int) anc1->header.count_read <<std::endl;
			std::cout << "counter write: " << (int) anc1->header.count_write << std::endl;
			std::cout << "TimeLastRead: " << (int) anc1->header.TimeLastRead.h
				<< ":" << (int) anc1->header.TimeLastRead.m 
				<< ":" << (int) anc1->header.TimeLastRead.s
				<< "."  << (int) anc1->header.TimeLastRead.ms << std::endl;
			std::cout << "TimeLastWrite: " << (int)anc1->header.TimeLastWrite.h
				<< ":" << (int)anc1->header.TimeLastWrite.m
				<< ":" << (int)anc1->header.TimeLastWrite.s
				<< "." << (int)anc1->header.TimeLastWrite.ms << std::endl;
		}


	}




	if (ConfReader->ReadConfigLOGGER(l) == ResultReqest::OK)
	{
		log->WriteLogINFO("READ LOGGER SUCCESS");
	}
	else
	{
		log->WriteLogINFO("READ LOGGER FAIL");
	}

	if (ConfReader->ReadConfigGATE(r) == ResultReqest::OK)
	{
		log->WriteLogINFO("READ GATE SUCCESS");
	}
	else
	{
		log->WriteLogINFO("READ GATE FAIL");
	}

	if (ConfReader->ReadConfigTransferUnits(rr) == ResultReqest::OK)
	{
		log->WriteLogINFO("READ UNITS SUCCESS");
	}
	else
	{
		log->WriteLogINFO("READ UNITS FAIL");
	}

	//gate::Adapter* sm = gate::CreateAdaptor(TypeAdapter::SharedMemory);

	/*ConfigSharedMemoryAdapter conf;
	conf.DataType = TypeData::ANALOG;
	conf.NameMemory = "asd";
	conf.size = 100;*/

	//sm->InitAdaptor(&conf);

	//std::unique_ptr<void> p = sm->GetInfoAdaptor(ParamInfoAdapter::Type);
	//std::unique_ptr<TypeAdapter> pp(std::static_pointer_cast<std::unique_ptr<TypeAdapter>>(p.release());

	//TypeAdapter we = *pp;
	/*ConfigDDSUnit config_sub;
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
	DDSUnit* sub = CreateDDSUnit(TypeDDSUnit::SUBSCRIBER, config_sub);*/
	
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
