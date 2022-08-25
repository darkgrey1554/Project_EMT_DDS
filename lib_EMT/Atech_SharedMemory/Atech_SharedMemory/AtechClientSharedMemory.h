#pragma once
#include <string>
#include <windows.h>
#include <aclapi.h>
#include <memory>
#include <vector>
#include <atomic>
#include <mutex>
#include <chrono>
#include <type_traits>
#include <cstdint>

template<class F> void f(F*) {};

template<>
void f<char>(char*) {};

namespace atech
{
	enum class ResultRequest
	{
		GOOD = 0,
		WAIT_INITIALIZATION,
		IGNOR,
		WARNANING_EXCESS_RANGE,

		ERROR_INITIALIZATION_SECURITYATTRIBUTS = 50,
		ERROR_INITIALIZATION_MUTEX,
		ERROR_OPENING_FILEMAPPING,
		ERROR_CREATE_FILEMAPPING,
		ERROR_MAPPING_MEMORY,
		ERROR_NOT_INDIFICATED,
		ERROR_MEMORY_ALREADY_EXIST,
		ERROR_MEMORY_ALREADY_INITIALIZATED,
		ERROR_BLOCK_MUTEX,
		ERROR_EXCESS_OFFSET
	};

	enum class TypeData
	{
		INT,
		FLOAT,
		DOUBLE,
		STRING
	};

	enum class Status
	{
		 Null,
		 GOOD,
		 INITIALIZATION,
		 ERROR_CONNECTION
	};

	class ClientSharedMemory
	{
		class SecurityHandle
		{

			PSID pEveryoneSID = NULL;
			PSECURITY_DESCRIPTOR pSD = NULL;
			EXPLICIT_ACCESS ea;
			SID_IDENTIFIER_AUTHORITY SIDAuthWorld = SECURITY_WORLD_SID_AUTHORITY;
			PACL pACL = NULL;
			SECURITY_ATTRIBUTES sa;
			DWORD lasterror = 0;

		public:

			unsigned int InitSecurityAttrubuts();
			DWORD getlasterror();
			SECURITY_ATTRIBUTES& getsecurityattrebut();
		};

		struct HeaderSharedMemory
		{
			long long TimeLastWrite;
			long long TimeLastRead;
			unsigned long count_write = 0;
			unsigned long count_read = 0;
			size_t size_data_int = 0;
			size_t size_data_float = 0;
			size_t size_data_double = 0;
			size_t size_data_char = 0;
			size_t size_str = 0;
		};

		HANDLE SM_Handle = NULL;   /// handle shared memory
		HANDLE Mutex_SM = NULL; /// handle mutex shared memory
		char* buffer = nullptr;
		std::string name;
		std::mutex _mutex_init;
		std::atomic<Status> _status{Status::Null};
		std::atomic<DWORD> _sys_error{0};
		HeaderSharedMemory* _header;

		std::shared_ptr<SecurityHandle> security_attr = nullptr;

		std::string CreateSMName(std::string str);
		std::string CreateSMMutexName(std::string str);
		void destroy();
		ResultRequest connect();
		ResultRequest create(size_t number_int, size_t number_float, size_t number_double, size_t number_str, size_t size_str);
		size_t get_size_memory(size_t number_int, size_t number_float, size_t number_double, size_t number_str, size_t size_str);
		long long get_time_LLmcs();			

		template<class T> ResultRequest WriteData(T point_data, size_t size_data, size_t offset);
		template<class T> ResultRequest WriteData(const std::vector<T>& vector, size_t offset);
		template<class T> ResultRequest ReadData(T point_data, size_t size_data, size_t offset);

		template<class V> TypeData get_type_data();
		size_t get_size_data(const TypeData& type);
		size_t get_offset(const TypeData& type, const size_t& offset);
		bool varification_offset(const size_t& size_block, const size_t& offset);
		ResultRequest calibration_size(const size_t& size_memory, size_t& size_source, const size_t& offset);
		template<class T> void update_data(T source, T target, size_t size);
		template<class T> void update_data(std::vector<T>& source, T* target, size_t size);
		template<class T> void read_data(T source, T target, size_t size);
		
		


		
		public:

