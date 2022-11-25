//#define FASTDDS_ENFORCE_LOG_INFO
//#ifdef HAVE_LOG_NO_INFO
//#undef HAVE_LOG_NO_INFO
//#endif // HAVE_LOG_NO_INFO
//#define HAVE_LOG_NO_INFO 1
//
//#ifdef HAVE_LOG_NO_WARNING
//#undef HAVE_LOG_NO_WARNING
//#endif // HAVE_LOG_NO_WARNING
//#define HAVE_LOG_NO_WARNING 1
//
//#ifdef HAVE_LOG_NO_ERROR
//#undef HAVE_LOG_NO_ERROR
//#endif // HAVE_LOG_NO_ERROR
//#define HAVE_LOG_NO_ERROR 1

#ifdef UA_LOGLEVEL
#undef UA_LOGLEVEL
#endif // HAVE_LOG_NO_ERROR
#define UA_LOGLEVEL 1000


#include <Module_CTRL/Module_CTRL.hpp>
#include <structs/FactoryDds.h>

char c = 'w';



int main()
{
	//logError(SampleCategory, "dsds")
	

	atech::srv::io::ctrl::ConfigUnitAid_DDS_ptr config_ctrl = std::make_shared<atech::srv::io::ctrl::ConfigUnitAid_DDS>();
	config_ctrl->type_aid = atech::srv::io::ctrl::TypeUnitAid::DDS;
	std::shared_ptr<atech::srv::io::ctrl::Module_CTRL> _module_ctrl = std::make_shared<atech::srv::io::ctrl::Module_CTRL>();
	_module_ctrl->LoadConfigService();
	_module_ctrl->InitDDSLayer();
	_module_ctrl->InitUnitAid(config_ctrl);
	_module_ctrl->InitModuleIO();


	while (1)
	{
		std::cin >> c;
		if (c == 'q') break;
	}

	return 0;
}