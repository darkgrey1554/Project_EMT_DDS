#include <iostream>
#include <string_view>
#include <format>
#include "spdlog/spdlog.h"
#include "LoggerScada.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/cfg/env.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"


template<typename... Args>
void fun(std::string_view str, Args&&... args)
{
    std::string helpstr = std::vformat(str, std::make_format_args(args...));

    return;
}

class A
{
public:

    template<typename... Args>  void fun(std::string_view str, Args&&... args)
    {
        std::string helpstr = std::vformat(str, std::make_format_args(args...));
        std::cout << helpstr <<std::endl;
        return;
    }
};

int main()
{
    LoggerSpaceScada::ConfigLogger conf;
    conf.file_mame = "log.txt";
    conf.file_path = "";
    conf.level = LoggerSpaceScada::LevelLog::Debug;
    conf.numbers_file = 3;
    conf.size_file = 1;
      
    std::shared_ptr <LoggerSpaceScada::LoggerScada> log = LoggerSpaceScada::GetLoggerScada(LoggerSpaceScada::TypeLogger::SPDLOG,conf);


    conf.file_mame.clear();
    conf.file_mame = "";
    std::shared_ptr <LoggerSpaceScada::LoggerScada> log2 = LoggerSpaceScada::GetLoggerScada(LoggerSpaceScada::TypeLogger::SPDLOG, conf);


    for (;;)
    {
        std::shared_ptr <LoggerSpaceScada::LoggerScada> log3 = LoggerSpaceScada::GetLoggerScada(LoggerSpaceScada::TypeLogger::SPDLOG, conf);
        log->Debug("asdasd {} {}", 1, 2);
        log2->Info("qwe {} {}", 1, 2);
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    spdlog::info("Welcome to spdlog!");
    spdlog::error("Some error message with arg: {}", 1);

    spdlog::warn("Easy padding in numbers like {:08d}", 12);
    spdlog::critical("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
    spdlog::info("Support for floats {:03.2f}", 1.23456);
    spdlog::info("Positional args are {1} {0}..", "too", "supported");
    spdlog::info("{:<30}", "left aligned");

    spdlog::set_level(spdlog::level::debug); // Set global log level to debug
    spdlog::debug("This message should be displayed..");

    // change log pattern
    spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");

    spdlog::debug("This message should be displayed..");
    // Compile time log levels
    // define SPDLOG_ACTIVE_LEVEL to desired level
    SPDLOG_TRACE("Some trace message with param {}", 42);
    SPDLOG_DEBUG("Some debug message");
}

