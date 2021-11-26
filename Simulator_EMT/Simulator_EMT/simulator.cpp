#include "simulator.hpp"

namespace scada_ate
{
	namespace emt
	{
		namespace simulator
		{
			Simulator::Simulator()
			{
				log = LoggerSpace::Logger::getpointcontact();
				shared_memory = std::make_shared<AdapterEMTScada>();
				return;
			}

			Simulator::~Simulator()
			{
				std::lock_guard<std::mutex> lock(guard_vector);
				stoper.request_stop();
				for (auto i = vector_threads_simulation.begin(); i != vector_threads_simulation.end(); i++)
				{
					if (i->joinable())
					{
						i->join();
					}
				}

				return;
			}

			ResultRequest Simulator::add_simulator(UnitSimulation unit)
			{
				ResultRequest res = ResultRequest::OK;
				std::lock_guard<std::mutex> lock(guard_vector);

				if (unit.type_transfer == TypeTransfer::PUBLISHER)
				{
					vector_threads_simulation.push_back(std::jthread(&Simulator::thread_write, this, unit, stoper.get_token()));
				}
				else if (unit.type_transfer == TypeTransfer::SUBSCRIBER)
				{

					vector_threads_simulation.push_back(std::jthread(&Simulator::thread_read, this, unit, stoper.get_token()));

				}
				else
				{
					res = ResultRequest::ERR;
				}

				return res;
			}

			void Simulator::thread_write(UnitSimulation unit, std::stop_token stoper)
			{
				std::chrono::milliseconds time;
				std::chrono::steady_clock::time_point last;
				std::chrono::steady_clock::time_point current;
				int step_int = 1;
				float step_float = (float)0.1;
				unsigned int size_type = 0;
				unsigned int res_write = 0;
				std::string helpstr;

				try
				{
					if (unit.type_data == TypeData::ANALOG || unit.type_data == TypeData::DISCRETE) size_type = 4;
					if (unit.type_data == TypeData::BINAR) size_type = 1;

					std::shared_ptr<char> buf(new char[unit.size * size_type], std::default_delete<char[]>());
					std::shared_ptr<int> k(new int[unit.size * size_type], std::default_delete<int[]>());

					for (int i = 0; i < unit.size; i++)
					{
						*(k.get() + i) = 1;
					}

					if (unit.type_data == TypeData::ANALOG) { fillin_buffer<float>((float*)buf.get(), unit.size); }
					else if (unit.type_data == TypeData::DISCRETE) { fillin_buffer<int>((int*)buf.get(), unit.size); }
					else if (unit.type_data == TypeData::BINAR) { fillin_buffer<char>(buf.get(), unit.size); }

					last = std::chrono::high_resolution_clock::now();
					current = std::chrono::high_resolution_clock::now();

					while (!stoper.stop_requested())
					{
						current = std::chrono::high_resolution_clock::now();
						time = std::chrono::duration_cast<std::chrono::milliseconds>(current - last);
						if (time.count() < unit.frequency) continue;
						last = std::chrono::high_resolution_clock::now();

						update_buffer(buf.get(), unit, k.get());

						res_write = shared_memory->WriteData(unit.type_data, buf.get(), unit.size, unit.point_name);
						if (res_write != 0)
						{
							helpstr.clear();
							helpstr = "Error WriteData : Publisher - " + unit.point_name;
							log->WriteLogERR(helpstr.c_str(), res_write, 0);
							return;
						}

						show_data(buf.get(), unit);
						
					}
				}
				catch (int& e)
				{
					helpstr.clear();
					helpstr = "Error Thread : Publisher - " + unit.point_name;
					log->WriteLogERR(helpstr.c_str(), res_write, 0);
					return;
				}
				catch (...)
				{
					helpstr.clear();
					helpstr = "Error Thread : Publisher - " + unit.point_name;
					log->WriteLogERR(helpstr.c_str(), res_write, 0);
					return;
				}
			}

			void Simulator::thread_read(UnitSimulation unit, std::stop_token stoper)
			{
				std::chrono::milliseconds time;
				std::chrono::steady_clock::time_point last;
				std::chrono::steady_clock::time_point current;
				unsigned int size_type = 0;

				if (unit.type_data == TypeData::ANALOG || unit.type_data == TypeData::DISCRETE) size_type = 4;
				if (unit.type_data == TypeData::BINAR) size_type = 1;


				std::shared_ptr<char> buf(new char[unit.size * size_type], std::default_delete<char[]>());

				for (int i = 0; i < unit.size * size_type; i++)
				{
					*(buf.get() + i) = 0;
				}

				float* point_buf_float = (float*)buf.get();
				int* point_buf_int = (int*)buf.get();
				char* point_buf_char = (char*)buf.get();;

				last = std::chrono::high_resolution_clock::now();
				current = std::chrono::high_resolution_clock::now();

				while (!stoper.stop_requested())
				{
					current = std::chrono::high_resolution_clock::now();
					time = std::chrono::duration_cast<std::chrono::milliseconds>(current - last);
					if (time.count() < unit.frequency) continue;
					last = std::chrono::high_resolution_clock::now();

					shared_memory->ReadData(unit.type_data, buf.get(), unit.size, unit.point_name);

					if (unit.show_console == ShowDataConsole::ON)
					{
						std::lock_guard<std::mutex> lock(guard_console);
						std::cout << " /// ***  Subscribtion - " << unit.point_name << " *** ///" << std::endl;
						for (unsigned int i = 0; i < unit.size_output; i++)
						{
							if (unit.type_data == TypeData::ANALOG)
							{
								std::cout << "value[" << i << "] = " << *(point_buf_float + i) << ";" << std::endl;
							}

							if (unit.type_data == TypeData::DISCRETE)
							{
								std::cout << "value[" << i << "] = " << *(point_buf_int + i) << ";" << std::endl;
							}

							if (unit.type_data == TypeData::BINAR)
							{
								std::cout << "value[" << i << "] = " << *(point_buf_char + i) << ";" << std::endl;
							}
						}
					}
				}

			}

