#include <iostream>
#include <string_view>
#include "spdlog/spdlog.h"
#include "spdlog/cfg/env.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <memory>
#include "spdlog/common.h"
#include <atomic>
#include "LoggerScada.h"
#include "LoggerScadaSpdDDS.h"

void t(int a)
{
	atech::logger::ILoggerScada_ptr log = std::make_shared<atech::logger::LoggerScadaSpdDds>();

	for (int i = 0; i < 1000; i++)
	{
		log->Debug("{}",a);
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void t2()
{
	atech::logger::ILoggerScada_ptr log = std::make_shared<atech::logger::LoggerScadaSpdDds>();

	for (int i = 0; i < 1000; i++)
	{
		log->Debug("2");
	}
}

int main()
{
	atech::logger::LoggerScadaSpdDds_ptr log = std::make_shared<atech::logger::LoggerScadaSpdDds>();

	std::shared_ptr<atech::logger::ConfigLoggerSpdDds> config = std::make_shared<atech::logger::ConfigLoggerSpdDds>();
	std::shared_ptr<atech::logger::ConfigLoggerSpdDds> config2 = std::make_shared<atech::logger::ConfigLoggerSpdDds>();

	config->datawriter_ptr = nullptr;
	config->file_name = "log_123";
	config->file_number = 3;
	config->file_path = "";
	config->file_size = 10;
	config->level = atech::logger::LevelLog::DEBUG;

	config2->datawriter_ptr = nullptr;
	config2->file_name = "log2";
	config2->file_number = 3;
	config2->file_path = "";
	config2->file_size = 10;
	config2->level = atech::logger::LevelLog::DEBUG;

	log->Init(config);

	log->Debug("MAIN");

	std::thread t1(t,1);
	std::thread t2(t,2);
	std::thread t3(t,3);
	std::thread t4(t,4);

	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	
	log->ClearLogger();
	log->Init(config2);

	t1.join();
	t2.join();
	t3.join();
	t4.join();

	return 0;
}

