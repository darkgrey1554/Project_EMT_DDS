#include <iostream>
#include <sys/file.h>
#include <chrono>
#include <thread>
#include <unistd.h>
#include "logger.h"
#include "daemon_init.h"
#define block_file_name "/home/user/block_file.txt"
#define proccess "/home/user/projects/test_blockfile/GATE_EMT_DDS/test_blockfile/test_blockfile/bin/x64/Release/test_process.out"

int main()
{

    InitDaemon();

    chdir("/home/user/log");

    LoggerSpace::Logger* log = LoggerSpace::Logger::getpointcontact();
    log->SetNameLog("LogVisor");
    log->TurnOnLog();

    int block_file = 0;
    int result_pid = 0;
    char error_block_file = 0;
    int sys_error_block_file = 0;
    pid_t pid;

    log->WriteLogWARNING("HELLO I AM PROCCESS", 0, errno);

    while (1)
    {
        block_file = open(block_file_name, O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
        if (block_file == -1)
        {
            error_block_file = 1;
            sys_error_block_file = errno;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            continue;
        }

        result_pid = flock(block_file, LOCK_EX | LOCK_NB);
        if (result_pid != 0)
        {
            error_block_file = 2;
            sys_error_block_file = errno;
            log->WriteLogWARNING("ERROR BLOCK FILE (PROCESS LIVE): ", 0, errno);           
            close(block_file);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            continue;
        }
        else
        {
            log->WriteLogWARNING("BLOCK FILE (PROCESS DEAD):", 0, errno);
            result_pid = flock(block_file, LOCK_UN | LOCK_NB);
            close(block_file);
            pid = fork();
            if (pid == 0)
            {
                result_pid = execv(proccess, NULL);
                if (result_pid == -1)
                {
                    log->WriteLogWARNING("ERROR CREATE PROCCESS", 0, errno);
                }
            }

            if (pid == -1)
            {
                log->WriteLogWARNING("ERROR CREATE PROCCESS", 0, errno);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            continue;
        }
        break;
    }

    return 0;
};