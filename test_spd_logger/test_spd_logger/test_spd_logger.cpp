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

void t()
{
	atech::logger::ILoggerScada_ptr log = std::make_shared<atech::logger::LoggerScadaSpdDds>();

	for (int i = 0; i < 5000; i++)
	{
		log->Debug("1");
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
	atech::logger::ILoggerScada_ptr log = std::make_shared<atech::logger::LoggerScadaSpdDds>();

	std::shared_ptr<atech::logger::ConfigLoggerSpdDds> config = std::make_shared<atech::logger::ConfigLoggerSpdDds>();

	config->datawriter_ptr = nullptr;
	config->file_name = "log_123";
	config->file_number = 3;
	config->file_path = "";
	config->file_size = 10;
	config->level = atech::logger::LevelLog::DEBUG;

	log->Init(config);

	log->Debug("MAIN");

	std::thread t1(t);
	std::thread t2(t);
	std::thread t3(t);
	std::thread t4(t);

	t1.join();
	t2.join();
	t3.join();
	t4.join();

	return 0;
}

