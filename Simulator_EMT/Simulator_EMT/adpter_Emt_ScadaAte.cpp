#include "adapter_EmtScadaAte.hpp"

namespace scada_ate
{
	namespace emt
	{
		AdapterEMTScada::AdapterEMTScada()
		{
			security_attr.InitSecurityAttrubuts();
		}

		AdapterEMTScada::AdapterEMTScada(std::string pointname) : point_name(pointname)
		{
			security_attr.InitSecurityAttrubuts();
		}

		AdapterEMTScada::~AdapterEMTScada()
		{
			std::lock_guard<std::mutex> lock(guard_map_object);

			for (auto iter = map_object.begin(); iter != map_object.end(); iter++)
			{
				UnmapViewOfFile(iter->second.bufffer);
				CloseHandle(iter->second.memory);
				CloseHandle(iter->second.mutex);
			}

			map_object.clear();
		}

		unsigned int AdapterEMTScada::ReadData(TypeData type, void* buf, unsigned int size, std::string pointname)
		{
			unsigned int result = 0;
			std::map<std::string, ObjectMemory>::iterator iter;
			DWORD result_winapi;
			char* buf_in = (char*)buf;
			char* buf_out = nullptr;
			std::time_t time_p;
			std::tm* time_now;
			std::chrono::system_clock::time_point time;
			std::chrono::milliseconds msec;
			unsigned int size_type = TypeDataToSizeByte(type);
			HeaderSharedMemory* head = nullptr;
			unsigned int size_ = 0;

			try
			{
				iter = FindCreateObject(pointname, type, size);

				if (iter == map_object.end()) throw 1;

				result_winapi = WaitForSingleObject(iter->second.mutex, 5000);
				if (result_winapi != WAIT_OBJECT_0)
				{
					throw 2;
				}

				head = reinterpret_cast<HeaderSharedMemory*>(iter->second.bufffer);

				if (head->typedata != type)
				{
					throw 3;
				}				

				buf_out = iter->second.bufffer + sizeof(HeaderSharedMemory);
				size_ = size > head->size_data ? head->size_data : size;

				for (int i = 0; i < size_type * size_; i++)
				{
					*(buf_in + i) = *(buf_out + i);
				}

				/// --- take time --- /// 

				time = std::chrono::system_clock::now();
				time_p = std::chrono::system_clock::to_time_t(time);
				time_now = std::localtime(&time_p);
				msec = std::chrono::duration_cast<std::chrono::milliseconds>(time.time_since_epoch())
					- std::chrono::duration_cast<std::chrono::milliseconds>
					(std::chrono::duration_cast<std::chrono::seconds>(time.time_since_epoch()));

				/// --- write time in header --- /// 
				head->count_read++;
				head->TimeLastRead.h = time_now->tm_hour;
				head->TimeLastRead.m = time_now->tm_min;
				head->TimeLastRead.s = time_now->tm_sec;
				head->TimeLastRead.ms = msec.count();
			}
			catch(int& e)
			{
				result = e;
			}
			catch (...)
			{
				result = UINT_MAX;
			}

			if (result_winapi == WAIT_OBJECT_0)
			{
				ReleaseMutex(iter->second.mutex);
			}
			return result;

		}

