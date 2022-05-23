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
float k_ = 0;
int number_machine = 0;


int test_pub()
{

	scada_ate::gate::ddsunit::ConfigDDSUnit conf_pub;

	shared_ptr<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory> conf_adapter = make_shared<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory>();
	conf_adapter.get()->NameChannel = "sm020";
	conf_adapter.get()->type_adapter = scada_ate::gate::adapter::TypeAdapter::SharedMemory;
	conf_adapter.get()->DataType = scada_ate::gate::adapter::TypeData::Extended;
	conf_adapter.get()->InfoType = scada_ate::gate::adapter::TypeInfo::Data;
	conf_adapter.get()->size_alarms = 0;
	conf_adapter.get()->size_data_int = size_data_m;
	conf_adapter.get()->size_data_float = size_data_m;
	conf_adapter.get()->size_data_double = 0;
	conf_adapter.get()->size_data_char = 0;
	conf_adapter.get()->size_str = 0;

	conf_pub.TypeUnit = scada_ate::gate::ddsunit::TypeDDSUnit::PUBLISHER;
	conf_pub.Typeinfo = scada_ate::gate::adapter::TypeInfo::Data;
	conf_pub.Typedata = scada_ate::gate::adapter::TypeData::Extended;
	conf_pub.Domen =0;
	conf_pub.Frequency = 100;
	conf_pub.Adapter = scada_ate::gate::adapter::TypeAdapter::SharedMemory;
	conf_pub.IP_MAIN = "192.168.0.174";
	conf_pub.Port_MAIN = 40000;
	conf_pub.PointName = "sm020";
	conf_pub.Transport = scada_ate::gate::ddsunit::TypeTransport::TCPv4;
	conf_pub.conf_adapter = conf_adapter;


	shared_ptr<scada_ate::gate::ddsunit::IDDSUnit> publisher_ = scada_ate::gate::ddsunit::CreateDDSUnit(conf_pub);
	publisher_->Initialization();

	std::shared_ptr<scada_ate::gate::adapter::IAdapter> ad_out = scada_ate::gate::adapter::CreateAdapter(scada_ate::gate::adapter::TypeAdapter::SharedMemory);
	ad_out->InitAdapter(conf_adapter);

	std::shared_ptr<DDSDataEx> buf_out = std::make_shared<DDSDataEx>();
	buf_out.get()->data_int().resize(size_data_m*k_);
	buf_out.get()->data_float().resize(size_data_m * k_);
	
	int count = 0;
	std::srand(std::time(nullptr));

	while (1)
	{
		{
			vector<DataExInt>& vec_i = buf_out->data_int();
			vector<DataExFloat>& vec_f = buf_out->data_float();
			//int r = std::rand();
			//int offset = r*1. / RAND_MAX * size_data_m * (1. - k_);
			int offset = 0;
			for (int i = 0; i < size_data_m * k_; i++)
			{
				vec_i[i].id_tag(offset + i);
				vec_i[i].value(count+i);
				vec_f[i].id_tag(offset + size_data_m + i);
				vec_f[i].value((count + i) * 0.0001);
			}
		}

		if (count % 10 == 0) cout << "count = " << count << endl;

		ad_out->WriteData(buf_out);
		count++;
		this_thread::sleep_for(30ms);
	}	
}

int test_sub()
{
	scada_ate::gate::ddsunit::ConfigDDSUnit conf_pub;

	shared_ptr<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory> conf_adapter = make_shared<scada_ate::gate::adapter::sem::ConfigAdapterSharedMemory>();
	conf_adapter.get()->NameChannel = "sm020";
	conf_adapter.get()->type_adapter = scada_ate::gate::adapter::TypeAdapter::SharedMemory;
	conf_adapter.get()->DataType = scada_ate::gate::adapter::TypeData::Extended;
	conf_adapter.get()->InfoType = scada_ate::gate::adapter::TypeInfo::Data;
	conf_adapter.get()->size_alarms = 0;
	conf_adapter.get()->size_data_int = size_data_m;
	conf_adapter.get()->size_data_float = size_data_m;
	conf_adapter.get()->size_data_double = 0;
	conf_adapter.get()->size_data_char = 0;
	conf_adapter.get()->size_str = 0;

	conf_pub.TypeUnit = scada_ate::gate::ddsunit::TypeDDSUnit::SUBSCRIBER;
	conf_pub.Typeinfo = scada_ate::gate::adapter::TypeInfo::Data;
	conf_pub.Typedata = scada_ate::gate::adapter::TypeData::Extended;
	conf_pub.Domen = 0;
	conf_pub.Frequency = 0;
	conf_pub.Adapter = scada_ate::gate::adapter::TypeAdapter::SharedMemory;
	conf_pub.IP_MAIN = "192.168.0.174";
	conf_pub.Port_MAIN = 40000;
	conf_pub.PointName = "sm020";
	conf_pub.Transport = scada_ate::gate::ddsunit::TypeTransport::TCPv4;
	conf_pub.conf_adapter = conf_adapter;


	shared_ptr<scada_ate::gate::ddsunit::IDDSUnit> publisher_ = scada_ate::gate::ddsunit::CreateDDSUnit(conf_pub);
	publisher_->Initialization();

	std::shared_ptr<scada_ate::gate::adapter::IAdapter> ad_out = scada_ate::gate::adapter::CreateAdapter(scada_ate::gate::adapter::TypeAdapter::SharedMemory);
	ad_out->InitAdapter(conf_adapter);

	std::shared_ptr<DDSDataEx> buf_out = std::make_shared<DDSDataEx>();
	int count = 0;

	while (1)
	{
 		ad_out->ReadData(buf_out);
		//cout << buf_out.get()->data_int().size() << std::endl;
		//cout << buf_out.get()->data_float().size() << std::endl;
		count++;
		std::this_thread::sleep_for(1000ms);
	}
}

int main(int argc, char* argv[])
{

	if (argc < 2) return 0;

	
	//number_machine = std::atoi(str.c_str());
	
	string str = argv[2];
	size_data_m = std::atoi(str.c_str());
	str = argv[3];
	k_ = std::atoi(str.c_str())/100.;
	str = argv[1];

	scada_ate::typetopics::SetMaxSizeDataCollectionInt(500000);
	scada_ate::typetopics::SetMaxSizeDataCollectionFloat(500000);
	scada_ate::typetopics::SetMaxSizeDDSDataExVectorInt(500000);
	scada_ate::typetopics::SetMaxSizeDDSDataExVectorFloat(500000);

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


