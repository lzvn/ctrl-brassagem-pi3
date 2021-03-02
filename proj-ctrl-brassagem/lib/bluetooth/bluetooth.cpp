#include "Arduino.h"
#include "bluetooth.h"

Bluetooth::Bluetooth(int rx_pin, int tx_pin) {
	_phone = SoftwareSerial(rx_pin, tx_pin);
	_phone.begin(COMM_SPEED);
}

Bluetooth::~Bluetooth() {
	
}

void Bluetooth::sendUpdate(Msg updt) {
	if(updt.id < 0 || updt.id > PARAM_MAX) return;
	
	_send(_stringifyUpdt(updt));
}

boolean Bluetooth::cmdAvailable() {
	return _phone.available();
}

Msg Bluetooth::getCmd() {
	Msg cmd;
	String cmd_string = "";
	
	while(cmdAvailable()) {
		cmd_string += _phone.readString();
	}
	
	if(cmd_string != "") {
		cmd = _extractCmd(cmd_string);
	} else {
		cmd.id = ERROR_INVALID_CMD;
		for(int i = 0; i < MAX_MSG_PARAM; i++) cmd.params[i] = -1;
	}

	return cmd;
}

Msg Bluetooth::_extractCmd(String cmd_string) {
	String aux = "";
	int var_count = 0;
	Msg cmd;

	for(int i = 0; i < cmd_string.length(); i++) {
		if(cmd_string[i] != VAR_SEPARATOR) {
			aux+=cmd_string[i];
		} else {
			if(var_count == 0) {
				cmd.id = aux.toInt();
				
				if(cmd.id < 0 || cmd.id > CMD_MAX) {
					cmd.id = ERROR_INVALID_CMD;
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

String Bluetooth::_stringifyUpdt(Msg updt) {
	String str_updt = "";
	str_updt += updt.id;
	str_updt += VAR_SEPARATOR;
	for(int i = 0; i < MAX_MSG_PARAM; i++) {
		str_updt+= updt.params[i];
		str_updt+= VAR_SEPARATOR;
	}

	str_updt += MSG_END;
	
	return str_updt;
}

void Bluetooth::_send(String msg) {
	while(_phone.println(msg) <= msg.length());
}
