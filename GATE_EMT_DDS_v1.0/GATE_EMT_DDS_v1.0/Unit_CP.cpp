#include "Unit_CP.h"
#include "Unit_DDS_CP.h"
#include "Unit_TCP_CP.h"

namespace scada_ate
{
	namespace controller_module_io
	{
		

		void Unit_CP::SetGlobalStatus(GlobalStatus value)
		{
			global_status.store(value, std::memory_order_relaxed);
			return;
		}

		GlobalStatus GetGlobalStatus()
		{
			return Unit_CP::global_status.load(std::memory_order_relaxed);
		};

		std::shared_ptr<Unit_CP> CreateUnit_CP(TypeUnitCP type)
		{
			if (type == TypeUnitCP::DDS) return std::make_shared<Unit_DDS_CP>();
			if (type == TypeUnitCP::TCP) return std::make_shared<Unit_TCP_CP>();

			return nullptr;
		};
	}
}