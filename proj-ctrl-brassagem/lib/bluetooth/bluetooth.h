#include "Arduino.h"
#include <SoftwareSerial.h>

#ifndef bluetooth_h
#define bluetooth_h

enum Cmd_codes {
	ERROR_INVALID_CD = -1, //usado apenas para indicar erros
	REQUEST_CD,
	START_CD
};

enum Param_codes {
	ERROR_INVALID_CD = -1, //usado apenas para indicar erros
	SLOPE_NUM_CD,
	TEMP_CD,
	DURATION_CD		
};

#define CMD_MAX START_CD
#define PARAM_MAX DURATION_CD
#define VAR_SEPARATOR '|'

typedef struct {
	int type; //código que identifica o comando
	float params[4]; //parâmetros possíveis do comando
} Command;

typedef struct {
	int param_code; //código que identifica qual variável a ser atualizada
	float value; //o novo valor
} Update

class Bluetooth {
public:
	Bluetooth(int rx_pin, int tx_pin, BrewController *brewer);
	~Bluetooth();

	void sendUpdate(Update updt); //envia uma atualização ao aplicativo
	Update getUpdate(int param_code);
	void updateAll(); //atualiza todas as variáveis
	boolean cmdAvailable(); //verifica se há um comando disponível
	
private:
	Command _extractCmd(String cmd_string); //transforma a string recebida em um comando
	String _stringifyUpdt(Update updt);
	void _send(String msg);

	SoftwareSerial _phone;
	BrewController *_brewer;
};

#endif
