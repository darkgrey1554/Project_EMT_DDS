#include <iostream>
#include "Config_Reader.h"

int main()
{
	
	ConfigReaderDDS* conf = new ConfigReaderDDS();
	conf->ReadConfigDDS();

	std::cout << "Hello" <<std::endl;

}