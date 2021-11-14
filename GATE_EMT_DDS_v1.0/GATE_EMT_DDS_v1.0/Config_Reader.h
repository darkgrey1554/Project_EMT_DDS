#pragma once

#include "structs.h"
#include <vector>
#include "logger.h"
#include <iostream>
#include <fstream>
#include "rapidjson/document.h"


namespace scada_ate
{
	namespace service_io
	{
		namespace config
		{
			class ConfigReader
			{
				LoggerSpace::Logger* log = LoggerSpace::Logger::getpointcontact();
				std::string name_config = "config.json";

				ResultReqest take_gate_idgate(rapidjson::Document& doc, ConfigGate& conf);

				ResultReqest take_logger_LogName(rapidjson::Document& doc, ConfigLogger& conf);
				ResultReqest take_logger_SysLogName(rapidjson::Document& doc, ConfigLogger& conf);
				ResultReqest take_logger_LogMode(rapidjson::Document& doc, ConfigLogger& conf);
				ResultReqest take_logger_StatusLog(rapidjson::Document& doc, ConfigLogger& conf);
				ResultReqest take_logger_StatusSysLog(rapidjson::Document& doc, ConfigLogger& conf);
				ResultReqest take_logger_SizeLogFile(rapidjson::Document& doc, ConfigLogger& conf);

				ResultReqest take_controllertcp_ip(rapidjson::Document& doc, ConfigContreller_TCP& conf);
				ResultReqest take_controllertcp_port(rapidjson::Document& doc, ConfigContreller_TCP& conf);

				ResultReqest take_controllerdds_domen(rapidjson::Document& doc, ConfigContreller_DDS& conf);
				ResultReqest take_controllerdds_typetransmite(rapidjson::Document& doc, ConfigContreller_DDS& conf);
				ResultReqest take_controllerdds_ipbase(rapidjson::Document& doc, ConfigContreller_DDS& conf);
				ResultReqest take_controllerdds_ipreserve(rapidjson::Document& doc, ConfigContreller_DDS& conf);
				ResultReqest take_controllerdds_portbase(rapidjson::Document& doc, ConfigContreller_DDS& conf);
				ResultReqest take_controllerdds_portreserve(rapidjson::Document& doc, ConfigContreller_DDS& conf);
				ResultReqest take_controllerdds_topiccommand(rapidjson::Document& doc, ConfigContreller_DDS& conf);
				ResultReqest take_controllerdds_topicanswer(rapidjson::Document& doc, ConfigContreller_DDS& conf);

				ResultReqest take_moduleio_domen(rapidjson::Document& doc, ConfigModule_IO& conf);
				ResultReqest take_moduleio_typetransmite(rapidjson::Document& doc, ConfigModule_IO& conf);
				ResultReqest take_moduleio_ipbase(rapidjson::Document& doc, ConfigModule_IO& conf);
				ResultReqest take_moduleio_ipreserve(rapidjson::Document& doc, ConfigModule_IO& conf);
				ResultReqest take_moduleio_portbase(rapidjson::Document& doc, ConfigModule_IO& conf);
				ResultReqest take_moduleio_portreserve(rapidjson::Document& doc, ConfigModule_IO& conf);
				ResultReqest take_moduleio_topicinfo(rapidjson::Document& doc, ConfigModule_IO& conf);
				

				ResultReqest StringToLogMode(std::string str, LoggerSpace::LogMode& value);
				ResultReqest StringToLogStatus(std::string str, LoggerSpace::Status& value);
				ResultReqest StringToTypeTransmiter(std::string str, TypeTransmiter& value);
				ResultReqest CheckIP(std::string str);

			public:

				ConfigReader();

				ResultReqest ReadConfigGATE(ConfigGate& conf);
				ResultReqest ReadConfigLOGGER(ConfigLogger& conf);
				ResultReqest ReadConfigMODULE_IO(ConfigModule_IO& conf);
				ResultReqest ReadConfigCONTROLLER_TCP(ConfigContreller_TCP& conf);
				ResultReqest ReadConfigCONTROLLER_DDS(ConfigContreller_DDS& conf);
				void SetNameConfigFile(std::string str);



			};

		};
	};
};

