#include <iostream>
#include <sys/file.h>
#include <logger.h>"
#include <chrono>
#include "daemon_init.h"
#include <unistd.h>
#include <sys/types.h>

constexpr auto block_file_name = "/home/user/block_file.txt";

int main()
{
    InitDaemon();
    chdir("/home/user/log");

    LoggerSpace::Logger* log;
    log = LoggerSpace::Logger::getpointcontact();
    log->SetNameLog("LogProccess");
    log->TurnOnLog();

    int block_file = 0;
    int result_pid = 0;
    char error_block_file = 0;
    int sys_error_block_file = 0;   


    while (1)
    {

        block_file = open(block_file_name, O_CREAT | O_TRUNC | O_WRONLY, S_IRWXU | S_IRWXG | S_IRWXO);
        if (block_file == -1)
        {
            error_block_file = 1;
            sys_error_block_file = errno;
            log->WriteLogWARNING("ERROR OPEN FILE", 0, errno);
            std::this_thread::sleep_for(std::chrono::milliseconds(4000));
            continue;
        }

        result_pid = flock(block_file, LOCK_EX | LOCK_NB);
        if (result_pid != 0)
        {
            close(block_file);
            error_block_file = 2;
            sys_error_block_file = errno;
            log->WriteLogWARNING("ERROR BLOCK FILE", 0, errno);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            continue;
        }
        break;
    }

    log->WriteLogWARNING("BLOCK FILE SUCCSESFUL", 0, errno);

    pid_t pid = getpid();
    std::string str;
    str += std::to_string(pid);
    if (write(block_file, str.c_str(), str.size()) != str.size())
    {
        log->WriteLogWARNING("ERROR WRITE PID IN FILE", 0, errno);
    }

    while (1)
    {
        log->WriteLogWARNING("I AM WORK, HARD WORK", 0, errno);
        std::this_thread::sleep_for(std::chrono::milliseconds(2500));
    }

	return 0;
};