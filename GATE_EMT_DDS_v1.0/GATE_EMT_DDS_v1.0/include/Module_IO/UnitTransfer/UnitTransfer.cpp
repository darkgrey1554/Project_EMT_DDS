#include "UnitTransfer.h"

namespace scada_ate::gate::adapter
{
	UnitTransfer::UnitTransfer(ConfigUnitTransfer& config)
	{
		this->config = config;
		log = LoggerSpaceScada::GetLoggerScada(LoggerSpaceScada::TypeLogger::SPDLOG);
	}

	UnitTransfer::~UnitTransfer()
	{
		if (_thread_transfer.joinable())
		{
			_command_thread_transfer.store(1);
			_thread_transfer.join();
		};

		if (_thread_ctrl_adapters.joinable())
		{
			_command_thread_ctrl_adapters.store(1);
			_thread_ctrl_adapters.join();
		};
	};

	ResultReqest UnitTransfer::InitUnit()
	{
		ResultReqest result{ ResultReqest::OK };

		const std::lock_guard<std::mutex> lock_init(mutex_init);

		StatusUnitTransfer status = _status.load(std::memory_order::memory_order_relaxed);
		if (status == StatusUnitTransfer::INITIALIZATION || status == StatusUnitTransfer::OK)
		{
			ResultReqest res = ResultReqest::IGNOR;
			return res;
		}

		try
		{
			if (update_mapping_input_adapter() != ResultReqest::OK) throw 1;
			if (update_mapping_output_adapter() != ResultReqest::OK) throw 2;

			_adapter_source = adapter::CreateAdapter(config.config_input_unit[0]);
			if (_adapter_source == nullptr) throw 3;
			if (_adapter_source->InitAdapter() != ResultReqest::OK) throw 4;
			
			_adapters_target.clear();
			for (auto& conf : config.config_output_unit)
			{
				_adapters_target[conf.get()->id_adapter] = adapter::CreateAdapter(conf);
				if (_adapters_target[conf.get()->id_adapter]->InitAdapter() != ResultReqest::ERR)
				{
					log->Warning("UnitTransfer id - {}: Error Initialization adapter: id - ", config.id, conf.get()->id_adapter);
				};
			}

			try
			{
				_thread_transfer = std::thread(&UnitTransfer::_thread_transfer, this);
				log->Debug("UnitTransfer id - {}: Start transfer thread", config.id);
			}
			catch (...)
			{
				throw 5;
			}

			try
			{
				_thread_ctrl_adapters = std::thread(&UnitTransfer::ctrl_adapter_thread, this);
				log->Debug("UnitTransfer id - {}: Start transfer control", config.id);
			}
			catch (...)
			{
				log->Warning("UnitTransfer id - {}: Error start transfer control: error: {} syserror: {}", config.id,0,0);
			}
			
			log->Debug("UnitTransfer id - {}: Initialization done", config.id);
			_status.store(StatusUnitTransfer::OK);
		}
		catch (int& e)
		{
			log->Critical("UnitTransfer id - {}: Error Initialization: error: {} syserror: {}", config.id, e, 0);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("UnitTransfer id-{}: Error Initialization: error: {} syserror: {}", config.id, 0, 0);
			result = ResultReqest::ERR;
		}

		return result;
	}

	ResultReqest UnitTransfer::update_mapping_input_adapter()
	{
		ResultReqest result{ ResultReqest::OK };

		try
		{
			IConfigAdapter* source = config.config_input_unit[0].get();
			source->vec_tags_source.clear();
			
			size_t size_maps = 0;
			for (auto& map : config.mapping)
			{
				size_maps += map.vec_links.size();
			};
			source->vec_tags_source.reserve(size_maps);

			for (auto& map : config.mapping)
			{
				for (auto& tag : map.vec_links)
				{
					if (std::find(source->vec_tags_source.begin(), source->vec_tags_source.end(), tag.source) == source->vec_tags_source.end())
					{
						source->vec_tags_source.push_back(tag.source);
					}
				}
			};
		}
		catch (int& e)
		{
			log->Critical("UnitTransfer id - {}: Error update_mapping_input_adapter: error: {} syserror: {}", config.id, e, 0);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("UnitTransfer id-{}: Error update_mapping_input_adapter: error: {} syserror: {}", config.id, 0, 0);
			result = ResultReqest::ERR;
		}

		return result;
	};

	ResultReqest UnitTransfer::update_mapping_output_adapter()
	{
		ResultReqest result{ ResultReqest::OK };

		try
		{
			_vec_frq.clear();
			_vec_frq.reserve(config.config_output_unit.size());

			for (auto& target : config.config_output_unit)
			{
				IConfigAdapter* adapter = target.get();
				adapter->vec_link_tags.clear();
				bool flag_is = false;
				for (auto& map : config.mapping)
				{
					if (adapter->id_map == map.id)
					{
						adapter->vec_link_tags = map.vec_links;
						flag_is = true;
						_vec_frq.push_back({ adapter->id_adapter, map.frequency, 0 });
						break;
					}
				}
				if (!flag_is)
				{
					log->Debug("UnitTransfer id - {}: Error update_mapping_output_adapter: mapping dont detected: adapter id - {}", config.id, adapter->id_adapter);
				}
			}
		}
		catch (int& e)
		{
			log->Critical("UnitTransfer id - {}: Error update_mapping_output_adapter: error: {} syserror: {}", config.id, e, 0);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("UnitTransfer id-{}: Error update_mapping_output_adapter: error: {} syserror: {}", config.id, 0, 0);
			result = ResultReqest::ERR;
		}

		return result;
	};

