#include "AdapterSharedMemory.hpp"

namespace scada_ate::gate::adapter::sem
{
	////////////////////////////////////////
	/// --- задает имя shared memory --- ///
	///////////////////////////////////////
	/// <param>
	/// source - уникальная часть имени памяти
	/// </param>
	/// <result> 
	/// - имя shared memory
	/// 
	std::string AdapterSharedMemory::CreateSMName()
	{
		std::string str;
		str += "Global\\";
		str += config.NameChannel;
		return str;
	}

	std::string AdapterSharedMemory::CreateSMMutexName()
	{
		std::string str;
		str += "Global\\Mutex_";
		str += config.NameChannel;
		return str;
	}
	/////////////////////////////////////////////////
	/// --- конструктор адаптера SharedMemory --- ///
	/////////////////////////////////////////////////

	AdapterSharedMemory::AdapterSharedMemory(std::shared_ptr<IConfigAdapter> config)
	{
		std::shared_ptr<ConfigAdapterSharedMemory> config_point = std::reinterpret_pointer_cast<ConfigAdapterSharedMemory>(config);

		if (config_point != nullptr && config_point->type_adapter == TypeAdapter::SharedMemory)
		{
			this->config = *config_point;
		}

		log = LoggerSpaceScada::GetLoggerScada(LoggerSpaceScada::TypeLogger::SPDLOG);
	};

	/////////////////////////////////////////////////
	/// --- деструктор адаптера SharedMemory --- ///
	////////////////////////////////////////////////

	AdapterSharedMemory::~AdapterSharedMemory()
	{
		UnmapViewOfFile(buf_data);
		CloseHandle(SM_Handle);
		CloseHandle(Mutex_SM);
	}

	/////////////////////////////////////////////////
	/// --- деструктор адаптера SharedMemory --- ///
	////////////////////////////////////////////////
	/// <param>
	/// conf - указатель на конфигурация адаптера (conf должен иметь тип ConfigSharedMemoryAdapter*)
	/// </param>
	/// <result> 
	/// - результаи операции в типе ResultReqest. В случаи ести результат равен ResultReqest::ERR 
	/// инициальзация прошла с ошибкой, подробности описанны в логировании.

