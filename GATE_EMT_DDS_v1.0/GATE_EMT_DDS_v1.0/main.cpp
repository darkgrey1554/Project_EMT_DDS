//#include <Module_IO/ModuleIO.hpp>
//#include <Module_CP/Module_CP.hpp>
//#include <configReader/ConfigReader_main/Config_Reader.hpp>
#include <LoggerScada.hpp>
#include "TypeTopicDDS/TypeTopics.h"
#include <Module_IO/DDSUnit/adapters/Adapters.hpp>
#include <Module_IO/DDSUnit/adapters/SharedMemory_Adapter/AdapterSharedMemory.hpp>
#include <Module_IO/DDSUnit/DDSUnit.hpp>

using namespace std;

vector<scada_ate::gate::ddsunit::ConfigDDSUnit> vector_configs_pub;
vector<scada_ate::gate::ddsunit::ConfigDDSUnit> vector_configs_sub;

vector<shared_ptr<scada_ate::gate::adapter::IConfigAdapter>> vec_config_apater_in;
vector<shared_ptr<scada_ate::gate::adapter::IConfigAdapter>> vec_config_apater_out;

int size_data_m = 10000;
int number_machine = 0;


void create_config_pub_m1()
{
	scada_ate::gate::ddsunit::ConfigDDSUnit conf_pub;

	{
		shared_ptr<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory> conf_adapter = make_shared<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory>();
		conf_adapter.get()->NameChannel = "sm001";
		conf_adapter.get()->type_adapter = scada_ate::gate::adapter::TypeAdapter::SharedMemory;
		conf_adapter.get()->DataType = scada_ate::gate::adapter::TypeData::Base;
		conf_adapter.get()->InfoType = scada_ate::gate::adapter::TypeInfo::Data;
		conf_adapter.get()->size_alarms = 0;
		conf_adapter.get()->size_data_int = size_data_m;
		conf_adapter.get()->size_data_float = size_data_m;
		conf_adapter.get()->size_data_double = 0;
		conf_adapter.get()->size_data_char = 0;
		conf_adapter.get()->size_str = 0;

		conf_pub.TypeUnit = scada_ate::gate::ddsunit::TypeDDSUnit::PUBLISHER;
		conf_pub.Typeinfo = scada_ate::gate::adapter::TypeInfo::Data;
		conf_pub.Typedata = scada_ate::gate::adapter::TypeData::Base;;
		conf_pub.Domen = 0;
		conf_pub.Frequency = 100;
		conf_pub.Adapter = scada_ate::gate::adapter::TypeAdapter::SharedMemory;
		conf_pub.IP_MAIN = "192.168.56.1";
		conf_pub.Port_MAIN = 40000;
		conf_pub.PointName = "sm001";
		conf_pub.Transport = scada_ate::gate::ddsunit::TypeTransport::TCPv4;
		conf_pub.conf_adapter = conf_adapter;
	}

	vector_configs_pub.push_back(conf_pub);

	{
		shared_ptr<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory> conf_adapter = make_shared<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory>();
		conf_adapter.get()->NameChannel = "sm002";
		conf_adapter.get()->type_adapter = scada_ate::gate::adapter::TypeAdapter::SharedMemory;
		conf_adapter.get()->DataType = scada_ate::gate::adapter::TypeData::Base;
		conf_adapter.get()->InfoType = scada_ate::gate::adapter::TypeInfo::Data;
		conf_adapter.get()->size_alarms = 0;
		conf_adapter.get()->size_data_int = size_data_m;
		conf_adapter.get()->size_data_float = size_data_m;
		conf_adapter.get()->size_data_double = 0;
		conf_adapter.get()->size_data_char = 0;
		conf_adapter.get()->size_str = 0;

		conf_pub.TypeUnit = scada_ate::gate::ddsunit::TypeDDSUnit::PUBLISHER;
		conf_pub.Typeinfo = scada_ate::gate::adapter::TypeInfo::Data;
		conf_pub.Typedata = scada_ate::gate::adapter::TypeData::Base;;
		conf_pub.Domen = 0;
		conf_pub.Frequency = 100;
		conf_pub.Adapter = scada_ate::gate::adapter::TypeAdapter::SharedMemory;
		conf_pub.IP_MAIN = "192.168.56.1";
		conf_pub.Port_MAIN = 40001;
		conf_pub.PointName = "sm002";
		conf_pub.Transport = scada_ate::gate::ddsunit::TypeTransport::TCPv4;
		conf_pub.conf_adapter = conf_adapter;
	}

	vector_configs_pub.push_back(conf_pub);

}

