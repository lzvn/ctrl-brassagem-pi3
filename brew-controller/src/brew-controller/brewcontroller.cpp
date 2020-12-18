#include "brewcontroller.h"

BrewController::BrewController(Timer *timer, int main_sensor_pin, Sensor *main_sensor, int main_actuator_pin, Actuator *main_actuator) {
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
		} else if(_devices[i][0] == main_actuator_pin) {
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
	/*
	_status = _BREW_STATE;
	_current_slope_addr = _CONF_END+1;
	run();
	*/
}

boolean BrewController::stop() {
	
}

boolean BrewController::reset() {
    /*
	_status = _REST_STATE;
	_timer->reset();
	_setConfig();
    */
}

boolean BrewController::activate(int output_pin) {
	if(_status != _STOP_BREW_STATE || _status != _REST_STATE) return false;
	
	_activateActuator(output_pin);
	return true;
}

boolean BrewController::deactivate(int output_pin) {
	if(_status != _STOP_BREW_STATE || _status != _REST_STATE) return false;
	
	_deactivateActuator(output_pin);
	return true;
}

boolean BrewController::run() {
	
}

boolean BrewController::setSlope(int position, unsigned int duration, float moist_temp, float tolerance) {
	int slope_addr = _getAddrOfSlope(position);
	boolean success = true;
	
	if(_status != _REST_STATE) success = false;

	if(success) {
		if(slope_addr < 0) {
			//cod inicio da rampa + cod processos extras + tamanhos dos parâmetros
			int size = 2 + _calcMemSize(duration) + _calcMemSize(moist_temp) + _calcMemSize(tolerance);
			success = _moveMemTail(_end_addr, _end_addr+size);
			if(success) {
				int addr = _end_addr;
				addr = _writeToMemory(addr, _SLOPE_START_ID);
				addr = _writeToMemory(addr, duration);
				addr = _writeToMemory(addr, moist_temp);
				addr = _writeToMemory(addr, tolerance);
				addr = _writeToMemory(addr, _NO_EXTRA_PROCS_ID);
			}
		} else {
			int addr = slope_addr + _STR2TIME;
			addr = _writeToMemory(addr, duration);
			addr = _writeToMemory(addr, moist_temp);
			addr = _writeToMemory(addr, tolerance);
		}
	}

	return success;
}

boolean BrewController::addProc2Slope(int position, int input_pin, int output_pin, float ref_value, float tolerance) {
	int slope_addr = _getAddrOfSlope(position);
	boolean success = true;
	
	if(_status != _REST_STATE) success = false;
	if(slope_addr < 0) success = false;

	int size = 2 + _calcMemSize(ref_value) + _calcMemSize(tolerance); //two from the pins + the rest
	int addr = 0;

	if(EEPROM.read(slope_addr+_STR2PROCID) == _NO_EXTRA_PROCS_ID) {
		size++;
		addr = slope_addr+_STR2PROCID+1;
	} else {
		EEPROM.write(slope_addr+_STR2PROCNUM, EEPROM.read(slope_addr+_STR2PROCNUM)+1);
		addr = slope_addr+_STR2PROCNUM+1;
		
		float num = EEPROM.read(addr);
		while(num!=_RECIPE_END_ID && num!=_SLOPE_START_ID) {
			addr++;
			num = EEPROM.read(addr);
		}
	}	
	success = _moveMemTail(addr, addr+size);
	if(success) {
		_end_addr += size;
		
		addr = _writeToMemory(addr, input_pin);
		addr = _writeToMemory(addr, ref_value);
		addr = _writeToMemory(addr, tolerance);
		addr = _writeToMemory(addr, output_pin);
	}

	return success;
}

boolean BrewController::rmvProc2Slope(int position, int input_pin, int output_pin, float ref_value, float tolerance) {
	int slope_addr = _getAddrOfSlope(position);
	boolean success = true;
	
	if(_status != _REST_STATE) success = false;
	if(slope_addr < 0) success = false;
	if(EEPROM.read(slope_addr+_STR2PROCID) == _NO_EXTRA_PROCS_ID) success = false;

	if(success) {
		int previous_addr = slope_addr+_STR2PROCNUM+1;
		int addr = previous_addr;
		float num = _readFromMemory(addr);
		float params[4] = {input_pin, ref_value, tolerance, output_pin}
		
		while(num != _SLOPE_START_ID && num != _RECIPE_END_ID) {
			
			if(num == params[0]) {
				boolean found = true;
				int size = 0;
				
				for(int i = 0; i < 4; i++) {
					if(num != params[i]) found = false;
					addr+=_calcMemSize(num);
					size += _calcMemSize(num);
					num = _readFromMemory(addr);
				}
				
				if(found) {
					addr++;
					previous_addr = addr - size;
					break;
				} else {
					addr ++;
					num = EEPROM._readFromMemory(addr);
					if(num == _SLOPE_START_ID || num == _RECIPE_END_ID) success = false;
				}
				
			} else {
				addr++;
				num = EEPROM._readFromMemory(addr);
				if(num == _SLOPE_START_ID || num == _RECIPE_END_ID) success = false;
			}
		}

		if(success) _moveMemTail(addr, previous_addr);
	}

	return success;
}

void BrewController::removeSlope(int position) {
	int addr = _getAddrOfSlope(position);
	
	if(_status != _REST_STATE) return;
	if(addr < 0) return;

	int next_addr = _getAddrOfSlope(position+1);
	if(next_addr < 0) {
		EEPROM.write(addr, _RECIPE_END_ID);
		for(int i = addr+1; i < _MEMORY_SIZE; i++) EEPROM.write(i, 0);
	} else {
		_moveMemTail(next_addr, addr);
	}
}

