#pragma once
#include <Module_IO/UnitTransfer/UnitTransfer.h>

namespace scada_ate::gate
{

	class Module_IO
	{
		std::map<int, std::unique_ptr<adapter::UnitTransfer>> map_units;

		public:

		
	};
}