void create_config_pub_m2()
{
	scada_ate::gate::ddsunit::ConfigDDSUnit conf_pub;

	{
		shared_ptr<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory> conf_adapter = make_shared<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory>();
		conf_adapter.get()->NameChannel = "sm003";
		conf_adapter.get()->type_adapter = scada_ate::gate::adapter::TypeAdapter::SharedMemory;
		conf_adapter.get()->DataType = scada_ate::gate::adapter::TypeData::Base;
		conf_adapter.get()->InfoType = scada_ate::gate::adapter::TypeInfo::Data;
		conf_adapter.get()->size_alarms = 0;
		conf_adapter.get()->size_data_int = size_data_m;
		conf_adapter.get()->size_data_float = size_data_m;
		conf_adapter.get()->size_data_double = 0;
		conf_adapter.get()->size_data_char = 0;
		conf_adapter.get()->size_str = 0;

		conf_pub.TypeUnit = scada_ate::gate::ddsunit::TypeDDSUnit::PUBLISHER;
		conf_pub.Typeinfo = scada_ate::gate::adapter::TypeInfo::Data;
		conf_pub.Typedata = scada_ate::gate::adapter::TypeData::Base;;
		conf_pub.Domen = 0;
		conf_pub.Frequency = 100;
		conf_pub.Adapter = scada_ate::gate::adapter::TypeAdapter::SharedMemory;
		conf_pub.IP_MAIN = "192.168.56.104";
		conf_pub.Port_MAIN = 40000;
		conf_pub.PointName = "sm003";
		conf_pub.Transport = scada_ate::gate::ddsunit::TypeTransport::TCPv4;
		conf_pub.conf_adapter = conf_adapter;
	}

	vector_configs_pub.push_back(conf_pub);

	{
		shared_ptr<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory> conf_adapter = make_shared<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory>();
		conf_adapter.get()->NameChannel = "sm004";
		conf_adapter.get()->type_adapter = scada_ate::gate::adapter::TypeAdapter::SharedMemory;
		conf_adapter.get()->DataType = scada_ate::gate::adapter::TypeData::Base;
		conf_adapter.get()->InfoType = scada_ate::gate::adapter::TypeInfo::Data;
		conf_adapter.get()->size_alarms = 0;
		conf_adapter.get()->size_data_int = size_data_m;
		conf_adapter.get()->size_data_float = size_data_m;
		conf_adapter.get()->size_data_double = 0;
		conf_adapter.get()->size_data_char = 0;
		conf_adapter.get()->size_str = 0;

		conf_pub.TypeUnit = scada_ate::gate::ddsunit::TypeDDSUnit::PUBLISHER;
		conf_pub.Typeinfo = scada_ate::gate::adapter::TypeInfo::Data;
		conf_pub.Typedata = scada_ate::gate::adapter::TypeData::Base;;
		conf_pub.Domen = 0;
		conf_pub.Frequency = 100;
		conf_pub.Adapter = scada_ate::gate::adapter::TypeAdapter::SharedMemory;
		conf_pub.IP_MAIN = "192.168.56.104";
		conf_pub.Port_MAIN = 40001;
		conf_pub.PointName = "sm004";
		conf_pub.Transport = scada_ate::gate::ddsunit::TypeTransport::TCPv4;
		conf_pub.conf_adapter = conf_adapter;
	}

	vector_configs_pub.push_back(conf_pub);

}

