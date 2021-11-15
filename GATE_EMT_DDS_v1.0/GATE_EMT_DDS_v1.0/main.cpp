

/*#include <iostream>
#include "Config_Reader.h"
#include "logger.h"
#include "Adapters.h"
#include "DDSUnit.h"
*/

#include "ModuleIO.h"
#include "Module_CP.h"
#include "Config_Reader.h"

int main()
{
	LoggerSpace::Logger* log = LoggerSpace::Logger::getpointcontact();
	std::shared_ptr<scada_ate::service_io::config::ConfigReader> config_reader = std::make_shared<scada_ate::service_io::config::ConfigReader>();
	
	ConfigLogger conf_log;
	config_reader->ReadConfigLOGGER(conf_log);
	log->SetLogMode(conf_log.LogMode);
	log->SetNameLog(conf_log.LogName.c_str());
	log->SetNameSysLog(conf_log.SysLogName.c_str());
	log->SetSizeFile(conf_log.SizeLogFile);
	if (conf_log.StatusLog == LoggerSpace::Status::ON) { log->TurnOnLog(); } 
	else { log->TurnOffLog(); }
	if (conf_log.StatusSysLog == LoggerSpace::Status::ON) { log->TurnOnSysLog(); }
	else { log->TurnOffSysLog(); }

	ConfigGate conf_gate;
	if ( config_reader->ReadConfigGATE(conf_gate) != ResultReqest::OK)
	{
		std::this_thread::sleep_for(std::chrono::seconds(2));
		return -1;
	}

	ConfigModule_IO config_module_io;
	config_reader->ReadConfigMODULE_IO(config_module_io);
	config_module_io.IdGate = conf_gate.IdGate;

	std::shared_ptr<scada_ate::module_io::Module_IO> module_io = std::make_shared <scada_ate::module_io::Module_IO>();
	if (module_io->InitModule(config_module_io) == ResultReqest::OK)
	{
		log->WriteLogINFO("Initional Module_IO done");
	};

	std::shared_ptr<scada_ate::controller_module_io::Module_CP> module_cp = std::make_shared <scada_ate::controller_module_io::Module_CP>();

	ConfigUnitCP_DDS config_controller_dds;
	if (config_reader->ReadConfigCONTROLLER_DDS(config_controller_dds) == ResultReqest::OK)
	{
		config_controller_dds.id_gate = conf_gate.IdGate;
		std::shared_ptr<scada_ate::controller_module_io::ConfigUnitCP_DDS> conf = std::make_shared<scada_ate::controller_module_io::ConfigUnitCP_DDS>();
		*(conf.get()) = config_controller_dds;
		if (module_cp->add_unit(conf, module_io) == ResultReqest::OK)
		{
			log->WriteLogINFO("Registred controller dds");
		}
		else
		{
			log->WriteLogWARNING("Error registred controller dds");
		}
	}

	ConfigUnitCP_TCP config_controller_tcp;
	if (config_reader->ReadConfigCONTROLLER_TCP(config_controller_tcp) == ResultReqest::OK)
	{
		config_controller_tcp.id_gate = conf_gate.IdGate;
		std::shared_ptr<scada_ate::controller_module_io::ConfigUnitCP_TCP> conf = std::make_shared<scada_ate::controller_module_io::ConfigUnitCP_TCP>();
		*(conf.get()) = config_controller_tcp;
		if (module_cp->add_unit(conf, module_io) == ResultReqest::OK)
		{
			log->WriteLogINFO("Registred controller tcp");
		}
		else
		{
			log->WriteLogWARNING("Error registred controller tcp");
		}
	}
	
	while (1)
	{
		if (scada_ate::controller_module_io::GetGlobalStatus() == scada_ate::controller_module_io::GlobalStatus::END) break;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

/*
*int main(int argc, char** argv)
{


	LoggerSpace::Logger* log = LoggerSpace::Logger::getpointcontact();
	log->TurnOnLog();
	log->WriteLogINFO("START");
	std::shared_ptr<ConfigSharedMemoryAdapter> config_writer = std::make_shared<ConfigSharedMemoryAdapter>();
	config_writer->NameMemory = "test_writer";
	config_writer->DataType = TypeData::ANALOG;
	config_writer->type_adapter = TypeAdapter::SharedMemory;
	config_writer->size = 100;

	std::shared_ptr<ConfigSharedMemoryAdapter> config_reader = std::make_shared<ConfigSharedMemoryAdapter>();;
	config_reader->NameMemory = "test_reader";
	config_reader->DataType = TypeData::ANALOG;
	config_reader->type_adapter = TypeAdapter::SharedMemory;
	config_reader->size = 100;

	std::shared_ptr<gate::Adapter> adapter_writer = gate::CreateAdapter(TypeAdapter::SharedMemory);
	std::shared_ptr<gate::Adapter> adapter_reader = gate::CreateAdapter(TypeAdapter::SharedMemory);

	adapter_writer->InitAdapter(config_writer);
	adapter_reader->InitAdapter(config_reader);

	ConfigDDSUnit config_DDS_writer;
	config_DDS_writer.Adapter = TypeAdapter::SharedMemory;
	config_DDS_writer.Domen = 1;
	config_DDS_writer.Frequency = 300;
	config_DDS_writer.PointName = "test_writer";
	config_DDS_writer.Size = 100;
	config_DDS_writer.Transmiter = TypeTransmiter::Broadcast;
	config_DDS_writer.Typedata = TypeData::ANALOG;
	config_DDS_writer.TypeUnit = TypeDDSUnit::PUBLISHER;

	ConfigDDSUnit config_DDS_reader;
	config_DDS_reader.Adapter = TypeAdapter::SharedMemory;
	config_DDS_reader.Domen = 1;
	config_DDS_reader.Frequency = 300;
	config_DDS_reader.PointName = "test_reader";
	config_DDS_reader.Size = 100;
	config_DDS_reader.Transmiter = TypeTransmiter::Broadcast;
	config_DDS_reader.Typedata = TypeData::ANALOG;
	config_DDS_reader.TypeUnit = TypeDDSUnit::SUBSCRIBER;

	std::shared_ptr<gate::DDSUnit> DDS_writer = gate::CreateDDSUnit(config_DDS_writer);
	std::shared_ptr<gate::DDSUnit> DDS_reader = gate::CreateDDSUnit(config_DDS_reader);

	DDS_writer->Initialization();
	DDS_reader->Initialization();

	float buf_out[100];
	float buf_in[100];
	for (int i = 0; i < 100; i++)
	{
		buf_out[i] = i;
	}

	while (1)
	{
		adapter_writer->WriteData(buf_out, 100);
		adapter_reader->ReadData(buf_in, 100);
		std::cout << "buf_in[0] = " << buf_in[0] << std::endl;
		std::cout << "buf_in[99] = " << buf_in[99] << std::endl;
		std::cout << "-----------------------------------------" << std::endl;

		for (int i = 0; i < 100; i++)
		{
			buf_out[i] += 0.1;
		}

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

};
*/

