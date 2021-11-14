#pragma once
#include "structs.h"
#include <fstream>
#include "rapidjson/document.h"
#include "logger.h"
#include <filesystem>

namespace scada_ate
{
	namespace module_io
	{
		class ReaderConfigUnits
		{
			std::string name_configunits = "configunits.json";
			std::string name_configunits_new = "configunits_new.json";
			std::string name_configunits_old = "configunits_new.json";
			LoggerSpace::Logger* log;

			ResultReqest CheckConfig(std::string file_name);

			ResultReqest take_domen(rapidjson::Document& doc, ConfigDDSUnit& conf, unsigned int i);
			ResultReqest take_typeunit(rapidjson::Document& doc, ConfigDDSUnit& conf, unsigned int i);
			ResultReqest take_typetransmite(rapidjson::Document& doc, ConfigDDSUnit& conf, unsigned int i);
			ResultReqest take_typeadapter(rapidjson::Document& doc, ConfigDDSUnit& conf, unsigned int i);
			ResultReqest take_pointname(rapidjson::Document& doc, ConfigDDSUnit& conf, unsigned int i);
			ResultReqest take_typedata(rapidjson::Document& doc, ConfigDDSUnit& conf, unsigned int i);
			ResultReqest take_size(rapidjson::Document& doc, ConfigDDSUnit& conf, unsigned int i);
			ResultReqest take_frequency(rapidjson::Document& doc, ConfigDDSUnit& conf, unsigned int i);
			ResultReqest take_portmain(rapidjson::Document& doc, ConfigDDSUnit& conf, unsigned int i);
			ResultReqest take_ipmain(rapidjson::Document& doc, ConfigDDSUnit& conf, unsigned int i);
			ResultReqest take_portreserve(rapidjson::Document& doc, ConfigDDSUnit& conf, unsigned int i);
			ResultReqest take_ipreserve(rapidjson::Document& doc, ConfigDDSUnit& conf, unsigned int i);

			ResultReqest StringToTypeUnit(std::string str, TypeDDSUnit& value);
			ResultReqest StringToTypeTransmiter(std::string str, TypeTransmiter& value);
			ResultReqest StringToTypeAdapter(std::string str, TypeAdapter& value);
			ResultReqest StringToTypeData(std::string str, TypeData& value);
			ResultReqest CheckIP(std::string str);

		public:

			ReaderConfigUnits();
			~ReaderConfigUnits();

			ResultReqest CheckNewConfig();
			ResultReqest CheckBaseConfig();
			ResultReqest UpdateNewConfig(std::string std);
			ResultReqest UpdateBaseConfig();

			ResultReqest ReadConfig(std::vector<ConfigDDSUnit>& vector_result);

		};
	}
}