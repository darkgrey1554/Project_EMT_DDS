#pragma once

namespace atech::common
{
	enum class CatalogCommand
	{
	   START,
	   STOP,
	   RESET,
	   SEND_STATUS,
	   SEND_PROCESS_INFO,
	   SEND_VERSION,
	   RECEIVE_CONFIG,
	   APPLY_CONFIG,
	   SET_LOGGING_LEVEL,
	   LOGGING
	};

	enum class CatalogStatus
	{
		Null,
		OK,
		ERR,
		WARNING,

		DEBUG,
		INFO,
		CRITICAL,

		ERROR_INIT,
		ERROR_RECEIVE,
		ERROR_CONNECTING
	};
}
/*
*	/////////////////////////////////////////////////////////////////////
*	@Command: START
*	@description: control command to start the entity
*	@parametr: {"id":[1,2,3]} json string
*		@ id: vector id entity of service to start
*		@ if id is empty - start the entire service and all its entities
* 
* 
*	@Answer: OK
*   @descriiption: strat all entities successful
*	@parametr: {"success":[1,2,3],"error":[],"unfound":[]}
* 
*	@Answer: ERROR
*   @descriiption: there is an entity with a startup error (an unfound entity may also be present)
*	@parametr: {"success":[1],"error":[2],"unfound":[3]}
* 
*	@Answer: WARNING
*   @descriiption: an unfound entity is present
*	@parametr: {"success":[1,2],"error":[],"unfound":[3]}
* 
* 
* 
* 
*  ///////////////////////////////////////////////////////////////////////
*  @Command: STOP
*  @description: control command to stop the entity
*  @parametr: {"id":[1,2,3]} json string
*		@ id: vector id entity of service to stop
*		@ if id is empty - stop the entire service and all its entities
* 
* 
*	@Answer: OK
*   @descriiption: stop all entities successful
*	@parametr: {"success":[1,2,3],"error":[],"unfound":[]}
* 
*	@Answer: ERROR
*   @descriiption: there is an entity with a stop error (an unfound entity may also be present)
*	@parametr: {"success":[1],"error":[2],"unfound":[3]}																  
* 
*	@Answer: WARNING
*   @descriiption: an unfound entity is present
*	@parametr: {"success":[1,2],"error":[],"unfound":[3]}
*
*	
* 
*   ///////////////////////////////////////////////////////////////////////
* 	@Command: RESET
*   @description: control command to reset the entity (reset is restart is the destruction and creation of the entity anew)
*   @parametr: {"id":[1,2,3]} json string
*		@ id: vector id entity of service to reset
*		@ if id is empty - reset the entire service and all its entities
* 
* 
*	@Answer: OK
*   @descriiption: reset all entities successful
*	@parametr: {"success":[1,2,3],"error":[],"unfound":[]}
* 
*	@Answer: ERROR
*   @descriiption: there is an entity with a reset error (an unfound entity may also be present)
*	@parametr: {"success":[1],"error":[2],"unfound":[3]}
* 
*	@Answer: WARNING
*   @descriiption: an unfound entity is present
*	@parametr: {"success":[1,2],"error":[],"unfound":[3]}
*	
* 
*	@note 
*	with id:[], there is no response for the command  
* 
* 
*  /////////////////////////////////////////////////////////////////////////////////////////
*  @Command: SEND_STATUS
*   @description: query of entity status
*   @parametr: {"id":[1,2,3]} json string
*		@ id: vector id entity of service to query
*		@ if id is empty - query status of service and all its entities
* 
* 
*	@Answer: OK
*   @descriiption: query of status all entities successful
*	@parametr: {"success":[{"id":1, "status":x},{"id":2, "status":x},{"id":3, "status":x}],"error":[],"unfound":[]}
* 
*	@Answer: ERROR
*   @descriiption: there is an entity with a error query of status (an unfound entity may also be present)
*	@parametr: {"success":[{"id":1, "status":x}],"error":[1],"unfound":[3]}
* 
*	@Answer: WARNING
*   @descriiption: an unfound entity is present
*	@parametr: {"success":[{"id":1, "status":x},{"id":2, "status":x}],"error":[],"unfound":[3]}	  
*
* 
* 
* /////////////////////////////////////////////////////////////////////////
*   @Command: SEND_PROCESS_INFO
*   @description: process information request
*   @parametr: NULL 
* 
*	@Answer: OK
*   @descriiption: 
*	@parametr: {"message"}
* 
*	@Answer: ERROR
*   @descriiption: error in collecting information 
*	@parametr: NULL
* 
* 
* 
* 	/////////////////////////////////////////////////////////////////////////
*   @Command: SEND_PROCESS_INFO
*   @description: process information request
*   @parametr: NULL 
* 
*	@Answer: OK
*   @descriiption: 
*	@parametr: {"message"}
* 
*	@Answer: ERROR
*   @descriiption: error in collecting information 
*	@parametr: NULL
* 
* 
* * /////////////////////////////////////////////////////////////////////////
*   @Command: RECEIVE_CONFIG
*   @description: accept new config
*   @parametr: {"size":12345};
		@size - size of config in baits
* 
*	@Answer: OK
*   @descriiption: 
*	@parametr: NULL
* 
*	@Answer: ERROR
*   @descriiption: error of accept new config 
*	@parametr: NULL
*	
* 	
* /////////////////////////////////////////////////////////////////////////
*   @Command: APPLY_CONFIG
*   @description: accept new config
*   @parametr: NULL
* 
*	@Answer: OK
*   @descriiption: 
*	@parametr: NULL
*
*	@Answer: ERROR
*   @descriiption: error of accept new config 
*	@parametr: NULL
*	
*
*  
*  /////////////////////////////////////////////////////////////////////////
*   @Command: SET_LOGGING_LEVEL
*   @description: accept new config
*   @parametr: {"Level":x}
* 
*	@Answer: OK
*   @descriiption: 
*	@parametr: NULL
*
*	@Answer: ERROR
*   @descriiption: error of accept new config 
*	@parametr: NULL
*
*	
*
*  
*  /////////////////////////////////////////////////////////////////////////
*   @Command: SEND_VERSION
*   @description: accept new config
*   @parametr: NULL
* 
*	@Answer: OK
*   @descriiption: 
*	@parametr: {"Version":"xx.yy.zz"}
*
*	@Answer: ERROR
*   @descriiption: error of accept new config 
*	@parametr: NULL
*/