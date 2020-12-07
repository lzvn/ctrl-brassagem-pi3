#include "brewcontroller.h"

BrewController::BrewController(Timer *timer, int main_sensor_pin, Sensor *main_sensor, Actuator *main_actuator) {
	_timer = timer;
	
	for(int i = 0; i < MAX_DEVICE_NUM; i++) {
		int pin = (i<4)?i+2:((i==4)?A0:A1);
		_devices[i][0] = pin;
		_devices[i][1] = false;
		_devices[i][2] = 0;

		if(_devices[i][0] == main_sensor_pin) {
			_main_sensor_index = i;
			_devices[i][2] = (int) main_sensor;
		} else if(_devices[i][0] == main_actuator->pin) {
			_main_actuator_index = i;
			_devices[i][1] = true;
			_devices[i][2] = (int) main_actuator;
		}
	}
	
	reset();
}

BrewController::~BrewController() {
	
}

boolean BrewController::start() {
	_active = true;
	_current_addr = CONF_END+1;
	_current_slope_addr = _current_addr;
	run();
}

boolean BrewController::stop() {
	
}

boolean BrewController::reset() {
	
}

boolean BrewController::activate(int output_pin) {
	if(_active != false) return false;
	
	_activateActuator(_indexOfPin(output_pin));
	return true;
}

boolean BrewController::deactivate(int output_pin) {
	if(_active != false) return false;
	
	_deactivateActuator(_indexOfPin(output_pin));
	return true;
}

boolean BrewController::run() {
	
}


//private methods

void BrewController::_activateActuator(int pin) {
	int index = _indexOfPin(pin);
	if(_devices[index][1] != true) return;
	Actuator* actuator = (Actuator*) _devices[index][2];
	actuator->active = true;
	digitalWrite(actuator->pin, HIGH);
}

void BrewController::_deactivateActuator(int pin) {
	int index = _indexOfPin(pin);
	if(_devices[index][1] != true) return;
	Actuator* actuator = (Actuator*) _devices[index][2];
	actuator->active = false;
	digitalWrite(actuator->pin, LOW);
}