void create_config_pub_m3()
{
	scada_ate::gate::ddsunit::ConfigDDSUnit conf_pub;

	{
		shared_ptr<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory> conf_adapter = make_shared<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory>();
		conf_adapter.get()->NameChannel = "sm004";
		conf_adapter.get()->type_adapter = scada_ate::gate::adapter::TypeAdapter::SharedMemory;
		conf_adapter.get()->DataType = scada_ate::gate::adapter::TypeData::Base;
		conf_adapter.get()->InfoType = scada_ate::gate::adapter::TypeInfo::Data;
		conf_adapter.get()->size_alarms = 0;
		conf_adapter.get()->size_data_int = size_data_m;
		conf_adapter.get()->size_data_float = size_data_m;
		conf_adapter.get()->size_data_double = 0;
		conf_adapter.get()->size_data_char = 0;
		conf_adapter.get()->size_str = 0;

		conf_pub.TypeUnit = scada_ate::gate::ddsunit::TypeDDSUnit::PUBLISHER;
		conf_pub.Typeinfo = scada_ate::gate::adapter::TypeInfo::Data;
		conf_pub.Typedata = scada_ate::gate::adapter::TypeData::Base;;
		conf_pub.Domen = 0;
		conf_pub.Frequency = 100;
		conf_pub.Adapter = scada_ate::gate::adapter::TypeAdapter::SharedMemory;
		conf_pub.IP_MAIN = "192.168.0.102";
		conf_pub.Port_MAIN = 40000;
		conf_pub.PointName = "sm004";
		conf_pub.Transport = scada_ate::gate::ddsunit::TypeTransport::TCPv4;
		conf_pub.conf_adapter = conf_adapter;
	}

	vector_configs_pub.push_back(conf_pub);

	{
		shared_ptr<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory> conf_adapter = make_shared<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory>();
		conf_adapter.get()->NameChannel = "sm006";
		conf_adapter.get()->type_adapter = scada_ate::gate::adapter::TypeAdapter::SharedMemory;
		conf_adapter.get()->DataType = scada_ate::gate::adapter::TypeData::Base;
		conf_adapter.get()->InfoType = scada_ate::gate::adapter::TypeInfo::Data;
		conf_adapter.get()->size_alarms = 0;
		conf_adapter.get()->size_data_int = size_data_m;
		conf_adapter.get()->size_data_float = size_data_m;
		conf_adapter.get()->size_data_double = 0;
		conf_adapter.get()->size_data_char = 0;
		conf_adapter.get()->size_str = 0;

		conf_pub.TypeUnit = scada_ate::gate::ddsunit::TypeDDSUnit::PUBLISHER;
		conf_pub.Typeinfo = scada_ate::gate::adapter::TypeInfo::Data;
		conf_pub.Typedata = scada_ate::gate::adapter::TypeData::Base;;
		conf_pub.Domen = 0;
		conf_pub.Frequency = 100;
		conf_pub.Adapter = scada_ate::gate::adapter::TypeAdapter::SharedMemory;
		conf_pub.IP_MAIN = "192.168.0.102";
		conf_pub.Port_MAIN = 40001;
		conf_pub.PointName = "sm006";
		conf_pub.Transport = scada_ate::gate::ddsunit::TypeTransport::TCPv4;
		conf_pub.conf_adapter = conf_adapter;
	}

	vector_configs_pub.push_back(conf_pub);

}