			ClientSharedMemory() = delete;
			ClientSharedMemory(std::string name);
			~ClientSharedMemory();
			ResultRequest Connect();
			ResultRequest ReConnect(std::string name);
			ResultRequest CreateAndConnect(std::string name,
				size_t number_int = 0,
				size_t number_float = 0,
				size_t number_double = 0,
				size_t number_str = 0,
				size_t size_str = 0);
			
			ResultRequest WriteData(int* point_data, size_t size_data, size_t offset = 0);
			ResultRequest WriteData(float* point_data, size_t size_data, size_t offset = 0);
			ResultRequest WriteData(double* point_data, size_t size_data, size_t offset = 0);
			ResultRequest WriteData(char* point_data, size_t size_data, size_t offset = 0);

			ResultRequest WriteData(const std::vector<int>& vector, size_t offset = 0);
			ResultRequest WriteData(const std::vector<float>& vector, size_t offset = 0);
			ResultRequest WriteData(const std::vector<double>& vector, size_t offset = 0);
			ResultRequest WriteData(const std::vector<std::string>& vector , size_t offset = 0);

			ResultRequest ReadData(int* point_data, size_t size_data, size_t offset = 0);
			ResultRequest ReadData(float* point_data, size_t size_data, size_t offset = 0);
			ResultRequest ReadData(double* point_data, size_t size_data, size_t offset = 0);
			ResultRequest ReadData(char* point_data, size_t size_data, size_t offset = 0);

			ResultRequest ReadData(std::vector<int>& vector, size_t offset = 0);
			ResultRequest ReadData(std::vector<float>& vector, size_t offset = 0);
			ResultRequest ReadData(std::vector<double>& vector, size_t offset = 0);
			ResultRequest ReadData(std::vector<std::string>& vector, size_t offset = 0);
	};


	unsigned int  ClientSharedMemory::SecurityHandle::InitSecurityAttrubuts()
	{
		std::string messeng;
		unsigned result = 0;
		DWORD res = 0;

		if (!AllocateAndInitializeSid(&SIDAuthWorld, 1, SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, &pEveryoneSID))
		{
			lasterror = GetLastError();
			result |= 1;
			return result;
		}

		ZeroMemory(&ea, 1 * sizeof(EXPLICIT_ACCESS));
		ea.grfAccessPermissions = KEY_ALL_ACCESS | MUTEX_ALL_ACCESS;
		ea.grfAccessMode = SET_ACCESS;
		ea.grfInheritance = NO_INHERITANCE;
		ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
		ea.Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
		ea.Trustee.ptstrName = (LPTSTR)pEveryoneSID;

		res = SetEntriesInAclA(1, (PEXPLICIT_ACCESSA)&ea, NULL, &pACL);
		if (res != ERROR_SUCCESS)
		{
			lasterror = GetLastError();
			result |= 2;
			return result;
		}

		pSD = (PSECURITY_DESCRIPTOR)LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
		if (pSD == NULL)
		{
			lasterror = GetLastError();
			result |= 4;
			return result;
		}

		if (!InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION))
		{
			lasterror = GetLastError();
			result |= 8;
			return result;
		}

		if (!SetSecurityDescriptorDacl(pSD, TRUE, pACL, FALSE))
		{
			lasterror = GetLastError();
			result |= 16;
			return result;
		}

		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = pSD;
		sa.bInheritHandle = FALSE;