		unsigned int AdapterEMTScada::WriteData(TypeData type, void* buf, unsigned int size, std::string pointname)
		{
			unsigned int result = 0;
			std::map<std::string, ObjectMemory>::iterator iter;
			DWORD result_winapi;
			char* buf_in = (char*)buf;
			char* buf_out = nullptr;
			std::time_t time_p;
			std::tm* time_now;
			std::chrono::system_clock::time_point time;
			std::chrono::milliseconds msec;
			unsigned int size_type = TypeDataToSizeByte(type);
			HeaderSharedMemory* head = nullptr;
			unsigned int size_ = 0;

			try
			{
				iter = FindCreateObject(pointname, type, size);

				if (iter == map_object.end()) throw 1;

				result_winapi = WaitForSingleObject(iter->second.mutex, 5000);
				if (result_winapi != WAIT_OBJECT_0)
				{
					throw 2;
				}

				head = (HeaderSharedMemory*)iter->second.bufffer;

				if (head->typedata != type) throw 3;


				buf_out = iter->second.bufffer + sizeof(HeaderSharedMemory);
				size_ = size > head->size_data ? head->size_data : size;

				for (int i = 0; i < size_type * size; i++)
				{
					*(buf_out + i) = *(buf_in + i);
				}			

				/// --- take time --- /// 

				time = std::chrono::system_clock::now();
				time_p = std::chrono::system_clock::to_time_t(time);
				time_now = std::localtime(&time_p);
				msec = std::chrono::duration_cast<std::chrono::milliseconds>(time.time_since_epoch())
					- std::chrono::duration_cast<std::chrono::milliseconds>
					(std::chrono::duration_cast<std::chrono::seconds>(time.time_since_epoch()));

				/// --- write time in header --- /// 
				head->count_write++;
				head->TimeLastWrite.h = time_now->tm_hour;
				head->TimeLastWrite.m = time_now->tm_min;
				head->TimeLastWrite.s = time_now->tm_sec;
				head->TimeLastWrite.ms = msec.count();
			}
			catch (int& e)
			{

				result = e;
			}
			catch (...)
			{
				result = UINT_MAX;
			}

			if (result_winapi == WAIT_OBJECT_0)
			{
				ReleaseMutex(iter->second.mutex);
			}
			return result;
		}

		unsigned int AdapterEMTScada::ReadData(TypeData type, void* buf, unsigned int size)
		{
			unsigned int res = 0;
			if (point_name.empty()) return 10;
			res = ReadData(type, buf, size, point_name);
			return res;
		}

		unsigned int AdapterEMTScada::WriteData(TypeData type, void* buf, unsigned int size)
		{
			unsigned int res = 0;
			if (point_name.empty()) return 10;
			res = WriteData(type, buf, size, point_name);
			return res;
		}

