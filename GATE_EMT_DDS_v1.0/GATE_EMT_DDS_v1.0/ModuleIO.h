#pragma once
#include "DDSUnit.h"
#include <map>

namespace scada_ate
{
	class Module_IO
	{
		std::map<std::string, std::shared_ptr<gate::DDSUnit>> Map_DDSUnits;
	};
}