	ResultReqest AdapterSharedMemory::InitAdapter()
	{
		
		ResultReqest res = ResultReqest::OK;
		unsigned int result = 0;
		unsigned long sys_error =0;
		size_t size_memory;

		/// --- guard from repeated usage --- ///
		const std::lock_guard<std::mutex> lock_init(mutex_init);

		StatusAdapter status = current_status.load(std::memory_order::memory_order_relaxed);
		if (status == StatusAdapter::INITIALIZATION || status == StatusAdapter::OK)
		{
			ResultReqest res = ResultReqest::IGNOR;
			return res;
		}
		current_status.store(StatusAdapter::INITIALIZATION, std::memory_order_relaxed);

		security_attr = std::make_shared<SecurityHandle>();
		std::string namememory;
		std::string namemutex;

		try
		{
			if (config.type_adapter != TypeAdapter::SharedMemory) throw 1;

			/// --- coping of configuration --- ///
			//config.vec_link_tasg;
			//config.vec_tags_source;

			log->Info("AdapterSharedMemory {}: Init START", this->config.NameChannel);

			/// --- initialization of security attributes --- ///
			
			if (security_attr == nullptr)
			{
				sys_error = 0;
				current_status.store(StatusAdapter::ERROR_INIT, std::memory_order_relaxed);
				throw 2;
			}
			
			result = security_attr->InitSecurityAttrubuts();
			if (result != 0)
			{
				sys_error = result;
				current_status.store(StatusAdapter::ERROR_INIT, std::memory_order_relaxed);
				throw 2;
			}
			
			log->Debug("AdapterSharedMemory {} : InitSecurityAttributs DONE", this->config.NameChannel);

			/// --- defining size units of data --- ///
			size_memory = GetSizeMemory();

			/// --- initialization handle of shared memory --- ///
			if (SM_Handle != NULL)
			{
				current_status.store(StatusAdapter::ERROR_INIT, std::memory_order_relaxed);
				throw 3;
			}

			namememory = CreateSMName();
			namemutex =  CreateSMMutexName();

			Mutex_SM = CreateMutexA(&security_attr->getsecurityattrebut(), TRUE, namemutex.c_str());
			if (Mutex_SM == NULL)
			{
				sys_error = GetLastError();
				current_status.store(StatusAdapter::ERROR_INIT, std::memory_order_relaxed);
				throw 4;
			}

			log->Debug("AdapterSharedMemory {} : CreateMutex DONE", this->config.NameChannel);

			SM_Handle = CreateFileMappingA(INVALID_HANDLE_VALUE, &security_attr->getsecurityattrebut(), PAGE_READWRITE, 0, size_memory, namememory.c_str());
			if (SM_Handle == NULL)
			{
				sys_error = GetLastError();
				current_status.store(StatusAdapter::ERROR_INIT, std::memory_order_relaxed);
				res = ResultReqest::ERR;
				throw 5;
			}

			log->Debug("AdapterSharedMemory {} : CreateFileMapping DONE", this->config.NameChannel);

			buf_data = (char*)MapViewOfFile(SM_Handle, FILE_MAP_ALL_ACCESS, 0, 0, size_memory);
			if (buf_data == NULL)
			{
				current_status.store(StatusAdapter::ERROR_INIT, std::memory_order_relaxed);
				res = ResultReqest::ERR;
				throw 6;
			}

			log->Debug("AdapterSharedMemory {} : MapViewOfFileg DONE", this->config.NameChannel);

			/// --- clear shared memory --- ///
			for (unsigned int i = 0; i < size_memory; i++)
			{
				*(buf_data + i) = 0;
			}

			/// --- initilization header shared memory --- ///
			HeaderSharedMemory* head = reinterpret_cast<HeaderSharedMemory*>(buf_data);
			head->TimeLastRead = 0;
			head->TimeLastWrite = 0;
			head->count_read = 0;
			head->count_write = 0;
			head->size_data_int = config.size_int_data;
			head->size_data_float = config.size_float_data;
			head->size_data_double = config.size_double_data;
			head->size_data_char = config.size_char_data;
			head->size_str = config.size_str;
			ReleaseMutex(Mutex_SM);
			
			/// --- init vector GetTags --- /// 
			
			data.resize(1);
			SetTags& set_data = *data.begin();

			for (InfoTag& tag : this->config.vec_tags_source)
			{
				if (tag.type == TypeValue::INT)
				{
					set_data.map_int_data[tag] = {0,0,0};
					continue;
				}

				if (tag.type == TypeValue::FLOAT)
				{
					set_data.map_float_data[tag] = { 0,0,0 };
					continue;
				}

				if (tag.type == TypeValue::DOUBLE)
				{
					set_data.map_double_data[tag] = { 0,0,0 };
					continue;
				}
				if (tag.type == TypeValue::CHAR)
				{
					set_data.map_char_data[tag] = { 0,'\0',0 };
					continue;
				}
				if (tag.type == TypeValue::STRING)
				{
					set_data.map_str_data[tag] = { 0,"",0};
					continue;
				}
			}
			

			current_status.store(StatusAdapter::OK, std::memory_order_relaxed);
			log->Info("AdapterSharedMemory {} : Init DONE", this->config.NameChannel);
		}
		catch (int& e)
		{
			log->Critical("AdapterSharedMemory {}: Error Init : error: {}, syserror: {}", this->config.NameChannel, e, sys_error);
			if (Mutex_SM != NULL) ReleaseMutex(Mutex_SM);
			CloseHandle(Mutex_SM);
			UnmapViewOfFile(buf_data);
			CloseHandle(SM_Handle);
			res = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterSharedMemory {}: Error Init : error: {}, syserror: {}", this->config.NameChannel, 0, 0);
			if (Mutex_SM != NULL) ReleaseMutex(Mutex_SM);
			CloseHandle(Mutex_SM);
			UnmapViewOfFile(buf_data);
			CloseHandle(SM_Handle);
			res = ResultReqest::ERR;
		}		

		return res;
	}

	///////////////////////////////////////////////////////////////////
	/// --- запрос специфичного параметра адаптера SharedMemory --- ///
	//////////////////////////////////////////////////////////////////
	/// <param>
	/// param - запрашиваемый параметр (enum ParamInfoAdapter) 
	/// </param>
	/// <result> 
	/// - возвращает информацию в типе std::shared_ptr<BaseAnswer>, который является родительским для типа std::shared_ptr<HeaderDataAnswerSM>

