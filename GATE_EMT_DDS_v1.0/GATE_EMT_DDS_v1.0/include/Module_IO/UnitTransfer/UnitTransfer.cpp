#include "UnitTransfer.hpp"

namespace scada_ate::gate::adapter
{
	UnitTransfer::UnitTransfer()
	{
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

		log->Debug("UnitTransfer id-{}: Destroy", id);
	};

	void UnitTransfer::transfer_thread()
	{
		long long time_current;
		long long time_last;
		std::deque<std::future<ResultReqest>> deq_write;
		
		try
		{
			for (auto& it : _vec_frq)
			{
				it.time = TimeConverter::GetTime_LLmcs();
			};
			time_last = TimeConverter::GetTime_LLmcs();

			for (;;)
			{
				if (_command_thread_transfer.load() != 0) break;
				for (;;)
				{
					time_current = TimeConverter::GetTime_LLmcs();
					if (time_current - time_last > frq_transfer - mcs_off) break;
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
					log->Warning("UnitTransfer id - {}: Error recive data", id);
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
			log->Critical("UnitTransfer id - {}: Error thread transfer: error: {} syserror: {}", id, e, 0);
		}
		catch (...)
		{
			log->Critical("UnitTransfer id-{}: Error thread transfer: error: {} syserror: {}", id, 0, 0);
		}

		return;
	};

	void UnitTransfer::ctrl_adapter_thread()
	{
		std::deque<std::pair<uint32_t, atech::common::Status>> status;

		try
		{
			for (;;)
			{
				if (_command_thread_ctrl_adapters.load() != 0) break;
				status.clear();
				_adapter_source->GetStatus(status);
				if (status.begin()->second == atech::common::Status::ERROR_INIT ||
					status.begin()->second == atech::common::Status::ERR ||
					status.begin()->second == atech::common::Status::NONE ||
					status.begin()->second == atech::common::Status::ERROR_CONNECTING)
				{
					ResultReqest res = _adapter_source->ReInit(status);
					if (res == ResultReqest::ERR)
					{
						log->Critical("UnitTransfer id - {}: Error ReInit adapter source: error: {} syserror: {}", id, 0, 0);
					}
					else
					{
						log->Warning("UnitTransfer id - {}: ReInit adapter source: error: {} syserror: {}", id, 0, 0);
					}
				}

				for (auto& it : _adapters_target)
				{
					status.clear();
					it.second->GetStatus(status);
					if (status.begin()->second == atech::common::Status::ERROR_INIT ||
						status.begin()->second == atech::common::Status::NONE ||
						status.begin()->second == atech::common::Status::ERROR_CONNECTING)
					{
						ResultReqest res = it.second->ReInit(status);
						if (res == ResultReqest::ERR)
						{
							log->Warning("UnitTransfer id - {}: Error ReInit adapter target id-{}: error: {} syserror: {}", id, it.first, 0, 0);
						}
						else
						{
							log->Warning("UnitTransfer id - {}: ReInit adapter target id- ", id, it.first);
						}
					}
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			}
		}
		catch (int& e)
		{
			log->Critical("UnitTransfer id - {}: Error controll adapters: error: {} syserror: {}", id, e, 0);
		}
		catch (...)
		{
			log->Critical("UnitTransfer id-{}: Error controll adapters: error: {} syserror: {}", id, 0, 0);
		};
	}

	ResultReqest UnitTransfer::start_transfer()
	{
		ResultReqest result{ ResultReqest::OK };

		try
		{
			if (!_adapter_source) throw 1;
			if (_adapters_target.empty()) throw 2;
			if (frq_transfer < 1000) throw 3;


			if (!_thread_transfer.joinable())
			{
				_command_thread_transfer.store(0);
				_thread_transfer = std::thread(&UnitTransfer::transfer_thread, this);
			}

			if (!_thread_ctrl_adapters.joinable())
			{
				_command_thread_transfer.store(0);
				_thread_ctrl_adapters = std::thread(&UnitTransfer::ctrl_adapter_thread, this);
			}

			_status.store(atech::common::Status::OK);
		}
		catch (int& e)
		{
			log->Critical("UnitTransfer id - {}: Error StartTransfer: error: {} syserror: {}", id, e, 0);
			result = ResultReqest::ERR;
			_status.store(atech::common::Status::ERR);
		}
		catch (...)
		{
			log->Critical("UnitTransfer id-{}: Error StartTransfer: error: {} syserror: {}", id, 0, 0);
			result = ResultReqest::ERR;
			_status.store(atech::common::Status::ERR);
		}

		return result;
	}	

	ResultReqest UnitTransfer::stop_transfer()
	{
		ResultReqest result{ ResultReqest::OK };
		
		try 
		{
			if (_status.load() != atech::common::Status::OK)
			{
				return ResultReqest::ERR;
			}

			if (_thread_transfer.joinable())
			{
				_command_thread_transfer.store(1);
				_thread_transfer.join();
			}

			_status.store(atech::common::Status::STOP);
		}
		catch (int& e)
		{
			log->Critical("UnitTransfer id - {}: Error StopTransfer: error: {} syserror: {}", id, e, 0);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("UnitTransfer id-{}: Error StopTransfer: error: {} syserror: {}", id, 0, 0);
			result = ResultReqest::ERR;
		}

		return result;
	}

	uint32_t UnitTransfer::GetId()
	{
		return id;
	}

	ResultReqest UnitTransfer::GetStatus(std::deque<std::pair<uint32_t, atech::common::Status>>& st, uint32_t id)
	{
		ResultReqest result{ ResultReqest::OK };

		if (id == 0 || id == this->id)
		{
			_adapter_source->GetStatus(st);
			for (auto& it : _adapters_target)
			{
				it.second->GetStatus(st);
			}

			st.push_back({ this->id, _status.load() });
		}
		else if (id == _adapter_source->GetId())
		{
			_adapter_source->GetStatus(st);
		}
		else if ( _adapters_target.count(id) != 0)
		{
			_adapters_target[id]->GetStatus(st);
		}
		else
		{
			result = ResultReqest::IGNOR;
		}

		return result;
	}

	ResultReqest UnitTransfer::Start(std::deque<std::pair<uint32_t, atech::common::Status>>& st,uint32_t id)
	{
		ResultReqest result{ ResultReqest::OK };
		const std::lock_guard<std::mutex> lock(_guarder);

		if (id == 0 || id == this->id)
		{
			_adapter_source->Start(st);
			for (auto& it : _adapters_target)
			{
				it.second->Start(st);
			}			
			start_transfer();
			st.push_back({ this->id,_status.load() });

		}
		else if (_adapter_source->GetId() == id)
		{
			_adapter_source->Start(st);

			
		}
		else if (_adapters_target.count(id) != 0)
		{
			_adapters_target[id]->Start(st);
		}
		else
		{
			result = ResultReqest::IGNOR;
		}

		return result;
	};

	ResultReqest UnitTransfer::Stop(std::deque<std::pair<uint32_t, atech::common::Status>>& st, uint32_t id)
	{
		ResultReqest result{ ResultReqest::OK };
		const std::lock_guard<std::mutex> lock(_guarder);

		if (id == 0 || id == this->id)
		{
			stop_transfer();
			st.push_back({ this->id,_status.load() });
			_adapter_source->Stop(st);
			for (auto& it : _adapters_target)
			{
				it.second->Stop(st);
			}
		}
		else if (_adapter_source->GetId() == id)
		{
			_adapter_source->Stop(st);
		}
		else if (_adapters_target.count(id) != 0)
		{
			_adapters_target[id]->Stop(st);
		}
		else
		{
			result = ResultReqest::IGNOR;
		}

		return result;				    
	};

	ResultReqest UnitTransfer::ReInit(std::deque<std::pair<uint32_t, atech::common::Status>>& st, uint32_t id)
	{
		ResultReqest result{ ResultReqest::OK };
		const std::lock_guard<std::mutex> lock(_guarder);

		if (id == 0 || id == this->id)
		{
			_command_thread_ctrl_adapters.store(1);
			_command_thread_transfer.store(1);
			if (_thread_ctrl_adapters.joinable())
			{
				_thread_ctrl_adapters.join();
			}
			if (_thread_transfer.joinable())
			{
				_thread_transfer.join();
			}

			_adapter_source->ReInit(st);
			for (auto& it : _adapters_target)
			{
				it.second->ReInit(st);
			}

			start_transfer();
			st.push_back({ this->id,_status.load() });
		}
		else if (_adapter_source->GetId() == id) 
		{
			 _adapter_source->ReInit(st);
		}
		else if(_adapters_target.count(id) !=0)
		{
			_adapters_target[id]->ReInit(st);
		}
		else
		{
			result = ResultReqest::IGNOR;
		}

		return result;
	}





	BuilderUnitTransfer::BuilderUnitTransfer() 
	{
		log = LoggerSpaceScada::GetLoggerScada(LoggerSpaceScada::TypeLogger::SPDLOG);
	};

	BuilderUnitTransfer::~BuilderUnitTransfer() {};

	ResultReqest BuilderUnitTransfer::CreateUnit(int64_t id) 
	{
		ResultReqest result{ ResultReqest::OK };

		try
		{
			_unit = std::make_unique<UnitTransfer>();
			if (!_unit) throw 1;
			_unit->id = id;
		}
		catch (int& e)
		{
			log->Critical("BuilderUnitTransfer: Error create unit id-{}: error: {} syserror: {}", id, e, 0);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("BuilderUnitTransfer: Error create unit id-{}: error: {} syserror: {}", id, 0, 0);
			result = ResultReqest::ERR;
		}
		
		return result;
	};
	
	std::unique_ptr<UnitTransfer> BuilderUnitTransfer::GetUnit()
	{
		return std::move(_unit);

	};
	
	ResultReqest BuilderUnitTransfer::SetFrqTransfer(int64_t frq) 
	{
		ResultReqest result{ ResultReqest::OK };

		try
		{
			if (!_unit) throw 1;
			_unit->frq_transfer = frq*1000;
		}
		catch (int& e)
		{
			log->Critical("BuilderUnitTransfer: Error set frequency: error: {} syserror: {}", e, 0);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("BuilderUnitTransfer: Error set frequency: error: {} syserror: {}", 0, 0);
			result = ResultReqest::ERR;
		}

		return result;
	};

	ResultReqest BuilderUnitTransfer::SetAdapterSource(IConfigAdapter_ptr config)
	{
		ResultReqest result{ ResultReqest::OK };

		try
		{
			if (!_unit) throw 1;
			adapter::IAdapter_ptr adapter = adapter::CreateAdapter(config);
			if (!adapter) throw 2;
			adapter->InitAdapter();
			_unit->_adapter_source = adapter;
		}
		catch (int& e)
		{
			log->Critical("BuilderUnitTransfer: Error set adapter id-{}: error: {} syserror: {}", config->id_adapter, e, 0);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("BuilderUnitTransfer: Error set adapter id-{}: error: {} syserror: {}", config->id_adapter, 0, 0);
			result = ResultReqest::ERR;
		}

		return result;
	};

	ResultReqest BuilderUnitTransfer::AddAdapterTarget(IConfigAdapter_ptr config, int64_t frq) 
	{
		ResultReqest result{ ResultReqest::OK };

		try
		{
			if (!_unit) throw 1;
			adapter::IAdapter_ptr adapter = adapter::CreateAdapter(config);
			if (!adapter) throw 2;
			if (adapter->InitAdapter() != ResultReqest::OK)
			{
				log->Warning("BuilderUnitTransfer: Error initional adapter id-{}", config->id_adapter);
			};
			_unit->_adapters_target[config->id_adapter] = adapter;
			_unit->_vec_frq.push_back({ config->id_adapter, frq*1000, 0});
		}
		catch (int& e)
		{
			log->Critical("BuilderUnitTransfer: Error set adapter id-{}: error: {} syserror: {}", config->id_adapter, e, 0);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("BuilderUnitTransfer: Error set adapter id-{}: error: {} syserror: {}", config->id_adapter, 0, 0);
			result = ResultReqest::ERR;
		}

		return result;
	};
}
