#include "Unit_TCP_CP.hpp"

namespace scada_ate
{
	namespace controller_module_io
	{

		Unit_TCP_CP::Unit_TCP_CP() { return; };

		Unit_TCP_CP::~Unit_TCP_CP() { return; };

		void  Unit_TCP_CP::SetCurrentStatus(StatusUnitCP value) 
		{
			return;
		};

		TypeUnitCP Unit_TCP_CP::GetTypeUnitCP() 
		{
			return TypeUnitCP::TCP;
		};

		StatusUnitCP Unit_TCP_CP::GetCurrentStatus() 
		{
			return StatusUnitCP::EMPTY;
		};

		ResultReqest Unit_TCP_CP::InitUnitCP(std::shared_ptr<ConfigUnitCP> config, std::shared_ptr<module_io::Module_IO>)
		{
			return ResultReqest::ERR;
		};
	}
}