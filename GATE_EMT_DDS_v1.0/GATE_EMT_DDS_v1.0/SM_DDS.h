#pragma once

#include "structs.h"
#include <windows.h>
#include <logger.h>
#include <chrono>
#include <time.h>
#include <stdio.h>
/// <summary>
///  класс реализует взаимодействие между ЕМТ и шлюзом DDS по Shared Memory
/// </summary>

std::string CreateNameMemoryDDS(TypeData type, TypeDirection val, unsigned int domen);

class SharedMemoryDDS
{
	
	HANDLE SM_Analog = NULL;
	HANDLE SM_Discrete = NULL;
	HANDLE SM_Binar = NULL;
	void* buf_analog = NULL;
	void* buf_discrete = NULL;
	void* buf_binar = NULL;
	unsigned int size_analog_memory = 0;
	unsigned int size_discrete_memory = 0;
	unsigned int size_binar_memory = 0;
	HANDLE Mut_Analog = NULL;
	HANDLE Mut_Binar = NULL;
	HANDLE Mut_Discrete = NULL;

	LoggerSpace::Logger* log;

	ResultReqest CreateMemoryAnalog (TypeDirection val, unsigned int size, std::string name);
	ResultReqest CreateMemoryDiscrete(TypeDirection val, unsigned int size, std::string name);
	ResultReqest CreateMemoryBinar(TypeDirection val, unsigned int size, std::string name);
	
public:

	ResultReqest CreateMemory(TypeData type, TypeDirection val,unsigned int size, std::string name);
	ResultReqest ReadMemory(TypeData type, void* buf, unsigned int size);
	ResultReqest WriteMemory(TypeData type, void* buf, int size);
	HeaderSharedMemory ReadHead(TypeData type);
	void DeleteMemory(TypeData type);

	SharedMemoryDDS();
	~SharedMemoryDDS();

};