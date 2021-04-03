#include "Arduino.h"
#include "actuatoronoff.h"

ActuatorOnOff::ActuatorOnOff(int pin, float ref_value = 25, float tolerance = 1) {
	_pin = pin;
	pinMode(_pin, OUTPUT);
	_ref_value = ref_value;
	_tolerance = (tolerance>=0)?tolerance:-tolerance;
	_active = false;
	_working = true;
}

ActuatorOnOff::~ActuatorOnOff() {
	
}

int ActuatorOnOff:: getPin() {
	return _pin;
}

boolean ActuatorOnOff::act(float input, boolean ignore_input = false) {
	//working é realmente necessário?
	boolean success = true;
	
	if(ignore_input) {
		_active = true;
		_working = true;
		digitalWrite(_pin, _ON);
	} else {
		if(input >= _ref_value) {
			digitalWrite(_pin, _OFF);
			_working = false;
			_active = false;
		} else if(input >= (_ref_value - _tolerance) && input < _ref_value) {
			if(_working) {
				digitalWrite(_pin, _ON);
				_active = true;
			} else {
				digitalWrite(_pin, _OFF);
				_active = false;
			}
		} else if(input < (_ref_value - _tolerance)) {
			digitalWrite(_pin, _ON);
			_active = true;
			_working = true;
		}
	}

	return success;
}

void ActuatorOnOff::deactivate() {
	digitalWrite(_pin, _OFF);
	_active = false;
	_working = false;
}

void ActuatorOnOff::setRefValue(float ref_value) {
	_ref_value = ref_value;
}
	
void ActuatorOnOff::setTolerance(float tolerance) {
	_tolerance = (tolerance>=0)?tolerance:-tolerance;
}

boolean ActuatorOnOff::isActive() {
	return _active;
}
	
float ActuatorOnOff::getRefValue() {
	return _ref_value;
}
	
float ActuatorOnOff::getTolerance() {
	return _tolerance;
}
