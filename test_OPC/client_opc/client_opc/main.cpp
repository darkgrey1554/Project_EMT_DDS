#include <iostream>

#include <open62541/client_config_default.h>
#include <open62541/client_highlevel.h>
#include <open62541/plugin/log_stdout.h>
#include <open62541/plugin/securitypolicy.h>
#include <open62541/plugin/securitypolicy_default.h>
#include <open62541/plugin/pki_default.h>
#include <stdlib.h>
#include "TimeConverter.hpp"

/* loadFile parses the certificate file.
 *
 * @param  path               specifies the file name given in argv[]
 * @return Returns the file content after parsing */
static UA_INLINE UA_ByteString
loadFile(const char* const path) {
	UA_ByteString fileContents = UA_STRING_NULL;

	/* Open the file */
	FILE* fp = fopen(path, "rb");
	if (!fp) {
		errno = 0; /* We read errno also from the tcp layer... */
		return fileContents;
	}

	/* Get the file length, allocate the data and read */
	fseek(fp, 0, SEEK_END);
	fileContents.length = (size_t)ftell(fp);
	fileContents.data = (UA_Byte*)UA_malloc(fileContents.length * sizeof(UA_Byte));
	if (fileContents.data) {
		fseek(fp, 0, SEEK_SET);
		size_t read = fread(fileContents.data, sizeof(UA_Byte), fileContents.length, fp);
		if (read != fileContents.length)
			UA_ByteString_clear(&fileContents);
	}
	else {
		fileContents.length = 0;
	}
	fclose(fp);

	return fileContents;
}


int test_connect_server();
int test_browse_data_v1();
int test_read_variable();
int test_read_request_sec();
int test_request_write();

class A
{
	public:
	A() { std::cout << "A" << std::endl; };
	A(A&) { std::cout << "A&" << std::endl; };
	A(A&&) { std::cout << "A&&" << std::endl; };
	~A() { std::cout << "~A" << std::endl; };
};

class  C
{
public:
	C() { std::cout << "C" << std::endl; };
	C(C&) { std::cout << "C&" << std::endl; };
	C(C&&) { std::cout << "C&&" << std::endl; };
	~C() { std::cout << "~C" << std::endl; };
};

C f(A a)
{
	C b;
	return std::move(b);
}

int main()
{
	
	test_request_write();

	return 0;
}	


int test_connect_server()
{
	std::cout << "TEST CONNECTION" << std::endl;

	UA_Client* client = UA_Client_new();
	UA_ClientConfig_setDefault(UA_Client_getConfig(client));

	UA_StatusCode retval = UA_Client_connect(client, "opc.tcp://LAPTOP-SMDMIQSL:53530/OPCUA/SimulationServer");
	if (retval != UA_STATUSCODE_GOOD) {
		UA_Client_delete(client);
		return (int)retval;
	}

	std::cout << "CONNECT DONE" << std::endl;

	return 0;
}

int test_browse_data_v1()
{
	std::cout << "test" << std::endl;

	UA_Client* client = UA_Client_new();
	UA_ClientConfig_setDefault(UA_Client_getConfig(client));

	UA_StatusCode retval = UA_Client_connect(client, "opc.tcp://LAPTOP-SMDMIQSL:53530/OPCUA/SimulationServer");
	if (retval != UA_STATUSCODE_GOOD) {
		UA_Client_delete(client);
		return (int)retval;
	}

	std::cout << "CONNECT DONE" << std::endl;

	std::cout << "Browsing nodes in objects folder:\n";

	printf("%-9s %-16s %-16s %-16s\n", "NAMESPACE", "NODEID", "BROWSE NAME", "DISPLAY NAME");
	UA_BrowseRequest bReq;
	UA_BrowseRequest_init(&bReq);
	bReq.requestedMaxReferencesPerNode = 0;
	bReq.nodesToBrowse = UA_BrowseDescription_new();
	bReq.nodesToBrowseSize = 1;
	bReq.nodesToBrowse[0].nodeId = UA_NODEID_STRING_ALLOC(3,"85/0:Simulation"); /* browse objects folder */
	bReq.nodesToBrowse[0].resultMask = UA_BROWSERESULTMASK_ALL; /* return everything */
	UA_BrowseResponse bResp = UA_Client_Service_browse(client, bReq);

	for (size_t i = 0; i < bResp.resultsSize; ++i) {
		for (size_t j = 0; j < bResp.results[i].referencesSize; ++j) {
			UA_ReferenceDescription* ref = &(bResp.results[i].references[j]);
			if (ref->nodeId.nodeId.identifierType == UA_NODEIDTYPE_NUMERIC) {
				printf("%-9d %-16d %-16.*s %-16.*s\n", ref->nodeId.nodeId.namespaceIndex,
					ref->nodeId.nodeId.identifier.numeric, (int)ref->browseName.name.length,
					ref->browseName.name.data, (int)ref->displayName.text.length,
					ref->displayName.text.data);
			}
			else if (ref->nodeId.nodeId.identifierType == UA_NODEIDTYPE_STRING) {
				printf("%-9d %-16.*s %-16.*s %-16.*s\n", ref->nodeId.nodeId.namespaceIndex,
					(int)ref->nodeId.nodeId.identifier.string.length,
					ref->nodeId.nodeId.identifier.string.data,
					(int)ref->browseName.name.length, ref->browseName.name.data,
					(int)ref->displayName.text.length, ref->displayName.text.data);
			}
			/* TODO: distinguish further types */
		}
	}

	for (size_t i = 0; i < bResp.resultsSize; ++i) 
	{
		for (size_t j = 0; j < bResp.results[i].referencesSize; ++j)
		{
			UA_ReferenceDescription* ref = &(bResp.results[i].references[j]);			
			std::cout << ref->isForward << std::endl;
		}
	}

	UA_BrowseRequest_clear(&bReq);
	UA_BrowseResponse_clear(&bResp);
	UA_Client_delete(client);

	return 0;
}

