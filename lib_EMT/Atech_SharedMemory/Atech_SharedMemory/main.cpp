#include <type_traits>
#include <cstdint>
#include <iostream>
#include "ClientSharedMemory.h"
#include<chrono>
#include<array>


void read_vector()
{
	atech::ClientSharedMemory memory("test_memory");

	memory.Connect();

	std::vector < std::string > vec(10);

	for (;;)
	{
		memory.ReadData(vec);

		std::cout << "float { ";
		for (auto& it : vec)
		{
			std::cout << it << ", ";
		}
		std::cout << "}" << std::endl;

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		
	}

	return;
};

void read_array()
{
	atech::ClientSharedMemory memory("test_memory");

	memory.Connect();

	std::array<char, 10> arr{};

	for (;;)
	{
		memory.ReadData(&arr.front(), 10);

		std::cout << "float { ";
		for (auto& it : arr)
		{
			std::cout << it << ", ";
		}
		std::cout << "}" << std::endl;

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	}

	return;
};

void write_vector()
{
	atech::ClientSharedMemory memory("test_memory");

	memory.Connect();

	std::vector<int> vec(100);
	int counter = 0;
	for (auto& it : vec)
	{
		it = counter;
		counter++;
	}

	for (;;)
	{
		for (auto& it : vec)
		{
			it ++;
		}
		memory.WriteData(vec);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	return;
}

void write_array()
{
	atech::ClientSharedMemory memory("test_memory");

	memory.Connect();

	std::array<int, 10> arr;
	for (int i = 0; i < arr.size(); i++)
	{
		arr[i] = i/10.;
	}

	for (;;)
	{
		for (auto& it : arr)
		{
			it++;
		}
		memory.WriteData(&arr.front(), arr.size());
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	return;
}

void write_array_str()
{
	atech::ClientSharedMemory memory("test_memory");

	memory.Connect();

	std::array<std::string,20> arr;
	int counter = 0;
	for (auto& it : arr)
	{
		it = "asdqwe123";
		counter++;
	}

	for (;;)
	{
		memory.WriteData(&arr.front(), arr.size());
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	return;
}

void write_vector_str ()
{
	atech::ClientSharedMemory memory("test_memory");

	memory.Connect();

	std::vector<std::string> vec(100);
	int counter = 0;
	for (auto& it : vec)
	{
		it = "asdqwe";
		counter++;
	}

	for (;;)
	{
		/*for (auto& it : vec)
		{
			it++;
		}*/
		memory.WriteData(vec);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	return;
}

void write_vector_char()
{
	atech::ClientSharedMemory memory("test_memory");

	memory.Connect();

	std::vector<char> vec(100);
	int counter = 0;
	for (auto& it : vec)
	{
		it = 'a' + counter;
		counter++;
	}

	for (;;)
	{
		/*for (auto& it : vec)
		{
			it++;
		}*/
		memory.WriteData(vec);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	return;
}

int main()
{
	write_array_str();
	write_array();

	return 0;
}