	void UnitTransfer::transfer_thread()
	{
		long long ftime_thread = 0;
		long long time_current;
		long long time_last;
		std::deque<std::future<ResultReqest>> deq_write;
		
		try
		{
			for (auto& it : _vec_frq)
			{
				it.time = TimeConverter::GetTime_LLmcs();
			};
			ftime_thread = config.frequency;
			time_last = TimeConverter::GetTime_LLmcs();

			for (;;)
			{
				if (_command_thread_transfer.load() != 0) break;
				for (;;)
				{
					time_current = TimeConverter::GetTime_LLmcs();
					if (time_current - time_last > ftime_thread - mcs_off) break;
					std::this_thread::sleep_for(std::chrono::milliseconds(5));
				}
				time_last = time_current;

				_queue_transfer.clear();
				for (auto& it : _vec_frq)
				{
					if (time_last - it.time > it.frq - mcs_off)
					{
						_queue_transfer.push_back(it.id);
						it.time = time_last;
					}
				}

				if (_queue_transfer.empty()) continue;

				if (_adapter_source->ReadData(&_data) != ResultReqest::OK)
				{
					log->Warning("UnitTransfer id - {}: Error recive data");
					continue;
				}

				while (!_queue_transfer.empty())
				{
					int& id_target = _queue_transfer.front();
					deq_write.push_back(std::async(std::launch::async, &adapter::IAdapter::WriteData, _adapters_target[id_target].get(), *_data));
					_queue_transfer.pop_front();
				}
				deq_write.clear();
			};
		}
		catch (int& e)
		{
			log->Critical("UnitTransfer id - {}: Error Initialization: error: {} syserror: {}", config.id, e, 0);
		}
		catch (...)
		{
			log->Critical("UnitTransfer id-{}: Error Initialization: error: {} syserror: {}", config.id, 0, 0);
		}

		return;
	};

	void UnitTransfer::ctrl_adapter_thread()
	{
		StatusAdapter _status;
		try
		{
			for (;;)
			{
				if (_command_thread_ctrl_adapters.load() != 0) break;

				_status = _adapter_source->GetStatusAdapter();
				if (_status == StatusAdapter::ERROR_INIT ||
					_status == StatusAdapter::Null ||
					_status == StatusAdapter::CRASH)
				{
					ResultReqest res = _adapter_source->InitAdapter();
					if (res == ResultReqest::ERR)
					{
						log->Critical("UnitTransfer id - {}: Error Init adapter source: error: {} syserror: {}", config.id, 0, 0);
					}
					else
					{
						log->Warning("UnitTransfer id - {}: Init adapter source: error: {} syserror: {}", config.id, 0, 0);
					}
				}

				for (auto& it : _adapters_target)
				{
					_status = it.second->GetStatusAdapter();
					if (_status == StatusAdapter::ERROR_INIT ||
						_status == StatusAdapter::Null ||
						_status == StatusAdapter::CRASH)
					{
						ResultReqest res = it.second->InitAdapter();
						if (res == ResultReqest::ERR)
						{
							log->Warning("UnitTransfer id - {}: Error Init adapter target id-{}: error: {} syserror: {}", config.id, it.first, 0, 0);
						}
						else
						{
							log->Warning("UnitTransfer id - {}: Init adapter target id- ", config.id, it.first);
						}
					}
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			}
		}
		catch (int& e)
		{
			log->Critical("UnitTransfer id - {}: Error controll adapters: error: {} syserror: {}", config.id, e, 0);
		}
		catch (...)
		{
			log->Critical("UnitTransfer id-{}: Error controll adapters: error: {} syserror: {}", config.id, 0, 0);
		};
	}

	ResultReqest UnitTransfer::StartTransfer()
	{
		ResultReqest result{ ResultReqest::OK };

		try
		{
			if (_status.load() != StatusUnitTransfer::OK)
			{
				return ResultReqest::IGNOR;
			}

			if (!_thread_transfer.joinable())
			{
				_command_thread_transfer.store(0);
				_thread_transfer = std::thread(&UnitTransfer::transfer_thread, this);
			}
		}
		catch (int& e)
		{
			log->Critical("UnitTransfer id - {}: Error StartTransfer: error: {} syserror: {}", config.id, e, 0);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("UnitTransfer id-{}: Error StartTransfer: error: {} syserror: {}", config.id, 0, 0);
			result = ResultReqest::ERR;
		}

		return result;
	}	

	ResultReqest UnitTransfer::StopTransfer()
	{
		ResultReqest result{ ResultReqest::OK };
		
		try 
		{
			if (_status.load() != StatusUnitTransfer::OK)
			{
				return ResultReqest::IGNOR;
			}

			if (_thread_transfer.joinable())
			{
				_command_thread_transfer.store(1);
				_thread_transfer.join();
			}
		}
		catch (int& e)
		{
			log->Critical("UnitTransfer id - {}: Error StopTransfer: error: {} syserror: {}", config.id, e, 0);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("UnitTransfer id-{}: Error StopTransfer: error: {} syserror: {}", config.id, 0, 0);
			result = ResultReqest::ERR;
		}

		return result;
	}

	ResultReqest UnitTransfer::MountAdapterSource(scada_ate::gate::adapter::IConfigAdapter& config) 
	{
		return ResultReqest::IGNOR;
	}
	
	ResultReqest UnitTransfer::AddAdapterTarget(const scada_ate::gate::adapter::IConfigAdapter& config, const Mapping& mapping)
	{
		return ResultReqest::IGNOR;
	}
}
