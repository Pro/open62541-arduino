//
// Created by profanter on 26.02.18.
// Copyright (c) 2018 fortiss GmbH. All rights reserved.
//

#ifndef ARDUINO_OPCUA_GRIPPER_H
#define ARDUINO_OPCUA_GRIPPER_H

#include <Arduino.h>
#include <open62541.h>

class Gripper {

private:
	UA_Logger logger;
	static const uint8_t PIN_MOTOR_PWM = 22;
	static const uint8_t PIN_MOTOR_DIR = 23;
	UA_Boolean isOpen = UA_FALSE;

public:

	explicit Gripper(UA_Logger _logger): logger(_logger) {

	}

	void initHardware() {
		pinMode(PIN_MOTOR_PWM, OUTPUT);
		pinMode(PIN_MOTOR_DIR, OUTPUT);

		digitalWrite(PIN_MOTOR_PWM, 0);
		digitalWrite(PIN_MOTOR_DIR, 0);

		grip(UA_TRUE, 512);
	}

	bool grip(UA_Boolean open, UA_UInt16 speed);
};


#endif //ARDUINO_OPCUA_GRIPPER_H