void create_config_sub_m1()
{
	scada_ate::gate::ddsunit::ConfigDDSUnit conf_pub;

	{
		shared_ptr<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory> conf_adapter = make_shared<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory>();
		conf_adapter.get()->NameChannel = "sm003";
		conf_adapter.get()->type_adapter = scada_ate::gate::adapter::TypeAdapter::SharedMemory;
		conf_adapter.get()->DataType = scada_ate::gate::adapter::TypeData::Base;
		conf_adapter.get()->InfoType = scada_ate::gate::adapter::TypeInfo::Data;
		conf_adapter.get()->size_alarms = 0;
		conf_adapter.get()->size_data_int = size_data_m;
		conf_adapter.get()->size_data_float = size_data_m;
		conf_adapter.get()->size_data_double = 0;
		conf_adapter.get()->size_data_char = 0;
		conf_adapter.get()->size_str = 0;

		conf_pub.TypeUnit = scada_ate::gate::ddsunit::TypeDDSUnit::SUBSCRIBER;
		conf_pub.Typeinfo = scada_ate::gate::adapter::TypeInfo::Data;
		conf_pub.Typedata = scada_ate::gate::adapter::TypeData::Base;;
		conf_pub.Domen = 0;
		conf_pub.Frequency = 100;
		conf_pub.Adapter = scada_ate::gate::adapter::TypeAdapter::SharedMemory;
		conf_pub.IP_MAIN = "192.168.56.104";
		conf_pub.Port_MAIN = 40000;
		conf_pub.PointName = "sm003";
		conf_pub.Transport = scada_ate::gate::ddsunit::TypeTransport::TCPv4;
		conf_pub.conf_adapter = conf_adapter;
	}

	vector_configs_sub.push_back(conf_pub);

	{
		shared_ptr<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory> conf_adapter = make_shared<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory>();
		conf_adapter.get()->NameChannel = "sm004";
		conf_adapter.get()->type_adapter = scada_ate::gate::adapter::TypeAdapter::SharedMemory;
		conf_adapter.get()->DataType = scada_ate::gate::adapter::TypeData::Base;
		conf_adapter.get()->InfoType = scada_ate::gate::adapter::TypeInfo::Data;
		conf_adapter.get()->size_alarms = 0;
		conf_adapter.get()->size_data_int = size_data_m;
		conf_adapter.get()->size_data_float = size_data_m;
		conf_adapter.get()->size_data_double = 0;
		conf_adapter.get()->size_data_char = 0;
		conf_adapter.get()->size_str = 0;

		conf_pub.TypeUnit = scada_ate::gate::ddsunit::TypeDDSUnit::SUBSCRIBER;
		conf_pub.Typeinfo = scada_ate::gate::adapter::TypeInfo::Data;
		conf_pub.Typedata = scada_ate::gate::adapter::TypeData::Base;;
		conf_pub.Domen = 0;
		conf_pub.Frequency = 100;
		conf_pub.Adapter = scada_ate::gate::adapter::TypeAdapter::SharedMemory;
		conf_pub.IP_MAIN = "192.168.56.104";
		conf_pub.Port_MAIN = 40001;
		conf_pub.PointName = "sm004";
		conf_pub.Transport = scada_ate::gate::ddsunit::TypeTransport::TCPv4;
		conf_pub.conf_adapter = conf_adapter;
	}

	vector_configs_sub.push_back(conf_pub);

}

void create_config_sub_m2()
{
	scada_ate::gate::ddsunit::ConfigDDSUnit conf_pub;

	{
		shared_ptr<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory> conf_adapter = make_shared<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory>();
		conf_adapter.get()->NameChannel = "sm001";
		conf_adapter.get()->type_adapter = scada_ate::gate::adapter::TypeAdapter::SharedMemory;
		conf_adapter.get()->DataType = scada_ate::gate::adapter::TypeData::Base;
		conf_adapter.get()->InfoType = scada_ate::gate::adapter::TypeInfo::Data;
		conf_adapter.get()->size_alarms = 0;
		conf_adapter.get()->size_data_int = size_data_m;
		conf_adapter.get()->size_data_float = size_data_m;
		conf_adapter.get()->size_data_double = 0;
		conf_adapter.get()->size_data_char = 0;
		conf_adapter.get()->size_str = 0;

		conf_pub.TypeUnit = scada_ate::gate::ddsunit::TypeDDSUnit::SUBSCRIBER;
		conf_pub.Typeinfo = scada_ate::gate::adapter::TypeInfo::Data;
		conf_pub.Typedata = scada_ate::gate::adapter::TypeData::Base;;
		conf_pub.Domen = 0;
		conf_pub.Frequency = 100;
		conf_pub.Adapter = scada_ate::gate::adapter::TypeAdapter::SharedMemory;
		conf_pub.IP_MAIN = "192.168.56.1";
		conf_pub.Port_MAIN = 40000;
		conf_pub.PointName = "sm001";
		conf_pub.Transport = scada_ate::gate::ddsunit::TypeTransport::TCPv4;
		conf_pub.conf_adapter = conf_adapter;
	}

	vector_configs_sub.push_back(conf_pub);

	{
		shared_ptr<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory> conf_adapter = make_shared<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory>();
		conf_adapter.get()->NameChannel = "sm002";
		conf_adapter.get()->type_adapter = scada_ate::gate::adapter::TypeAdapter::SharedMemory;
		conf_adapter.get()->DataType = scada_ate::gate::adapter::TypeData::Base;
		conf_adapter.get()->InfoType = scada_ate::gate::adapter::TypeInfo::Data;
		conf_adapter.get()->size_alarms = 0;
		conf_adapter.get()->size_data_int = size_data_m;
		conf_adapter.get()->size_data_float = size_data_m;
		conf_adapter.get()->size_data_double = 0;
		conf_adapter.get()->size_data_char = 0;
		conf_adapter.get()->size_str = 0;

		conf_pub.TypeUnit = scada_ate::gate::ddsunit::TypeDDSUnit::SUBSCRIBER;
		conf_pub.Typeinfo = scada_ate::gate::adapter::TypeInfo::Data;
		conf_pub.Typedata = scada_ate::gate::adapter::TypeData::Base;;
		conf_pub.Domen = 0;
		conf_pub.Frequency = 100;
		conf_pub.Adapter = scada_ate::gate::adapter::TypeAdapter::SharedMemory;
		conf_pub.IP_MAIN = "192.168.56.1";
		conf_pub.Port_MAIN = 40001;
		conf_pub.PointName = "sm002";
		conf_pub.Transport = scada_ate::gate::ddsunit::TypeTransport::TCPv4;
		conf_pub.conf_adapter = conf_adapter;
	}

	vector_configs_sub.push_back(conf_pub);

}

