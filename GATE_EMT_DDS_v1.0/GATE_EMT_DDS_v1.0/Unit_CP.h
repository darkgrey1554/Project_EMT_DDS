#pragma once
#include "ModuleIO.h"
#include "struct_unit_cp.h"

namespace scada_ate
{
	namespace controller_module_io
	{
		class Unit_CP
		{

		protected:
			std::weak_ptr< module_io::Module_IO > object_control;
			std::atomic<StatusUnitCP> status = StatusUnitCP::EMPTY;
			static std::atomic<GlobalStatus> global_status;
			void SetGlobalStatus(GlobalStatus value);
			virtual void SetCurrentStatus(StatusUnitCP value) = 0;
			friend GlobalStatus GetGlobalStatus();

		public:
			virtual ~Unit_CP() {};
			virtual TypeUnitCP GetTypeUnitCP() = 0;
			virtual StatusUnitCP GetCurrentStatus() = 0;
			virtual ResultReqest InitUnitCP(std::shared_ptr<ConfigUnitCP> config, std::shared_ptr<Module_IO>) = 0;
		};

		std::shared_ptr<Unit_CP> CreateUnit_CP(TypeUnitCP type);

		std::atomic<GlobalStatus> Unit_CP::global_status(GlobalStatus::WORK);
		GlobalStatus GetGlobalStatus();
	}
	
}