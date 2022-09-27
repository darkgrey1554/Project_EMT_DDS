#pragma once

#include <string>
#include <windows.h>
#include <aclapi.h>
#include <memory>
#include <vector>
#include <atomic>
//#include <mutex>
#include <chrono>
#include <type_traits>
#include <cstdint>


namespace scada_ate{
namespace lib { 
namespace emt
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
		CHAR,
		STRING
	};

	enum class Status
	{
		 Null,
		 GOOD,
		 INITIALIZATION,
		 ERROR_CONNECTION
	};

	template<class T> 
	struct Value
	{
		T value {};
		char quality = 0;
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
			size_t size_str_data = 0;
			size_t size_str = 0;
		};

		HANDLE SM_Handle = NULL;   /// handle shared memory
		HANDLE Mutex_SM = NULL; /// handle mutex shared memory
		char* buffer = nullptr;
		std::string name;
		//std::mutex _mutex_init;
		std::atomic<Status> _status{Status::Null};
		std::atomic<DWORD> _sys_error{0};
		HeaderSharedMemory* _header;

		std::shared_ptr<SecurityHandle> security_attr = nullptr;

		std::string CreateSMName(std::string str);
		std::string CreateSMMutexName(std::string str);
		void destroy();
		ResultRequest connect();
		ResultRequest create(size_t number_int, size_t number_float, size_t number_double, size_t number_char, size_t number_str, size_t size_str);
		size_t get_size_memory(size_t number_int, size_t number_float, size_t number_double, size_t number_char, size_t number_str, size_t size_str);
		long long get_time_LLmcs();
		template<class V> TypeData get_type_data();
		size_t get_size_data(const TypeData& type);
		size_t get_offset(const TypeData& type, const size_t& offset);
		bool varification_offset(const size_t& size_block, const size_t& offset);
		ResultRequest calibration_size(const size_t& size_memory, size_t& size_source, const size_t& offset);

		template<class T> ResultRequest WriteData(const std::vector<Value<T>>& vector, size_t offset);
		template<class T> ResultRequest ReadData(std::vector<Value<T>>& vector, size_t offset);

