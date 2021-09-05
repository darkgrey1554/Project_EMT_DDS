#pragma once

#include "structs.h"
#include "SM_DDS.h"
#include "Config_Reader.h"

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

	ConfigReaderDDS* ConfigReader;




public :

	ResultReqest init_gate();
	void GetInfoAboutGate();
	void GetStatusGate();
	void StartGate();
	void StopGate();
	void RestartGate();
	void RestartRebornGate();
};

