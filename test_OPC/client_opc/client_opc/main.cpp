#include <iostream>

#include <open62541/client_config_default.h>
#include <open62541/client_highlevel.h>
#include <open62541/plugin/log_stdout.h>
#include <open62541/plugin/securitypolicy.h>
#include <open62541/plugin/securitypolicy_default.h>
#include <open62541/plugin/pki_default.h>
#include <stdlib.h>
#include "TimeConverter.hpp"
#include <vector>
#include <algorithm>
#include <numeric>
#include <string>  
#include <variant>
#include<fstream>
#include <sstream>
#include <thread>


/* loadFile parses the certificate file.
 *
 * @param  path               specifies the file name given in argv[]
 * @return Returns the file content after parsing */
UA_ByteString
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

UA_ByteString
loadFile_new(const char* const path) {

	UA_ByteString fileContents = UA_STRING_NULL;
	std::ifstream file;
	file.open(path, std::ios::binary | std::ios::ate);
	if (!file.is_open())
	{
		return fileContents;
	}

	fileContents.length = file.tellg();
	fileContents.data = (UA_Byte*)UA_malloc(fileContents.length * sizeof(UA_Byte));
	if(fileContents.data) {
		file.seekg(0);
		file.read((char*)fileContents.data, fileContents.length);
	}
	else 
	{
		fileContents.length = 0;
	}

	file.close();

	return fileContents;
}


int test_connect_server();
int test_browse_data_v1();
int test_read_variable();
int test_read_request_sec();
int test_request_write();
int test_request_read_mass1000_();
int test_request_read_mass1000_onetoone();
int test_request_write_mass1000_onetoone();
int test_write_next_read();
int check_connent_session();
int test_request_write_mass100_andvaleu3();

int test_request_write_scalar();