		template<class T> void write_data(const std::vector<Value<T>>& vector, T* target, size_t size);
		template<class T> void read_data(std::vector<Value<T>>& vector, T* source, size_t size);
		
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
				size_t number_char = 0,
				size_t number_str = 0,
				size_t size_str = 0);

			Status GetStatus()
			{
				return _status.load();
			};

			DWORD GetSysError()
			{
				return _sys_error.load();
			};

			ResultRequest WriteData(const std::vector<Value<int>>& vector, size_t offset = 0);
			ResultRequest WriteData(const std::vector<Value<float>>& vector, size_t offset = 0);
			ResultRequest WriteData(const std::vector<Value<double>>& vector, size_t offset = 0);
			ResultRequest WriteData(const std::vector<Value<char>>& vector, size_t offset = 0);
			ResultRequest WriteData(const std::vector<Value<std::string>>& vector, size_t offset = 0);

			ResultRequest ReadData(std::vector<Value<int>>& vector, size_t offset = 0);
			ResultRequest ReadData(std::vector<Value<float>>& vector, size_t offset = 0);
			ResultRequest ReadData(std::vector<Value<double>>& vector, size_t offset = 0);
			ResultRequest ReadData(std::vector<Value<char>>& vector, size_t offset = 0);
			ResultRequest ReadData(std::vector<Value<std::string>>& vector, size_t offset = 0);
													  
			ResultRequest WriteData(const std::vector<int>& vector, size_t offset = 0);
			ResultRequest WriteData(const std::vector<float>& vector, size_t offset = 0);
			ResultRequest WriteData(const std::vector<double>& vector, size_t offset = 0);
			ResultRequest WriteData(const std::vector<char>& vector, size_t offset = 0);
			ResultRequest WriteData(const std::vector<std::string>& vector, size_t offset = 0);

			ResultRequest ReadData(std::vector<int>& vector, size_t offset = 0);
			ResultRequest ReadData(std::vector<float>& vector, size_t offset = 0);
			ResultRequest ReadData(std::vector<double>& vector, size_t offset = 0);
			ResultRequest ReadData(std::vector<char>& vector, size_t offset = 0);
			ResultRequest ReadData(std::vector<std::string>& vector, size_t offset = 0);

			ResultRequest WriteData(int* point_data, size_t size_data, size_t offset = 0);
			ResultRequest WriteData(float* point_data, size_t size_data, size_t offset = 0);
			ResultRequest WriteData(double* point_data, size_t size_data, size_t offset = 0);
			ResultRequest WriteData(char* point_data, size_t size_data, size_t offset = 0);
			ResultRequest WriteData(std::string* point_data, size_t size_data, size_t offset = 0);

			ResultRequest ReadData(int* point_data, size_t size_data, size_t offset = 0);
			ResultRequest ReadData(float* point_data, size_t size_data, size_t offset = 0);
			ResultRequest ReadData(double* point_data, size_t size_data, size_t offset = 0);
			ResultRequest ReadData(char* point_data, size_t size_data, size_t offset = 0);
			ResultRequest ReadData(std::string* point_data, size_t size_data, size_t offset = 0);

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
		//std::lock_guard<std::mutex> lock(_mutex_init);
		destroy();
	};

	ResultRequest ClientSharedMemory::Connect()
	{
		//std::lock_guard<std::mutex> lock(_mutex_init);
		return connect();
	};

	ResultRequest ClientSharedMemory::ReConnect(std::string name)
	{
		//std::lock_guard<std::mutex> lock(_mutex_init);
		this->name = name;
		destroy();
		return connect();
	};

	ResultRequest ClientSharedMemory::CreateAndConnect(std::string name,
		size_t number_int,
		size_t number_float,
		size_t number_double,
		size_t number_char,
		size_t number_str,
		size_t size_str)
	{
		ResultRequest result{ ResultRequest::GOOD };
		//std::lock_guard<std::mutex> lock(_mutex_init);
		result = create(number_int, number_float, number_double, number_char, number_str, size_str);
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
			};

			if (current_status == Status::INITIALIZATION)
			{
				throw ResultRequest::WAIT_INITIALIZATION;
			};

			_status.store(Status::INITIALIZATION);

			if (security_attr == nullptr)
			{
				security_attr = std::make_shared<SecurityHandle>();
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
			
			SM_Handle = OpenFileMappingA(FILE_MAP_READ|FILE_MAP_WRITE, FALSE, CreateSMName(name).c_str());
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

	ResultRequest ClientSharedMemory::create(size_t number_int, size_t number_float, size_t number_double, size_t number_char, size_t number_str, size_t size_str)
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
			else if (current_status == Status::INITIALIZATION)
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

			size_t size_memory = get_size_memory(number_int, number_float, number_double, number_char, number_str, size_str);
			SM_Handle = CreateFileMappingA(INVALID_HANDLE_VALUE, &security_attr->getsecurityattrebut(), PAGE_READWRITE, 0, (DWORD)size_memory, CreateSMName(name).c_str());
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
			head->size_data_char = number_char;
			head->size_str_data = number_str;
			head->size_str = size_str;

			_status.store(Status::GOOD);
			ReleaseMutex(Mutex_SM);
		}
		catch (ResultRequest& e)
		{
			result = e;
			ReleaseMutex(Mutex_SM);
			destroy();
		}
		catch (...)
		{
			result = ResultRequest::ERROR_NOT_INDIFICATED;
			ReleaseMutex(Mutex_SM);
			destroy();
		}

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

	size_t ClientSharedMemory::get_size_memory(size_t number_int, size_t number_float, size_t number_double, size_t number_char, size_t number_str, size_t size_str)
	{
		size_t result = 0;
		result += sizeof(HeaderSharedMemory); // size header
		result += (sizeof(int) + sizeof(char)) * number_int; // size DataCollectionInt		
		result += (sizeof(float) + sizeof(char)) * number_float; // size DataCollectionFloat		
		result += (sizeof(double) + sizeof(char)) * number_double; // size DataCollectionDouble		
		result += (sizeof(char) + sizeof(char)) * number_char; // size DataCollectionChar  	
		result += (sizeof(char) * size_str + sizeof(char) + sizeof(char)) * number_str; // size DataCollectionStr
		return result;
	};

	size_t ClientSharedMemory::get_offset(const TypeData& type_data,const size_t& offset)
	{
		size_t value = 0;
		value += sizeof(HeaderSharedMemory);

		if (type_data == TypeData::INT) { value += (sizeof(int)+sizeof(char))* offset; return value; }
		else { value += (sizeof(int) + sizeof(char)) * _header->size_data_int; };

		if (type_data == TypeData::FLOAT) { value += (sizeof(float) + sizeof(char)) * offset; return value; }
		else { value += (sizeof(float) + sizeof(char)) * _header->size_data_float; };

		if (type_data == TypeData::DOUBLE) { value += (sizeof(double) + sizeof(char)) * offset; return value; }
		else { value += (sizeof(double) + sizeof(char)) * _header->size_data_double; };

		if (type_data == TypeData::CHAR) { value += (sizeof(char) + sizeof(char)) * offset; return value; }
		else { value += (sizeof(char) + sizeof(char)) * _header->size_data_char; };

		if (type_data == TypeData::STRING) { value += (_header->size_str * sizeof(char) + sizeof(char)*2)* offset; return value; }
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
		if (type == TypeData::CHAR)
		{
			return _header->size_data_char;
		}
		if (type == TypeData::STRING)
		{
			return _header->size_str_data;
		}

		return 0;
	}

	long long ClientSharedMemory::get_time_LLmcs()
	{
		return  std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now()).time_since_epoch().count();
	}

	template<class V> TypeData ClientSharedMemory::get_type_data()
	{
		if (std::is_same< V, int>::value)
		{
			return TypeData::INT;
		}
		else if (std::is_same< V, float>::value)
		{
			return TypeData::FLOAT;
		}
		else if (std::is_same< V, double>::value)
		{
			return TypeData::DOUBLE;
		}
		else if (std::is_same< V, char>::value)
		{
			return TypeData::CHAR;
		}
		else if (std::is_same< V, std::string>::value)
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
		if (size_memory < size_source + offset)
		{
			size_source = size_memory - offset;
			return ResultRequest::WARNANING_EXCESS_RANGE;
		}
		return ResultRequest::GOOD;
	}


	template<class T> ResultRequest ClientSharedMemory::WriteData(const std::vector<Value<T>>& vector, size_t offset)
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

			_header = (HeaderSharedMemory*)buffer;

			TypeData type_data = get_type_data<T>();
			size_t size_memory = get_size_data(type_data);
			size_t size_data = vector.size();

			if (!varification_offset(size_memory, offset)) throw ResultRequest::ERROR_EXCESS_OFFSET;
			result = calibration_size(size_memory, size_data, offset);

			T* buf = (T*)(buffer + get_offset(type_data, offset));

			write_data<T>(vector, buf, size_data);

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

	template<> ResultRequest ClientSharedMemory::WriteData<std::string>(const std::vector<Value<std::string>>& vector, size_t offset)
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

			_header = (HeaderSharedMemory*)buffer;

			TypeData type_data = get_type_data<std::string>();
			size_t size_memory = get_size_data(type_data);
			size_t size_data = vector.size();

			if (!varification_offset(size_memory, offset)) throw ResultRequest::ERROR_EXCESS_OFFSET;
			result = calibration_size(size_memory, size_data, offset);

			char* buf = (char*)(buffer + get_offset(type_data, offset));

			for (size_t i = 0; i < size_data; i++)
			{
				const std::string& str = vector[i].value;
				size_t counter = str.size() > _header->size_str ? _header->size_str : str.size();
				str.copy(buf, counter);
				for (;;)
				{
					if (counter >= _header->size_str + sizeof(char)) break;
					*(buf + counter) = '\0';
					counter++;
				}
				buf += _header->size_str + 1;
				*buf = vector[i].quality;
				buf++;
			}

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

	template<class T> ResultRequest ClientSharedMemory::ReadData(std::vector<Value<T>>& vector, size_t offset)
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

			_header = (HeaderSharedMemory*)buffer;

			TypeData type_data = get_type_data<T>();
			size_t size_memory = get_size_data(type_data);
			size_t size_data = vector.size();

			if (!varification_offset(size_memory, offset)) throw ResultRequest::ERROR_EXCESS_OFFSET;
			result = calibration_size(size_memory, size_data, offset);

			T* buf = (T*)(buffer + get_offset(type_data, offset));

			read_data<T>(vector, buf, size_data);

			_header->count_read++;
			_header->TimeLastRead= get_time_LLmcs();

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

	template<> ResultRequest ClientSharedMemory::ReadData<std::string>(std::vector<Value<std::string>>& vector, size_t offset)
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

			_header = (HeaderSharedMemory*)buffer;

			TypeData type_data = get_type_data<std::string>();
			size_t size_memory = get_size_data(type_data);
			size_t size_data = vector.size();

			if (!varification_offset(size_memory, offset)) throw ResultRequest::ERROR_EXCESS_OFFSET;
			result = calibration_size(size_memory, size_data, offset);

			char* buf = (char*)(buffer + get_offset(type_data, offset));

			for (size_t i = 0; i < size_data; i++)
			{
				size_t counter = 0;
				std::string& str = vector[i].value;
				*(buf + _header->size_str) = '\0';
				str.clear();
				str = buf;
				buf += _header->size_str + 1;
				vector[i].quality = *buf;
				buf++;
			}

			_header->count_read++;
			_header->TimeLastRead = get_time_LLmcs();

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

	template<class T> void ClientSharedMemory::write_data(const std::vector<Value<T>>& vector, T* target, size_t size)
	{
		char* of = (char*)target;
		for (size_t i = 0; i < size; i++)
		{
			*(T*)of = vector[i].value;
			of += sizeof(T);
			*of = vector[i].quality;
			of++;
		};
	}

	template<class T> void ClientSharedMemory::read_data(std::vector<Value<T>>& vector, T* source, size_t size)
	{
		char* of = (char*)source;
		for (size_t i = 0; i < size; i++)
		{
			vector[i].value = *(T*)of;
			of += sizeof(T);
			vector[i].quality = *of;
			of++;
		};
	};


	ResultRequest ClientSharedMemory::ReadData(std::vector<Value<int>>& vector, size_t offset)
	{
		return ReadData<int>(vector, offset);
	};

	ResultRequest ClientSharedMemory::ReadData(std::vector<Value<float>>& vector, size_t offset)
	{
		return ReadData<float>(vector, offset);
	};

	ResultRequest ClientSharedMemory::ReadData(std::vector<Value<double>>& vector, size_t offset)
	{
		return ReadData<double>(vector, offset);
	}

	ResultRequest ClientSharedMemory::ReadData(std::vector<Value<char>>& vector, size_t offset)
	{
		return ReadData<char>(vector, offset);
	};

	ResultRequest ClientSharedMemory::ReadData(std::vector<Value<std::string>>& vector, size_t offset)
	{
		return ReadData<std::string>(vector, offset);
	}

	ResultRequest ClientSharedMemory::ReadData(std::vector<int>& vector, size_t offset)
	{
		ResultRequest result;
		std::vector<Value<int>> vector_mirror(vector.size());
		result = ReadData<int>(vector_mirror, offset);
		int count = 0;
		for (auto& it : vector)
		{
			it = vector_mirror[count].value;
			count++;
		}
		return result;
	};

	ResultRequest ClientSharedMemory::ReadData(std::vector<float>& vector, size_t offset)
	{
		ResultRequest result;
		std::vector<Value<float>> vector_mirror(vector.size());
		result = ReadData<float>(vector_mirror, offset);
		int count = 0;
		for (auto& it : vector)
		{
			it = vector_mirror[count].value;
			count++;
		}
		return result;
	};

	ResultRequest ClientSharedMemory::ReadData(std::vector<double>& vector, size_t offset)
	{
		ResultRequest result;
		std::vector<Value<double>> vector_mirror(vector.size());
		result = ReadData<double>(vector_mirror, offset);
		int count = 0;
		for (auto& it : vector)
		{
			it = vector_mirror[count].value;
			count++;
		}
		return result;
	};

	ResultRequest ClientSharedMemory::ReadData(std::vector<char>& vector, size_t offset)
	{
		ResultRequest result;
		std::vector<Value<char>> vector_mirror(vector.size());
		result = ReadData<char>(vector_mirror, offset);
		int count = 0;
		for (auto& it : vector)
		{
			it = vector_mirror[count].value;
			count++;
		}
		return result;
	};

	ResultRequest ClientSharedMemory::ReadData(std::vector<std::string>& vector, size_t offset)
	{
		ResultRequest result;
		std::vector<Value<std::string>> vector_mirror(vector.size());
		result = ReadData<std::string>(vector_mirror, offset);
		int count = 0;
		for (auto& it : vector)
		{
			it = std::move(vector_mirror[count].value);
			count++;
		}
		return result;
	};

	ResultRequest ClientSharedMemory::ReadData(int* point_data, size_t size_data, size_t offset)
	{
		ResultRequest result;
		std::vector<Value<int>> vector_mirror(size_data);
		result = ReadData<int>(vector_mirror, offset);
		int count = 0;
		for (auto& it : vector_mirror)
		{
			*point_data = it.value;
			point_data++;
			count++;
		}
		return result;
	};

	ResultRequest ClientSharedMemory::ReadData(float* point_data, size_t size_data, size_t offset)
	{
		ResultRequest result;
		std::vector<Value<float>> vector_mirror(size_data);
		result = ReadData<float>(vector_mirror, offset);
		int count = 0;
		for (auto& it : vector_mirror)
		{
			*point_data = it.value;
			point_data++;
			count++;
		}
		return result;
	};

	ResultRequest ClientSharedMemory::ReadData(double* point_data, size_t size_data, size_t offset)
	{
		ResultRequest result;
		std::vector<Value<double>> vector_mirror(size_data);
		result = ReadData<double>(vector_mirror, offset);
		int count = 0;
		for (auto& it : vector_mirror)
		{
			*point_data = it.value;
			point_data++;
			count++;
		}
		return result;
	};

	ResultRequest ClientSharedMemory::ReadData(char* point_data, size_t size_data, size_t offset)
	{
		ResultRequest result;
		std::vector<Value<char>> vector_mirror(size_data);
		result = ReadData<char>(vector_mirror, offset);
		int count = 0;
		for (auto& it : vector_mirror)
		{
			*point_data = it.value;
			point_data++;
			count++;
		}
		return result;
	};

	ResultRequest ClientSharedMemory::ReadData(std::string* point_data, size_t size_data, size_t offset)
	{
		ResultRequest result;
		std::vector<Value<std::string>> vector_mirror(size_data);
		result = ReadData<std::string>(vector_mirror, offset);
		int count = 0;
		for (auto& it : vector_mirror)
		{
			*point_data = std::move(it.value);
			point_data++;
			count++;
		}
		return result;
	}


	ResultRequest  ClientSharedMemory::WriteData(const std::vector<Value<int>>& vector, size_t offset)
	{
		return WriteData<int>(vector, offset);
	};

	ResultRequest ClientSharedMemory::WriteData(const std::vector<Value<float>>& vector, size_t offset)
	{
		return WriteData<float>(vector, offset);
	};

	ResultRequest ClientSharedMemory::WriteData(const std::vector<Value<double>>& vector, size_t offset)
	{
		return WriteData <double>(vector, offset);
	};

	ResultRequest ClientSharedMemory::WriteData(const std::vector<Value<char>>& vector, size_t offset)
	{
		return WriteData <char>(vector, offset);
	};

	ResultRequest ClientSharedMemory::WriteData(const std::vector<Value<std::string>>& vector, size_t offset)
	{
		return WriteData <std::string>(vector, offset);
	};

	ResultRequest ClientSharedMemory::WriteData(const std::vector<int>& vector, size_t offset)
	{
		std::vector<Value<int>> vector_mirror(vector.size());
		size_t count = 0;
		for (auto& it : vector_mirror)
		{
			it.value = vector[count];
			it.quality = 0;
			count++;
		}
		return WriteData<int>(vector_mirror, offset);
	};

	ResultRequest ClientSharedMemory::WriteData(const std::vector<float>& vector, size_t offset)
	{
		std::vector<Value<float>> vector_mirror(vector.size());
		size_t count = 0;
		for (auto& it : vector_mirror)
		{
			it.value = vector[count];
			it.quality = 0;
			count++;
		}
		return WriteData<float>(vector_mirror, offset);
	};

	ResultRequest ClientSharedMemory::WriteData(const std::vector<double>& vector, size_t offset)
	{
		std::vector<Value<double>> vector_mirror(vector.size());
		size_t count = 0;
		for (auto& it : vector_mirror)
		{
			it.value = vector[count];
			it.quality = 0;
			count++;
		}
		return WriteData<double>(vector_mirror, offset);
	};

	ResultRequest ClientSharedMemory::WriteData(const std::vector<char>& vector, size_t offset)
	{
		std::vector<Value<char>> vector_mirror(vector.size());
		size_t count = 0;
		for (auto& it : vector_mirror)
		{
			it.value = vector[count];
			it.quality = 0;
			count++;
		}
		return WriteData<char>(vector_mirror, offset);
	};

	ResultRequest ClientSharedMemory::WriteData(const std::vector<std::string>& vector, size_t offset)
	{
		std::vector<Value<std::string>> vector_mirror(vector.size());
		size_t count = 0;
		for (auto& it : vector_mirror)
		{
			it.value = vector[count];
			it.quality = 0;
			count++;
		}
		return WriteData<std::string>(vector_mirror, offset);
	}

	ResultRequest ClientSharedMemory::WriteData(int* point_data, size_t size_data, size_t offset)
	{
		std::vector<Value<int>> vector_mirror(size_data);
		size_t count = 0;
		for (auto& it : vector_mirror)
		{
			it.value = point_data[count];
			it.quality = 0;
			count++;
		}
		return WriteData<int>(vector_mirror, offset);
	};

	ResultRequest ClientSharedMemory::WriteData(float* point_data, size_t size_data, size_t offset)
	{
		std::vector<Value<float>> vector_mirror(size_data);
		size_t count = 0;
		for (auto& it : vector_mirror)
		{
			it.value = point_data[count];
			it.quality = 0;
			count++;
		}
		return WriteData<float>(vector_mirror, offset);
	};

	ResultRequest ClientSharedMemory::WriteData(double* point_data, size_t size_data, size_t offset)
	{
		std::vector<Value<double>> vector_mirror(size_data);
		size_t count = 0;
		for (auto& it : vector_mirror)
		{
			it.value = point_data[count];
			it.quality = 0;
			count++;
		}
		return WriteData<double>(vector_mirror, offset);
	};

	ResultRequest ClientSharedMemory::WriteData(char* point_data, size_t size_data, size_t offset)
	{
		std::vector<Value<char>> vector_mirror(size_data);
		size_t count = 0;
		for (auto& it : vector_mirror)
		{
			it.value = point_data[count];
			it.quality = 0;
			count++;
		}
		return WriteData<char>(vector_mirror, offset);
	};

	ResultRequest ClientSharedMemory::WriteData(std::string* point_data, size_t size_data, size_t offset)
	{
		std::vector<Value<std::string>> vector_mirror(size_data);
		size_t count = 0;
		for (auto& it : vector_mirror)
		{
			it.value = point_data[count];
			it.quality = 0;
			count++;
		}
		return WriteData<std::string>(vector_mirror, offset);
	};
}

}

}
