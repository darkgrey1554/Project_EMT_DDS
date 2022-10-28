#include <iostream>
#include "mon/include/mon/scd_process.h"
#include <thread>
#include <windows.h>



int main()
{


	
	while (1)
	{
		{
			int id = atech::Process::GetHostID();
			std::string name_host = atech::Process::GetHostName();
			std::cout << "Host_Name" << name_host << std::endl;
			float cpu_useg = atech::Process::GetProcessCpuUsage();
			std::cout << "CPU " << cpu_useg << std::endl;
			int pr_id = atech::Process::GetProcessID();
			std::cout << "ID " << pr_id << std::endl;
			std::string str_ida = atech::Process::GetProcessIDAsString();
			std::cout << "IDA " << str_ida << std::endl;
			unsigned long mem = atech::Process::GetProcessMemory();
			std::cout << "Memory " << mem << std::endl;
			std::string pr_name = atech::Process::GetProcessName();
			std::cout << "Name_pr " << pr_name << std::endl;

			std::cout << std::endl;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
	

	return 0;
};