int main()
{	
	//test_read_request_sec();
	test_request_write();
	//test_request_read_mass1000_();
	//check_connent_session();
	//test_request_read_mass1000_onetoone();
	//test_request_write_mass1000_onetoone();
	//test_request_write_mass100_andvaleu3();
	//test_request_write_scalar();
	//test_write_next_read();
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
	trustList[0] = loadFile("uagateway.der");
	//trustList[0] = loadFile("SimulationServer@LAPTOP-SMDMIQSL_2048.der");
	UA_ByteString* revocationList = NULL;
	size_t revocationListSize = 0;

	UA_Client* client = UA_Client_new();
	UA_ClientConfig* cc = UA_Client_getConfig(client);
	cc->securityMode = UA_MESSAGESECURITYMODE_SIGN; /* require encryption */
	cc->securityPolicyUri = UA_STRING_ALLOC("http://opcfoundation.org/UA/SecurityPolicy#Basic256Sha256");
	UA_StatusCode retVal = UA_ClientConfig_setDefaultEncryption(cc, certificate, privateKey,
		trustList, trustListSize,
		revocationList, revocationListSize);

	//UA_SecurityPolicy_Basic256Sha256(cc->securityPolicies, certificate, privateKey,&cc->logger);
	UA_SecurityPolicy_None(cc->securityPolicies, certificate, &cc->logger);

	cc->clientDescription.applicationUri = UA_STRING_ALLOC("urn:open62541.client.application");

	UA_ByteString_clear(&certificate);
	UA_ByteString_clear(&privateKey);
	UA_ByteString_clear(&trustList[0]);

	cc->securityMode = UA_MESSAGESECURITYMODE_SIGN;

	//UA_StatusCode retval = UA_Client_connect(client, "opc.tcp://LAPTOP-SMDMIQSL:53530/OPCUA/SimulationServer");
	UA_StatusCode retval = UA_Client_connect(client, "opc.tcp://192.168.0.120:48050");
	if (retval != UA_STATUSCODE_GOOD) {
		UA_Client_delete(client);
		return (int)retval;
	}

	UA_ReadRequest _request;
	UA_ReadResponse _respone;
	UA_ReadRequest_init(&_request);
	UA_String out = UA_STRING_NULL;

	UA_ReadValueId* vv = (UA_ReadValueId*)UA_Array_new(7, &UA_TYPES[UA_TYPES_READVALUEID]);

	for (int i=0;  i < 7 ; i++)
	{
		UA_ReadValueId_init(vv+i);
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
	vv[5].nodeId = UA_NODEID_NUMERIC(3, 1007);
	vv[5].attributeId = UA_ATTRIBUTEID_VALUE;
	vv[5].indexRange = UA_STRING_ALLOC("2");
	vv[6].nodeId = UA_NODEID_NUMERIC(3, 1010);
	vv[6].attributeId = UA_ATTRIBUTEID_VALUE;

	_request.requestHeader.timestamp = UA_DateTime_now();
	_request.nodesToRead = vv;
	_request.nodesToReadSize = 7;
	

	UA_print(&_request, &UA_TYPES[UA_TYPES_READREQUEST], &out);
	printf("%.*s\n", (int)out.length, out.data);
	UA_String_clear(&out);

	_respone = UA_Client_Service_read(client, _request);


	UA_print(&_respone, &UA_TYPES[UA_TYPES_READRESPONSE], &out);
	printf("%.*s\n", (int)out.length, out.data);
	UA_String_clear(&out);

	//Sleep(20000);


	/* Clean up */

	UA_ReadResponse_clear(&_respone);
	UA_ReadRequest_clear(&_request);
	UA_Client_delete(client); /* Disconnects the client internally */

	std::cout << "CONNECT DONE" << std::endl;
}

int test_request_write()
{
	std::cout << "TEST REQUEST WTITE" << std::endl;

	UA_ByteString certificate = loadFile("client_cert.der");
	UA_ByteString privateKey = loadFile("client_key.der");
	size_t trustListSize = 1;

	//UA_ByteString* trustList;
	UA_STACKARRAY(UA_ByteString, trustList, trustListSize);
	//trustList[0] = loadFile("uaservercpp.der");
	trustList[0] = loadFile("uagateway.der");
	//trustList[0] = loadFile("SimulationServer@LAPTOP-SMDMIQSL_2048.der");
	UA_ByteString* revocationList = NULL;
	size_t revocationListSize = 0;

	UA_Client* client = UA_Client_new();
	UA_ClientConfig* cc = UA_Client_getConfig(client);
	cc->securityMode = UA_MESSAGESECURITYMODE_SIGN; /* require encryption */
	//cc->securityMode = UA_MESSAGESECURITYMODE_NONE; /* require encryption */
	//cc->securityPolicyUri = UA_STRING_ALLOC("http://opcfoundation.org/UA/SecurityPolicy#None");
	//cc->securityPolicyUri = UA_STRING_ALLOC("http://opcfoundation.org/UA/SecurityPolicy#Basic256Sha256");
	//cc->securityPolicyUri = UA_STRING_ALLOC("http://opcfoundation.org/UA/SecurityPolicy#Basic256");
	UA_StatusCode retVal = UA_ClientConfig_setDefaultEncryption(cc, certificate, privateKey,
		trustList, trustListSize,
		revocationList, revocationListSize);

	//UA_SecurityPolicy_Basic256Sha256(cc->securityPolicies, certificate, privateKey,&cc->logger);
	UA_SecurityPolicy_None(cc->securityPolicies, certificate, &cc->logger);

	cc->clientDescription.applicationUri = UA_STRING_ALLOC("urn:open62541.client.application");

	UA_ByteString_clear(&certificate);
	UA_ByteString_clear(&privateKey);
	UA_ByteString_clear(&trustList[0]);

	//UA_StatusCode retval = UA_Client_connect(client, "opc.tcp://LAPTOP-SMDMIQSL:53530/OPCUA/SimulationServer");
	UA_StatusCode retval = UA_Client_connect(client, "opc.tcp://192.168.0.120:48050");
	if (retval != UA_STATUSCODE_GOOD) {
		UA_Client_delete(client);
		return (int)retval;
	}

	/////////////////

	/////////////////


	UA_WriteRequest _request;
	UA_WriteResponse _respone;
	UA_WriteRequest_init(&_request);

	UA_Variant value;
	UA_Variant_init(&value);
	int16_t v[100]; for (int i = 0; i < 99; i++) v[i] = 100;
	float f = 300;
	UA_Variant_setArrayCopy(&value, &v,1,&UA_TYPES[UA_TYPES_INT16]);
	//UA_Variant_setScalarCopy(&value, &f, &UA_TYPES[UA_TYPES_FLOAT]);
	//UA_Variant_setScalarCopy(&value, &v, &UA_TYPES[UA_TYPES_INT16]);

	UA_WriteValue* vv = (UA_WriteValue*)UA_Array_new(1, &UA_TYPES[UA_TYPES_WRITEVALUE]);
	UA_WriteValue_init(vv);
	_request.nodesToWriteSize = 1;
	_request.nodesToWrite = vv;
	//_request.nodesToWrite[0].nodeId = UA_NODEID_STRING_ALLOC(2, "Demo.Static.Arrays.Int16");
	_request.nodesToWrite[0].nodeId = UA_NODEID_STRING_ALLOC(7, "MAIN.mass_int_opc");
	//_request.nodesToWrite[0].nodeId = UA_NODEID_NUMERIC(3, 1007);
	_request.nodesToWrite[0].indexRange = UA_STRING_ALLOC("1");
	_request.nodesToWrite[0].attributeId = UA_AttributeId::UA_ATTRIBUTEID_VALUE;
	_request.nodesToWrite[0].value.hasValue = true;
	_request.nodesToWrite[0].value.hasServerPicoseconds = false;
	_request.nodesToWrite[0].value.hasServerTimestamp = false;
	_request.nodesToWrite[0].value.hasSourcePicoseconds = false;
	_request.nodesToWrite[0].value.hasSourceTimestamp = false;
	_request.nodesToWrite[0].value.hasStatus = true;
	_request.nodesToWrite[0].value.status = UA_STATUSCODE_GOOD;
	//_request.nodesToWrite[0].value.sourceTimestamp = TimeConverter::GetTime_LLmcs() * 10LL + UA_DATETIME_UNIX_EPOCH;
	//_request.nodesToWrite[0].value.serverTimestamp = TimeConverter::GetTime_LLmcs() * 10LL + UA_DATETIME_UNIX_EPOCH;

	_request.nodesToWrite[0].value.value.type = &UA_TYPES[UA_TYPES_INT16];
	//_request.nodesToWrite[0].value.value.type = &UA_TYPES[UA_TYPES_FLOAT];
	//_request.nodesToWrite[0].value.value.storageType = UA_VARIANT_DATA_NODELETE;
	_request.nodesToWrite[0].value.value = value;
	

	for (;;)
	{

		_respone = UA_Client_Service_write(client, _request);
		UA_WriteResponse_clear(&_respone);
		break;
		
	}

	UA_WriteRequest_clear(&_request);
	UA_WriteResponse_clear(&_respone);

	UA_Client_disconnect(client);
	UA_Client_delete(client);

	return 0;
}

int test_request_read_mass1000_()
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
	cc->securityMode = UA_MESSAGESECURITYMODE_SIGN; /* require encryption */
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

	//cc->securityMode = UA_MESSAGESECURITYMODE_SIGN;

	UA_StatusCode retval = UA_Client_connect(client, "opc.tcp://LAPTOP-SMDMIQSL:53530/OPCUA/SimulationServer");
	if (retval != UA_STATUSCODE_GOOD) {
		UA_Client_delete(client);
		return (int)retval;
	}

	UA_ReadRequest _request;
	UA_ReadResponse _respone;
	UA_ReadRequest_init(&_request);
	UA_String out = UA_STRING_NULL;

	UA_ReadValueId* vv = (UA_ReadValueId*)UA_Array_new(1, &UA_TYPES[UA_TYPES_READVALUEID]);

	for (int i = 0; i < 1; i++)
	{
		UA_ReadValueId_init(vv + i);
	}

	vv[0].nodeId = UA_NODEID_NUMERIC(3, 1010);
	vv[0].attributeId = UA_ATTRIBUTEID_VALUE;

	_request.requestHeader.timestamp = UA_DateTime_now();
	_request.nodesToRead = vv;
	_request.nodesToReadSize = 1;


	//UA_print(&_request, &UA_TYPES[UA_TYPES_READREQUEST], &out);
	//printf("%.*s\n", (int)out.length, out.data);
	//UA_String_clear(&out);

	int iter = 0;
	std::vector<long long> time(10000);
	long long t = 0;

	for (;;)
	{
		if (iter >= 10000) break;
		t = TimeConverter::GetTime_LLmcs();
		_respone = UA_Client_Service_read(client, _request);
		time[iter] = TimeConverter::GetTime_LLmcs() - t;
		iter++;
		UA_ReadResponse_clear(&_respone);
	}

	UA_print(&_respone, &UA_TYPES[UA_TYPES_READRESPONSE], &out);
	printf("%.*s\n", (int)out.length, out.data);
	UA_String_clear(&out);

	Sleep(20000);

	long long sum_of_elems = std::accumulate(time.begin(), time.end(), 0);

	

	UA_ReadResponse_clear(&_respone);
	UA_ReadRequest_clear(&_request);
	UA_Client_delete(client); /* Disconnects the client internally */

	std::cout << "CONNECT DONE" << std::endl;
	std::cout << "TIME REQUEST: " << sum_of_elems * 1.0 / time.size() << "mcs" << std::endl;
	Sleep(1000);

	return 0;
}

