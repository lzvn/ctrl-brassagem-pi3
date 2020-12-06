#include "brewcontroller.h"

BrewController::BrewController(Timer *timer, int main_sensor_pin, Sensor *main_sensor, Actuator *main_actuator) {
	_timer = timer;
	_main_sensor_pin = main_sensor_pin;
	_main_sensor = main_sensor;
	_main_actuator = main_actuator;
	reset();
}

BrewController::~BrewController() {
	
}

boolean BrewController::start() {
}

boolean BrewController::stop() {
	
}

boolean BrewController::reset() {
	
}

boolean BrewController::activate(int output_pin) {
	
}

boolean BrewController::run() {
	
}
