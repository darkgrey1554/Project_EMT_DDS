#include <Module_CTRL/Module_CTRL.hpp>

int main()
{

	std::shared_ptr<atech::srv::io::ctrl::Module_CTRL> _module_ctrl = std::make_shared<atech::srv::io::ctrl::Module_CTRL>();
	_module_ctrl->LoadConfigService();
	_module_ctrl->InitDDSLayer();
	_module_ctrl->

	while (1)
	{
		char c;
		std::cin >> c;
		if (c == 'q') break;
	}

	return 0;
}