int test_request_read_mass1000_onetoone()
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
	//UA_ClientConfig_setDefault(UA_Client_getConfig(client));
	UA_ClientConfig* cc = UA_Client_getConfig(client);
	cc->securityMode = UA_MESSAGESECURITYMODE_SIGN; /* require encryption */
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

	cc->securityMode = UA_MESSAGESECURITYMODE_SIGN;

	UA_StatusCode retval = UA_Client_connect(client, "opc.tcp://LAPTOP-SMDMIQSL:53530/OPCUA/SimulationServer");
	if (retval != UA_STATUSCODE_GOOD) {
		UA_Client_delete(client);
		return (int)retval;
	}

	UA_ReadRequest _request;
	UA_ReadResponse _respone;
	UA_ReadRequest_init(&_request);
	UA_String out = UA_STRING_NULL;

	size_t size_value = 1;

	UA_ReadValueId* vv = (UA_ReadValueId*)UA_Array_new(size_value, &UA_TYPES[UA_TYPES_READVALUEID]);

	for (int i = 0; i < size_value; i++)
	{
		UA_ReadValueId_init(vv + i);
	}

	for (int i = 0; i < size_value; i++)
	{
		vv[i].nodeId = UA_NODEID_NUMERIC(3, 1007);
		vv[i].attributeId = UA_ATTRIBUTEID_VALUE;
		vv[i].indexRange = UA_STRING_ALLOC(std::to_string(i).c_str());
	}


	_request.requestHeader.timestamp = UA_DateTime_now();
	_request.nodesToRead = vv;
	_request.nodesToReadSize = size_value;


	//UA_print(&_request, &UA_TYPES[UA_TYPES_READREQUEST], &out);
	//printf("%.*s\n", (int)out.length, out.data);
	//UA_String_clear(&out);

	int iter = 0;
	std::vector<long long> time(10000);
	long long t = 0;

	for (;;)
	{
		if (iter >= 10000) break;
		t = TimeConverter::GetTime_LLmcs();
		_respone = UA_Client_Service_read(client, _request);
		time[iter] = TimeConverter::GetTime_LLmcs() - t;
		
		//UA_print(&_respone, &UA_TYPES[UA_TYPES_READRESPONSE], &out);
		//printf("%.*s\n", (int)out.length, out.data);
		//UA_String_clear(&out);;
		//Sleep(5000);
		UA_ReadResponse_clear(&_respone);
		iter++;
		if (iter % 100 == 0)
		{
			std::cout << iter / 100 << "%" << std::endl;
		}
	}	

	long long sum_of_elems = std::accumulate(time.begin(), time.end(), 0);
	std::cout << "TIME REQUEST: " << sum_of_elems * 1.0 / time.size() << "mcs" << std::endl;
	Sleep(1000);
	UA_ReadResponse_clear(&_respone);
	UA_ReadRequest_clear(&_request);
	UA_Client_delete(client); /* Disconnects the client internally */	

	return 0;
}