int test_read_variable()
{
	std::cout << "TEST READ VARIABLE" << std::endl;

	UA_Client* client = UA_Client_new();
	UA_ClientConfig_setDefault(UA_Client_getConfig(client));

	UA_StatusCode retval = UA_Client_connect(client, "opc.tcp://LAPTOP-SMDMIQSL:53530/OPCUA/SimulationServer");
	if (retval != UA_STATUSCODE_GOOD) {
		UA_Client_delete(client);
		return (int)retval;
	}

	UA_Variant value;
	UA_Variant_init(&value);

	/* NodeId of the variable holding the current time */
	const UA_NodeId nodeId = UA_NODEID_NUMERIC(3, 1001);
	retval = UA_Client_readValueAttribute(client, nodeId, &value);

	if (retval == UA_STATUSCODE_GOOD &&
		UA_Variant_hasArrayType(&value, &UA_TYPES[UA_TYPES_INT32])) {
		for (int i = 0; i < value.arrayLength; i++)
		{
			std::cout << "data[" << i << "]=" << ((int*)value.data)[i] << std::endl;
		}
	}

	/* Clean up */
	UA_Variant_clear(&value);
	UA_Client_delete(client); /* Disconnects the client internally */

	std::cout << "CONNECT DONE" << std::endl;
}

int test_read_request_sec()
{
	std::cout << "TEST READ VARIABLE" << std::endl;

	
	UA_ByteString certificate = loadFile("client_cert.der");
	UA_ByteString privateKey = loadFile("client_key.der");
	size_t trustListSize = 1;

	//UA_ByteString* trustList;
	UA_STACKARRAY(UA_ByteString, trustList, trustListSize);
	trustList[0] = loadFile("SimulationServer@LAPTOP-SMDMIQSL_2048.der");
	UA_ByteString* revocationList = NULL;
	size_t revocationListSize = 0;

	UA_Client* client = UA_Client_new();
	UA_ClientConfig* cc = UA_Client_getConfig(client);
	cc->securityMode = UA_MESSAGESECURITYMODE_SIGNANDENCRYPT; /* require encryption */
	cc->securityPolicyUri = UA_STRING_ALLOC("http://opcfoundation.org/UA/SecurityPolicy#Basic256");
	UA_StatusCode retVal = UA_ClientConfig_setDefaultEncryption(cc, certificate, privateKey,
		trustList, trustListSize,
		revocationList, revocationListSize);

	//UA_SecurityPolicy_Basic256(cc->securityPolicies, certificate, privateKey,&cc->logger);
	UA_SecurityPolicy_None(cc->securityPolicies, certificate, &cc->logger);

	cc->clientDescription.applicationUri = UA_STRING_ALLOC("urn:open62541.client.application");

	UA_ByteString_clear(&certificate);
	UA_ByteString_clear(&privateKey);
	UA_ByteString_clear(&trustList[0]);

	cc->securityMode = UA_MESSAGESECURITYMODE_SIGNANDENCRYPT;

	UA_StatusCode retval = UA_Client_connect(client, "opc.tcp://LAPTOP-SMDMIQSL:53530/OPCUA/SimulationServer");
	if (retval != UA_STATUSCODE_GOOD) {
		UA_Client_delete(client);
		return (int)retval;
	}

	UA_ReadRequest _request;
	UA_ReadResponse _respone;
	UA_ReadRequest_init(&_request);
	UA_String out = UA_STRING_NULL;

	UA_ReadValueId vv[7];

	for (auto& v : vv)
	{
		UA_ReadValueId_init(&v);
	}

	vv[0].nodeId = UA_NODEID_NUMERIC(3, 1001);
	vv[0].attributeId = UA_ATTRIBUTEID_VALUE;
	vv[1].nodeId = UA_NODEID_NUMERIC(3, 1002);
	vv[1].attributeId = UA_ATTRIBUTEID_VALUE;
	vv[2].nodeId = UA_NODEID_NUMERIC(3, 1003);
	vv[2].attributeId = UA_ATTRIBUTEID_VALUE;
	vv[3].nodeId = UA_NODEID_NUMERIC(3, 1004);
	vv[3].attributeId = UA_ATTRIBUTEID_VALUE;
	vv[4].nodeId = UA_NODEID_NUMERIC(3, 1005);
	vv[4].attributeId = UA_ATTRIBUTEID_VALUE;
	vv[5].nodeId = UA_NODEID_NUMERIC(3, 1006);
	vv[5].attributeId = UA_ATTRIBUTEID_VALUE;
	vv[6].nodeId = UA_NODEID_NUMERIC(3, 1010);
	vv[6].attributeId = UA_ATTRIBUTEID_VALUE;

	_request.requestHeader.timestamp = UA_DateTime_now();
	_request.nodesToRead = vv;
	_request.nodesToReadSize = 7;
	

	UA_print(&_request, &UA_TYPES[UA_TYPES_READREQUEST], &out);
	printf("%.*s\n", (int)out.length, out.data);
	UA_String_clear(&out);

	_respone = UA_Client_Service_read(client, _request);
	
	(_respone.results + 5)->value;
	(_respone.results + 6)->value;

	UA_print(&_respone, &UA_TYPES[UA_TYPES_READRESPONSE], &out);
	printf("%.*s\n", (int)out.length, out.data);
	UA_String_clear(&out);

	Sleep(20000);


	/* Clean up */

	//UA_ReadResponse_clear(&_respone);
	//UA_ReadResponse_delete (&_respone);
	//UA_ReadRequest_clear(&_request);
	//UA_ReadRequest_delete(&_request);
	UA_Client_delete(client); /* Disconnects the client internally */

	std::cout << "CONNECT DONE" << std::endl;
}

