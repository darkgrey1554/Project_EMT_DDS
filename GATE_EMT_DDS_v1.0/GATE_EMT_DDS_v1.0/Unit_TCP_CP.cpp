#include "Unit_TCP_CP.h"

namespace scada_ate
{
	namespace controller_module_io
	{
		void  Unit_TCP_CP::SetCurrentStatus(StatusUnitCP value) {};

		TypeUnitCP Unit_TCP_CP::GetTypeUnitCP() {};
		StatusUnitCP Unit_TCP_CP::GetCurrentStatus() {};
		ResultReqest Unit_TCP_CP::InitUnitCP(std::shared_ptr<ConfigUnitCP> config, std::shared_ptr<module_io::Module_IO>) {};
	}
}