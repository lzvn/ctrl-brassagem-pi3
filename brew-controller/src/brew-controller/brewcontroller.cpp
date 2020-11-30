#include "brewcontroller.h"

BrewController::BrewController(Timer *timer) {
	_timer = timer;
	reset();
}

BrewController::~BrewController() {
	
}

boolean BrewController::start() {

	_timer->start();
}

boolean BrewController::stop() {
	
}

boolean BrewController::reset() {
	
}

boolean BrewController::activate(int output_pin) {
	
}

boolean BrewController::run() {
	
}
