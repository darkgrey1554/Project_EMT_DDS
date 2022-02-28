#include <Module_IO/ModuleIO.hpp>
#include <Module_CP/Module_CP.hpp>
#include <configReader/ConfigReader_main/Config_Reader.hpp>
#include <LoggerScada.hpp>
#include "TypeTopicDDS/TypeTopics.h"

int main()
{

	std::cout << "### Service IO of SCADA ATE ###" << std::endl;
	std::cout << "Start service ..." << std::endl;

	LoggerSpaceScada::ConfigLogger config_logger;
	config_logger.file_mame = "loggate";
	config_logger.file_path;
	config_logger.numbers_file = 10;
	config_logger.size_file = 10;
	config_logger.level = LoggerSpaceScada::LevelLog::Debug;

	std::shared_ptr<LoggerSpaceScada::LoggerScada> log = LoggerSpaceScada::GetLoggerScada(LoggerSpaceScada::TypeLogger::SPDLOG, config_logger);
	std::shared_ptr<scada_ate::service_io::config::ConfigReader> config_reader = std::make_shared<scada_ate::service_io::config::ConfigReader>();

	/*LoggerSpace::Logger* log = LoggerSpace::Logger::getpointcontact();
	ConfigLogger conf_log;
	config_reader->ReadConfigLOGGER(conf_log);
	log->SetLogMode(conf_log.LogMode);
	log->SetNameLog(conf_log.LogName.c_str());
	log->SetNameSysLog(conf_log.SysLogName.c_str());
	log->SetSizeFile(conf_log.SizeLogFile);
	if (conf_log.StatusLog == LoggerSpace::Status::ON) { log->TurnOnLog(); } 
	else { log->TurnOffLog(); }
	if (conf_log.StatusSysLog == LoggerSpace::Status::ON) { log->TurnOnSysLog(); }
	else { log->TurnOffSysLog(); }*/

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
		log->Info("Initional Module_IO done");
		std::cout << "Initialization Module_IO done" << std::endl;

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
			log->Info("Registration controller dds done");
			std::cout << "Registration controller dds done" << std::endl;
		}
		else
		{
			log->Warning("Error registred controller dds");
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
			log->Info("Registred controller tcp");
		}
		else
		{
			log->Warning("Error registred controller tcp");
		}
	}

	std::cout << "Service IO started" << std::endl;
	
	while (1)
	{
		if (scada_ate::controller_module_io::GetGlobalStatus() == scada_ate::controller_module_io::GlobalStatus::END) break;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