void BrewController::resetSlope(int position, boolean reset_all = false) {
	int addr = _getAddrOfSlope(position);
	
	if(_status != _REST_STATE) return;
	if(addr < 0) return;
	
	_resetSlope(addr, reset_all);
	if(reset_all) {
		int next_slope_addr = _getAddrOfSlope(position+1);
		_moveMemTail(next_slope_addr, addr+_STR2PROCNUM);
	}
}

void BrewController::resetAllSlopes() {
	if(_status != _REST_STATE) return;
	_clearMemory(false)	;
}

float BrewController::getSlopeTemp(int position) {
	float temp = 0;
	if(_status == _ERROR_STATE || _status == _REST_STATE) {
		temp = 0;
	} else {
		int slope_addr = _getAddrOfSlope(position);
		temp = _readFromMemory(slope_addr + _STR2TEMP);
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
	boolean success = true;
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
	
	if(index == _main_sensor_index || index == _main_actuator_index || index < 0 || _status != _REST_STATE) {
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
		reading = ((Sensor *) _devices[index][_DEV_COL])->read();

	return reading;
}

boolean BrewController::isActuatorOn(int pin) {
	boolean is_on = false;
	int index = _indexOfPin(pin);
	if(index>=0 && _devices[index][_TYPE_COL]==true && _devices[index][_DEV_COL]!=-1)
		is_on = ((Actuator *)(_devices[index][_DEV_COL]))->isActive();
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
	else duration = _readFromMemory(_current_slope_addr + _STR2TIME);
	return duration;
}

unsigned int BrewController::getMemoryLeft() {
	return _MEMORY_SIZE - _end_addr;
}

unsigned int BrewController::getStatus() {
	return _status;
}

//métodos privados

int BrewController::_writeToMemory(int addr, float number) {
	int next_addr = addr + _calcMemSize(number);

	//por hora, a escrita nos códigos numéricos não está habilitada	
	if(number < 0) next_addr = -1; //retirar se implementar negativos
	if((int) number != number) next_addr = -1; //retirar se implementar frações
	if(number > 249) next_addr = -1; //retirar se implementar numero maior que 249
	
	if(addr < 0 || addr >= _MEMORY_SIZE) next_addr = -1;

	if(next_addr > 0) EEPROM.write(addr, number);

	return next_addr;
}

float BrewController::_readFromMemory(int addr) {
	float reading = EEPROM.read(addr);
	return reading;
}

void BrewController::_clearMemory(boolean clear_config = true) {
	int addr = 0;

	if(clear_config)
		for(addr = 0; addr <= _CONF_END; addr++) EEPROM.write(addr, 0);
	_resetSlope(addr, true);
	_current_slope_addr = addr;
	addr += _STR2PROCID+1;
	EEPROM.write(addr, _RECIPE_END_ID);
	_end_addr = addr;
	
	for(addr = _end_addr+1; addr < _MEMORY_SIZE; addr++) EEPROM.write(addr, 0);
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
	actuator->act(0, true);
}

void BrewController::_deactivateActuator(int pin) {
	int index = _indexOfPin(pin);
	if(_devices[index][_TYPE_COL] != true) return;
	Actuator* actuator = (Actuator*) _devices[index][2];
	actuator->deactivate();
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
		if(position > 0 && EEPROM.read(addr) == _RECIPE_END_ID) {
			addr = -1;
			break;
		}
	}
	return addr;
}

void BrewController::_resetSlope(int slope_addr, boolean reset_all) {
	if(slope_addr <= _CONF_END) return;
	if(slope_addr >= _end_addr) return;
	while(EEPROM.read(slope_addr) != _SLOPE_START_ID) slope_addr--;
	
	int default_temp = 25;
	int default_time = 0;
	int default_tol = 1;
	
	EEPROM.write(slope_addr, _SLOPE_START_ID);
	EEPROM.write(slope_addr+_STR2TIME, default_time);
	EEPROM.write(slope_addr+_STR2TEMP, default_temp);
	EEPROM.write(slope_addr+_STR2TOL, default_tol);
	if(reset_all) EEPROM.write(slope_addr+_STR2PROCID, _NO_EXTRA_PROCS_ID);
}

boolean BrewController::_moveMemTail(int current_addr, int new_addr) {
	boolean success = true;
	boolean deleting = new_addr < current_addr; //true if memory is being deleted
	int distance = new_addr - current_addr; //distance the block will move

	if(current_addr == new_addr) success = false;
	if(!deleting && -distance >= _MEMORY_SIZE - _end_addr) success = false;
	if(current_addr > _end_addr) success = false;
	
	if(success) {
			int start_addr = 0;
			int end_addr = 0;
			int inc = 0;
		if(deleting) {
			start_addr = current_addr;
			end_addr = _end_addr;
			inc = 1;
		} else {
			start_addr = _end_addr;
			end_addr = current_addr;
			inc = -1;
		}

		for(int addr = start_addr; addr <= end_addr; addr += inc) {
			EEPROM.write(addr + distance, EEPROM.read(addr));
		}
		_end_addr += distance;
	
		if(!deleting) {
			EEPROM.write(current_addr, _SLOPE_START_ID);
			_resetSlope(current_addr, true);
		} else {
			for(int addr = _end_addr+1; addr < _MEMORY_SIZE; addr++) EEPROM.write(addr, 0);
		}
	}

	return success;
}

int BrewController::_calcMemSize(float number) {
	int size = 1;
	if((int) number != number) size = 5;
	else if(number > 250 && (int) number == number) size = 4;

	return size;
}
