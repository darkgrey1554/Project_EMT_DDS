#pragma once
#include "structs.h"
#include <vector>
#include "logger.h"

class KKSReader
{
	LoggerSpace::Logger* log;

public :

	KKSReader();

	std::vector<KKSUnit> vector_analog;
	std::vector<KKSUnit> vector_discrete;
	std::vector<KKSUnit> vector_binar;
	
	ResultReqest ReadKKSlist(std::string file_name);
	unsigned int ReturnMaxPosition(TypeData type);
	unsigned int size_analog();
	unsigned int size_discrete();
	unsigned int size_binar();

};
