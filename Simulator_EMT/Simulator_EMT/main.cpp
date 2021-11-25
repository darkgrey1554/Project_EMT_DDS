#include "adapter_EmtScadaAte.hpp"
#include "config_reader_simulater.hpp"
#include "simulator.hpp"

using namespace scada_ate;

int main()
{

	std::shared_ptr<emt::config::ConfigReaderSimulator> config_reader = std::make_shared<emt::config::ConfigReaderSimulator>();
	std::vector<UnitSimulation> vector_units_simulation;
	std::shared_ptr<emt::simulator::Simulator> simulator_ = std::make_shared<emt::simulator::Simulator>();

	config_reader->ReadConfig(vector_units_simulation);

	for (auto i = vector_units_simulation.begin(); i != vector_units_simulation.end(); i++)
	{
		simulator_->add_simulator(*i);
	}

	while (1)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
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