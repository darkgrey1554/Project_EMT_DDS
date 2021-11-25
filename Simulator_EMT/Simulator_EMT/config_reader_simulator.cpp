#include "config_reader_simulater.hpp"
#include "rapidjson/document.h"
#include <fstream>


namespace scada_ate
{
	namespace emt
	{
		namespace config
		{
			ConfigReaderSimulator::ConfigReaderSimulator()
			{
				log = LoggerSpace::Logger::getpointcontact();
			}

			ConfigReaderSimulator::ConfigReaderSimulator(std::string name)
			{
				log = LoggerSpace::Logger::getpointcontact();
				name_file.clear();
				name_file = name;
			}

			ConfigReaderSimulator::~ConfigReaderSimulator()
			{

			}

			ResultRequest ConfigReaderSimulator::ReadConfig(std::vector<UnitSimulation>& out_vector)
			{
				std::fstream file;
				ResultRequest result = ResultRequest::OK;
				UnitSimulation unit;
				std::vector<UnitSimulation> vector_sinulators;

				file.open(name_file, std::ios::in);
				if (!file.is_open())
				{
					throw 1;
				}
				/// --- reading the whole file --- ///
				std::string str((std::istreambuf_iterator<char>(file)),
					std::istreambuf_iterator<char>());

				document.Parse(str.c_str());

				if (!document.HasMember("Units")) throw 2;
				if (!document["Units"].IsArray()) throw 3;
				if (document["Units"].Size() <= 0) throw 4;

				vector_sinulators.clear();

				for (int i = 0; i < document["Units"].Size(); i++)
				{
					if (take_type_transfer(unit, i) != ResultRequest::OK) { result = ResultRequest::ERR; continue; }
					if (take_point_name(unit, i) != ResultRequest::OK) { result = ResultRequest::ERR; continue; }
					if (take_type_data(unit, i) != ResultRequest::OK) { result = ResultRequest::ERR; continue; }
					if (take_size(unit, i) != ResultRequest::OK) { result = ResultRequest::ERR; continue; }
					if (take_frequency(unit, i) != ResultRequest::OK) { result = ResultRequest::ERR; }
					if (take_type_signal(unit, i) != ResultRequest::OK) { result = ResultRequest::ERR; }
					if (take_amplitude(unit, i) != ResultRequest::OK) { result = ResultRequest::ERR; }
					if (take_show(unit, i) != ResultRequest::OK) { result = ResultRequest::ERR; }
					if (take_output(unit, i) != ResultRequest::OK) { result = ResultRequest::ERR; }

					vector_sinulators.push_back(unit);
				}

				out_vector.clear();
				out_vector = vector_sinulators;
			}

			ResultRequest ConfigReaderSimulator::take_type_transfer(UnitSimulation& unit, unsigned int i)
			{
				ResultRequest res = ResultRequest::OK;
				std::string helpstr;

				try
				{
					if (!document["Units"][i].HasMember("TypeTransfer")) throw 1;
					if (!document["Units"][i]["TypeTransfer"].IsString()) throw 2;
					helpstr.clear();
					helpstr = document["Units"][i]["TypeTransfer"].GetString();
					if (StringToTypeTransfer(helpstr, unit.type_transfer) != ResultRequest::OK) throw 3;
				}
				catch (int& e)
				{
					log->WriteLogWARNING("Error ConfigReaderSimulator: error take_type_transfer", e, 0);
					res = ResultRequest::ERR;
				}
				catch (...)
				{
					log->WriteLogWARNING("Error ConfigReaderSimulator:  error take_type_transfer", 0, 0);
					res = ResultRequest::ERR;
				}

				return res;
			}

			ResultRequest ConfigReaderSimulator::take_point_name(UnitSimulation& unit, unsigned int i)
			{
				ResultRequest res = ResultRequest::OK;
				std::string helpstr;

				try
				{
					if (!document["Units"][i].HasMember("PointName")) throw 1;
					if (!document["Units"][i]["PointName"].IsString()) throw 2;
					helpstr.clear();
					helpstr = document["Units"][i]["PointName"].GetString();
					if (helpstr.empty()) throw 3;
					unit.point_name.clear();
					unit.point_name = helpstr;
				}
				catch (int& e)
				{
					log->WriteLogWARNING("Error ConfigReaderSimulator: error take_point_name", e, 0);
					res = ResultRequest::ERR;
				}
				catch (...)
				{
					log->WriteLogWARNING("Error ConfigReaderSimulator:  error take_point_name", 0, 0);
					res = ResultRequest::ERR;
				}

				return res;
			}

