#include "Arduino.h"
#include "actuatorpidgas.h"

ActuatorPIDGas::ActuatorPIDGas(int first_pin, float ref_value, float tolerance, float kp, float ki, float kd) {
	for(int i = 0; i < 4; i++) _pins[i] = first_pin + i;
	_motor = AccelStepper(_MOTOR_INTERFACE, _pins[0], _pins[2], _pins[1], _pins[3]); //pinos trocados
	_motor.setCurrentPosition(_motor.currentPosition());
	_motor.setMaxSpeed(_SPEED);
	_motor.setAcceleration(_ACCEL);
	
	_ref_value = ref_value;
	_tolerance = tolerance;
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
	if(!_valve_open) return;
	deactivate();
	_setValveAngle(-1);
}

// métodos da interface

boolean ActuatorPIDGas::act(float input, boolean ignore_input = false) {
	boolean success = true; //não vou fazer verificações por falta de tempo

	if(ignore_input) {
		_setValveAngle(_MAX_ANGLE);
		_active = true;
	} else if(input >= _ref_value) {
		if(_active) {
		_setValveAngle(_MIN_ANGLE);
		_active = false;
		}
	} else {
		int time_step = millis() - _time_ref;
		_integral += (input - _last_input)*time_step;
		float new_angle = (input - _last_input)*_kd/time_step + _kp*input + _ki*_integral;
		if(new_angle > _MAX_ANGLE) new_angle = _MAX_ANGLE;
		if(new_angle < _MIN_ANGLE) new_angle = _MIN_ANGLE;

		//a tolerância vai ser usada como menor valor entre o angulo atual e o novo que causa movimento do motor
		//isso serve para reduzir o movimento do motor e seu aquecimento
		float angle_diff = (new_angle > _current_angle)?(new_angle - _current_angle):(_current_angle - new_angle);
		if(angle_diff > _tolerance) _setValveAngle(new_angle);
		
		_active = true;
	}

	_time_ref = millis();
	_last_input = input;
	return success;
}

void ActuatorPIDGas::deactivate() {
	if(!_valve_open && !_active) return;
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

//angulo em passos/volta completa
void ActuatorPIDGas::_setValveAngle(float angle) {
	if(!_valve_open && (angle > 0 && angle < _MAX_ANGLE)) _resetValve();
	if(angle > _MAX_ANGLE) angle = _MAX_ANGLE;
	else if(angle < _MIN_ANGLE && angle > 0) angle = _MIN_ANGLE;
	else if(angle <= 0) angle = (float) -30.0/_TOTAL_STEPS; //para o caso de fechamento da válvula

	int position = (int) angle*_TOTAL_STEPS;

	_motor.moveTo(position);
	_motor.setSpeed(_SPEED);
	_motor.runToNewPosition(position);
	_current_angle = angle;
}

void ActuatorPIDGas::_resetValve() {
	_valve_open = true;
	_setValveAngle(0);
	_setValveAngle(_MAX_ANGLE);
	delay(1000); //tempo para acender o fogo
	_setValveAngle(_MIN_ANGLE);
}
