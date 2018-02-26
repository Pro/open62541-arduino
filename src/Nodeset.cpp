//
// Created by profanter on 26.02.18.
// Copyright (c) 2018 fortiss GmbH. All rights reserved.
//

#include "Nodeset.h"

#include <functional>


template <typename T>
struct Callback;

template <typename Ret, typename... Params>
struct Callback<Ret(Params...)> {
	template <typename... Args>
	static Ret callback(Args... args) {
		return func(args...);
	}
	static std::function<Ret(Params...)> func;
};

template <typename Ret, typename... Params>
std::function<Ret(Params...)> Callback<Ret(Params...)>::func;

/* This is not implemented on embedded devices.
 */
namespace std {
	void __throw_bad_function_call() { Serial.println("EXCEPTION: Bad function call"); while(1) {}}
}

UA_StatusCode Nodeset::gripperMethod(UA_Server *server,
			  const UA_NodeId *sessionId, void *sessionContext,
			  const UA_NodeId *methodId, void *methodContext,
			  const UA_NodeId *objectId, void *objectContext,
			  size_t inputSize, const UA_Variant *input,
			  size_t outputSize, UA_Variant *output) {
	if (inputSize != 2  || !UA_NodeId_equal(&input[0].type->typeId, &UA_TYPES[UA_TYPES_BOOLEAN].typeId) ||
			!UA_NodeId_equal(&input[1].type->typeId, &UA_TYPES[UA_TYPES_UINT16].typeId)) {
		UA_LOG_ERROR(logger, UA_LOGCATEGORY_USERLAND, "Invalid args for gripper method");
		return UA_STATUSCODE_BADINVALIDARGUMENT;
	}
	UA_Boolean dir = *((UA_Boolean*)input[0].data);
	UA_UInt16 speed = *((UA_UInt16*)input[1].data);

	return gripper.grip(dir, speed) ? UA_STATUSCODE_GOOD : UA_STATUSCODE_BADINVALIDSTATE;
}

UA_StatusCode Nodeset::addGripperMethod() {
	/* Two input arguments */
	UA_Argument inputArguments[2];
	UA_Argument_init(&inputArguments[0]);
	inputArguments[0].description = UA_LOCALIZEDTEXT("", "DoOpen");
	inputArguments[0].name = UA_STRING("DoOpen");
	inputArguments[0].dataType = UA_TYPES[UA_TYPES_BOOLEAN].typeId;
	inputArguments[0].valueRank = -1;

	UA_Argument_init(&inputArguments[1]);
	inputArguments[1].description = UA_LOCALIZEDTEXT("", "Speed");
	inputArguments[1].name = UA_STRING("Speed");
	inputArguments[1].dataType = UA_TYPES[UA_TYPES_UINT16].typeId;
	inputArguments[1].valueRank = -1; /* scalar */

	/* Add the method node */
	UA_MethodAttributes incAttr = UA_MethodAttributes_default;
	incAttr.description = UA_LOCALIZEDTEXT("", "Grip");
	incAttr.displayName = UA_LOCALIZEDTEXT("", "Grip");
	incAttr.executable = true;
	incAttr.userExecutable = true;

	Callback<UA_StatusCode(UA_Server*,const UA_NodeId*, void*, const UA_NodeId *, void *, const UA_NodeId *,
						   void *, size_t,
						   const UA_Variant *, size_t,
						   UA_Variant *)>::func = std::bind(&Nodeset::gripperMethod, this, std::placeholders::_1,
															std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
															std::placeholders::_5, std::placeholders::_6, std::placeholders::_7,
															std::placeholders::_8, std::placeholders::_9, std::placeholders::_10,
															std::placeholders::_11);
	UA_MethodCallback func = static_cast<UA_MethodCallback>(Callback<UA_StatusCode(UA_Server*,const UA_NodeId*, void*, const UA_NodeId *, void *, const UA_NodeId *,
																				   void *, size_t,
																				   const UA_Variant *, size_t,
																				   UA_Variant *)>::callback);


	UA_Server_addMethodNode(server, UA_NODEID_STRING(1, "Grip"),
							UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
							UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
							UA_QUALIFIEDNAME(1, "Grip"),
							incAttr, func,
							2, inputArguments, 0, NULL,
							NULL, NULL);
}