			ResultRequest ConfigReaderSimulator::take_type_data(UnitSimulation& unit, unsigned int i)
			{
				ResultRequest res = ResultRequest::OK;
				std::string helpstr;

				try
				{
					if (!document["Units"][i].HasMember("TypeData")) throw 1;
					if (!document["Units"][i]["TypeData"].IsString()) throw 2;
					helpstr.clear();
					helpstr = document["Units"][i]["TypeData"].GetString();
					if (StringToTypeData(helpstr, unit.type_data) != ResultRequest::OK) throw 3;
				}
				catch (int& e)
				{
					log->WriteLogWARNING("Error ConfigReaderSimulator: error take_type_data", e, 0);
					res = ResultRequest::ERR;
				}
				catch (...)
				{
					log->WriteLogWARNING("Error ConfigReaderSimulator:  error take_type_data", 0, 0);
					res = ResultRequest::ERR;
				}

				return res;
			}

			ResultRequest ConfigReaderSimulator::take_type_signal(UnitSimulation& unit, unsigned int i)
			{
				ResultRequest res = ResultRequest::OK;
				std::string helpstr;

				try
				{
					if (!document["Units"][i].HasMember("TypeSignal")) throw 1;
					if (!document["Units"][i]["TypeSignal"].IsString()) throw 2;
					helpstr.clear();
					helpstr = document["Units"][i]["TypeSignal"].GetString();
					if (StringToTypeSignal(helpstr, unit.type_signal) != ResultRequest::OK) throw 3;
				}
				catch (int& e)
				{
					log->WriteLogWARNING("Error ConfigReaderSimulator: error take_type_data", e, 0);
					res = ResultRequest::ERR;
					unit.type_signal = TypeSignal::TRIANGLE;
				}
				catch (...)
				{
					log->WriteLogWARNING("Error ConfigReaderSimulator:  error take_type_data", 0, 0);
					res = ResultRequest::ERR;
					unit.type_signal = TypeSignal::TRIANGLE;
				}

				return res;
			}

			ResultRequest ConfigReaderSimulator::take_size(UnitSimulation& unit, unsigned int i)
			{
				ResultRequest res = ResultRequest::OK;
				try
				{
					if (!document["Units"][i].HasMember("Size")) throw 1;
					if (!document["Units"][i]["Size"].IsUint()) throw 2;
					unit.size = document["Units"][i]["Size"].GetUint();
					if (unit.size == 0) throw 3;
				}
				catch (int& e)
				{
					log->WriteLogWARNING("Error ConfigReaderSimulator: error take_size", e, 0);
					res = ResultRequest::ERR;
				}
				catch (...)
				{
					log->WriteLogWARNING("Error ConfigReaderSimulator:  error take_size", 0, 0);
					res = ResultRequest::ERR;
				}

				return res;
			}

			ResultRequest ConfigReaderSimulator::take_frequency(UnitSimulation& unit, unsigned int i)
			{
				ResultRequest res = ResultRequest::OK;
				try
				{
					if (!document["Units"][i].HasMember("Frequency")) throw 1;
					if (!document["Units"][i]["Frequency"].IsUint()) throw 2;
					unit.frequency = document["Units"][i]["Frequency"].GetUint();
					if (unit.frequency == 0) unit.frequency = 1000;
				}
				catch (int& e)
				{
					log->WriteLogWARNING("Error ConfigReaderSimulator: error take_frequency", e, 0);
					res = ResultRequest::ERR;
					unit.frequency = 1000;
				}
				catch (...)
				{
					log->WriteLogWARNING("Error ConfigReaderSimulator:  error take_frequency", 0, 0);
					res = ResultRequest::ERR;
					unit.frequency = 1000;
				}

				return res;
			}

			ResultRequest ConfigReaderSimulator::take_amplitude(UnitSimulation& unit, unsigned int i)
			{
				ResultRequest res = ResultRequest::OK;
				try
				{
					if (!document["Units"][i].HasMember("Amplitude")) throw 1;
					if (!document["Units"][i]["Amplitude"].IsUint()) throw 2;
					unit.amplitude = document["Units"][i]["Amplitude"].GetUint();
					if (unit.amplitude == 0) unit.amplitude = 10;
				}
				catch (int& e)
				{
					log->WriteLogWARNING("Error ConfigReaderSimulator: error take_amplitude", e, 0);
					res = ResultRequest::ERR;
					unit.amplitude = 10;
				}
				catch (...)
				{
					log->WriteLogWARNING("Error ConfigReaderSimulator:  error take_amplitude", 0, 0);
					res = ResultRequest::ERR;
					unit.amplitude = 10;
				}

				return res;
			}

			ResultRequest ConfigReaderSimulator::take_show(UnitSimulation& unit, unsigned int i)
			{
				ResultRequest res = ResultRequest::OK;
				std::string helpstr;

				try
				{
					if (!document["Units"][i].HasMember("Show")) throw 1;
					if (!document["Units"][i]["Show"].IsString()) throw 2;
					helpstr.clear();
					helpstr = document["Units"][i]["Show"].GetString();
					if (StringToShowDataConsole(helpstr, unit.show_console) != ResultRequest::OK) throw 3;
				}
				catch (int& e)
				{
					log->WriteLogWARNING("Error ConfigReaderSimulator: error take_show", e, 0);
					res = ResultRequest::ERR;
					unit.show_console = ShowDataConsole::OFF;
				}
				catch (...)
				{
					log->WriteLogWARNING("Error ConfigReaderSimulator: error take_show", 0, 0);
					res = ResultRequest::ERR;
					unit.show_console = ShowDataConsole::OFF;
				}

				return res;
			}