int test_request_write()
{
	std::cout << "TEST REQUEST WTITE" << std::endl;

	UA_Client* client = UA_Client_new();
	UA_ClientConfig_setDefault(UA_Client_getConfig(client));

	UA_StatusCode retval = UA_Client_connect(client, "opc.tcp://LAPTOP-SMDMIQSL:53530/OPCUA/SimulationServer");
	if (retval != UA_STATUSCODE_GOOD) {
		UA_Client_delete(client);
		return (int)retval;
	}

	UA_WriteRequest _request;
	UA_WriteResponse _respone;
	UA_WriteRequest_init(&_request);
	int _value_1 = 10;
	int _value_2 = 100;

	UA_WriteValue* vv = (UA_WriteValue*)UA_Array_new(2, &UA_TYPES[UA_TYPES_WRITEVALUE]);

	UA_WriteValue_init(vv);
	UA_WriteValue_init(vv+1);

	_request.nodesToWriteSize = 2;
	_request.nodesToWrite = vv;

	_request.nodesToWrite[0].nodeId = UA_NODEID_NUMERIC(3, 1007);
	_request.nodesToWrite[0].attributeId = UA_ATTRIBUTEID_VALUE;
	_request.nodesToWrite[0].value.hasValue = true;
	_request.nodesToWrite[0].value.value.type = &UA_TYPES[UA_TYPES_INT32];
	_request.nodesToWrite[0].value.value.storageType = UA_VARIANT_DATA_NODELETE;
	_request.nodesToWrite[0].value.value.data = &_value_1;
	_request.nodesToWrite[0].value.hasSourceTimestamp = true;

	_request.nodesToWrite[1].nodeId = UA_NODEID_NUMERIC(3, 1020);
	_request.nodesToWrite[1].attributeId = UA_ATTRIBUTEID_VALUE;
	_request.nodesToWrite[1].value.hasValue = true;
	_request.nodesToWrite[1].value.value.type = &UA_TYPES[UA_TYPES_INT32];
	_request.nodesToWrite[1].value.value.storageType = UA_VARIANT_DATA_NODELETE;
	_request.nodesToWrite[1].value.value.data = &_value_2;

	
	

	for (;;)
	{
		_value_1++;
		_value_2++;
		_request.nodesToWrite[0].value.sourceTimestamp = TimeConverter::GetTime_LLmcs() * 10; //UA_DATETIME_UNIX_EPOCH + TimeConverter::GetTime_LLmcs()*10;
		UA_DateTime();
		//_request.nodesToWrite[0].value.serverTimestamp = 0;
		_respone = UA_Client_Service_write(client, _request);
		for (int i = 0; i < _respone.resultsSize; i++)
		{
			if (_respone.results[i] == UA_STATUSCODE_GOOD)
			{
				std::cout << "GOOD" << std::endl;
			}
			else
			{
				std::cout << "FALSE" << std::endl;
			}  			
		}
		if (_value_1 > 20) break;
		Sleep(1000);
	}

	UA_WriteRequest_clear(&_request);
	UA_WriteResponse_clear(&_respone);

	UA_Client_disconnect(client);
	UA_Client_delete(client);

	return 0;
}