int test_request_write_mass1000_onetoone()
{
	std::cout << "TEST WRITE REQUEST" << std::endl;

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
	cc->securityMode = UA_MESSAGESECURITYMODE_SIGN; /* require encryption */
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

	cc->securityMode = UA_MESSAGESECURITYMODE_SIGN;

	UA_StatusCode retval = UA_Client_connect(client, "opc.tcp://LAPTOP-SMDMIQSL:53530/OPCUA/SimulationServer");
	if (retval != UA_STATUSCODE_GOOD) {
		UA_Client_delete(client);
		return (int)retval;
	}

	UA_WriteRequest _request;
	UA_WriteResponse _respone;
	UA_WriteRequest_init(&_request);
	size_t size_values = 1000;

	UA_Variant* value = (UA_Variant*)UA_Array_new(size_values, &UA_TYPES[UA_TYPES_VARIANT]);
	for (int i = 0; i < size_values; i++)
	{
		UA_Variant_init(value+i);
		int v = 20;
		UA_Variant_setArrayCopy(value+i, &v, 1, &UA_TYPES[UA_TYPES_INT32]);
	}
	

	UA_WriteValue* vv = (UA_WriteValue*)UA_Array_new(size_values, &UA_TYPES[UA_TYPES_WRITEVALUE]);

	for (int i = 0; i < size_values; i++)
	{
		UA_WriteValue_init(vv+i);
	}
	_request.nodesToWriteSize = size_values;
	_request.nodesToWrite = vv;
	
	for (int i = 0; i < size_values; i++)
	{
		_request.nodesToWrite[i].nodeId = UA_NODEID_NUMERIC(3, 1007);
		_request.nodesToWrite[i].attributeId = UA_ATTRIBUTEID_VALUE;
		_request.nodesToWrite[i].value.hasValue = true;
		_request.nodesToWrite[i].value.value.type = &UA_TYPES[UA_TYPES_INT32];
		_request.nodesToWrite[i].value.value.storageType = UA_VARIANT_DATA_NODELETE;
		_request.nodesToWrite[i].indexRange = UA_STRING_ALLOC(std::to_string(i).c_str());
		_request.nodesToWrite[i].value.value = *(value+i);
		_request.nodesToWrite[i].value.hasSourceTimestamp = true;
	}
	
	long long t = 0;
	std::vector<long long> _time(10000);
	int counter = 0;

	for (;;)
	{
		t = TimeConverter::GetTime_LLmcs();
		_respone = UA_Client_Service_write(client, _request);
		_time[counter] = TimeConverter::GetTime_LLmcs() - t;
		UA_WriteResponse_clear(&_respone);
		counter++;
		if (counter % 100 == 0)
		{
			std::cout << counter / 100 << "%" << std::endl;
		}
		if (counter >= 10000) break;
	}


	UA_WriteRequest_clear(&_request);
	UA_WriteResponse_clear(&_respone);

	long long sum = std::accumulate(_time.begin(), _time.end(), 0);
	std::cout << "TIME REQUEST WRITE: " << sum * 1.0 / _time.size() << " mcs" << std::endl;

	UA_Client_disconnect(client);
	UA_Client_delete(client);
	
	return 0;
}

