#pragma once

#include <structs/structs.hpp>
#include <vector>
#include <logger_crossplatform/logger.h>
#include <iostream>
#include <fstream>
#include <rapidjson/document.h>
#include <structs/struct_unit_cp.hpp>

using namespace scada_ate::controller_module_io;

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

				ResultReqest take_controllertcp_ip(rapidjson::Document& doc, ConfigUnitCP_TCP& conf);
				ResultReqest take_controllertcp_port(rapidjson::Document& doc, ConfigUnitCP_TCP& conf);

				ResultReqest take_controllerdds_domen(rapidjson::Document& doc, ConfigUnitCP_DDS& conf);
				ResultReqest take_controllerdds_typetransmite(rapidjson::Document& doc, ConfigUnitCP_DDS& conf);
				ResultReqest take_controllerdds_ipbase(rapidjson::Document& doc, ConfigUnitCP_DDS& conf);
				ResultReqest take_controllerdds_ipreserve(rapidjson::Document& doc, ConfigUnitCP_DDS& conf);
				ResultReqest take_controllerdds_portbase(rapidjson::Document& doc, ConfigUnitCP_DDS& conf);
				ResultReqest take_controllerdds_portreserve(rapidjson::Document& doc, ConfigUnitCP_DDS& conf);
				ResultReqest take_controllerdds_topiccommand(rapidjson::Document& doc, ConfigUnitCP_DDS& conf);
				ResultReqest take_controllerdds_topicanswer(rapidjson::Document& doc, ConfigUnitCP_DDS& conf);

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
				ResultReqest StringToTypeTransmite(std::string str, TypeTransmite& value);
				ResultReqest CheckIP(std::string str);

			public:

				ConfigReader();

				ResultReqest ReadConfigGATE(ConfigGate& conf);
				ResultReqest ReadConfigLOGGER(ConfigLogger& conf);
				ResultReqest ReadConfigMODULE_IO(ConfigModule_IO& conf);
				ResultReqest ReadConfigCONTROLLER_TCP(ConfigUnitCP_TCP& conf);
				ResultReqest ReadConfigCONTROLLER_DDS(ConfigUnitCP_DDS& conf);
				void SetNameConfigFile(std::string str);



			};

		};
	};
};

