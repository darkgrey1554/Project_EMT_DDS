#include <type_traits>
#include <cstdint>
#include <iostream>
#include "ClientSharedMemory.h"
#include<chrono>
#include<array>
#include<thread>
#include <cstddef>

namespace atech = scada_ate::lib::emt;

char c = 'w';

void f(int& i)
{};



void fun_write(atech::ClientSharedMemory& client, size_t s_i, size_t s_f)
{
	//std::vector<int> buf_int(s_i);
	//std::vector<float> buf_float(s_f);
	int* buf_int = new int[s_i];
	float* buf_float = new float[s_f];
	for (int i = 0; i < s_i; i++)
	{
		buf_int[i] = i;
	}
	for (int i = 0; i < s_f; i++)
	{
		buf_float[i] = i;
	}

	while (c != 'q')
	{
		//client.WriteData(buf_int);
		//client.WriteData(buf_float);
		client.WriteData(buf_int,1000);
		client.WriteData(buf_float, 2000);
		/*for (auto& iter : buf_int)
		{
			if (iter > 100) { iter = 0; }
			else { iter += 1; };
		}

		for (auto& iter : buf_float)
		{
			if (iter > 100) { iter = 0; }
			else { iter += 0.1; };
		}*/

		for(int i = 0; i < s_i; i++)
		{
			buf_int[i]+=1;
		}
		for (int i = 0; i < s_f; i++)
		{
			buf_float[i]+=0.1;
		}

		std::this_thread::sleep_for(std::chrono::seconds(1));		
	}
}

void fun_read(atech::ClientSharedMemory& client, size_t s_i, size_t s_f)
{
	std::vector<int> buf_int(s_i);
	std::vector<float> buf_float(s_f);

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

//int main(int argv, char* argc[])
//{
//	std::cout << sizeof(HeaderSharedMemory) << std::endl;
//	std::cout << alignof(HeaderSharedMemory) << std::endl;
//	std::cout << "long long\t" << sizeof(long long) << std::endl;
//	std::cout << "unsigned long\t" << sizeof(unsigned long) << std::endl;
//	std::cout << "size_t\t\t" << sizeof(std::size_t) << std::endl;
//	std::cout << "asd\t\t" << sizeof(asd) << std::endl;
//
//	return 0;
//}

int main(int argv, char* argc[])
{
	if (argv != 5) return 1;

	std::string command = argc[1];
	std::string name_memory = argc[2];
	size_t size_int = std::atoi(argc[3]);
	size_t size_float = std::atoi(argc[4]);
	

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
		t = std::thread(fun_write, std::ref(memory), size_int, size_float);
	}
	else
	{
		t = std::thread(fun_read, std::ref(memory), size_int, size_float);
	}

	while (c != 'q')
	{
		std::cin >> c;
	}

	t.join();

	return 0;
}