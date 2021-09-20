#include "KKS_Reader.h"
#include <iostream>
#include <fstream>

KKSReader::KKSReader()
{
	log = LoggerSpace::Logger::getpointcontact();
}

ResultReqest KKSReader::ReadKKSlist(std::string file_name)
{

	std::string str;
	std::string kks;
	std::string helpstr;
	unsigned int position;
	std::ifstream file;
	KKSUnit unit;

	file.open(file_name.c_str());
	size_t pos[2] { 0,0 };
	if (!file.is_open())
	{
		log->WriteLogWARNING("ERROR OPEN KKSLIST", 1, 0);
		return ResultReqest::ERR;
	}

	while (!file.eof())
	{
		str.clear();
		std::getline(file, str, '\n');
		pos[0] = str.find('\t', 0);
		if (pos[0] == std::string::npos)
		{
			log->WriteLogWARNING("ERROR LIST KKS", 2, 0);
			continue;
		}

		pos[1] = str.find('\t', pos[0]+1);
		if (pos[1] == std::string::npos)
		{
			log->WriteLogWARNING("ERROR LIST KKS", 3, 0);
			continue;
		}

		kks = str.substr(0,(size_t)pos[0]);
		helpstr.clear();
		helpstr = str.substr((size_t)pos[1] + 1);
		position = atoi(helpstr.c_str());

		unit.KKS = kks;
		unit.position = position;

		if (str.find("Analog") != -1)
		{
			{
				auto iter = vector_analog.begin();
				if (vector_analog.empty())
				{
					vector_analog.push_back(unit);
					continue;
				}

				for (; iter != vector_analog.end();)
				{
					if ((*iter).position > position)
					{
						break;
					}
					iter++;
				}

				if (iter == vector_analog.end())
				{
					vector_analog.push_back(unit);
					continue;
				}

				vector_analog.insert(iter, unit);
			}
		}

		if (str.find("Discrete") != -1)
		{
			{
				auto iter = vector_discrete.begin();
				if (vector_discrete.empty())
				{
					vector_discrete.push_back(unit);
					continue;
				}

				for (; iter != vector_discrete.end();)
				{
					if ((*iter).position > position)
					{
						break;
					}
					iter++;
				}

				if (iter == vector_discrete.end())
				{
					vector_discrete.push_back(unit);
					continue;
				}

				vector_discrete.insert(iter, unit);
			}
		}

		if (str.find("Binar") != -1)
		{
			{
				auto iter = vector_binar.begin();
				if (vector_analog.empty())
				{
					vector_binar.push_back(unit);
					continue;
				}

				for (; iter != vector_binar.end();)
				{
					if ((*iter).position > position)
					{
						break;
					}
					iter++;
				}

				if (iter == vector_binar.end())
				{
					vector_binar.push_back(unit);
					continue;
				}

				vector_binar.insert(iter, unit);
			}
		}

	}

	file.close();
	return ResultReqest::OK;
}

unsigned int KKSReader::ReturnMaxPosition(TypeData type)
{
	if (type == TypeData::ANALOG)
	{
		if (vector_analog.empty()) return 0;
		return vector_analog.back().position;
	}

	if (type == TypeData::DISCRETE)
	{
		if (vector_discrete.empty()) return 0;
		return vector_discrete.back().position;
	}

	if (type == TypeData::BINAR)
	{
		if (vector_binar.empty()) return 0;
		return vector_binar.back().position;
	}

	return 0;
}

unsigned int KKSReader::size_analog()
{
	if (vector_analog.empty()) return 0;
	return vector_analog.back().position + 1;
}
unsigned int KKSReader::size_discrete()
{
	if (vector_discrete.empty()) return 0;
	return vector_discrete.back().position + 1;
}
unsigned int KKSReader::size_binar()
{
	if (vector_binar.empty()) return 0;
	return vector_binar.back().position + 1;
}