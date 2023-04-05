#include <Module_IO/UnitTransfer/adapters/SharedMemory_Adapter/AdapterSharedMemory.hpp>
#include <algorithm>
#include <numeric>
#include <string>
#include <functional>

using namespace scada_ate;
using namespace scada_ate::gate::adapter;
using namespace scada_ate::gate::adapter::sem;

class TT
{
	static long long time;
public:
		void FiksTime() { time = TimeConverter::GetTime_LLmcs(); };
		long long GetTicks() { return TimeConverter::GetTime_LLmcs() - time; }

};

long long TT::time = 0;

int main()
{

	std::shared_ptr<scada_ate::gate::adapter::IAdapter> adapt_in;
	std::shared_ptr<scada_ate::gate::adapter::IAdapter> adapt_out;

	size_t size_data = 100000;

	std::vector<InfoTag> _tags_in{size_data};
	std::vector<InfoTag> _tags_out{ size_data };

	std::vector<LinkTags> link_tags{ size_data };

	for (size_t i = 0; i < size_data/2; i++)
	{
		_tags_in[i].tag = "II" + std::to_string(i);
		_tags_in[i].offset = i;
		_tags_in[i].type = scada_ate::gate::adapter::TypeValue::INT;

		_tags_in[i+ size_data/2].tag = "IF" + std::to_string(i+ size_data/2);
		_tags_in[i+ size_data/2].offset = i;
		_tags_in[i+ size_data/2].type = scada_ate::gate::adapter::TypeValue::FLOAT;

		_tags_out[i].tag = "OI" + std::to_string(i);
		_tags_out[i].offset = i;
		_tags_out[i].type = scada_ate::gate::adapter::TypeValue::INT;

		_tags_out[i + size_data/2].tag = "OF" + std::to_string(i + size_data/2);
		_tags_out[i + size_data/2].offset = i;
		_tags_out[i + size_data/2].type = scada_ate::gate::adapter::TypeValue::FLOAT;

		link_tags[i].source = _tags_in[i];
		link_tags[i].target = _tags_out[i];
		link_tags[i].type_registration = TypeRegistration::RECIVE;

		link_tags[i+ size_data/2].source = _tags_in[i+ size_data/2];
		link_tags[i+ size_data/2].target = _tags_out[i+ size_data/2];
		link_tags[i+ size_data/2].type_registration = TypeRegistration::RECIVE;
	}

	std::shared_ptr<ConfigAdapterSharedMemory> config_in = std::make_shared<ConfigAdapterSharedMemory>();
	config_in->type_adapter = scada_ate::gate::adapter::TypeAdapter::SharedMemory;
	config_in->NameChannel = "point_001";
	config_in->size_int_data = size_data/2;
	config_in->size_float_data = size_data/2;
	config_in->size_double_data = 0;
	config_in->size_char_data = 0;
	config_in->size_str_data = 0;
	config_in->vec_tags_source = _tags_in;

	std::shared_ptr<ConfigAdapterSharedMemory> config_out = std::make_shared<ConfigAdapterSharedMemory>();;
	config_out->type_adapter = scada_ate::gate::adapter::TypeAdapter::SharedMemory;
	config_out->NameChannel = "point_001";
	config_out->size_int_data = size_data/2;
	config_out->size_float_data = size_data/2;
	config_out->size_double_data = 0;
	config_out->size_char_data = 0;
	config_out->size_str_data = 0;
	config_out->vec_link_tags = link_tags;

	adapt_in = CreateAdapter(config_in);
	adapt_out = CreateAdapter(config_out);

	adapt_in->InitAdapter();
	adapt_out->InitAdapter();

	std::deque<SetTags> data_send(1);
	std::deque<SetTags>* data_recive = &data_send;// = data_send;
	int count = 0;

	for (InfoTag& tag : _tags_in)
	{
		if (tag.type == scada_ate::gate::adapter::TypeValue::FLOAT)
		{
			data_send[0].map_data[tag] = {0, count * 0.1f , 1};
		}
		else if (tag.type == scada_ate::gate::adapter::TypeValue::INT)
		{
			data_send[0].map_data[tag] = {0, count, 1};
		}

		count++;
	}

	adapt_out->WriteData(data_send);
	adapt_in->ReadData(&data_recive);


	//ValueFloat val_f;
	//ValueInt val_i;

	int c =0;
	size_t count_iter = 40000;
	size_t count_ = 0;
	long long time_read = 0;
	long long time_write = 0;
	TT time_master;

	while (1)
	{
		c = 0;

		/*for (auto& tag : _tags_in)
		{
			if (tag.type == scada_ate::gate::adapter::TypeValue::FLOAT)
			{
				val_f = data_recive.map_float_data[tag];
				std::cout << std::cout.width(8) << std::left << tag.tag << "{ " << val_f.value << ", " << val_f.quality << " }" << std::endl;
			}
			else if (tag.type == scada_ate::gate::adapter::TypeValue::INT)
			{
				val_i = data_recive.map_int_data[tag];
				std::cout.width(8);
				std::cout << std::left << tag.tag << "{ " << val_i.value << ", " << val_i.quality << " }" << std::endl;;
			}

			c++;
			if (c > 100) break;
		}*/

		c = 0;

		time_master.FiksTime();
		adapt_out->WriteData(data_send);
		time_write += time_master.GetTicks();
		
		time_master.FiksTime();
		adapt_in->ReadData(&data_recive);
		time_read += time_master.GetTicks();

		count_++;

		if (count_ >= count_iter) break;
		std::cout << (count_ * 100.) / count_iter << "%" << std::endl;

		//Sleep(1000);

	}


	std::cout << "time read : " << time_read * 1. / count_iter << " mcs" << std::endl;
	std::cout << "time write : " << time_write * 1. / count_iter << " mcs" << std::endl;
	

	return 0;
}