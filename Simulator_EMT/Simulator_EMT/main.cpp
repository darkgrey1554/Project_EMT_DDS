#include "adapter_EmtScadaAte.hpp"
#include "config_reader_simulater.hpp"
#include "simulator.hpp"
#include <conio.h>

using namespace scada_ate;

void show_config_publisher(UnitSimulation unit)
{
	std::cout << "Type transfer: " << "Publisher" << std::endl;
	std::cout << "Point contact: " << unit.point_name << std::endl;
	std::cout << "Type Data: ";
	if (unit.type_data == TypeData::ANALOG) std::cout << "Analog" << std::endl;
	if (unit.type_data == TypeData::DISCRETE) std::cout << "Discrete" << std::endl;
	if (unit.type_data == TypeData::BINAR) std::cout << "Binar" << std::endl;
	std::cout << "Size: " << unit.size << std::endl;

	std::cout << "Type Signal: ";
	if (unit.type_signal == TypeSignal::CONSTANT) std::cout << "Constant" << std::endl;
	if (unit.type_signal == TypeSignal::SAW) std::cout << "Saw" << std::endl;
	if (unit.type_signal == TypeSignal::SINE) std::cout << "Sine" << std::endl;
	if (unit.type_signal == TypeSignal::TRIANGLE) std::cout << "Tringle" << std::endl;

	std::cout << "Amplitude: " << unit.amplitude << std::endl;
	std::cout << "Frequency: " << unit.frequency << " ms" << std::endl;
	std::cout << "Show data: ";
	if (unit.show_console == ShowDataConsole::ON) std::cout << "ON" << std::endl;
	if (unit.show_console == ShowDataConsole::OFF) std::cout << "OFF" << std::endl;
	std::cout << std::endl;	
}

void show_config_subscriber(UnitSimulation unit)
{
	std::cout << "Type transfer: " << "Subscriber" << std::endl;
	std::cout << "Point contact: " << unit.point_name << std::endl;
	std::cout << "Type Data: ";
	if (unit.type_data == TypeData::ANALOG) std::cout << "Analog" << std::endl;
	if (unit.type_data == TypeData::DISCRETE) std::cout << "Discrete" << std::endl;
	if (unit.type_data == TypeData::BINAR) std::cout << "Binar" << std::endl;
	std::cout << "Size: " << unit.size << std::endl;
	std::cout << "Show data: ";
	if (unit.show_console == ShowDataConsole::ON) std::cout << "ON" << std::endl;
	if (unit.show_console == ShowDataConsole::OFF) std::cout << "OFF" << std::endl;
	std::cout << std::endl;
}

int main()
{

	std::shared_ptr<emt::config::ConfigReaderSimulator> config_reader = std::make_shared<emt::config::ConfigReaderSimulator>();
	std::vector<UnitSimulation> vector_units_simulation;
	std::shared_ptr<emt::simulator::Simulator> simulator_ = std::make_shared<emt::simulator::Simulator>();

	config_reader->ReadConfig(vector_units_simulation);

	std::cout << "### Simulator SCADA ATE ### " << std::endl;
	std::cout << "Current configuration: " << std::endl;
	int j = 0;
	for (auto v : vector_units_simulation)
	{
		j++;
		std::cout << "Unit "<< j<< ":" << std::endl;
		if (v.type_transfer == TypeTransfer::PUBLISHER) show_config_publisher(v);
		if (v.type_transfer == TypeTransfer::SUBSCRIBER) show_config_subscriber(v);
	}
	
	std::cout << "Press any button to start" << std::endl;

	getch();

	for (auto i = vector_units_simulation.begin(); i != vector_units_simulation.end(); i++)
	{
		simulator_->add_simulator(*i);
	}

	while (1)
	{
		std::this_thread::sleep_for(std::chrono::seconds(100));
	}

	return 0;
}

/*int main()
{
	std::shared_ptr<scada_ate::emt::AdapterEMTScada> adapter_publisher = std::make_shared<scada_ate::emt::AdapterEMTScada>("sm001");
	std::shared_ptr<scada_ate::emt::AdapterEMTScada> adapter_subscriber = std::make_shared<scada_ate::emt::AdapterEMTScada>("sm002");

	int* buf_out= new int[100];
	int* buf_in = new int[100];
	int count = 0;
	while(1)
	{
		count++;

		for (int i = 0; i < 100; i++)
		{
			buf_out[i] = count+i;
		}

		adapter_publisher->WriteData(TypeData::ANALOG, buf_out, 100);

		adapter_subscriber->ReadData(TypeData::ANALOG, buf_in, 100);

		for (int i = 0; i < 10; i++)
		{
			std::cout << buf_in[i] << std::endl;
		}

		std::cout << "////////////////////////////" << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	return 0;
}*/