int test_request_write_mass100_andvaleu3()
{
	std::cout << "TEST WRITE REQUEST" << std::endl;

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
	cc->securityMode = UA_MESSAGESECURITYMODE_SIGN; /* require encryption */
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

	cc->securityMode = UA_MESSAGESECURITYMODE_SIGN;

	UA_StatusCode retval = UA_Client_connect(client, "opc.tcp://LAPTOP-SMDMIQSL:53530/OPCUA/SimulationServer");
	if (retval != UA_STATUSCODE_GOOD) {
		UA_Client_delete(client);
		return (int)retval;
	}


	UA_WriteRequest _request;
	UA_WriteResponse _respone;
	UA_WriteRequest_init(&_request);
	size_t size_values = 100;

	UA_Variant* value = (UA_Variant*)UA_Array_new(size_values+3, &UA_TYPES[UA_TYPES_VARIANT]);
	for (int i = 0; i < size_values; i++)
	{
		UA_Variant_init(value + i);
		int v = 50+i;
		UA_Variant_setArrayCopy(value + i, &v,1,&UA_TYPES[UA_TYPES_INT32]);
	}

	{
		UA_Variant_init(value+100);
		int v = 100;
		UA_Variant_setScalarCopy(value + 100, &v, &UA_TYPES[UA_TYPES_INT32]);
	}

	{
		UA_Variant_init(value + 101);
		int v = 101;
		UA_Variant_setScalarCopy(value + 101, &v, &UA_TYPES[UA_TYPES_INT32]);
	}

	{
		UA_Variant_init(value + 102);
		int v = 102;
		UA_Variant_setScalarCopy(value + 102, &v, &UA_TYPES[UA_TYPES_INT32]);
	}

	UA_WriteValue* vv = (UA_WriteValue*)UA_Array_new(size_values+3, &UA_TYPES[UA_TYPES_WRITEVALUE]);

	for (int i = 0; i < size_values+3; i++)
	{
		UA_WriteValue_init(vv + i);
	}

	_request.nodesToWriteSize = size_values+3;
	_request.nodesToWrite = vv;

	for (int i = 0; i < size_values; i++)
	{
		_request.nodesToWrite[i].nodeId = UA_NODEID_NUMERIC(3, 1007);
		_request.nodesToWrite[i].attributeId = UA_ATTRIBUTEID_VALUE;
		_request.nodesToWrite[i].value.hasValue = true;
		_request.nodesToWrite[i].value.value.type = &UA_TYPES[UA_TYPES_INT32];
		_request.nodesToWrite[i].value.value.storageType = UA_VARIANT_DATA_NODELETE;
		_request.nodesToWrite[i].indexRange = UA_STRING_ALLOC(std::to_string(i).c_str());
		_request.nodesToWrite[i].value.value = *(value + i);
		_request.nodesToWrite[i].value.hasSourceTimestamp = true;
	}

	{
		_request.nodesToWrite[100].nodeId = UA_NODEID_NUMERIC(3, 1008);
		_request.nodesToWrite[100].attributeId = UA_ATTRIBUTEID_VALUE;
		_request.nodesToWrite[100].value.hasValue = true;
		_request.nodesToWrite[100].value.value.type = &UA_TYPES[UA_TYPES_INT32];
		_request.nodesToWrite[100].value.value.storageType = UA_VARIANT_DATA_NODELETE;
		_request.nodesToWrite[100].indexRange = UA_STRING_ALLOC(std::to_string(0).c_str());
		_request.nodesToWrite[100].value.value = *(value + 100);
		_request.nodesToWrite[100].value.hasSourceTimestamp = true;
	}

	{
		_request.nodesToWrite[101].nodeId = UA_NODEID_NUMERIC(3, 1009);
		_request.nodesToWrite[101].attributeId = UA_ATTRIBUTEID_VALUE;
		_request.nodesToWrite[101].value.hasValue = true;
		_request.nodesToWrite[101].value.value.type = &UA_TYPES[UA_TYPES_INT32];
		_request.nodesToWrite[101].value.value.storageType = UA_VARIANT_DATA_NODELETE;
		_request.nodesToWrite[101].indexRange = UA_STRING_ALLOC(std::to_string(0).c_str());
		_request.nodesToWrite[101].value.value = *(value + 101);
		_request.nodesToWrite[101].value.hasSourceTimestamp = true;
	}

	{
		_request.nodesToWrite[102].nodeId = UA_NODEID_NUMERIC(3, 1010);
		_request.nodesToWrite[102].attributeId = UA_ATTRIBUTEID_VALUE;
		_request.nodesToWrite[102].value.hasValue = true;
		_request.nodesToWrite[102].value.value.type = &UA_TYPES[UA_TYPES_INT32];
		_request.nodesToWrite[102].value.value.storageType = UA_VARIANT_DATA_NODELETE;
		_request.nodesToWrite[102].indexRange = UA_STRING_ALLOC(std::to_string(0).c_str());
		_request.nodesToWrite[102].value.value = *(value + 102);
		_request.nodesToWrite[102].value.hasSourceTimestamp = true;
	}


	long long t = 0;
	std::vector<long long> _time(10000);
	int counter = 0;

	for (;;)
	{
		t = TimeConverter::GetTime_LLmcs();
		_respone = UA_Client_Service_write(client, _request);
		_time[counter] = TimeConverter::GetTime_LLmcs() - t;
		UA_WriteResponse_clear(&_respone);
		std::this_thread::sleep_for(std::chrono::milliseconds(3000));
		counter++;
		if (counter > 5) break;
	}


	UA_WriteRequest_clear(&_request);
	UA_WriteResponse_clear(&_respone);

	//long long sum = std::accumulate(_time.begin(), _time.end(), 0);
	//std::cout << "TIME REQUEST WRITE: " << sum * 1.0 / _time.size() << " mcs" << std::endl;

	UA_Client_disconnect(client);
	UA_Client_delete(client);

	return 0;
}

