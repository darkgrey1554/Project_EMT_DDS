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
	config_reader = new ConfigReaderDDS();

	if (config_reader->ReadConfigGate(config) == ResultReqest::ERR)
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