void create_config_sub_m3()
{
	scada_ate::gate::ddsunit::ConfigDDSUnit conf_pub;

	{
		shared_ptr<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory> conf_adapter = make_shared<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory>();
		conf_adapter.get()->NameChannel = "sm003";
		conf_adapter.get()->type_adapter = scada_ate::gate::adapter::TypeAdapter::SharedMemory;
		conf_adapter.get()->DataType = scada_ate::gate::adapter::TypeData::Base;
		conf_adapter.get()->InfoType = scada_ate::gate::adapter::TypeInfo::Data;
		conf_adapter.get()->size_alarms = 0;
		conf_adapter.get()->size_data_int = size_data_m;
		conf_adapter.get()->size_data_float = size_data_m;
		conf_adapter.get()->size_data_double = 0;
		conf_adapter.get()->size_data_char = 0;
		conf_adapter.get()->size_str = 0;

		conf_pub.TypeUnit = scada_ate::gate::ddsunit::TypeDDSUnit::SUBSCRIBER;
		conf_pub.Typeinfo = scada_ate::gate::adapter::TypeInfo::Data;
		conf_pub.Typedata = scada_ate::gate::adapter::TypeData::Base;;
		conf_pub.Domen = 0;
		conf_pub.Frequency = 100;
		conf_pub.Adapter = scada_ate::gate::adapter::TypeAdapter::SharedMemory;
		conf_pub.IP_MAIN = "192.168.0.101";
		conf_pub.Port_MAIN = 40001;
		conf_pub.PointName = "sm003";
		conf_pub.Transport = scada_ate::gate::ddsunit::TypeTransport::TCPv4;
		conf_pub.conf_adapter = conf_adapter;
	}

	vector_configs_sub.push_back(conf_pub);

	{
		shared_ptr<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory> conf_adapter = make_shared<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory>();
		conf_adapter.get()->NameChannel = "sm005";
		conf_adapter.get()->type_adapter = scada_ate::gate::adapter::TypeAdapter::SharedMemory;
		conf_adapter.get()->DataType = scada_ate::gate::adapter::TypeData::Base;
		conf_adapter.get()->InfoType = scada_ate::gate::adapter::TypeInfo::Data;
		conf_adapter.get()->size_alarms = 0;
		conf_adapter.get()->size_data_int = size_data_m;
		conf_adapter.get()->size_data_float = size_data_m;
		conf_adapter.get()->size_data_double = 0;
		conf_adapter.get()->size_data_char = 0;
		conf_adapter.get()->size_str = 0;

		conf_pub.TypeUnit = scada_ate::gate::ddsunit::TypeDDSUnit::SUBSCRIBER;
		conf_pub.Typeinfo = scada_ate::gate::adapter::TypeInfo::Data;
		conf_pub.Typedata = scada_ate::gate::adapter::TypeData::Base;;
		conf_pub.Domen = 0;
		conf_pub.Frequency = 100;
		conf_pub.Adapter = scada_ate::gate::adapter::TypeAdapter::SharedMemory;
		conf_pub.IP_MAIN = "192.168.0.100";
		conf_pub.Port_MAIN = 40001;
		conf_pub.PointName = "sm005";
		conf_pub.Transport = scada_ate::gate::ddsunit::TypeTransport::TCPv4;
		conf_pub.conf_adapter = conf_adapter;
	}

	vector_configs_sub.push_back(conf_pub);

}

void create_config_pub(int machine)
{
	if (machine == 1) create_config_pub_m1();
	if (machine == 2) create_config_pub_m2();
	if (machine == 3) create_config_pub_m3();
}

