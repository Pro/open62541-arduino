//
// Created by profanter on 26.02.18.
// Copyright (c) 2018 fortiss GmbH. All rights reserved.
//

#ifndef ARDUINO_OPCUA_NODESET_H
#define ARDUINO_OPCUA_NODESET_H

#include <Arduino.h>

#include "open62541.h"
#include "Gripper.h"

class Nodeset {
	UA_Server *server;
	UA_Logger logger;
	Gripper gripper;


	void initHardware() {
		gripper.initHardware();
	}

	UA_StatusCode gripperMethod(UA_Server *server,
				const UA_NodeId *sessionId, void *sessionContext,
				const UA_NodeId *methodId, void *methodContext,
				const UA_NodeId *objectId, void *objectContext,
				size_t inputSize, const UA_Variant *input,
				size_t outputSize, UA_Variant *output);

	UA_StatusCode addGripperMethod();
public:
	explicit Nodeset(UA_Server *_server, UA_Logger _logger): server(_server), logger(_logger), gripper(_logger) {
		initHardware();
	}

	UA_StatusCode createNodes() {
		return addGripperMethod();
	}

	virtual ~Nodeset() = default;
};


#endif //ARDUINO_OPCUA_NODESET_H
