#include "Arduino.h"
#include <SoftwareSerial.h>
#include "bluetooth.h"

Interface::Interface(SoftwareSerial* phone) {
	_phone = phone;
	_phone->begin(COMM_SPEED);
}

Interface::~Interface() {

}

void Interface::sendUpdate(Msg updt, int params_num = MAX_MSG_PARAM, boolean intify = false) {
	if(updt.id < 0 || updt.id > PARAM_MAX) return;
	_send(_stringifyUpdt(updt, params_num, intify));
}

boolean Interface::cmdAvailable() {
	return _phone->available();
}

Msg Interface::getCmd() {
	Msg cmd;
	String cmd_string = "";

	while(cmdAvailable()) {
		cmd_string += _phone->readString();
	}

	Serial.print("Comando recebido: ");
	Serial.println(cmd_string);

	if(cmd_string != "") {
		cmd = _extractCmd(cmd_string);
	} else {
		cmd.id = ERROR_INVALID_CMD;
		for(int i = 0; i < MAX_MSG_PARAM; i++) cmd.params[i] = -1;
	}

	return cmd;
}

Msg Interface::_extractCmd(String cmd_string) {
	String aux = "";
	int var_count = 0;
	Msg cmd;

	for(int i = 0; i < cmd_string.length(); i++) {
		if(cmd_string[i] == MSG_END) {
			continue;
		} else if(cmd_string[i] != VAR_SEPARATOR && cmd_string[i] != MSG_END) {
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

String Interface::_stringifyUpdt(Msg updt, int params_num = MAX_MSG_PARAM, boolean intify = false) {
	String str_updt = "";
	str_updt += updt.id;
	str_updt += VAR_SEPARATOR;
	
	for(int i = 0; i < params_num; i++) {
		if(updt.params[i] == 0 || intify)	str_updt+= (int) updt.params[i];
		else str_updt+= updt.params[i];
		
		str_updt+= VAR_SEPARATOR;
	}

	if(params_num < MAX_MSG_PARAM) {
		str_updt += MSG_END;
		str_updt += MSG_END;
	}

	str_updt += DELIMITER;
	str_updt += DELIMITER;

	return str_updt;
}

void Interface::_send(String msg) {
	Serial.print("Msg to be sent: ");
	Serial.println(msg);
	
	for(int i = 0; i < msg.length(); i++) {
		_phone->print(msg[i]);
		delay(10);
	}
}
