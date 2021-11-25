#pragma once
#include "rapidjson/document.h"
#include <string>
#include <vector>
#include "logger.h"
#include "structs.hpp"

namespace scada_ate
{
	namespace emt
	{
		namespace config
		{
			
			class ConfigReaderSimulator
			{
				std::string name_file = "config_simulator.json";
				rapidjson::Document document;
				LoggerSpace::Logger* log;

				ResultRequest take_type_transfer(UnitSimulation& unit, unsigned int i);
				ResultRequest take_point_name(UnitSimulation& unit, unsigned int i);
				ResultRequest take_type_data(UnitSimulation& unit, unsigned int i);
				ResultRequest take_size(UnitSimulation& unit, unsigned int i);
				ResultRequest take_frequency(UnitSimulation& unit, unsigned int i);
				ResultRequest take_type_signal(UnitSimulation& unit, unsigned int i);
				ResultRequest take_amplitude(UnitSimulation& unit, unsigned int i);
				ResultRequest take_show(UnitSimulation& unit, unsigned int i);
				ResultRequest take_output(UnitSimulation& unit, unsigned int i);

				ResultRequest StringToTypeTransfer(std::string str, TypeTransfer& transfer);
				ResultRequest StringToTypeData(std::string str, TypeData& transfer);
				ResultRequest StringToShowDataConsole(std::string str, ShowDataConsole& transfer);
				ResultRequest StringToTypeSignal(std::string str, TypeSignal& transfer);

			public:

				ConfigReaderSimulator();
				ConfigReaderSimulator(std::string name);
				~ConfigReaderSimulator();

				ResultRequest ReadConfig(std::vector<UnitSimulation>& out_vector);

			};
		}
	}
}