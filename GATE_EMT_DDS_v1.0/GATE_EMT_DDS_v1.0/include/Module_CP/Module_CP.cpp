#include "Module_CP.hpp"

namespace scada_ate
{
	namespace controller_module_io
	{		
		Module_CP::Module_CP()
		{
			log = LoggerSpace::Logger::getpointcontact();
		}

		Module_CP::~Module_CP() {}

		ResultReqest Module_CP::add_unit(std::shared_ptr<ConfigUnitCP> config, std::shared_ptr<module_io::Module_IO> module_control, unsigned int id)
		{
			unsigned int id_unit = 0;
			ResultReqest res = ResultReqest::OK;

			try
			{
				std::lock_guard<std::mutex> lock(mutex_guard_interface);

				if (id != 0)
				{
					id_unit = id;
				}
				else
				{
					if (map_units_control.empty())
					{
						id_unit = 1;
					}
					else
					{
						id_unit = map_units_control.rbegin()->first + 1;
					}
				}

				if (map_units_control.find(id_unit) != map_units_control.end()) throw 1;

				map_units_control.insert({ id_unit, CreateUnit_CP(config->type_unit) });
				if (map_units_control[id_unit]->InitUnitCP(config, module_control) != ResultReqest::OK)
				{
					map_units_control.erase(id_unit);
					throw 2;
				};
			}
			catch (int& e)
			{
				log->WriteLogERR("Error Module_CP: error add_unit: ", e, 0);
				res = ResultReqest::ERR;
			}
			catch (...)
			{
				log->WriteLogERR("Error Module_CP: error add_unit: ", 0, 0);
				res = ResultReqest::ERR;
			}

			return res;
		}

		ResultReqest Module_CP::clear(unsigned int id) 
		{
			ResultReqest res = ResultReqest::OK;

			if (map_units_control.find(id) != map_units_control.end())
			{
				map_units_control.erase(id);
			}
			else
			{
				res = ResultReqest::ERR;
			}

			return res;
		}

		ResultReqest Module_CP::clearall() 
		{
			map_units_control.clear();
			
			return ResultReqest::OK;
		}
	}
}