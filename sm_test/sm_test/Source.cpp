#include <iostream>
#include "ClientSharedMemory.h"
#include<thread>

namespace atech = scada_ate::lib::emt;

void example_input_data()
{
	std::string name_memory_input_data = "name_memory_input_data"; // the name of the shared memory area
	std::vector<int> vector_digital_input_data(100);  // buffer for reading digital data ( the size corresponds to the amount of data being read)
	std::vector<float> vector_analog_input_data(100); // buffer for reading analog data	 ( the size corresponds to the amount of data being read)

	atech::ClientSharedMemory memory_input(name_memory_input_data);	// client initialization
	
	while (1)
	{
		atech::ResultRequest result;
		std::cout << "TRY CONNECT" << std::endl;
		result = memory_input.Connect();  // client connection
		if  ( result == atech::ResultRequest::GOOD) break;
		std::cout << "ERROR CONNECT: " << static_cast<int>(result) << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	std::cout << "CONNECT DONE " << std::endl;
	memory_input.ReadData(vector_digital_input_data); // reading data from shared memory 
	memory_input.ReadData(vector_analog_input_data); // returns atech::ResultRequest::IGNOR if there is no connection
}

void example_output_data()
{
	std::string name_memory_output_data = "name_memory_output_data"; // the name of the shared memory area
	std::vector<int> vector_digital_output_data(100);  // buffer for writing digital data ( the size corresponds to the amount of data being write)
	std::vector<float> vector_analog_output_data(100); // buffer for writing analog data	 ( the size corresponds to the amount of data being write)

	atech::ClientSharedMemory memory_input(name_memory_output_data);	// client initialization

	while (1)
	{
		atech::ResultRequest result;
		std::cout << "TRY CONNECT" << std::endl;
		result = memory_input.Connect();  // client connection
		if (result == atech::ResultRequest::GOOD) break;
		std::cout << "ERROR CONNECT: " << static_cast<int>(result) << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	std::cout << "CONNECT DONE " << std::endl;
	memory_input.WriteData(vector_digital_output_data); // writing data from shared memory 
	memory_input.WriteData(vector_analog_output_data); // returns atech::ResultRequest::IGNOR if there is no connection
}

int main()
{
	example_output_data();
	example_input_data();
	return 0;
}