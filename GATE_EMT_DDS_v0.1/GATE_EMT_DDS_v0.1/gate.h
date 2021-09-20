#pragma once

#include "structs.h"
#include "SM_DDS.h"
#include "Config_Reader.h"
#include <vector>

class Gate
{

public:

	Gate* CreateGate(Type_Gate type);
	virtual ~Gate() = 0;

	virtual ResultReqest init_gate() = 0;
	virtual void GetInfoAboutGate() = 0;
	virtual void GetStatusGate() = 0;
	virtual void StartGate() = 0;
	virtual void StopGate() = 0;
	virtual void RestartGate() = 0;
	virtual void RestartRebornGate() = 0;
};


class DDS_Gate : Gate
{

	std::vector<InfoDDSUnit> Publishers;
	std::vector<InfoDDSUnit> Subscribers;
	ConfigReaderDDS reader_config;
	

public :

	ResultReqest init_gate();
	void GetInfoAboutGate();
	void GetStatusGate();
	void StartGate();
	void StopGate();
	void RestartGate();
	void RestartRebornGate();
};