int test_request_write_scalar()
{
	std::cout << "TEST WRITE REQUEST" << std::endl;

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
	cc->securityMode = UA_MESSAGESECURITYMODE_SIGN; /* require encryption */
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

	cc->securityMode = UA_MESSAGESECURITYMODE_SIGN;

	UA_StatusCode retval = UA_Client_connect(client, "opc.tcp://LAPTOP-SMDMIQSL:53530/OPCUA/SimulationServer");
	if (retval != UA_STATUSCODE_GOOD) {
		UA_Client_delete(client);
		return (int)retval;
	}


	UA_WriteRequest _request;
	UA_WriteResponse _respone;
	UA_WriteRequest_init(&_request);
	size_t size_values = 1;

	UA_Variant* value = (UA_Variant*)UA_Array_new(size_values, &UA_TYPES[UA_TYPES_VARIANT]);
	{
		UA_Variant_init(value);
		int v = 102;
		UA_Variant_setScalarCopy(value, &v, &UA_TYPES[UA_TYPES_INT32]);
	}

	UA_WriteValue* vv = (UA_WriteValue*)UA_Array_new(size_values , &UA_TYPES[UA_TYPES_WRITEVALUE]);

	for (int i = 0; i < size_values; i++)
	{
		UA_WriteValue_init(vv + i);
	}

	_request.nodesToWriteSize = size_values;
	_request.nodesToWrite = vv;

	for (int i = 0; i < size_values; i++)
	{
		_request.nodesToWrite[i].nodeId = UA_NODEID_NUMERIC(3, 1008);
		_request.nodesToWrite[i].attributeId = UA_ATTRIBUTEID_VALUE;
		_request.nodesToWrite[i].value.hasValue = true;
		_request.nodesToWrite[i].value.value.type = &UA_TYPES[UA_TYPES_INT32];
		_request.nodesToWrite[i].value.value.storageType = UA_VARIANT_DATA_NODELETE;
		_request.nodesToWrite[i].indexRange = UA_STRING_ALLOC(std::to_string(i).c_str());
		_request.nodesToWrite[i].value.value = *(value + i);
		_request.nodesToWrite[i].value.hasSourceTimestamp = true;
	}

	long long t = 0;
	std::vector<long long> _time(10000);
	int counter = 0;

	for (;;)
	{
		t = TimeConverter::GetTime_LLmcs();
		_respone = UA_Client_Service_write(client, _request);
		_time[counter] = TimeConverter::GetTime_LLmcs() - t;
		UA_WriteResponse_clear(&_respone);
		std::this_thread::sleep_for(std::chrono::milliseconds(3000));
		counter++;
		if (counter > 5) break;
	}


	UA_WriteRequest_clear(&_request);
	UA_WriteResponse_clear(&_respone);

	//long long sum = std::accumulate(_time.begin(), _time.end(), 0);
	//std::cout << "TIME REQUEST WRITE: " << sum * 1.0 / _time.size() << " mcs" << std::endl;

	UA_Client_disconnect(client);
	UA_Client_delete(client);

	return 0;
}

