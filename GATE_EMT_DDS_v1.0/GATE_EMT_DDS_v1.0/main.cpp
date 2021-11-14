#include <iostream>
#include "Config_Reader.h"
#include "logger.h"
#include "Adapters.h"
#include "DDSUnit.h"

int main()
{

}

/*
*int main(int argc, char** argv)
{


	LoggerSpace::Logger* log = LoggerSpace::Logger::getpointcontact();
	log->TurnOnLog();
	log->WriteLogINFO("START");
	std::shared_ptr<ConfigSharedMemoryAdapter> config_writer = std::make_shared<ConfigSharedMemoryAdapter>();
	config_writer->NameMemory = "test_writer";
	config_writer->DataType = TypeData::ANALOG;
	config_writer->type_adapter = TypeAdapter::SharedMemory;
	config_writer->size = 100;

	std::shared_ptr<ConfigSharedMemoryAdapter> config_reader = std::make_shared<ConfigSharedMemoryAdapter>();;
	config_reader->NameMemory = "test_reader";
	config_reader->DataType = TypeData::ANALOG;
	config_reader->type_adapter = TypeAdapter::SharedMemory;
	config_reader->size = 100;

	std::shared_ptr<gate::Adapter> adapter_writer = gate::CreateAdapter(TypeAdapter::SharedMemory);
	std::shared_ptr<gate::Adapter> adapter_reader = gate::CreateAdapter(TypeAdapter::SharedMemory);

	adapter_writer->InitAdapter(config_writer);
	adapter_reader->InitAdapter(config_reader);

	ConfigDDSUnit config_DDS_writer;
	config_DDS_writer.Adapter = TypeAdapter::SharedMemory;
	config_DDS_writer.Domen = 1;
	config_DDS_writer.Frequency = 300;
	config_DDS_writer.PointName = "test_writer";
	config_DDS_writer.Size = 100;
	config_DDS_writer.Transmiter = TypeTransmiter::Broadcast;
	config_DDS_writer.Typedata = TypeData::ANALOG;
	config_DDS_writer.TypeUnit = TypeDDSUnit::PUBLISHER;

	ConfigDDSUnit config_DDS_reader;
	config_DDS_reader.Adapter = TypeAdapter::SharedMemory;
	config_DDS_reader.Domen = 1;
	config_DDS_reader.Frequency = 300;
	config_DDS_reader.PointName = "test_reader";
	config_DDS_reader.Size = 100;
	config_DDS_reader.Transmiter = TypeTransmiter::Broadcast;
	config_DDS_reader.Typedata = TypeData::ANALOG;
	config_DDS_reader.TypeUnit = TypeDDSUnit::SUBSCRIBER;

	std::shared_ptr<gate::DDSUnit> DDS_writer = gate::CreateDDSUnit(config_DDS_writer);
	std::shared_ptr<gate::DDSUnit> DDS_reader = gate::CreateDDSUnit(config_DDS_reader);

	DDS_writer->Initialization();
	DDS_reader->Initialization();

	float buf_out[100];
	float buf_in[100];
	for (int i = 0; i < 100; i++)
	{
		buf_out[i] = i;
	}

	while (1)
	{
		adapter_writer->WriteData(buf_out, 100);
		adapter_reader->ReadData(buf_in, 100);
		std::cout << "buf_in[0] = " << buf_in[0] << std::endl;
		std::cout << "buf_in[99] = " << buf_in[99] << std::endl;
		std::cout << "-----------------------------------------" << std::endl;

		for (int i = 0; i < 100; i++)
		{
			buf_out[i] += 0.1;
		}

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

};
*/