	std::shared_ptr<IAnswer> AdapterSharedMemory::GetInfoAdapter(ParamInfoAdapter param)
	{
		std::shared_ptr<IAnswer> answer = nullptr;
		std::shared_ptr<AnswerSharedMemoryHeaderData> answerHeaderData = nullptr;

		switch (param)
		{

		/// --- ответ на запрос Header DATA --- ///
		case ParamInfoAdapter::HeaderData:

			
			answerHeaderData = AnswerRequestHeaderData();
			answer = std::reinterpret_pointer_cast<IAnswer>(answerHeaderData);
			break;

		/// --- ответ не предусмотрен данным типом адапрета (ResultReqest::IGNOR) --- ///
		default:

			answer = std::make_shared<IAnswer>();
			answer->param = param;
			answer->result = ResultReqest::IGNOR;
			answer->type_adapter = TypeAdapter::SharedMemory;

			break;
		}
				
		return answer;
	}

	/////////////////////////////////////////////////
	/// --- запрос типа адаптера SharedMemory --- ///
	/////////////////////////////////////////////////
	/// <result> 
	/// - возвращает тип адаптера (TypeAdapter::SharedMemory;)

	TypeAdapter AdapterSharedMemory::GetTypeAdapter()
	{
		return TypeAdapter::SharedMemory;
	}
	
	/////////////////////////////////////////////////////////////
	/// --- запрос текущего статуса адаптера SharedMemory --- ///
	////////////////////////////////////////////////////////////
	/// <result> 
	/// - возвращает статус адаптера (StatusAdapter)
	StatusAdapter AdapterSharedMemory::GetStatusAdapter()
	{
		return current_status.load(std::memory_order::memory_order_relaxed);
	}
	
	//////////////////////////////////////////////////////
	/// --- функция чтения данных из SharedMemory --- ///
	/////////////////////////////////////////////////////

	ResultReqest AdapterSharedMemory::ReadData(std::deque<SetTags>** _data)
	{	
		DWORD mutex_win32 = 0;
		DWORD err_win32 = 0;
		ResultReqest res = ResultReqest::OK;

		try
		{
			if (current_status.load(std::memory_order_relaxed) != StatusAdapter::OK)
			{
				log->Warning("AdapterSharedMemory {}: ReadData IGNOR: error {}", config.NameChannel, 1);
				return ResultReqest::IGNOR;
			}

			log->Debug("AdapterSharedMemory {}: ReadData START", config.NameChannel);	

			HeaderSharedMemory* head = reinterpret_cast<HeaderSharedMemory*>(buf_data);
			char* _buf = buf_data;

			int int_value = 0;
			float float_value = 0;
			double double_value = 0;
			char char_value = 0;
			std::string str_value;
			char quality = 0;
			size_t offset=0;
			long long time_data;

			SetTags& set_data = *data.begin();

			mutex_win32 = WaitForSingleObject(Mutex_SM, 5000);
			if (mutex_win32 != WAIT_OBJECT_0)
			{
				err_win32 = GetLastError();
				throw 1;
			}

			time_data = head->TimeLastWrite;

			for (auto it = set_data.map_int_data.begin(); it != set_data.map_int_data.end(); it++)
			{
				offset = TakeOffset(it->first.type, it->first.offset);
				if (offset == 0) continue;
				_buf = buf_data + offset;
				it->second.value = *(int*)_buf;
				_buf += sizeof(int);
				it->second.quality = *_buf;
				it->second.time = time_data;
			}

			for (auto it = set_data.map_float_data.begin(); it != set_data.map_float_data.end(); it++)
			{
				offset = TakeOffset(it->first.type, it->first.offset);
				if (offset == 0) continue;
				_buf = buf_data + offset;
				it->second.value = *(float*)_buf;
				_buf = _buf + sizeof(float);
				it->second.quality = *_buf;
				it->second.time = time_data;
			}

			for (auto it = set_data.map_double_data.begin(); it != set_data.map_double_data.end(); it++)
			{
				offset = TakeOffset(it->first.type, it->first.offset);
				if (offset == 0) continue;
				_buf = buf_data + offset;
				it->second.value = *(double*)_buf;
				_buf = _buf + sizeof(double);
				it->second.quality = *_buf;
				it->second.time = time_data;
			}

			for (auto it = set_data.map_char_data.begin(); it != set_data.map_char_data.end(); it++)
			{
				offset = TakeOffset(it->first.type, it->first.offset);
				if (offset == 0) continue;
				_buf = buf_data + offset;
				it->second.value = *_buf;
				_buf = _buf + sizeof(char);
				it->second.quality = *_buf;
				it->second.time = time_data;
			}

			for (auto it = set_data.map_str_data.begin(); it != set_data.map_str_data.end(); it++)
			{
				offset = TakeOffset(it->first.type, it->first.offset);
				if (offset == 0) continue;
				_buf = buf_data + offset;
				it->second.value = *_buf;
				_buf = _buf + config.size_str;
				it->second.quality = *_buf;
				it->second.time = time_data;
			}

			/// --- write time in header --- /// 
			set_data.time_source = head->TimeLastWrite;
			head->TimeLastRead = TimeConverter::GetTime_LLmcs();
			head->count_read++;

			*_data = &data;
		}
		catch (int& e)
		{
			
			log->Critical("AdapterSharedMemory {}: ERROR ReadData: error: {}, syserror: {}", config.NameChannel, e, err_win32);
			res = ResultReqest::ERR;

		}
		catch (...)
		{
			log->Critical("AdapterSharedMemory {}: ERROR ReadData: error: {}, syserror: {}", config.NameChannel, 0, 0);
			res = ResultReqest::ERR;
		}

		//if (mutex_win32 == WAIT_OBJECT_0) ReleaseMutex(Mutex_SM);
		return res;
	}