int test_write_next_read()
{
	std::cout << "TEST WRITE REQUEST" << std::endl;

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
	cc->securityMode = UA_MESSAGESECURITYMODE_SIGN; /* require encryption */
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

	cc->securityMode = UA_MESSAGESECURITYMODE_SIGN;

	UA_StatusCode retval = UA_Client_connect(client, "opc.tcp://LAPTOP-SMDMIQSL:53530/OPCUA/SimulationServer");
	if (retval != UA_STATUSCODE_GOOD) {
		UA_Client_delete(client);
		return (int)retval;
	}

	{
		UA_WriteRequest _request;
		UA_WriteResponse _respone;
		UA_WriteRequest_init(&_request);
		size_t size_values = 1;
		int v = 99;

		UA_String st = UA_String_fromChars("asdsad");
		//UA_String_clear(&st);
		UA_String* pst = &st;
		UA_Variant* value = (UA_Variant*)UA_Array_new(size_values, &UA_TYPES[UA_TYPES_VARIANT]);
		for (int i = 0; i < size_values; i++)
		{
			UA_Variant_init(value + i);
			
			//UA_Variant_setScalarCopy(value + i, &v, &UA_TYPES[UA_TYPES_INT32]);
			UA_Variant_setScalar(value + i, &st, &UA_TYPES[UA_TYPES_STRING]);
			//UA_Variant_setArrayCopy(value + i, &v, 1, &UA_TYPES[UA_TYPES_INT32]);
		}

		for (int i = 0; i < 100; i++)
		{
			//UA_Variant_clear(value);
			UA_Variant_setScalar(value, &st, &UA_TYPES[UA_TYPES_STRING]);
		}


		v = 67;

		UA_WriteValue* vv = (UA_WriteValue*)UA_Array_new(size_values, &UA_TYPES[UA_TYPES_WRITEVALUE]);

		for (int i = 0; i < size_values; i++)
		{
			UA_WriteValue_init(vv + i);
		}
		_request.nodesToWriteSize = size_values;
		_request.nodesToWrite = vv;

		for (int i = 0; i < size_values; i++)
		{
			_request.nodesToWrite[i].nodeId = UA_NODEID_NUMERIC(3, 1007);
			_request.nodesToWrite[i].attributeId = UA_ATTRIBUTEID_VALUE;
			_request.nodesToWrite[i].value.value = *(value + i);
			_request.nodesToWrite[i].value.hasValue = true;
			_request.nodesToWrite[i].value.value.type = &UA_TYPES[UA_TYPES_INT32];
			_request.nodesToWrite[i].value.value.storageType = UA_VARIANT_DATA;//_NODELETE;
			//_request.nodesToWrite[i].value.value.data = &v;
			_request.nodesToWrite[i].indexRange = UA_STRING_ALLOC(std::to_string(i).c_str());
			_request.nodesToWrite[i].value.hasSourceTimestamp = true;
		}

		_respone = UA_Client_Service_write(client, _request);

		UA_WriteRequest_clear(&_request);
		UA_WriteResponse_clear(&_respone);
	}

	Sleep(1000);

	{
		UA_ReadRequest _request;
		UA_ReadResponse _respone;
		UA_ReadRequest_init(&_request);
		UA_String out = UA_STRING_NULL;

		UA_ReadValueId* vv = (UA_ReadValueId*)UA_Array_new(1, &UA_TYPES[UA_TYPES_READVALUEID]);

		for (int i = 0; i < 1; i++)
		{
			UA_ReadValueId_init(vv + i);
		}

		vv[0].nodeId = UA_NODEID_NUMERIC(3, 1007);
		vv[0].attributeId = UA_ATTRIBUTEID_VALUE;

		_request.requestHeader.timestamp = UA_DateTime_now();
		_request.nodesToRead = vv;
		_request.nodesToReadSize = 1;

		_respone = UA_Client_Service_read(client, _request);

		UA_print(&_respone, &UA_TYPES[UA_TYPES_READRESPONSE], &out);
		printf("%.*s\n", (int)out.length, out.data);
		UA_String_clear(&out);

		UA_ReadResponse_clear(&_respone);
		UA_ReadRequest_clear(&_request);
	}

	UA_Client_delete(client);

	return 0;
}

