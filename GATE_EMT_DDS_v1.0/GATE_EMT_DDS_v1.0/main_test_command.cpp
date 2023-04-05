
#include <Module_CTRL/Module_CTRL.hpp>
#include <structs/FactoryDds.h>

char c = 'w';
//alignas(64)


struct alignas(64) Value
{
	//long long time = 0;
	std::variant<int, float, double, char, std::string> value;
	//char quality = 0;
};

struct alignas(64) Values
{
	//long long time = 0;
	int value;
	//char quality = 0;
};

int main()
{

	long long qwe = 0;;

	std::vector<Value> map1(4000);
	std::vector<Values> map2(4000);
	long long start = TimeConverter::GetTime_LLmcs();
	long long end = TimeConverter::GetTime_LLmcs();

	for (int i = 0; i < 4000; i++)
	{
		auto& v = map1[i];
		//v.quality = 0;
		//v.time = i;
		v.value = i;
	}

	for (int i = 0; i < 4000; i++)
	{
		auto& v = map2[i];
		//v.quality = 0;
		//v.time = i;
		v.value = i;
	}


	for (int i = 0; i < 100000; i++)
	{
		start = TimeConverter::GetTime_LLmcs();
		for (int i = 0; i < 4000; i++)
		{
			//auto& v = map1[i];
			map1[i].value = i;
			//v.quality = 0;
			//v.time = i;
			//v.value = i;
		}
		end = TimeConverter::GetTime_LLmcs();
		qwe += end - start;
		start = end;
	}

	std::cout << qwe * 1. / 100000 << std::endl;

	qwe = 0;
	for (int i = 0; i < 100000; i++)
	{
		start = TimeConverter::GetTime_LLmcs();
		for (int i = 0; i < 4000; i++)
		{
			map2[i].value = i;
			//v.quality = 0;
			//v.time = i;
			//v.value = i;
		}
		end = TimeConverter::GetTime_LLmcs();
		qwe += end - start;
		start = end;
	}

	std::cout << qwe * 1. / 100000 << std::endl;;



	/*std::shared_ptr<atech::srv::io::FactoryDDS>_factory_dds = atech::srv::io::FactoryDDS::get_instance();
	
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

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	_module_ctrl->InitUnitAid(config_ctrl);
	_module_ctrl->InitModuleIO();

	while (1)
	{
		std::cin >> c;
		if (c == 'q') break;
	}*/

	

	return 0;
}