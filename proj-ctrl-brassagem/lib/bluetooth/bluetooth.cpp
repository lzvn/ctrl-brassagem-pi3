#include "Arduino.h"
#include "bluetooth.h"

Bluetooth::Bluetooth(int rx_pin, int tx_pin, BrewController *brewer) {
	_brewer = brewer;
	_phone = SoftwareSerial(rx_pin, tx_pin);
}

Bluetooth::~Bluetooth() {
	
}

void Bluetooth::sendUpdate(Update updt) {
	if(updt.param_code < 0 || updt.param_code > PARAM_MAX) return;
	
	_send(_stringifyUpdt(updt));
}

/////////////////////////////////////////////////
//TO BE COMPLETED
/////////////////////////////////////////////////
Update Bluetooth::getUpdate(int param_code) {
	Update updt;
	if(param_code < 0 || param_code > PARAM_MAX) {
		updt.param_code = ERROR_INVALID_CD;
		updt.value = ERROR_INVALID_CD;
	}

	if(update.param_code >= 0) {
		updt.param_code = param_code;

		switch(param_code) {
		case SLOPE_NUM_CD:
			updt.value = _brewer->getCurrentSlopeNumber();
			break;
		case TEMP_CD:
			break;
		case DURATION_CD:
			break;
		}
	}

	return updt;
}

void Bluetooth::updateAll() {
	for(int i = 0; i <= PARAM_MAX; i++) {
		sendUpdate(getUpdate(i));
		delay(100);
	}
}

boolean Bluetooth::cmdAvailable() {
	return _phone.available();
}

Command Bluetooth::_extractCmd(String cmd_string) {
	String aux = "";
	int var_count = 0;
	Command cmd;

	for(int i = 0; i < cmd_string.length(); i++) {
		if(cmd_string[i] != VAR_SEPARATOR) {
			aux+=cmd_string[i];
		} else {
			if(var_count == 0) {
				cmd.type = aux.toInt();
				
				if(cmd.type < 0 || cmd.type > CMD_MAX) {
					cmd.type = ERROR_INVALID_CD;
					for(int i = 0; i < 4; i++) cmd.params[i] = -1;
					break;
				}
				
				var_count++;
				aux = "";
				
			} else if(var_count > 0 && var_count < 5) {
				cmd.params[var_count-1] = aux.toFloat();
				var_count++;
				aux = "";
			} else {
				break;
			}
		}
	}
	
	return cmd;
}

String Bluetooth::_stringifyUpdt(Update updt) {
	String str_updt = "";
	str_updt+= updt.param_code;
	str_updt += VAR_SEPARATOR;
	str_updt += updt.value;
	return str_updt;
}
void Bluetooth::_send(String msg) {
	while(_phone.println(msg) <= msg.length());
}
