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


int test_pub()
{

	scada_ate::gate::ddsunit::ConfigDDSUnit conf_pub;
	size_t size_data = size_data_m / 2;

	shared_ptr<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory> conf_adapter = make_shared<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory>();
	conf_adapter.get()->NameChannel = "sm010";
	conf_adapter.get()->type_adapter = scada_ate::gate::adapter::TypeAdapter::SharedMemory;
	conf_adapter.get()->DataType = scada_ate::gate::adapter::TypeData::Base;
	conf_adapter.get()->InfoType = scada_ate::gate::adapter::TypeInfo::Data;
	conf_adapter.get()->size_alarms = 0;
	conf_adapter.get()->size_data_int = size_data;
	conf_adapter.get()->size_data_float = size_data;
	conf_adapter.get()->size_data_double = 0;
	conf_adapter.get()->size_data_char = 0;
	conf_adapter.get()->size_str = 0;

	conf_pub.TypeUnit = scada_ate::gate::ddsunit::TypeDDSUnit::PUBLISHER;
	conf_pub.Typeinfo = scada_ate::gate::adapter::TypeInfo::Data;
	conf_pub.Typedata = scada_ate::gate::adapter::TypeData::Base;;
	conf_pub.Domen =0;
	conf_pub.Frequency = 100;
	conf_pub.Adapter = scada_ate::gate::adapter::TypeAdapter::SharedMemory;
	conf_pub.IP_MAIN = "192.168.0.174";
	conf_pub.Port_MAIN = 40000;
	conf_pub.PointName = "sm010";
	conf_pub.Transport = scada_ate::gate::ddsunit::TypeTransport::TCPv4;
	conf_pub.conf_adapter = conf_adapter;


	shared_ptr<scada_ate::gate::ddsunit::IDDSUnit> publisher_ = scada_ate::gate::ddsunit::CreateDDSUnit(conf_pub);
	publisher_->Initialization();

	std::shared_ptr<scada_ate::gate::adapter::IAdapter> ad_out = scada_ate::gate::adapter::CreateAdapter(scada_ate::gate::adapter::TypeAdapter::SharedMemory);
	ad_out->InitAdapter(conf_adapter);

	std::shared_ptr<DDSData> buf_out = std::make_shared<DDSData>();
	buf_out.get()->data_int().value().resize(size_data);
	buf_out.get()->data_int().quality().resize(size_data);
	buf_out.get()->data_float().value().resize(size_data);
	buf_out.get()->data_float().quality().resize(size_data);
	
	int count = 0;

	while (1)
	{
		{
			vector<int>& vec_i = buf_out->data_int().value();
			vector<float>& vec_f = buf_out->data_float().value();

			for (int i = 0; i < size_data; i++)
			{
				vec_i[i] = count + i;
				vec_f[i] = (count + i) * 0.0001;
			}
		}

		ad_out->WriteData(buf_out);
		count++;
		this_thread::sleep_for(100ms);
	}	
}

int test_sub()
{
	scada_ate::gate::ddsunit::ConfigDDSUnit conf_pub;
	size_t size_data = size_data_m / 2;

	shared_ptr<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory> conf_adapter = make_shared<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory>();
	conf_adapter.get()->NameChannel = "sm010";
	conf_adapter.get()->type_adapter = scada_ate::gate::adapter::TypeAdapter::SharedMemory;
	conf_adapter.get()->DataType = scada_ate::gate::adapter::TypeData::Base;
	conf_adapter.get()->InfoType = scada_ate::gate::adapter::TypeInfo::Data;
	conf_adapter.get()->size_alarms = 0;
	conf_adapter.get()->size_data_int = size_data;
	conf_adapter.get()->size_data_float = size_data;
	conf_adapter.get()->size_data_double = 0;
	conf_adapter.get()->size_data_char = 0;
	conf_adapter.get()->size_str = 0;

	conf_pub.TypeUnit = scada_ate::gate::ddsunit::TypeDDSUnit::SUBSCRIBER;
	conf_pub.Typeinfo = scada_ate::gate::adapter::TypeInfo::Data;
	conf_pub.Typedata = scada_ate::gate::adapter::TypeData::Base;
	conf_pub.Typerecieve = scada_ate::gate::ddsunit::TypeRecieve::LISTEN;
	conf_pub.Domen = 0;
	conf_pub.Frequency = 0;
	conf_pub.Adapter = scada_ate::gate::adapter::TypeAdapter::SharedMemory;
	conf_pub.IP_MAIN = "192.168.0.174";
	conf_pub.Port_MAIN = 40000;
	conf_pub.PointName = "sm010";
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
		std::this_thread::sleep_for(100ms);
	}
}

int main(int argc, char* argv[])
{

	if (argc < 2) return 0;
	
	string str = argv[2];
	size_data_m = std::atoi(str.c_str());

	str = argv[1];

	scada_ate::typetopics::SetMaxSizeDataCollectionInt(size_data_m/2);
	scada_ate::typetopics::SetMaxSizeDataCollectionFloat(size_data_m/2);
	scada_ate::typetopics::SetMaxSizeDDSDataExVectorInt(size_data_m/2);
	scada_ate::typetopics::SetMaxSizeDDSDataExVectorFloat(size_data_m/2);

	std::cout << " --- SCADA ATE ----" << std::endl;
	std::cout << "START GATE FOR EMT " << std::endl;
	std::cout << "Config: " << std::endl;
	std::cout << "size data transfer: " << size_data_m << std::endl;
	std::cout << "transmission frequency: 100 ms" << std::endl;
	std::cout << " -------------- " << std::endl;
	LoggerSpaceScada::ConfigLogger conf_log;
	conf_log.file_mame = "log";
	conf_log.file_path;
	conf_log.level = LoggerSpaceScada::LevelLog::Info;
	conf_log.numbers_file = 5;
	conf_log.size_file = 10;

	std::shared_ptr<LoggerSpaceScada::ILoggerScada> log = LoggerSpaceScada::GetLoggerScada(LoggerSpaceScada::TypeLogger::SPDLOG, conf_log);
	
	std::jthread t_out;
	if (str.compare("pub") == 0) t_out = std::jthread(test_pub);
	if (str.compare("sub") == 0) t_out = std::jthread(test_sub);


	while (1)
	{


		this_thread::sleep_for(1000ms);
	}
	return 0;
}

