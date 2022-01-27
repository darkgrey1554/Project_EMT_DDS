#pragma once
#include "Unit_CP/Unit_CP.hpp"
#include <Module_IO/ModuleIO.hpp>
#include <map>
#include <mutex>

namespace scada_ate
{
	namespace controller_module_io
	{
		class Module_CP
		{

			LoggerSpace::Logger* log;
			std::map <unsigned int , std::shared_ptr<Unit_CP>> map_units_control;
			std::mutex mutex_guard_interface;

		public:

			Module_CP();
			~Module_CP();
			ResultReqest add_unit(std::shared_ptr<ConfigUnitCP> config, std::shared_ptr<module_io::Module_IO> module_control, unsigned int id = 0);
			ResultReqest clear(unsigned int id);
			ResultReqest clearall();
		};
	}
}