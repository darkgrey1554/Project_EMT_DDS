#include <iostream>
#include <sys/file.h>
#include <chrono>
#include <thread>
#include <unistd.h>
//#include "logger.h"
#include <signal.h>
#include <cstring>
#define block_file_name "/home/user/block_file.txt"
#define proccess "/home/user/projects/test_block/bin/x64/Release/test_block.out"

int main(int arg, char** args)
{
    /*chdir("/home/user/log");
    LoggerSpace::Logger* log = LoggerSpace::Logger::getpointcontact();
    log->SetNameLog("LogVisor");
    log->TurnOnLog();*/
    std::string str_help;

    int block_file = 0;
    int res = 0;
    char simvol = 0;
    char error_block_file = 0;
    int sys_error_block_file = 0;
    pid_t pid;

    std::cout << "HELLO I AM VISOR" << std::endl;
    //log->WriteLogWARNING("HELLO I AM VISOR", 0, errno);

    if (arg == 2 && std::strcmp(args[1],"status") == 0)
    {
        block_file = open(block_file_name, O_RDONLY);
        if (block_file == -1)
        {
            std::cout << "ERROR OPEN FILE : PROCCESS DEAD " << errno << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            return 0;
        }

        res = flock(block_file, LOCK_EX | LOCK_NB);
        if (res != 0)
        {
            error_block_file = 2;
            sys_error_block_file = errno;
            //log->WriteLogWARNING("ERROR BLOCK FILE: PROCESS LIVE ", 0, errno);
            std::cout << "ERROR BLOCK FILE: PROCESS LIVE " << errno << std::endl;
        }
        else
        {
            std::cout << "BLOCK FILE SUCCESS : PROCCESS DEAD " << errno << std::endl;
            flock(block_file, LOCK_UN);
            close(block_file);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            return 0;
        }

        while (read(block_file, &simvol, 1) > 0)
        {
            str_help += simvol;
        }
        std::cout << "PID PROCCESS: " << str_help << std::endl;
        close(block_file);
        return 0;
    }

    if (arg == 2 && std::strcmp(args[1], "kill") == 0)
    {
        block_file = open(block_file_name, O_RDONLY);
        if (block_file == -1)
        {
            std::cout << "ERROR OPEN FILE : PROCCESS DEAD " << errno << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            return 0;
        }

        res = flock(block_file, LOCK_EX | LOCK_NB);
        if (res != 0)
        {
            error_block_file = 2;
            sys_error_block_file = errno;
            //log->WriteLogWARNING("ERROR BLOCKING FILE: PROCESS LIVE ", 0, errno);
        }
        else
        {
            std::cout << "BLOCKING FILE SUCCESS : PROCCESS DEAD " << errno << std::endl;
            flock(block_file, LOCK_UN);
            close(block_file);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            return 0;
        }

        while (read(block_file, &simvol, 1) > 0)
        {
            str_help += simvol;
        }
        std::cout << "PID PROCCESS: " << str_help << std::endl;
        
        if (kill(std::stoi(str_help), 9) != 0)
        {
            std::cout << "ERROR KILL PROCCESS: " << errno << std::endl;
            return 0;
        }

        std::cout << "PROCCESS DEAD: " << errno << std::endl;
        close(block_file);
        return 0;
    }
    
    if (arg == 2 && std::strcmp(args[1], "create") == 0)
    {
        block_file = open(block_file_name, O_RDONLY);
        if (block_file == -1)
        {
            std::cout << "ERROR OPEN FILE : PROCCESS DEAD " << errno << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            return 0;
        }

        res = flock(block_file, LOCK_EX | LOCK_NB);
        if (res != 0)
        {
            error_block_file = 2;
            sys_error_block_file = errno;
            std::cout << "ERROR BLOCK FILE: PROCESS LIVE " << errno << std::endl;
            close(block_file);
            return 0;
        }
        else
        {
            std::cout << "BLOCKING FILE SUCCESS : PROCCESS DEAD " << errno << std::endl;
            flock(block_file, LOCK_UN);
            close(block_file);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }

        int pid_f;
        pid_f = fork();

        if (pid_f == 0)
        {
            pid_f = execv(proccess, NULL);
            if (pid_f == -1)
            {
                std::cout << "ERROR CREATE PROCCESS: " << errno << std::endl;
            }
        }
        else if (pid == -1)
        {
            std::cout << "ERROR CREATE PROCCESS: " << std::endl;
        }
        
        std::cout << "CREATE PROCCESS PID: "<< pid_f << std::endl;
        return 0;
    }
    
    return 0;
};