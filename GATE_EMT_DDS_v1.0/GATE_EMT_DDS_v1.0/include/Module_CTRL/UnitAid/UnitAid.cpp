#include <Module_CTRL/UnitAid/UnitAid.hpp>
#include "UnitAid_DDS/UnitAid_DDS.hpp"

namespace atech::srv::io::ctrl
{
	std::shared_ptr<UnitAid> CreateUnit_CP(std::shared_ptr<IConfigUnitAid> config)
	{
		if (config->type_aid == TypeUnitAid::DDS) return std::make_shared<UnitAid_DDS>(config);

		return nullptr;
	};
}