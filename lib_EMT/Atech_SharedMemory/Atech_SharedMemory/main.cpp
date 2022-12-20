#include <type_traits>
#include <cstdint>
#include <iostream>
#include "ClientSharedMemory.h"
#include<chrono>
#include<array>
#include<thread>

namespace atech = scada_ate::lib::emt;

char c = 'w';

void fun_write(atech::ClientSharedMemory& client)
{

	std::vector<int> buf_int(100);
	std::vector<float> buf_float(100);
	for (int i = 0; i < 100; i++)
	{
		buf_int[i] = i;
		buf_float[i] = i;
	}

	while (c != 'q')
	{
		client.WriteData(buf_int);
		client.WriteData(buf_float);

		for (int i = 0; i < 100; i++)
		{
			buf_int[i]+=1;
			buf_float[i]+=0.1;
		}

		std::this_thread::sleep_for(std::chrono::seconds(1));		
	}
}

void fun_read(atech::ClientSharedMemory& client)
{
	std::vector<int> buf_int(100);
	std::vector<float> buf_float(100);

	while (c != 'q')
	{
		client.ReadData(buf_int);
		client.ReadData(buf_float);

		for (int i = 0; i < 100; i++)
		{
			std::cout << "--- Receive --- \n";
			std::cout << "buf_int[0] = " << buf_int[0] << "\n";
			std::cout << "buf_int[last] = " << buf_int[buf_int.size()-1] << "\n";
			std::cout << "buf_float[0] = " << buf_float[0] << "\n";
			std::cout << "buf_float[last] = " << buf_float[buf_float.size() - 1] << "\n";
			std::cout << std::endl;
		}

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

int main(int argv, char* argc[])
{
	if (argv != 3) return 1;

	std::string command = argc[1];
	std::string name_memory = argc[2];
	

	atech::ClientSharedMemory memory(name_memory);

	while (1)
	{
		std::cout << "TRY CONNECT" << std::endl;
		if (memory.Connect() == atech::ResultRequest::GOOD) break;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	
	std::cout << "CONNECT DONE" << std::endl;

	std::thread  t;
	if (command == "w")
	{
		t = std::thread(fun_write, memory);
	}
	else
	{
		std::thread t = std::thread(fun_read, memory);
	}

	while (c != 'q')
	{
		std::cin >> c;
	}

	t.join();

	return 0;
}