	//////////////////////////////////////////////////////
	/// --- функция записи данных в SharedMemory --- ///
	/////////////////////////////////////////////////////

	ResultReqest AdapterSharedMemory::WriteData(const std::deque<SetTags>& _data)
	{	
		DWORD mutex_win32 = 0;
		DWORD err_win32 = 0;
		ResultReqest res = ResultReqest::OK;

		try
		{
			if (current_status.load(std::memory_order_relaxed) != StatusAdapter::OK)
			{
				log->Debug("AdapterSharedMemory {}: WriteData IGNOR", config.NameChannel);
				return ResultReqest::IGNOR;
			}

			log->Debug("AdapterSharedMemory {}: WriteData START", config.NameChannel);

			mutex_win32 = WaitForSingleObject(Mutex_SM, 5000);
			if (mutex_win32 != WAIT_OBJECT_0)
			{
				err_win32 = GetLastError();
				throw 1;
			}		

			
			if (data.empty()) throw 2;
			const SetTags& data_in = *_data.rbegin();

			for (const LinkTags& link_tag : this->config.vec_link_tags)
			{

				if (link_tag.source.type == TypeValue::INT)
				{
					set_data(data_in.map_int_data.at(link_tag.source), link_tag);
					continue;
				}

				if (link_tag.source.type == TypeValue::FLOAT)
				{
					set_data(data_in.map_float_data.at(link_tag.source), link_tag);
					continue;
				}

				if (link_tag.source.type == TypeValue::DOUBLE)
				{
					
					set_data(data_in.map_double_data.at(link_tag.source), link_tag);
					continue;
				}

				if (link_tag.source.type == TypeValue::CHAR)
				{
					set_data(data_in.map_char_data.at(link_tag.source), link_tag);
					continue;
				}

				if (link_tag.source.type == TypeValue::STRING)
				{
					set_data(data_in.map_str_data.at(link_tag.source), link_tag);
					continue;
				}

			}

			HeaderSharedMemory* head = (HeaderSharedMemory*)buf_data;
			head->count_write++;
			head->TimeLastWrite = TimeConverter::GetTime_LLmcs();

		}
		catch (int& e)
		{
			
			log->Critical("AdapterSharedMemory {}: ERROR WriteData: error: {}, syserror: {}", config.NameChannel, e, err_win32);
			res = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("AdapterSharedMemory {}: ERROR WriteData: error: {}, syserror: {}", config.NameChannel, 0, 0);
			res = ResultReqest::ERR;
		}		

		if (mutex_win32 == WAIT_OBJECT_0) ReleaseMutex(Mutex_SM);
		return res;
	}



