// CMakeProject1.cpp: определяет точку входа для приложения.
//

#include "CMakeProject1.h"
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>

using namespace std;
using namespace eprosima::fastdds::dds;
using namespace eprosima::fastrtps::types;
using namespace std::chrono_literals;

int main()
{
	cout << "Hello CMake." << endl;

	DomainParticipant* participant_pub;
	participant_pub = DomainParticipantFactory::get_instance()->create_participant(0, PARTICIPANT_QOS_DEFAULT);

	return 0;
}