void create_config_sub(int machine)
{
	if (machine == 1) create_config_sub_m1();
	if (machine == 2) create_config_sub_m2();
	if (machine == 3) create_config_sub_m3();
}

int test_pub()
{
	

	scada_ate::gate::ddsunit::ConfigDDSUnit conf_pub;

	shared_ptr<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory> conf_adapter = make_shared<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory>();
	conf_adapter.get()->NameChannel = "sm001";
	conf_adapter.get()->type_adapter = scada_ate::gate::adapter::TypeAdapter::SharedMemory;
	conf_adapter.get()->DataType = scada_ate::gate::adapter::TypeData::Base;
	conf_adapter.get()->InfoType = scada_ate::gate::adapter::TypeInfo::Data;
	conf_adapter.get()->size_alarms = 0;
	conf_adapter.get()->size_data_int = 100;
	conf_adapter.get()->size_data_float = 0;
	conf_adapter.get()->size_data_double = 0;
	conf_adapter.get()->size_data_char = 0;
	conf_adapter.get()->size_str = 0;

	conf_pub.TypeUnit = scada_ate::gate::ddsunit::TypeDDSUnit::PUBLISHER;
	conf_pub.Typeinfo = scada_ate::gate::adapter::TypeInfo::Data;
	conf_pub.Typedata = scada_ate::gate::adapter::TypeData::Base;;
	conf_pub.Domen =0;
	conf_pub.Frequency = 100;
	conf_pub.Adapter = scada_ate::gate::adapter::TypeAdapter::SharedMemory;
	conf_pub.IP_MAIN = "127.0.0.1";
	conf_pub.Port_MAIN = 40000;
	conf_pub.PointName = "sm001";
	conf_pub.Transport = scada_ate::gate::ddsunit::TypeTransport::TCPv4;
	conf_pub.conf_adapter = conf_adapter;


	shared_ptr<scada_ate::gate::ddsunit::IDDSUnit> publisher_ = scada_ate::gate::ddsunit::CreateDDSUnit(conf_pub);
	publisher_->Initialization();

	std::shared_ptr<scada_ate::gate::adapter::IAdapter> ad_out = scada_ate::gate::adapter::CreateAdapter(scada_ate::gate::adapter::TypeAdapter::SharedMemory);
	ad_out->InitAdapter(conf_adapter);

	std::shared_ptr<DDSData> buf_out = std::make_shared<DDSData>();
	buf_out.get()->data_int().value().resize(100);
	buf_out.get()->data_int().quality().resize(100);
	
	int count = 0;

	while (1)
	{
		for (int i = 0; i < 100; i++)
		{
			buf_out.get()->data_int().value()[i] = i+count;
			buf_out.get()->data_int().quality()[i] = 1;
		}

		ad_out->WriteData(buf_out);
		count++;
		std::this_thread::sleep_for(1000ms);
	}
}

