#pragma once

#include <windows.h>
#include "security_handle.h"
#include <vector>
#include <mutex>
#include <map>
#include "structs.hpp"

namespace scada_ate
{
	namespace emt
	{

		class AdapterEMTScada
		{
			SecurityHandle security_attr;

			std::map<std::string, ObjectMemory> map_object;
			std::mutex guard_map_object;

			std::string point_name;
			unsigned int TypeDataToSizeByte(TypeData type);
			std::string CreataNameMutexMemory(std::string sourse);
			std::string CreataNameMemory(std::string sourse);
			std::map<std::string, ObjectMemory>::iterator FindCreateObject(std::string name, TypeData type, int size);
			void add_map_object(std::string name, TypeData type, unsigned int size);

		public:
			AdapterEMTScada();
			AdapterEMTScada(std::string pointname);
			~AdapterEMTScada();
			unsigned int ReadData(TypeData type, void* buf, unsigned int size);
			unsigned int WriteData(TypeData type, void* buf, unsigned int size);
			unsigned int ReadData(TypeData type, void* buf, unsigned int size, std::string pointname);
			unsigned int WriteData(TypeData type, void* buf, unsigned int size, std::string pointname);
			unsigned int WatchData(void* buf, HeaderSharedMemory& head, unsigned int size, std::string pointname);
		};
	}
}