
#include <Module_CTRL/Module_CTRL.hpp>
#include <structs/FactoryDds.h>

char c = 'w';

int main()
{
	std::shared_ptr<atech::srv::io::FactoryDDS>_factory_dds = atech::srv::io::FactoryDDS::get_instance();
	
	std::shared_ptr<atech::logger::ConfigLoggerSpdDds> config = std::make_shared<atech::logger::ConfigLoggerSpdDds>();
	config->datawriter_ptr = nullptr;
	config->file_name = "log_test";
	config->file_number = 3;
	config->file_size = 10;
	config->level = atech::logger::LevelLog::DEBUG;

	atech::logger::LoggerScadaSpdDds_ptr log = std::make_shared<atech::logger::LoggerScadaSpdDds>();
	log->Init(config);
	

	atech::srv::io::ctrl::ConfigUnitAid_DDS_ptr config_ctrl = std::make_shared<atech::srv::io::ctrl::ConfigUnitAid_DDS>();
	config_ctrl->type_aid = atech::srv::io::ctrl::TypeUnitAid::DDS;
	std::shared_ptr<atech::srv::io::ctrl::Module_CTRL> _module_ctrl = std::make_shared<atech::srv::io::ctrl::Module_CTRL>();
	_module_ctrl->LoadConfigService();
	_module_ctrl->InitDDSLayer();

	
	//_factory_dds->registration_topic("dds_status", atech::srv::io::TypeTopic::DDSStatus);
	//log->SetDataWriterDDS(_factory_dds->get_datawriter("dds_status"));

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	_module_ctrl->InitUnitAid(config_ctrl);
	_module_ctrl->InitModuleIO();

	while (1)
	{
		std::cin >> c;
		if (c == 'q') break;
	}

	return 0;
}