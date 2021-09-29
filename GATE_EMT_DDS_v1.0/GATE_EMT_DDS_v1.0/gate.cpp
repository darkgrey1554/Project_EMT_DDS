#include "gate.h"

Gate* CreateGate(Type_Gate type)
{
	Gate* gate = nullptr;
	if (type == Type_Gate::DDS) gate = new DDS_Gate();
	return gate;
};

DDS_Gate::DDS_Gate()
{
	log = LoggerSpace::Logger::getpointcontact();
};

DDS_Gate::~DDS_Gate()
{

};

ResultReqest DDS_Gate::init_gate()
{
	ResultReqest result = ResultReqest::OK;
	DomainParticipantQos participantQos;
	config_reader = new ConfigReader();

	if (config_reader->ReadConfigGATE(config) == ResultReqest::ERR)
	{
		result = ResultReqest::ERR;
		return result;
	}

	participantQos.name("Participant_subscriber");

	/// --- Config Transport Layer --- ///
	///                                ///
	//////////////////////////////////////

	participant_ = DomainParticipantFactory::get_instance()->create_participant(config.Domen, participantQos, nullptr);

	if (participant_ == nullptr)
	{
		log->WriteLogERR("ERROR CREATE PARTICIPANT", 0, 0);
		result = ResultReqest::ERR;
		return result;
	}
	
};


DDS_Gate::SubListener::SubListener() {};
DDS_Gate::SubListener::~SubListener() {};

void DDS_Gate::SubListener::on_subscription_matched(DataReader*, const SubscriptionMatchedStatus& info) {};
void DDS_Gate::SubListener::on_data_available(DataReader* reader) {};

void DDS_Gate::GetInfoAboutGate() {};
void DDS_Gate::GetStatusGate() {};
void DDS_Gate::StartGate() {};
void DDS_Gate::StopGate() {};
void DDS_Gate::RestartGate() {};
void DDS_Gate::RebornGate() {};

void DDS_Gate::TakeConfigUnits() {};
void DDS_Gate::WriteConfigUnitsInFile() {};
void DDS_Gate::ParseCommand() {};
void DDS_Gate::AnswerCommand() {};