int check_connent_session()
{
	UA_ByteString certificate = loadFile_new("client_cert.der");
	UA_ByteString privateKey = loadFile_new("client_key.der");
	size_t trustListSize = 1;

	//UA_ByteString* trustList;
	UA_STACKARRAY(UA_ByteString, trustList, trustListSize);
	trustList[0] = loadFile("SimulationServer@LAPTOP-SMDMIQSL_2048.der");
	UA_ByteString* revocationList = NULL;
	size_t revocationListSize = 0;

	UA_Client* client = UA_Client_new();
	UA_ClientConfig* cc = UA_Client_getConfig(client);
	cc->securityMode = UA_MESSAGESECURITYMODE_SIGN; /* require encryption */
	cc->securityPolicyUri = UA_STRING_ALLOC("http://opcfoundation.org/UA/SecurityPolicy#Basic256");
	UA_StatusCode retVal = UA_ClientConfig_setDefaultEncryption(cc, certificate, privateKey,
		trustList, trustListSize,
		revocationList, revocationListSize);

	//UA_SecurityPolicy_Basic256(cc->securityPolicies, certificate, privateKey,&cc->logger);
	UA_SecurityPolicy_None(cc->securityPolicies, certificate, &cc->logger);

	cc->secureChannelLifeTime = 30000;
	cc->requestedSessionTimeout = 10000;

	cc->clientDescription.applicationUri = UA_STRING_ALLOC("urn:open62541.client.application");

	UA_ByteString_clear(&certificate);
	UA_ByteString_clear(&privateKey);
	UA_ByteString_clear(&trustList[0]);

	//cc->securityMode = UA_MESSAGESECURITYMODE_SIGN;

	UA_StatusCode retval = UA_Client_connect(client, "opc.tcp://LAPTOP-SMDMIQSL:53530/OPCUA/SimulationServer");
	if (retval != UA_STATUSCODE_GOOD) {
		UA_Client_delete(client);
		return (int)retval;
	}

	UA_ReadRequest _request;
	UA_ReadResponse _respone;
	UA_ReadRequest_init(&_request);
	UA_String out = UA_STRING_NULL;

	UA_ReadValueId* vv = (UA_ReadValueId*)UA_Array_new(1, &UA_TYPES[UA_TYPES_READVALUEID]);

	for (int i = 0; i < 1; i++)
	{
		UA_ReadValueId_init(vv + i);
	}

	vv[0].nodeId = UA_NODEID_NUMERIC(3, 1001);
	vv[0].attributeId = UA_ATTRIBUTEID_VALUE;

	_request.requestHeader.timestamp = UA_DateTime_now();
	_request.nodesToRead = vv;
	_request.nodesToReadSize = 1;


	//UA_print(&_request, &UA_TYPES[UA_TYPES_READREQUEST], &out);
	//printf("%.*s\n", (int)out.length, out.data);
	//UA_String_clear(&out);

	int iter = 0;
	std::vector<long long> time(10000);
	long long t = 0;
	UA_StatusCode st_code;
	UA_SecureChannelState st_shs;
	UA_SessionState st_ses;
	for (;;)
	{
		std::cout << "CLIENT STATAUS:" << std::endl;
		UA_Client_getState(client, &st_shs, &st_ses, &st_code);
		std::cout << st_code << std::endl;
		std::cout << st_shs << std::endl;
		std::cout << st_ses << std::endl;
		if (iter >= 10000) break;
		t = TimeConverter::GetTime_LLmcs();
		_respone = UA_Client_Service_read(client, _request);
		time[iter] = TimeConverter::GetTime_LLmcs() - t;
		iter++;

		std::cout << UA_DATETIME_UNIX_EPOCH + TimeConverter::GetTime_LLmcs() * 10 << std::endl;
		std::cout << UA_DateTime_now() << std::endl;

		Sleep(1000);
		UA_print(&_respone, &UA_TYPES[UA_TYPES_READRESPONSE], &out);
		printf("%.*s\n", (int)out.length, out.data);
		UA_String_clear(&out);
		UA_ReadResponse_clear(&_respone);
	} 	

	Sleep(20000);

	long long sum_of_elems = std::accumulate(time.begin(), time.end(), 0);



	UA_ReadResponse_clear(&_respone);
	UA_ReadRequest_clear(&_request);
	UA_Client_delete(client); /* Disconnects the client internally */

	std::cout << "CONNECT DONE" << std::endl;
	std::cout << "TIME REQUEST: " << sum_of_elems * 1.0 / time.size() << "mcs" << std::endl;
	Sleep(1000);

	return 0;
}