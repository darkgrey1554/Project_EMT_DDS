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
					if (unit.type_signal == TypeSignal::TRIANGLE)
					{
						vector_threads_simulation.push_back(std::jthread(&Simulator::thread_triangle, this, unit, stoper.get_token()));
					}

					if (unit.type_signal == TypeSignal::CONSTANT)
					{
						vector_threads_simulation.push_back(std::jthread(&Simulator::thread_constant, this, unit, stoper.get_token()));
					}

					if (unit.type_signal == TypeSignal::SINE)
					{
						vector_threads_simulation.push_back(std::jthread(&Simulator::thread_sine, this, unit, stoper.get_token()));
					}

					if (unit.type_signal == TypeSignal::SAW)
					{
						vector_threads_simulation.push_back(std::jthread(&Simulator::thread_saw, this, unit, stoper.get_token()));
					}
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

			void Simulator::thread_triangle(UnitSimulation unit, std::stop_token stoper)
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

					float* point_buf_float = (float*)buf.get();
					int* point_buf_int = (int*)buf.get();
					char* point_buf_char = buf.get();

					for (int i = 0; i < unit.size; i++)
					{
						*(k.get() + i) = 1;
					}

					if (unit.type_data == TypeData::ANALOG) { fillin_buffer<float>(point_buf_float, unit.size); }
					else if (unit.type_data == TypeData::DISCRETE) { fillin_buffer<int>(point_buf_int, unit.size); }
					else if (unit.type_data == TypeData::BINAR) { fillin_buffer<char>(point_buf_char, unit.size); }

					last = std::chrono::high_resolution_clock::now();
					current = std::chrono::high_resolution_clock::now();

					while (!stoper.stop_requested())
					{
						current = std::chrono::high_resolution_clock::now();
						time = std::chrono::duration_cast<std::chrono::milliseconds>(current - last);
						if (time.count() < unit.frequency) continue;
						last = std::chrono::high_resolution_clock::now();

						if (unit.type_signal == TypeSignal::TRIANGLE)
						{
							if (unit.type_data == TypeData::ANALOG) { update_buffer_triangle<float>(point_buf_float, unit.size, k.get(), step_float, unit.amplitude); }
							else if (unit.type_data == TypeData::DISCRETE) { update_buffer_triangle<int>(point_buf_int, unit.size, k.get(), step_int, unit.amplitude); }
							else if (unit.type_data == TypeData::BINAR) { update_buffer_triangle<int>(point_buf_int, unit.size, k.get(), 0, unit.amplitude); }
						}
						else if (unit.type_signal == TypeSignal::SINE)
						{

						}
						else if (unit.type_signal == TypeSignal::SAW)
						{

						}
						else if (unit.type_signal == TypeSignal::CONSTANT)
						{

						}

						res_write = shared_memory->WriteData(unit.type_data, buf.get(), unit.size, unit.point_name);
						if (res_write != 0)
						{
							helpstr.clear();
							helpstr = "Error WriteData : Publisher - " + unit.point_name;
							log->WriteLogERR(helpstr.c_str(), res_write, 0);
							return;
						}

						if (unit.show_console == ShowDataConsole::ON)
						{
							std::lock_guard<std::mutex> lock(guard_console);
							std::cout << " /// ***  Publication - " << unit.point_name << " *** ///" << std::endl;
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
				catch (...)
				{
					helpstr.clear();
					helpstr = "Error WriteData : Publisher - " + unit.point_name;
					log->WriteLogERR(helpstr.c_str(), 0, 0);
					return;
				}			
				
				return;
			}

			void Simulator::thread_sine(UnitSimulation unit, std::stop_token stoper)
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
					
					float* point_buf_float = (float*)buf.get();
					int* point_buf_int = (int*)buf.get();
					char* point_buf_char = buf.get();

					
					if (unit.type_data == TypeData::ANALOG) { fillin_buffer<float>(point_buf_float, unit.size); }
					else if (unit.type_data == TypeData::DISCRETE) { fillin_buffer<int>(point_buf_int, unit.size); }
					else if (unit.type_data == TypeData::BINAR) { fillin_buffer<char>(point_buf_char, unit.size); }

					last = std::chrono::high_resolution_clock::now();
					current = std::chrono::high_resolution_clock::now();

					while (!stoper.stop_requested())
					{
						current = std::chrono::high_resolution_clock::now();
						time = std::chrono::duration_cast<std::chrono::milliseconds>(current - last);
						if (time.count() < unit.frequency) continue;
						last = std::chrono::high_resolution_clock::now();

						if (unit.type_data == TypeData::ANALOG) { update_buffer_sine<float>(point_buf_float, unit.size, 10000, unit.amplitude); }
						else if (unit.type_data == TypeData::DISCRETE) { update_buffer_sine<int>(point_buf_int, unit.size, 10000, unit.amplitude); }
						else if (unit.type_data == TypeData::BINAR) { update_buffer_sine<int>(point_buf_int, 10000, 0, unit.amplitude); }						
						
						res_write = shared_memory->WriteData(unit.type_data, buf.get(), unit.size, unit.point_name);
						if (res_write != 0)
						{
							helpstr.clear();
							helpstr = "Error WriteData : Publisher - " + unit.point_name;
							log->WriteLogERR(helpstr.c_str(), res_write, 0);
							return;
						}

						if (unit.show_console == ShowDataConsole::ON)
						{
							std::lock_guard<std::mutex> lock(guard_console);
							std::cout << " /// ***  Publication - " << unit.point_name << " *** ///" << std::endl;
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
				catch (...)
				{
					helpstr.clear();
					helpstr = "Error WriteData : Publisher - " + unit.point_name;
					log->WriteLogERR(helpstr.c_str(), 0, 0);
					return;
				}

				return;
			}
			void Simulator::thread_constant(UnitSimulation unit, std::stop_token stoper)
			{}
			void Simulator::thread_saw(UnitSimulation unit, std::stop_token stoper)
			{}

			void Simulator::thread_read(UnitSimulation unit, std::stop_token stoper)
			{
				std::chrono::milliseconds time;
				std::chrono::steady_clock::time_point last;
				std::chrono::steady_clock::time_point current;
				unsigned int size_type = 0;

				if (unit.type_data == TypeData::ANALOG || unit.type_data == TypeData::DISCRETE) size_type = 4;
				if (unit.type_data == TypeData::BINAR) size_type = 1;


				std::shared_ptr<char> buf(new char[unit.size * size_type], std::default_delete<char[]>());

				for (int i = 0; i < unit.size* size_type; i++)
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
								std::cout << "value[" << i << "] = " << *(point_buf_int  + i) << ";" << std::endl;
							}

							if (unit.type_data == TypeData::BINAR)
							{
								std::cout << "value[" << i << "] = " << *(point_buf_char + i) << ";" << std::endl;
							}
						}
					}
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
				std::chrono::microseconds time =
					std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock().now().time_since_epoch());

				float alfa = (time.count() % freq * 1.) / freq;

				for (int i = 0; i < size; i++)
				{
					*(buf + i) = Amp * sin(alfa * 2 * std::numbers::pi+2*std::numbers::pi/100);
				}
			}
		}		

	}
}