	//////////////////////////////////////////////////////
	/// --- функция чтения заголовка SharedMemory --- ///
	/////////////////////////////////////////////////////
	/// <result> 
	/// - возвращает данные в виде указателя на структуру HeaderDataAnswerSM

	std::shared_ptr<AnswerSharedMemoryHeaderData> AdapterSharedMemory::AnswerRequestHeaderData()
	{
		std::shared_ptr<AnswerSharedMemoryHeaderData> point = std::make_shared<AnswerSharedMemoryHeaderData>();
		HeaderSharedMemory* head = (HeaderSharedMemory*)buf_data;
		DWORD mutex_win32 = 0;
		DWORD err_win32 = 0;

		/// <summary>
		/// доделать
		/// </summary>
		return 	point;
	}

	//////////////////////////////////////////////////////
	/// --- функция расчета размера shared memory --- ///
	/////////////////////////////////////////////////////

	size_t AdapterSharedMemory::GetSizeMemory()
	{
		size_t result = 0;
		result += sizeof(HeaderSharedMemory); // size header

		offset_int = result; // save offset to int (for fast acсess)
		result += (sizeof(int) + sizeof(char)) * config.size_int_data; // size DataCollectionInt
		
		offset_float = result;
		result += (sizeof(float) + sizeof(char)) * config.size_float_data; // size DataCollectionFloat

		offset_double = result;
		result += (sizeof(double) + sizeof(char)) * config.size_double_data; // size DataCollectionDouble
	
		offset_char = result;
		result += (sizeof(char) + sizeof(char)) * config.size_char_data; // size DataCollectionChar

		offset_str = result;
		result += (sizeof(char)*config.size_str + sizeof(char) + sizeof(char)) * config.size_str_data; // size DataCollectionStr

		return result;
	}

	size_t AdapterSharedMemory::TakeOffset(const TypeValue& type_value, const size_t& ofs) const
	{
		if (type_value == TypeValue::INT)
		{
			if (ofs >= config.size_int_data) return 0;
			return offset_int + ofs * (sizeof(int) + sizeof(char));
		}

		if (type_value == TypeValue::FLOAT)
		{
			if (ofs >= config.size_float_data) return 0;
			return offset_float + ofs * (sizeof(float) + sizeof(char));
		}

		if (type_value == TypeValue::DOUBLE)
		{
			if (ofs >= config.size_double_data) return 0;
			return offset_double + ofs * (sizeof(double) + sizeof(char));
		}

		if (type_value == TypeValue::DOUBLE)
		{
			if (ofs >= config.size_double_data) return 0;
			return offset_double + ofs * (sizeof(double) + sizeof(char));
		}

		if (type_value == TypeValue::CHAR)
		{
			if (ofs >= config.size_char_data) return 0;
			return offset_char + ofs * (sizeof(char) + sizeof(char));
		}

		if (type_value == TypeValue::STRING)
		{
			if (ofs >= config.size_str_data) return 0;
			return offset_char + ofs * (config.size_str + sizeof(char));
		}

		return 0;
	}

	int AdapterSharedMemory::demask(const int& value, int mask_source, const int& value_target, const int& mask_target)
	{
		int val_out = value & mask_source;
		if (mask_target > mask_source)
		{
			while (mask_target != mask_source)
			{
				mask_source <<= 1;
				val_out <<= 1;
			}
		}
		else
		{
			while (mask_target != mask_source)
			{
				mask_source >>= 1;
				val_out >>= 1;
			}
		}

		val_out = value_target & (~mask_target) | val_out;
		return val_out;
	}

	void AdapterSharedMemory::set_data(const ValueInt& value, const LinkTags& link)
	{
		char* buf;
		int val_last;
		int val_current = 0;

		buf = buf_data + TakeOffset(link.target.type, link.target.offset);
		val_last = *((int*)buf);
		
		if (link.source.mask != 0)
		{
			val_current = demask(value.value, link.source.mask, val_last, link.target.mask);
		}
		else
		{
			val_current = value.value;
		}

		if (link.type_registration == TypeRegistration::RECIVE)
		{
			
			*((int*)buf) = val_current;
			buf += sizeof(int);
			*buf = value.quality;
			return;
		}

		if (link.type_registration == TypeRegistration::UPDATE)
		{
			if (val_last == val_current) return;
			*((int*)buf) = val_current;
			buf += sizeof(int);
			*buf = value.quality;
			return;
		}

		if (link.type_registration == TypeRegistration::DELTA)
		{
			if (abs(val_last - val_current) > (int)link.delta)
			{
				*((int*)buf) = val_current;
				buf += sizeof(int);
				*buf = value.quality;
				return;
			}
		}
	}

