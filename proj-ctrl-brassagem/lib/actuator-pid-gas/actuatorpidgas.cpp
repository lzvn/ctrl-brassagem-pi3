#include "Arduino.h"
#include "actuatorpidgas.h"

ActuatorPIDGas::ActuatorPIDGas(int first_pin, float ref_value, float kp, float ki, float kd) {
	for(int i = 0; i < 4; i++) _pins[i] = first_pin + i;
	_motor = Stepper(_TOTAL_STEPS/4, _pins[0], _pins[1], _pins[2], _pins[3]);
	_motor.setSpeed(_SPEED);
	
	_ref_value = ref_value;
	_tolerance = 0;
	_active = false;
	_valve_open = false;
	_current_angle = 0;
	_kp = kp;
	_ki = ki;
	_kd = kd;
	_last_input = 0;
	_integral = 0;
	_time_ref = millis();
}

ActuatorPIDGas::~ActuatorPIDGas() {
}

int ActuatorPIDGas:: getFirstPin() {
	return _pins[0];
}

boolean ActuatorPIDGas::isValveOpen() {
	return _valve_open;
}

void ActuatorPIDGas::closeValve() {
	deactivate();
	_setValveAngle(0);
}

// métodos da interface

boolean ActuatorPIDGas::act(float input, boolean ignore_input = false) {
	boolean success = true; //não vou fazer verificações por falta de tempo

	if(ignore_input) {
		_setValveAngle(_MAX_ANGLE);
		_active = true;
	} else if(input >= _ref_value) {
		_setValveAngle(_MIN_ANGLE);
		_active = false;
	} else {
		int time_step = millis() - _time_ref;
		_integral += (input - _last_input)*time_step;
		int new_angle = (int) ( (input - _last_input)*_kd/time_step + _kp*input + _ki*_integral );
		if(new_angle > _MAX_ANGLE) new_angle = _MAX_ANGLE;
		if(new_angle < _MIN_ANGLE) new_angle = _MIN_ANGLE;
		_setValveAngle(new_angle);
		_active = true;
	}

	_time_ref = millis();
	_last_input = input;
	return success;
}

void ActuatorPIDGas::deactivate() {
	_setValveAngle(_MIN_ANGLE);
	_active = false;
}

void ActuatorPIDGas::setRefValue(float ref_value) {
	_ref_value = ref_value;
}

void ActuatorPIDGas::setTolerance(float tolerance) {
	if(tolerance < 0) tolerance = 0;
	_tolerance = tolerance;
}

boolean ActuatorPIDGas::isActive() {
	return _active;
}
	
float ActuatorPIDGas::getRefValue() {
	return _ref_value;
}

float ActuatorPIDGas::getTolerance() {
	return _tolerance;
}

// métodos privados

void ActuatorPIDGas::_setValveAngle(int angle) {
	if(!_valve_open) _resetValve();
	if(angle > _MAX_ANGLE) angle = _MAX_ANGLE;
	else if(angle < _MIN_ANGLE && angle > 0) angle = _MIN_ANGLE;
	else if(angle < 0) angle = 0;

	int distance = angle - _current_angle; //negativo para fechar, positivo para abrir
	distance = (int) (distance*_TOTAL_STEPS/360);
	if(distance != 0) _motor.step(distance);
	_current_angle = angle;
}

void ActuatorPIDGas::_resetValve() {
	_valve_open = true;
	_setValveAngle(0);
	_setValveAngle(_MAX_ANGLE);
	delay(1000); //tempo para acender o fogo
	_setValveAngle(_MIN_ANGLE);
}