int test_sub()
{
	scada_ate::typetopics::SetMaxSizeDataCollectionInt(100);

	LoggerSpaceScada::ConfigLogger conf_log;
	conf_log.file_mame = "log";
	conf_log.file_path;
	conf_log.level = LoggerSpaceScada::LevelLog::Debug;
	conf_log.numbers_file = 5;
	conf_log.size_file = 10;

	std::shared_ptr<LoggerSpaceScada::ILoggerScada> log = LoggerSpaceScada::GetLoggerScada(LoggerSpaceScada::TypeLogger::SPDLOG, conf_log);

	scada_ate::gate::ddsunit::ConfigDDSUnit conf_pub;

	shared_ptr<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory> conf_adapter = make_shared<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory>();
	conf_adapter.get()->NameChannel = "sm002";
	conf_adapter.get()->type_adapter = scada_ate::gate::adapter::TypeAdapter::SharedMemory;
	conf_adapter.get()->DataType = scada_ate::gate::adapter::TypeData::Base;
	conf_adapter.get()->InfoType = scada_ate::gate::adapter::TypeInfo::Data;
	conf_adapter.get()->size_alarms = 0;
	conf_adapter.get()->size_data_int = 100;
	conf_adapter.get()->size_data_float = 0;
	conf_adapter.get()->size_data_double = 0;
	conf_adapter.get()->size_data_char = 0;
	conf_adapter.get()->size_str = 0;

	conf_pub.TypeUnit = scada_ate::gate::ddsunit::TypeDDSUnit::SUBSCRIBER;
	conf_pub.Typeinfo = scada_ate::gate::adapter::TypeInfo::Data;
	conf_pub.Typedata = scada_ate::gate::adapter::TypeData::Base;;
	conf_pub.Domen = 0;
	conf_pub.Frequency = 100;
	conf_pub.Adapter = scada_ate::gate::adapter::TypeAdapter::SharedMemory;
	conf_pub.IP_MAIN = "127.0.0.1";
	conf_pub.Port_MAIN = 40000;
	conf_pub.PointName = "sm002";
	conf_pub.Transport = scada_ate::gate::ddsunit::TypeTransport::TCPv4;
	conf_pub.conf_adapter = conf_adapter;


	shared_ptr<scada_ate::gate::ddsunit::IDDSUnit> publisher_ = scada_ate::gate::ddsunit::CreateDDSUnit(conf_pub);
	publisher_->Initialization();

	std::shared_ptr<scada_ate::gate::adapter::IAdapter> ad_out = scada_ate::gate::adapter::CreateAdapter(scada_ate::gate::adapter::TypeAdapter::SharedMemory);
	ad_out->InitAdapter(conf_adapter);

	std::shared_ptr<DDSData> buf_out;
	int count = 0;

	while (1)
	{
 		ad_out->ReadData(buf_out);

		count++;
		std::this_thread::sleep_for(1000ms);
	}
}

void thread_out()
{
	shared_ptr<DDSData> data = make_shared<DDSData>();
	data->data_int().value().resize(size_data_m);
	data->data_int().quality().resize(size_data_m);
	data->data_float().value().resize(size_data_m);
	data->data_float().quality().resize(size_data_m);

	vector<shared_ptr<scada_ate::gate::adapter::IAdapter>> vec_adapters;

	for (auto i = 0; i<vec_config_apater_out.size(); i++)
	{	
		vec_adapters.push_back(scada_ate::gate::adapter::CreateAdapter(vec_config_apater_out[i]->type_adapter));
		vec_adapters[i]->InitAdapter(vec_config_apater_out[i]);
	}

	int count = 1000 * number_machine;

	for (;;)
	{
		{
			vector<int>& vec_i = data->data_int().value();
			vector<float>& vec_f = data->data_float().value();

			for (int i = 0; i < size_data_m; i++)
			{
				vec_i[i] = count + i;
				vec_f[i] = (count + i)*0.0001;
			}
		}

		for (auto m : vec_adapters)
		{
			m->WriteData(data);
		}
		count++;
		this_thread::sleep_for(100ms);
	}
}

void thread_in()
{
	shared_ptr<DDSData> data;

	vector<shared_ptr<scada_ate::gate::adapter::IAdapter>> vec_adapters;
	vector<string> name_point;

	for (auto i = 0; i < vec_config_apater_in.size(); i++)
	{
		vec_adapters.push_back(scada_ate::gate::adapter::CreateAdapter(vec_config_apater_in[i]->type_adapter));
		vec_adapters[i]->InitAdapter(vec_config_apater_in[i]);
		name_point.push_back(static_pointer_cast<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory>(vec_config_apater_in[i])->NameChannel);
	}


	for (;;)
	{
		

		for (auto i = 0; i < vec_config_apater_in.size(); i++)
		{
			vec_adapters[i]->ReadData(data);
			std::cout << "*** DATA "<< name_point[i] << " ***" << std::endl;
			cout << "time:" << TimeConverter::LLmcsToSysClock(data->time_source())<<endl;
			cout << "first element int:" << *data->data_int().value().begin() << endl;
			cout << "last element int:" << *data->data_int().value().rbegin() << endl;
			cout << "first element float:" << *data->data_float().value().begin() << endl;
			cout << "last element float:" << *data->data_float().value().rbegin() << endl;
			cout<<std::endl;
		}

		this_thread::sleep_for(1000ms);
	}
}