		unsigned int AdapterEMTScada::WatchData(void* buf, HeaderSharedMemory& head, unsigned int size, std::string pointname)
		{
			HANDLE mutex = NULL;
			HANDLE memory = NULL;
			char* buffer = nullptr;
			ObjectMemory object;
			unsigned int size_type = 0;
			HANDLE Mutex_SM = NULL;
			HANDLE SM_Handle = NULL;
			int size_;
			HeaderSharedMemory* header = nullptr;
			std::string namememory = CreataNameMemory(pointname);
			std::string namemutex = CreataNameMutexMemory(pointname);
			char* buf_out;
			char* buf_;
			unsigned int result = 0;

			try
			{
				Mutex_SM = CreateMutexA(&security_attr.getsecurityattrebut(), TRUE, namemutex.c_str());
				if (Mutex_SM == NULL) throw 3;

				SM_Handle = OpenFileMappingA(FILE_ALL_ACCESS, FALSE, namememory.c_str());
				if (SM_Handle == NULL)
				{
					throw 4;
				}
				else
				{
					buffer = (char*)MapViewOfFile(SM_Handle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
					if (buffer == NULL) throw 5;
				}

				head = *((HeaderSharedMemory*)buffer);

				if (head.typedata == TypeData::ANALOG || head.typedata == TypeData::DISCRETE) size_type = sizeof(float);
				if (head.typedata == TypeData::BINAR ) size_type = sizeof(char);
				size_ = size > head.size_data ? head.size_data : size;
				buf_out = (char*)buf;
				buf_ = buffer + sizeof(HeaderSharedMemory);

				for (unsigned int i = 0; i < size_type * size_; i++)
				{
					*(buf_out + i) = *(buf_+i);
				}


			}
			catch(int& e)
			{
				result = e;
			}
			catch (...)
			{
				result = UINT_MAX;
			}

			if (buffer != NULL) UnmapViewOfFile(buffer);
			if (SM_Handle != NULL) CloseHandle(SM_Handle);
			if (Mutex_SM != NULL)
			{
				ReleaseMutex(Mutex_SM);
				CloseHandle(Mutex_SM);
				Mutex_SM = NULL;
			}

			return result;
		}

		unsigned int AdapterEMTScada::TypeDataToSizeByte(TypeData type)
		{
			if (type == TypeData::ANALOG) return sizeof(float);
			if (type == TypeData::DISCRETE) return sizeof(int);
			if (type == TypeData::BINAR) return sizeof(char);

			return 0;
		}

		std::string AdapterEMTScada::CreataNameMutexMemory(std::string sourse)
		{
			return "Global\\Mutex_" + sourse;
		}

		std::string AdapterEMTScada::CreataNameMemory(std::string sourse)
		{
			return "Global\\" + sourse;
		}

		std::map<std::string, ObjectMemory>::iterator AdapterEMTScada::FindCreateObject(std::string name, TypeData type, int size)
		{
			std::map<std::string, ObjectMemory>::iterator iter;
			int result;

			iter = map_object.find(name);
			if (iter != map_object.end()) return iter;

			std::lock_guard<std::mutex> lock(guard_map_object);
			iter = map_object.find(name);
			if (iter != map_object.end()) return iter;
			
			add_map_object(name, type ,size);

			return map_object.find(name);
		}

		void AdapterEMTScada::add_map_object(std::string name, TypeData type, unsigned int size)
		{
			HANDLE mutex = NULL;
			HANDLE memory = NULL;
			char* buffer = nullptr;
			ObjectMemory object;
			unsigned int size_type = 0;
			HANDLE Mutex_SM = NULL;
			HANDLE SM_Handle = NULL;
			HeaderSharedMemory* header = nullptr;
			std::string namememory = CreataNameMemory(name);
			std::string namemutex = CreataNameMutexMemory(name);

			try
			{
				size_type = TypeDataToSizeByte(type);
				if (size_type == 0) throw 1;

				Mutex_SM = CreateMutexA(&security_attr.getsecurityattrebut(), TRUE, namemutex.c_str());
				if (Mutex_SM == NULL) throw 3;

				SM_Handle = OpenFileMappingA(FILE_ALL_ACCESS, FALSE, namememory.c_str());
				if (SM_Handle == NULL)
				{
					SM_Handle = CreateFileMappingA(INVALID_HANDLE_VALUE, &security_attr.getsecurityattrebut(), PAGE_READWRITE, 0, size * size_type + sizeof(HeaderSharedMemory), namememory.c_str());
					if (SM_Handle == NULL) throw 4;
					buffer = (char*)MapViewOfFile(SM_Handle, FILE_MAP_ALL_ACCESS, 0, 0, size * size_type + sizeof(HeaderSharedMemory));
					if (buffer == NULL) throw 5;

					header = (HeaderSharedMemory*)buffer;
					header->typedata = type;
					header->size_data = size;
					header->count_read = 0;
					header->count_write = 0;
					header->TimeLastRead.h = 0;
					header->TimeLastRead.m = 0;
					header->TimeLastRead.s = 0;
					header->TimeLastRead.ms = 0;
					header->TimeLastWrite.h = 0;
					header->TimeLastWrite.m = 0;
					header->TimeLastWrite.s = 0;
					header->TimeLastWrite.ms = 0;
				}
				else
				{
					buffer = (char*)MapViewOfFile(SM_Handle, FILE_MAP_ALL_ACCESS, 0, 0, size * size_type + sizeof(HeaderSharedMemory));
					if (buffer == NULL) throw 5;
				}				

				object.mutex = Mutex_SM;
				object.memory = SM_Handle;
				object.bufffer = buffer;

				map_object.insert({ name,object });

			}
			catch (...)
			{
				if (buffer != NULL) UnmapViewOfFile(buffer);
				if (SM_Handle != NULL) CloseHandle(SM_Handle);
				if (Mutex_SM != NULL)
				{
					ReleaseMutex(Mutex_SM);
					CloseHandle(Mutex_SM);
					Mutex_SM = NULL;
				}
			}

			ReleaseMutex(Mutex_SM);
			return;
		}

	}
}