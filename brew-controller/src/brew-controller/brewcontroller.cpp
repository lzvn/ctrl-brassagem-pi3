#include "brewcontroller.h"

BrewController::BrewController(Timer *timer, int main_sensor_pin, Sensor *main_sensor, Actuator *main_actuator) {
	_timer = timer;
	boolean success = false;
	
	for(int i = 0; i < _MAX_DEVICE_NUM; i++) {
		int pin = (i<4)?i+2:((i==4)?A0:A1);
		_devices[i][_PIN_COL] = pin;
		_devices[i][_TYPE_COL] = false;
		_devices[i][_DEV_COL] = -1;

		if(_devices[i][0] == main_sensor_pin) {
			_main_sensor_index = i;
			_devices[i][_DEV_COL] = (int) main_sensor;
			success = true;
		} else if(_devices[i][0] == main_actuator->pin) {
			_main_actuator_index = i;
			_devices[i][_TYPE_COL] = true;
			_devices[i][_DEV_COL] = (int) main_actuator;
			success = true;
		}
	}

	_setConfig();
	reset();

	if(!success) _setErrorState();
}

BrewController::~BrewController() {
	
}

boolean BrewController::start() {
	_active = true;
	_current_slope_addr = _CONF_END+1;
	run();
}

boolean BrewController::stop() {
	
}

boolean BrewController::reset() {
	_active = false;
	_timer->reset();
	_setConfig();
}

boolean BrewController::activate(int output_pin) {
	if(_active != false) return false;
	
	_activateActuator(output_pin);
	return true;
}

boolean BrewController::deactivate(int output_pin) {
	if(_active != false) return false;
	
	_deactivateActuator(output_pin);
	return true;
}

boolean BrewController::run() {
	
}

boolean BrewController::setSlope(int position, unsigned int duration, float moist_temp, float tolerance) {
	
}

boolean BrewController::addProc2Slope(int position, int input_pin, int output_pin, float ref_value, float tolerance) {
	
}

void BrewController::resetSlope(int position) {
	
}

void BrewController::resetAllSlopes() {

	if(_status == _ERROR_STATE || _status == _STOP_BREW_STATE || _status == _BREW_STATE)
		return;

	_clearMemory(false)	;
}

float BrewController::getSlopeTemp(int position) {
	float temp;
	if(_status == _ERROR_STATE || _status == _REST_STATE) {
		temp = 0;
	} else {
		temp = _readFromMemory(_getAddrOfSlope(position)+1);
	}
	return temp;
}

int BrewController::getCurrentSlopeNumber() {
	int slope_num = 1;
	int addr = _CONF_END+2; //a primeira rampa foi contada na declaração de slope_num pois é obrigatória
	while(addr != _current_slope_addr && addr != _end_addr) {
		if(EEPROM.read(addr) == _SLOPE_START_ID) slope_num++;
	}
	return slope_num;
}

//talvez eu devesse juntar esses num addDevice
boolean BrewController::addSensor(int pin, Sensor *sensor) {
	boolean = success = true;
	int index = _indexOfPin(pin);
	if(index < 0 || _devices[index][_DEV_COL] == -1 || _status == _ERROR_STATE || _status == _REST_STATE)
		success = false;
	
	if(success) {
		_devices[index][_TYPE_COL] = false;
		_devices[index][_DEV_COL] = (int) sensor;
	}

	return success;
}

boolean BrewController::addActuator(int pin, Actuator *actuator) {
	boolean success = true;
	int index = _indexOfPin(pin);
	if(index < 0 || _devices[index][_DEV_COL] == -1 || _status == _ERROR_STATE || _status == _REST_STATE)
		success = false;
	
	if(success) {
		_devices[index][_TYPE_COL] = true;
		_devices[index][_DEV_COL] = (int) actuator;
	}

	return success;
}

boolean BrewController::clear(int pin) {
	if(_status == _ERROR_STATE) return false;
	
	int index = _indexOfPin(pin);
	boolean success = true;
	
	if(index == _main_sensor_index || index == _main_actuator_index || index < 0 || _active == true) {
		success = false;
	} else {
		_devices[index][_TYPE_COL] = false;
		_devices[index][_DEV_COL] = -1;
	}

	return success;
}