int main(int argc, char* argv[])
{

	if (argc < 2) return 0;
	string str = argv[1];

	number_machine = std::atoi(str.c_str());

	scada_ate::typetopics::SetMaxSizeDataCollectionInt(size_data_m);
	scada_ate::typetopics::SetMaxSizeDataCollectionFloat(size_data_m);

	LoggerSpaceScada::ConfigLogger conf_log;
	conf_log.file_mame = "log";
	conf_log.file_path;
	conf_log.level = LoggerSpaceScada::LevelLog::Debug;
	conf_log.numbers_file = 5;
	conf_log.size_file = 10;

	std::shared_ptr<LoggerSpaceScada::ILoggerScada> log = LoggerSpaceScada::GetLoggerScada(LoggerSpaceScada::TypeLogger::SPDLOG, conf_log);

	create_config_pub(number_machine);
	create_config_sub(number_machine);

	std::vector<shared_ptr<scada_ate::gate::ddsunit::IDDSUnit>> vec_units;


	for (auto m : vector_configs_pub)
	{
		vec_units.push_back(scada_ate::gate::ddsunit::CreateDDSUnit(m));
		vec_config_apater_out.push_back(m.conf_adapter);
	}

	for (auto m : vector_configs_sub)
	{
		vec_units.push_back(scada_ate::gate::ddsunit::CreateDDSUnit(m));
		vec_config_apater_in.push_back(m.conf_adapter);
	}

	for (auto m : vec_units)
	{
		m->Initialization();
	}

	std::jthread t_out(thread_out);
	std::jthread t_in(thread_in);

	while (1)
	{


		this_thread::sleep_for(1000ms);
	}
	return 0;
}


/*

int main()
{
	LoggerSpaceScada::ConfigLogger conf_log;
	conf_log.file_mame = "log";
	conf_log.file_path;
	conf_log.level = LoggerSpaceScada::LevelLog::Debug;
	conf_log.numbers_file = 5;
	conf_log.size_file = 10;

	std::shared_ptr<LoggerSpaceScada::ILoggerScada> log = LoggerSpaceScada::GetLoggerScada(LoggerSpaceScada::TypeLogger::SPDLOG, conf_log);

	std::shared_ptr<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory> conf_out = std::make_shared<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory >();
	std::shared_ptr<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory> conf_in = std::make_shared<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory >();

	conf_out.get()->NameChannel = "sm001";
	conf_out.get()->type_adapter = scada_ate::gate::adapter::TypeAdapter::SharedMemory;
	conf_out.get()->DataType = scada_ate::gate::adapter::TypeData::Extended;
	conf_out.get()->InfoType = scada_ate::gate::adapter::TypeInfo::Data;
	conf_out.get()->size_alarms = 0;
	conf_out.get()->size_data_int = 10;
	conf_out.get()->size_data_float = 0;
	conf_out.get()->size_data_double =0;
	conf_out.get()->size_data_char=0;
	conf_out.get()->size_str=0;
	conf_out.get()->v_tags.resize(10);
	for (int i = 0; i < conf_out.get()->size_data_int; i++)
	{
		conf_out.get()->v_tags[i] = 1000 + i;
	}

	std::shared_ptr<scada_ate::gate::adapter::IAdapter> ad1 = scada_ate::gate::adapter::CreateAdapter(scada_ate::gate::adapter::TypeAdapter::SharedMemory);
	std::shared_ptr<scada_ate::gate::adapter::IAdapter> ad2 = scada_ate::gate::adapter::CreateAdapter(scada_ate::gate::adapter::TypeAdapter::SharedMemory);

	std::shared_ptr<DDSDataEx> buf_out = std::make_shared<DDSDataEx>();
	std::shared_ptr<DDSDataEx> buf_in;

	ad1->InitAdapter(conf_out);
	ad2->InitAdapter(conf_out);

	buf_out.get()->data_int().resize(10);
	for (int i = 0; i < 10; i++)
	{
		buf_out.get()->data_int()[i].value() = i;
		buf_out.get()->data_int()[i].quality() = 1;
		buf_out.get()->data_int()[i].id_tag() =1000+i;
	}

	ad1->WriteData(buf_out);
	ad2->ReadData(buf_in);	

	buf_out.get()->data_int().resize(10);
	for (int i = 5; i < 10; i++)
	{
		buf_out.get()->data_int()[i].value() = i+1;
		buf_out.get()->data_int()[i].quality() = 1;
		buf_out.get()->data_int()[i].id_tag() = 1000 + i;
	}

	ad1->WriteData(buf_out);
	ad2->ReadData(buf_in);
	
	ad2->ReadData(buf_in, scada_ate::gate::adapter::ModeRead::Full);

	return 0;
}
*/