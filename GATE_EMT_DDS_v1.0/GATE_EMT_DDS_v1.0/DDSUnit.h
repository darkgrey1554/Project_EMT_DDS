#pragma once
#include "structs.h"
#include "SM_DDS.h"
#include "KKS_Reader.h"

class DDSUnit
{


public:




};

class DDSUnit_Subcriber : DDSUnit
{
	SharedMemoryDDS* SharedMemoryUnit;
	KKSReader* readerkks;

public:

	DDSUnit_Subcriber();
	~DDSUnit_Subcriber();

	void Stop();
	void Start();
	void GetCurrentStatus();
	void GetConfig();
	void Restart();
};