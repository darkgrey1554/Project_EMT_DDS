#pragma once
#include "adapter_EmtScadaAte.hpp"
#include "config_reader_simulater.hpp"
#include <thread>
#include "structs.hpp"
#include "logger.h"
#include <math.h>
#include <numbers>

namespace scada_ate
{
	namespace emt
	{
		namespace simulator
		{
			class Simulator
			{
				std::vector<std::jthread> vector_threads_simulation;
				std::mutex guard_vector;
				std::stop_source stoper;
				std::shared_ptr<AdapterEMTScada> shared_memory = nullptr;
				std::mutex guard_console;
				LoggerSpace::Logger* log;

				void thread_triangle(UnitSimulation unit, std::stop_token stoper);
				void thread_sine(UnitSimulation unit, std::stop_token stoper);
				void thread_constant(UnitSimulation unit, std::stop_token stoper);
				void thread_saw(UnitSimulation unit, std::stop_token stoper);
				void thread_read(UnitSimulation unit, std::stop_token stoper);
				template <typename T> void fillin_buffer(T* buf, unsigned int size);
				template <typename T> void fillin_buffer(T* buf, unsigned int size, T value);
				template <typename T> void update_buffer_triangle(T* buf, unsigned int size, int* k, T step, unsigned int Amp);
				template <typename T> void update_buffer_sine(T* buf, unsigned int size, unsigned int freq, unsigned int Amp);

			public:

				Simulator();
				~Simulator();
				ResultRequest add_simulator(UnitSimulation unit);
			};
		}		
	}
}