			ResultRequest ConfigReaderSimulator::take_output(UnitSimulation& unit, unsigned int i)
			{
				ResultRequest res = ResultRequest::OK;
				try
				{
					if (!document["Units"][i].HasMember("Output")) throw 1;
					if (!document["Units"][i]["Output"].IsUint()) throw 2;
					unit.size_output = document["Units"][i]["Output"].GetUint();

				}
				catch (int& e)
				{
					log->WriteLogWARNING("Error ConfigReaderSimulator: error take_output", e, 0);
					res = ResultRequest::ERR;
					unit.size_output = 0;
				}
				catch (...)
				{
					log->WriteLogWARNING("Error ConfigReaderSimulator:  error take_output", 0, 0);
					res = ResultRequest::ERR;
					unit.size_output = 0;
				}

				return res;
			}

			ResultRequest ConfigReaderSimulator::StringToTypeTransfer(std::string str, TypeTransfer& transfer)
			{
				ResultRequest res = ResultRequest::ERR;
				try
				{
					if (str.compare("Publication") == 0)
					{
						transfer = TypeTransfer::PUBLISHER;
						res = ResultRequest::OK;
					}

					if (str.compare("Subscription") == 0)
					{
						transfer = TypeTransfer::SUBSCRIBER;
						res = ResultRequest::OK;
					}
				}
				catch (...)
				{
					log->WriteLogWARNING("Error ConfigReaderSimilator: error TypeTransfer:", 0, 0);
					res = ResultRequest::ERR;
				}

				return res;
			}

			ResultRequest ConfigReaderSimulator::StringToTypeData(std::string str, TypeData& transfer)
			{
				ResultRequest res = ResultRequest::ERR;
				try
				{
					if (str.compare("ANALOG") == 0)
					{
						transfer = TypeData::ANALOG;
						res = ResultRequest::OK;
					}

					if (str.compare("DISCRETE") == 0)
					{
						transfer = TypeData::DISCRETE;
						res = ResultRequest::OK;
					}

					if (str.compare("BINAR") == 0)
					{
						transfer = TypeData::BINAR;
						res = ResultRequest::OK;
					}
				}
				catch (...)
				{
					log->WriteLogWARNING("Error ConfigReaderSimilator: error TypeTypeData:", 0, 0);
					res = ResultRequest::ERR;
				}

				return res;
			}

			ResultRequest ConfigReaderSimulator::StringToShowDataConsole(std::string str, ShowDataConsole& transfer)
			{
				ResultRequest res = ResultRequest::ERR;
				try
				{
					if (str.compare("ON") == 0)
					{
						transfer = ShowDataConsole::ON;
						res = ResultRequest::OK;
					}

					else if (str.compare("OFF") == 0)
					{
						transfer = ShowDataConsole::OFF;
						res = ResultRequest::OK;
					}
					else
					{
						throw 1;
					}
					
				}
				catch (int& e)
				{
					log->WriteLogWARNING("Error ConfigReaderSimilator: error showdataconsole:", e, 0);
					res = ResultRequest::ERR;
				}
				catch (...)
				{
					log->WriteLogWARNING("Error ConfigReaderSimilator: error showdataconsole:", 0, 0);
					res = ResultRequest::ERR;
				}

				return res;
			}

			ResultRequest ConfigReaderSimulator::StringToTypeSignal(std::string str, TypeSignal& transfer)
			{
				ResultRequest res = ResultRequest::ERR;
				try
				{
					if (str.compare("Triangle") == 0)
					{
						transfer = TypeSignal::TRIANGLE;
						res = ResultRequest::OK;
					}

					if (str.compare("Constant") == 0)
					{
						transfer = TypeSignal::CONSTANT;
						res = ResultRequest::OK;
					}

					if (str.compare("Saw") == 0)
					{
						transfer = TypeSignal::SAW;
						res = ResultRequest::OK;
					}

					if (str.compare("Sine") == 0)
					{
						transfer = TypeSignal::SINE;
						res = ResultRequest::OK;
					}

					throw 1;
				}
				catch (int& e)
				{
					log->WriteLogWARNING("Error ConfigReaderSimilator: error typesignal:", e, 0);
					res = ResultRequest::ERR;
				}
				catch (...)
				{
					log->WriteLogWARNING("Error ConfigReaderSimilator: error typesignal:", 0, 0);
					res = ResultRequest::ERR;
				}

				return res;
			}
		}
	}
}