			void Simulator::update_buffer(void* buf, UnitSimulation unit, int* k)
			{
				if (unit.type_signal == TypeSignal::SINE)
				{
					if (unit.type_data == TypeData::ANALOG) { update_buffer_sine<float>((float*)buf, unit.size, 10000, unit.amplitude); }
					else if (unit.type_data == TypeData::DISCRETE) { update_buffer_sine<int>((int*)buf, unit.size, 10000, unit.amplitude); }
				}

				if (unit.type_signal == TypeSignal::TRIANGLE)
				{
					if (unit.type_data == TypeData::ANALOG) { update_buffer_triangle<float>((float*)buf, unit.size, k, 0.1, unit.amplitude); }
					else if (unit.type_data == TypeData::DISCRETE) { update_buffer_triangle<int>((int*)buf, unit.size, k, 1, unit.amplitude); }
				}

				if (unit.type_data == TypeData::BINAR)
				{
					updateDataBinar((char*)buf, unit.size);
				}

				if (unit.type_signal == TypeSignal::SAW)
				{
					if (unit.type_data == TypeData::ANALOG) { update_buffer_saw<float>((float*)buf, unit.size, 0.1, unit.amplitude); }
					else if (unit.type_data == TypeData::DISCRETE) { update_buffer_saw<int>((int*)buf, unit.size, 1, unit.amplitude); }
				}

			}

			template <typename T> void  Simulator::fillin_buffer(T* buf, unsigned int size)
			{
				for (unsigned int i = 0; i < size; i++)
				{
					*(buf + i) = i;
				}
			}

			template <typename T> void  Simulator::fillin_buffer(T* buf, unsigned int size, T value)
			{
				for (unsigned int i = 0; i < size; i++)
				{
					*(buf + i) = value;
				}
			}

			template <typename T> void  Simulator::update_buffer_triangle(T* buf, unsigned int size, int* k, T step, unsigned int Amp)
			{
				if (step != 0)
				{
					for (int i = 0; i < size; i++)
					{
						*(buf + i) = *(buf + i) + (*(k + i)) * step;
						if (*(buf + i) >= (T)Amp) *(k + i) = -1;
						if (*(buf + i) <= -((T)Amp)) *(k + i) = 1;
					}
				}
			}

			template <typename T> void  Simulator::update_buffer_sine(T* buf, unsigned int size, unsigned int freq, unsigned int Amp)
			{
				std::chrono::milliseconds time =
					std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock().now().time_since_epoch());

				float alfa = (time.count() % freq * 1.) / freq;

				for (int i = 0; i < size; i++)
				{
					*(buf + i) = Amp * sin(alfa * 2 * std::numbers::pi + 2 * std::numbers::pi / 100 * i);
				}
			}

			template <typename T> void  Simulator::update_buffer_saw(T* buf, unsigned int size, T step, unsigned int Amp)
			{
				for (int i = 0; i < size; i++)
				{
					*(buf + i) = *(buf + i) + step;
					if (*(buf + i) > Amp) *(buf + i) = 0;
				}
			}
			
			void Simulator::updateDataBinar(char* buf, unsigned int size)
			{
				for (int i = 0; i < size; i++)
				{
					*(buf + i) = (*(buf + i) + 1) & 1;
				}
			}

			void Simulator::show_data(void* buf, UnitSimulation unit)
			{
				if (unit.show_console == ShowDataConsole::ON)
				{
					std::lock_guard<std::mutex> lock(guard_console);
					std::cout << " /// ***  Publication - " << unit.point_name << " *** ///" << std::endl;
					for (unsigned int i = 0; i < unit.size_output; i++)
					{
						if (unit.type_data == TypeData::ANALOG)
						{
							std::cout << "value[" << i << "] = " << *((float*)buf + i) << ";" << std::endl;
						}

						if (unit.type_data == TypeData::DISCRETE)
						{
							std::cout << "value[" << i << "] = " << *((int*)buf + i) << ";" << std::endl;
						}

						if (unit.type_data == TypeData::BINAR)
						{
							std::cout << "value[" << i << "] = " << *((char*)buf + i) << ";" << std::endl;
						}
					}
				}
			}
		}

	}
}