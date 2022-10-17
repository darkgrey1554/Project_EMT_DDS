#include <Module_IO/Module_IO.hpp>

namespace scada_ate::gate
{
	Module_IO::Module_IO()
	{
		log = LoggerSpaceScada::GetLoggerScada(LoggerSpaceScada::TypeLogger::SPDLOG);
	};

	Module_IO::~Module_IO()
	{
	};

	ResultReqest Module_IO::SetNodeID(int64_t id)
	{
		_node_id = id;
		return ResultReqest::OK;
	}

	ResultReqest Module_IO::ClearModule()
	{
		ResultReqest result{ ResultReqest::OK };

		try
		{
			_map_units.clear();
		}
		catch (int& e)
		{
			log->Critical("Module_IO node-{} : Error ClearModule : error: {} syserror: {}", _node_id, e, 0);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("Module_IO node-{} : Error ClearModule : error: {} syserror: {}", _node_id, 0, 0);
			result = ResultReqest::ERR;
		}

		return result;
	};

	ResultReqest Module_IO::AddUnit(const adapter::ConfigUnitTransfer& config)
	{
		ResultReqest result{ ResultReqest::OK };

		try
		{
			if (verification_config_unit(config) != ResultReqest::OK) throw 1;

			std::unique_ptr<adapter::BuilderUnitTransfer> builder = std::make_unique<adapter::BuilderUnitTransfer>();

			if (builder->CreateUnit(config.id) != ResultReqest::OK) throw 2;
			if (builder->SetFrqTransfer(config.frequency) != ResultReqest::OK) throw 3;

			auto& source = config.config_input_unit[0];
			source->vec_tags_source.clear();

			for (auto& map : config.mapping)
			{
				for (auto& tag : map.vec_links)
				{
					if (std::find(source->vec_tags_source.begin(), source->vec_tags_source.end(), tag.source) == source->vec_tags_source.end())
					{
						source->vec_tags_source.push_back(tag.source);
					}
				}
			}

			if (builder->SetAdapterSource(source) != ResultReqest::OK) throw 4;

			for (auto& target : config.config_output_unit)
			{
				int flag = 0;
				target->vec_link_tags.clear();
				for (auto& map : config.mapping)
				{
					if (map.id == target->id_map)
					{
						target->vec_link_tags = map.vec_links;
						builder->AddAdapterTarget(target, map.frequency);
						flag = 1;
						break;
					}
				}
				if (!flag)
				{
					log->Warning("BuilderUnitTransfer: missing mapping for adapter id-{}", target->id_adapter);
				}
			}

			_map_units.insert(std::pair{ config.id, builder->GetUnit() });
		}
		catch (int& e)
		{  
			log->Critical("Module_IO: Error AddUnit id-{} : error: {} syserror: {}", config.id, e, 0);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("Module_IO: Error AddUnit id-{} : error: {} syserror: {}", config.id, 0, 0);
			result = ResultReqest::ERR;
		}
					
		return result;
	};

	ResultReqest Module_IO::GetStatusUnit(adapter::StatusUnitTransfer& status, int64_t id)
	{
		ResultReqest result{ ResultReqest::OK };
		
		try
		{
			if (_map_units.count(id) == 0) return ResultReqest::IGNOR;
			status = _map_units[id]->GetStatus();
		}
		catch (int& e)
		{
			log->Critical("Module_IO node-{} : Error GetStatusUnit id-{} : error: {} syserror: {}", _node_id, id, e, 0);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("Module_IO node-{} : Error GetStatusUnit id-{} : error: {} syserror: {}", _node_id, id, 0, 0);
			result = ResultReqest::ERR;
		}
		
		return result;
	};

	ResultReqest Module_IO::RemoveUnit(int64_t id)
	{
		ResultReqest result{ ResultReqest::OK };

		try 
		{
			if (_map_units.count(id) == 0) return ResultReqest::IGNOR;
			_map_units.erase(id);
		}
		catch (int& e)
		{
			log->Critical("Module_IO node-{} : Error RemoveUnit id-{} : error: {} syserror: {}", _node_id, id, e, 0);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("Module_IO node-{} : Error RemoveUnit id-{} : error: {} syserror: {}", _node_id, id, 0, 0);
			result = ResultReqest::ERR;
		}

		return result;
	};

	ResultReqest Module_IO::verification_config_unit(const adapter::ConfigUnitTransfer& config)
	{
		ResultReqest result{ ResultReqest::OK };

		try
		{
			if (_map_units.count(config.id) != 0) throw 1;
			if (config.frequency < 0) throw 2;
			if (config.config_input_unit.empty()) throw 3;
			if (config.config_output_unit.empty()) throw 4;

			for (auto& it : config.config_input_unit)
			{
				if (!it) throw 5;
			}

			for (auto& it : config.config_output_unit)
			{
				if (!it) throw 6;
			}

			if (config.mapping.empty()) throw 6;
		}
		catch (int& e)
		{
			log->Critical("Module_IO node-{}: Error verification unit id-{} : error: {} syserror:{}", _node_id, config.id, e, 0);
			result = ResultReqest::ERR;
		}
		catch (...)
		{
			log->Critical("Module_IO node-{}: Error verification unit id-{} : error: {} syserror:{}", _node_id, config.id, 0, 0);
			result = ResultReqest::ERR;
		}

		return result;
	}

	ResultReqest Module_IO::StartUnit(int64_t id)
	{
		if (_map_units.count(id) == 0) return ResultReqest::IGNOR;
		return _map_units[id]->StartTransfer();
	};

	ResultReqest Module_IO::StopUnit(int64_t id)
	{
		if (_map_units.count(id) == 0) return ResultReqest::IGNOR;
		return _map_units[id]->StopTransfer();
	};
	
	ResultReqest Module_IO::ReinitUnit(int64_t id)
	{
		return ResultReqest::IGNOR;
	}
}