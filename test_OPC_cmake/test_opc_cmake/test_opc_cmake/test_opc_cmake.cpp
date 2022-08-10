// test_opc_cmake.cpp: определяет точку входа для приложения.
//

#include "test_opc_cmake.h"

using namespace std;

int main()
{
	cout << "Hello CMake." << endl;

    UA_Client* client = UA_Client_new();
    UA_ClientConfig_setDefault(UA_Client_getConfig(client));
    UA_StatusCode retval = UA_Client_connect(client, "opc.tcp://localhost:4840");
    if (retval != UA_STATUSCODE_GOOD) {
        UA_Client_delete(client);
        return (int)retval;
    }

	return 0;
}
