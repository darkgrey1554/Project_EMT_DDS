#pragma once
#include "Unit_CP.h"

namespace scada_ate
{
	namespace controller_module_io
	{
		class Unit_TCP_CP : Unit_CP
		{
		protected:
			
			void SetCurrentStatus(StatusUnitCP value);

		public:

			TypeUnitCP GetTypeUnitCP();
			StatusUnitCP GetCurrentStatus();
			ResultReqest InitUnitCP(std::shared_ptr<ConfigUnitCP> config, std::shared_ptr<module_io::Module_IO>);
		};
	}
}