boolean BrewController::isPinInUse(int pin) {
	int index = _indexOfPin(pin);
	boolean pin_in_use = true;
	if(index >= 0 && _devices[index][_DEV_COL] == -1) pin_in_use = false;
	
	return pin_in_use;
}

float BrewController::getSensorReading(int pin) {
	float reading = _MIN_INF;
	int index = _indexOfPin(pin);

	if(_status == _ERROR_STATE) return reading;

	if(index >=0 && _devices[index][_TYPE_COL] == false && _devices[index][_DEV_COL] != -1)
		reading = (Sensor *) _devices[index][_DEV_COL]->read();

	return reading;
}

boolean BrewController::isActuatorOn(int pin) {
	boolean is_on = false;
	int index = _indexOfPin(pin);
	if(index>=0 && _devices[index][_TYPE_COL]==true && _devices[index][_DEV_COL]!=-1)
		is_on = (Actuator *)(_devices[index][_DEV_COL])->active;
	return is_on;
}

unsigned int BrewController::getTimeLeft() {
	unsigned int time_left;
	if(_status == _ERROR_STATE || _status == _REST_STATE) time_left = 0;
	else time_left = _timer->timeLeft();
	return time_left;
}

unsigned int BrewController::getCurrentSlopeDuration() {
	unsigned int duration;
	if(_status == _ERROR_STATE || _status == _REST_STATE) duration = 0;
	else duration = _readFromMemory(_current_slope_addr + 1);
	return duration;
}

unsigned int BrewController::getMemoryLeft() {
	return _MEMORY_SIZE - _end_addr;
}

unsigned int BrewController::getStatus() {
	return _status;
}

//métodos privados

float _readFromMemory(int addr) {
	return EEPROM.read(addr);
}

void _clearMemory(boolean clear_config = true) {
	int addr = 0;
	int default_temp = 25;
	int default_time = 0;
	int default_tol = 1;
	if(clear_config)
		for(addr = 0; addr <= _CONF_END; addr++) EEPROM.write(addr, 0);
	EEPROM.write(addr, _SLOPE_START_ID);
	EEPROM.write(addr+1, default_time);
	EEPROM.write(addr+2, default_temp);
	EEPROM.write(addr+3, default_tol);
	EEPROM.write(addr+4, _NO_EXTRA_PROCS_ID);
	EEPROM.write(addr+5, _RECIPE_END_ID);
	_end_addr = addr+5;
	_current_slope_addr = addr;
	
	for(addr = _end_addr+1; addr < _MEMORY_SIZE; addr++) EEPROM.write(addr, 0)
}

void BrewController::_setConfig() {
}

int BrewController::_indexOfPin(int pin) {
	int index = -1;
	for(int i = 0; i < _MAX_DEVICE_NUM; i++) {
		if(_devices[i][_PIN_COL] == pin) {
			index = i;
			break;
		}
	}
	return index;
}

void BrewController::_activateActuator(int pin) {
	int index = _indexOfPin(pin);
	if(_devices[index][_TYPE_COL] != true) return;
	Actuator* actuator = (Actuator*) _devices[index][2];
	actuator->active = true;
	digitalWrite(actuator->pin, HIGH);
}

void BrewController::_deactivateActuator(int pin) {
	int index = _indexOfPin(pin);
	if(_devices[index][_TYPE_COL] != true) return;
	Actuator* actuator = (Actuator*) _devices[index][2];
	actuator->active = false;
	digitalWrite(actuator->pin, LOW);
}

void BrewController::_setErrorState() {
	_status = _ERROR_STATE;
	reset();
}

int BrewController::_getAddrOfSlope(int position) {
	int addr = _CONF_END+1;
	while(position>0) {
		if(EEPROM.read(addr) == _SLOPE_START_ID) position--;
		if(position > 0) addr++;
	}
	return addr;
}
