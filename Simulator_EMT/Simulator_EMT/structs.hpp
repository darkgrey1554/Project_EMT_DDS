#pragma once
#include <string>

namespace scada_ate
{


	enum class TypeTransfer
	{
		PUBLISHER,
		SUBSCRIBER
	};

	enum class TypeData
	{
		ANALOG,
		DISCRETE,
		BINAR,
		ZERO
	};

	enum class TypeSignal
	{
		CONSTANT,
		TRIANGLE,
		SAW,
		SINE
	};

	enum class ShowDataConsole
	{
		ON,
		OFF
	};

	struct UnitSimulation
	{
		TypeTransfer type_transfer;
		std::string point_name;
		TypeData type_data;
		unsigned int size;
		TypeSignal type_signal;
		unsigned int  frequency;
		unsigned int amplitude;
		ShowDataConsole show_console;
		unsigned int size_output;
	};

	enum class ResultRequest
	{
		OK,
		ERR
	};

	struct TimeUnit
	{
		unsigned char h = 0;
		unsigned char m = 0;
		unsigned char s = 0;
		unsigned short ms = 0;
	};

	struct HeaderSharedMemory
	{
		TypeData typedata = TypeData::ZERO;
		TimeUnit TimeLastWrite;
		TimeUnit TimeLastRead;
		unsigned int size_data = 0;
		unsigned long count_write = 0;
		unsigned long count_read = 0;
	};

	struct ObjectMemory
	{
		HANDLE mutex = NULL;
		HANDLE memory = NULL;
		char* bufffer = NULL;
	};

}