		return result;
	}

	DWORD  ClientSharedMemory::SecurityHandle::getlasterror()
	{
		return lasterror;
	}

	SECURITY_ATTRIBUTES& ClientSharedMemory::SecurityHandle::getsecurityattrebut()
	{
		return sa;
	}

	ClientSharedMemory::ClientSharedMemory(std::string name) : name(name), _header(nullptr)
	{
	};																		

	ClientSharedMemory::~ClientSharedMemory()
	{
		std::lock_guard<std::mutex> lock(_mutex_init);
		destroy();
	};

	ResultRequest ClientSharedMemory::Connect()
	{
		std::lock_guard<std::mutex> lock(_mutex_init);
		return connect();
	};

	ResultRequest ClientSharedMemory::ReConnect(std::string name)
	{
		std::lock_guard<std::mutex> lock(_mutex_init);
		this->name = name;
		destroy();
		return connect();
	};

	ResultRequest ClientSharedMemory::CreateAndConnect(std::string name,
		size_t number_int,
		size_t number_float,
		size_t number_double,
		size_t number_str,
		size_t size_str)
	{
		ResultRequest result{ ResultRequest::GOOD };
		std::lock_guard<std::mutex> lock(_mutex_init);
		result = create(number_int, number_float, number_double, number_str, size_str);
		if (result != ResultRequest::GOOD) destroy();
		return result;
	}
								   	
	ResultRequest ClientSharedMemory::connect()
	{
		ResultRequest result{ ResultRequest::GOOD };
		Status current_status = _status.load();

		try
		{
			if (current_status == Status::GOOD)
			{
				return result;
			}
			else if (current_status == Status::INITIALIZATION);
			{
				throw ResultRequest::WAIT_INITIALIZATION;
			}
			_status.store(Status::INITIALIZATION);

			if (security_attr == nullptr)
			{
				unsigned int res = security_attr->InitSecurityAttrubuts();
				if (res != 0)
				{
					_sys_error.store(res);
					throw ResultRequest::ERROR_INITIALIZATION_SECURITYATTRIBUTS;
				}
			}  			

			Mutex_SM = CreateMutexA(&security_attr->getsecurityattrebut(), FALSE, CreateSMMutexName(name).c_str());
			if (Mutex_SM == NULL)
			{
				_sys_error.store(GetLastError());
				throw  ResultRequest::ERROR_INITIALIZATION_MUTEX;
			}
			
			SM_Handle = OpenFileMappingA(FILE_ALL_ACCESS, FALSE, CreateSMName(name).c_str());
			if (SM_Handle == NULL)
			{
				_sys_error.store(GetLastError());
				throw  ResultRequest::ERROR_OPENING_FILEMAPPING;
			}

			buffer = (char*)MapViewOfFile(SM_Handle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
			if (buffer == NULL)
			{
				_sys_error.store(GetLastError());
				throw  ResultRequest::ERROR_MAPPING_MEMORY;
			}

			_status.store(Status::GOOD);
		}
		catch (ResultRequest& e)
		{
			destroy();
			_status.store(Status::ERROR_CONNECTION);
			result = e;
		}
		catch (...)
		{
			destroy();
			_status.store(Status::ERROR_CONNECTION);
			result = ResultRequest::ERROR_NOT_INDIFICATED;
		}

		return result;
	}

	ResultRequest ClientSharedMemory::create(size_t number_int, size_t number_float, size_t number_double, size_t number_str, size_t size_str)
	{
		ResultRequest result{ ResultRequest::GOOD };
		Status current_status = _status.load();
		
		try
		{
			if (current_status == Status::GOOD)
			{
				if (this->name == name) throw ResultRequest::ERROR_MEMORY_ALREADY_INITIALIZATED;
				destroy();
			}
			else if (current_status == Status::INITIALIZATION);
			{
				throw ResultRequest::WAIT_INITIALIZATION;
			}
			_status.store(Status::INITIALIZATION);

			if (security_attr == nullptr)
			{
				unsigned int res = security_attr->InitSecurityAttrubuts();
				if (res != 0)
				{
					_sys_error.store(res);
					throw ResultRequest::ERROR_INITIALIZATION_SECURITYATTRIBUTS;
				}
			}

			Mutex_SM = CreateMutexA(&security_attr->getsecurityattrebut(), TRUE, CreateSMMutexName(name).c_str());
			if (Mutex_SM == NULL)
			{
				_sys_error.store(GetLastError());
				throw  ResultRequest::ERROR_INITIALIZATION_MUTEX;
			}

			SM_Handle = OpenFileMappingA(FILE_ALL_ACCESS, FALSE, CreateSMName(name).c_str());
			if (SM_Handle != NULL)
			{
				throw  ResultRequest::ERROR_MEMORY_ALREADY_EXIST;
			}

			size_t size_memory = get_size_memory(number_int, number_float, number_double, number_str, size_str);
			SM_Handle = CreateFileMappingA(INVALID_HANDLE_VALUE, &security_attr->getsecurityattrebut(), PAGE_READWRITE, 0, size_memory, CreateSMName(name).c_str());
			if (SM_Handle == NULL)
			{
				_sys_error.store(GetLastError());
				throw ResultRequest::ERROR_CREATE_FILEMAPPING;
			};

			buffer = (char*)MapViewOfFile(SM_Handle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
			if (buffer == NULL)
			{
				_sys_error.store(GetLastError());
				throw  ResultRequest::ERROR_MAPPING_MEMORY;
			}

			for (unsigned int i = 0; i < size_memory; i++)
			{
				*(buffer + i) = 0;
			}

			/// --- initilization header shared memory --- ///
			HeaderSharedMemory* head = reinterpret_cast<HeaderSharedMemory*>(buffer);
			head->TimeLastRead = 0;
			head->TimeLastWrite = 0;
			head->count_read = 0;
			head->count_write = 0;
			head->size_data_int = number_int;
			head->size_data_float = number_float;
			head->size_data_double = number_double;
			head->size_data_char = number_str;
			head->size_str = size_str;

			_status.store(Status::GOOD);;
		}
		catch (ResultRequest& e)
		{
			
			result = e;
		}
		catch (...)
		{
			result = ResultRequest::ERROR_NOT_INDIFICATED;
		}

		ReleaseMutex(Mutex_SM);
		return result;
	}

	void ClientSharedMemory::destroy()
	{
		if (SM_Handle != NULL)
		{
			CloseHandle(SM_Handle);
			SM_Handle = NULL;
		}
		if (Mutex_SM != NULL)
		{
			CloseHandle(Mutex_SM);
			SM_Handle = NULL;
		}
		if (buffer != nullptr)
		{
			UnmapViewOfFile(buffer);
			buffer = nullptr;
		}
	}

	std::string ClientSharedMemory::CreateSMName(std::string name_source)
	{
		std::string str;
		str += "Global\\";
		str += name_source;
		return str;
	}

	std::string ClientSharedMemory::CreateSMMutexName(std::string name_source)
	{
		std::string str;
		str += "Global\\Mutex_";
		str += name_source;
		return str;
	}

	size_t ClientSharedMemory::get_size_memory(size_t number_int, size_t number_float, size_t number_double, size_t number_str, size_t size_str)
	{
		size_t size = 0;
		size += sizeof(HeaderSharedMemory);
		size += number_int * sizeof(int);
		size += number_float * sizeof(float);
		size += number_double * sizeof(double);
		size += size_str * number_str;
	};

	size_t ClientSharedMemory::get_offset(const TypeData& type_data,const size_t& offset)
	{
		size_t value = 0;
		value += sizeof(HeaderSharedMemory);

		if (type_data == TypeData::INT) { value += sizeof(int) * offset; return value; }
		else { value += sizeof(int) * _header->size_data_int; };

		if (type_data == TypeData::FLOAT) { value += sizeof(float) * offset; return value; }
		else { value += sizeof(float) * _header->size_data_float; };

		if (type_data == TypeData::DOUBLE) { value += sizeof(double) * offset; return value; }
		else { value += sizeof(double) * _header->size_data_double; };

		if (type_data == TypeData::STRING) { value += _header->size_str * offset; return value; }
		else { value = 0; };

		return value;
	}

	size_t ClientSharedMemory::get_size_data(const TypeData& type)
	{
		if (type == TypeData::INT)
		{
			return _header->size_data_int;
		}
		if (type == TypeData::FLOAT)
		{
			return _header->size_data_float;
		}
		if (type == TypeData::DOUBLE)
		{
			return _header->size_data_double;
		}
		if (type == TypeData::STRING)
		{
			return _header->size_data_char;
		}
	}

	long long ClientSharedMemory::get_time_LLmcs()
	{
		return  std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now()).time_since_epoch().count();
	}



	template<class T> ResultRequest ClientSharedMemory::WriteData(T point_data, size_t size_data, size_t offset)
	{
		ResultRequest result{ ResultRequest::GOOD };
		DWORD res = 0;
		if (Mutex_SM == NULL || buffer == nullptr) return ResultRequest::IGNOR;

		try
		{
			res = WaitForSingleObject(Mutex_SM, 5000);
			if (res != WAIT_OBJECT_0)
			{
				_sys_error.store(GetLastError());
				throw ResultRequest::ERROR_BLOCK_MUTEX;
			}

			TypeData type_data = get_type_data<decltype(point_data)>();
			size_t size_memory = get_size_data(type_data);

			if (!varification_offset(size_memory, offset)) throw ResultRequest::ERROR_EXCESS_OFFSET;
			result = calibration_size(size_memory, size_data, offset);

			T buf = (T)(buffer + get_offset(TypeData::INT, offset));

			update_data<T>(point_data, buf, size_data);

			_header->count_write++;
			_header->TimeLastWrite = get_time_LLmcs();

		}
		catch (ResultRequest& e)
		{
			result = e;
		}
		catch (...)
		{
			result = ResultRequest::ERROR_NOT_INDIFICATED;
		}

		if (res == WAIT_OBJECT_0) ReleaseMutex(Mutex_SM);

		return result;
	}

	template<class T> ResultRequest ClientSharedMemory::WriteData(const std::vector<T>& vector, size_t offset)
	{
		ResultRequest result{ ResultRequest::GOOD };
		DWORD res = 0;
		if (Mutex_SM == NULL || buffer == nullptr) return ResultRequest::IGNOR;

		try
		{
			res = WaitForSingleObject(Mutex_SM, 5000);
			if (res != WAIT_OBJECT_0)
			{
				_sys_error.store(GetLastError());
				throw ResultRequest::ERROR_BLOCK_MUTEX;
			}

			TypeData type_data = get_type_data<T*>();
			size_t size_memory = get_size_data(type_data);
			size_t size_data = vector.size();

			if (!varification_offset(size_memory, offset)) throw ResultRequest::ERROR_EXCESS_OFFSET;
			result = calibration_size(size_memory, size_data, offset);

			T buf = (T)(buffer + get_offset(TypeData::INT, offset));

			update_data<T>(vector, buf, size_data);

			_header->count_write++;
			_header->TimeLastWrite = get_time_LLmcs();

		}
		catch (ResultRequest& e)
		{
			result = e;
		}
		catch (...)
		{
			result = ResultRequest::ERROR_NOT_INDIFICATED;
		}

		if (res == WAIT_OBJECT_0) ReleaseMutex(Mutex_SM);

		return result;
	}

	template<> ResultRequest ClientSharedMemory::WriteData<std::string>(const std::vector<std::string>& vector, size_t offset)
	{
		ResultRequest result{ ResultRequest::GOOD };
		DWORD res = 0;
		if (Mutex_SM == NULL || buffer == nullptr) return ResultRequest::IGNOR;

		try
		{
			res = WaitForSingleObject(Mutex_SM, 5000);
			if (res != WAIT_OBJECT_0)
			{
				_sys_error.store(GetLastError());
				throw ResultRequest::ERROR_BLOCK_MUTEX;
			}

			TypeData type_data = get_type_data<char*>();
			size_t size_memory = get_size_data(type_data);
			size_t size_data = vector.size();

			if (!varification_offset(size_memory, offset)) throw ResultRequest::ERROR_EXCESS_OFFSET;
			result = calibration_size(size_memory, size_data, offset);

			char* buf = (char*)(buffer + get_offset(TypeData::STRING, offset));

			for (size_t i = 0; i < size_data; i++)
			{
				int counter = 0;
				const std::string& str = vector[i];
				for (;;)
				{
					if (counter >= str.size() || counter >= _header->size_str) break;
					*buf = str[counter];
					buf++;
					counter++;
				}

				for (;;)
				{
					if (counter >= _header->size_str) break;
					*buf = '\0';
					buf++;
					counter++;
				}
			}


		}
		catch (ResultRequest& e)
		{
			result = e;
		}
		catch (...)
		{
			result = ResultRequest::ERROR_NOT_INDIFICATED;
		}

		if (res == WAIT_OBJECT_0) ReleaseMutex(Mutex_SM);

		return result;
	}

	template<class T> ResultRequest ClientSharedMemory::ReadData(T point_data, size_t size_data, size_t offset)
	{
		ResultRequest result{ ResultRequest::GOOD };
		DWORD res = 0;
		if (Mutex_SM == NULL || buffer == nullptr) return ResultRequest::IGNOR;

		res = WaitForSingleObject(Mutex_SM, 5000);
		if (res != WAIT_OBJECT_0)
		{
			_sys_error.store(GetLastError());
			throw ResultRequest::ERROR_BLOCK_MUTEX;
		}

		TypeData type_data = get_type_data<decltype(point_data)>();
		size_t size_memory = get_size_data(type_data);

		if (!varification_offset(size_memory, offset)) throw ResultRequest::ERROR_EXCESS_OFFSET;
		result = calibration_size(size_memory, size_data, offset);


	}

	template<class V> TypeData ClientSharedMemory::get_type_data()
	{
		if (std::is_same< V, int*>::value)
		{
			return TypeData::INT;
		}
		else if (std::is_same< V, float*>::value)
		{
			return TypeData::FLOAT;
		}
		else if (std::is_same< V, double*>::value)
		{
			return TypeData::DOUBLE;
		}
		else if (std::is_same< V, char*>::value)
		{
			return TypeData::STRING;
		}
	};

	bool ClientSharedMemory::varification_offset(const size_t& size_block, const size_t& offset)
	{
		if (_header->size_data_int <= offset) return false;
		return true;
	}

	ResultRequest ClientSharedMemory::calibration_size(const size_t& size_memory, size_t& size_source, const size_t& offset)
	{
		if (size_memory > size_source + offset)
		{
			size_source = size_memory - offset;
			return ResultRequest::WARNANING_EXCESS_RANGE;
		}
		return ResultRequest::GOOD;
	}

	template<class T> void ClientSharedMemory::update_data(T source, T target, size_t size)
	{
		for (size_t i = 0; i < size; i++)
		{
			*(target + i) = *(source + i);
		}
	}

	template<class T> void ClientSharedMemory::update_data(std::vector<T>& source, T* target, size_t size)
	{
		for (size_t i = 0; i < size; i++)
		{
			*(target + i) = source[i];
		}
	}

	template<> void ClientSharedMemory::update_data<char*>(char* source, char* target, size_t size)
	{
		for (size_t i = 0; i < size; i++)
		{
			size_t counter = 0;
			for (;;)
			{
				if (*source == '\0' || counter >= _header->size_str)
				{
					source++;
					break;
				}
				*target = *source;
				target++;
				source++;
				counter++;
			}

			for (;;)
			{
				if (counter >= _header->size_str) break;
				*target = '\0';
				target++;
				counter++;
			}
		}
	}

	template<class T> void ClientSharedMemory::read_data(T source, T target, size_t size)
	{
		for (size_t i = 0; i < size; i++)
		{
			*(source + i) = *(target + i);
		}
	}

	template<> void ClientSharedMemory::read_data<char*>(char* source, char* target, size_t size)
	{
		for (size_t i = 0; i < size; i++)
		{
			size_t counter = 0;
			for (;;)
			{
				if (counter >= _header->size_str || source == '\0') break;
				*target = *source;
				target++;
				source++;
				counter++;
			}

			if (counter < _header->size_str)
			{
				source += _header->size_str - counter;
			}
		}
	}




	ResultRequest ClientSharedMemory::WriteData(int* point_data, size_t size_data, size_t offset)
	{
		return WriteData<int*>(point_data, size_data, offset);
	}

	ResultRequest ClientSharedMemory::WriteData(float* point_data, size_t size_data, size_t offset)
	{
		return WriteData<float*>(point_data, size_data, offset);
	}

	ResultRequest ClientSharedMemory::WriteData(double* point_data, size_t size_data, size_t offset)
	{
		return WriteData<double*>(point_data, size_data, offset);
	}

	ResultRequest ClientSharedMemory::WriteData(char* point_data, size_t size_data, size_t offset)
	{
		return WriteData<char*>(point_data, size_data, offset);
	}

	ResultRequest  ClientSharedMemory::WriteData(const std::vector<int>& vector, size_t offset)
	{
		return WriteData<int>(vector, offset);
	}

	ResultRequest  ClientSharedMemory::WriteData(const std::vector<float>& vector, size_t offset)
	{
		return WriteData<float>(vector, offset);
	}

	ResultRequest  ClientSharedMemory::WriteData(const std::vector<double>& vector, size_t offset)
	{
		return WriteData<double>(vector, offset);
	}

	ResultRequest  ClientSharedMemory::WriteData(const std::vector<std::string>& vector, size_t offset)
	{
		return WriteData<std::string>(vector, offset);
	}

}
