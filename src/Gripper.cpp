//
// Created by profanter on 26.02.18.
// Copyright (c) 2018 fortiss GmbH. All rights reserved.
//

#include "Gripper.h"


bool Gripper::grip(UA_Boolean open, UA_UInt16 speed) {

	UA_LOG_INFO(logger, UA_LOGCATEGORY_USERLAND, "Gripping with dir %d and speed %d", open, speed );

	if (open == isOpen)
		return false;

	digitalWrite(PIN_MOTOR_DIR, open ? HIGH : LOW);
	digitalWrite(PIN_MOTOR_PWM, speed > 0 ? HIGH : LOW);

	if (speed > 0) {
		delay(3000);
		digitalWrite(PIN_MOTOR_PWM, 0);
		isOpen = open;
	}


	return true;
}