	void AdapterSharedMemory::set_data(const ValueFloat& value, const LinkTags& link)
	{
		char* buf;
		float val_last;

		buf = buf_data + TakeOffset(link.target.type, link.target.offset);
		val_last = *((float*)buf);

		if (link.type_registration == TypeRegistration::RECIVE)
		{

			*((float*)buf) = value.value;
			buf += sizeof(float);
			*buf = value.quality;
			return;
		}

		if (link.type_registration == TypeRegistration::UPDATE)
		{
			if (val_last == value.value) return;
			*((float*)buf) = value.value;
			buf += sizeof(float);
			*buf = value.quality;
			return;
		}

		if (link.type_registration == TypeRegistration::DELTA)
		{
			if (fabs(val_last - value.value) > link.delta)
			{
				*((float*)buf) = value.value;
				buf += sizeof(float);
				*buf = value.quality;
				return;
			}
		}
	}

	void AdapterSharedMemory::set_data(const ValueDouble& value, const LinkTags& link)
	{
		char* buf;
		double val_last;

		buf = buf_data + TakeOffset(link.target.type, link.target.offset);
		val_last = *((double*)buf);

		if (link.type_registration == TypeRegistration::RECIVE)
		{

			*((double*)buf) = value.value;
			buf += sizeof(double);
			*buf = value.quality;
			return;
		}

		if (link.type_registration == TypeRegistration::UPDATE)
		{
			if (val_last == value.value) return;
			*((double*)buf) = value.value;
			buf += sizeof(double);
			*buf = value.quality;
			return;
		}

		if (link.type_registration == TypeRegistration::DELTA)
		{
			if (fabs(val_last - value.value) > link.delta)
			{
				*((double*)buf) = value.value;
				buf += sizeof(double);
				*buf = value.quality;
				return;
			}
		}
	}

	void AdapterSharedMemory::set_data(const ValueChar& value, const LinkTags& link)
	{
		char* buf;
		char val_last;

		buf = buf_data + TakeOffset(link.target.type, link.target.offset);
		val_last = *((char*)buf);

		if (link.type_registration == TypeRegistration::RECIVE)
		{

			*((char*)buf) = value.value;
			buf += sizeof(char);
			*buf = value.quality;
			return;
		}

		if (link.type_registration == TypeRegistration::UPDATE)
		{
			if (val_last == value.value) return;
			*((char*)buf) = value.value;
			buf += sizeof(char);
			*buf = value.quality;
			return;
		}

		if (link.type_registration == TypeRegistration::DELTA)
		{
			if (fabs(val_last - value.value) > link.delta)
			{
				*((char*)buf) = value.value;
				buf += sizeof(char);
				*buf = value.quality;
				return;
			}
		}
	}

	void AdapterSharedMemory::set_data(const ValueString& value, const LinkTags& link)
	{
		char* buf;
		std::string val_last;

		buf = buf_data + TakeOffset(link.target.type, link.target.offset);
		val_last = *((char*)buf);

		if (link.type_registration == TypeRegistration::RECIVE)
		{
			size_t i = 0;
			const char* str = value.value.c_str();
			while (i < config.size_str && i < value.value.size()) 
			{
				*((char*)buf) = *(str + i);
				buf += sizeof(char);
				i++;
			}
			*buf = value.quality;
			return;
		}

		if (link.type_registration == TypeRegistration::UPDATE)
		{
			if (val_last.compare(value.value) == 0) return;
			size_t i = 0;
			const char* str = value.value.c_str();
			while (i < config.size_str && i < value.value.size())
			{
				*((char*)buf) = *(str + i);
				buf += sizeof(char);
				i++;
			}
			*buf = value.quality;
			return;
		}
	}
}

