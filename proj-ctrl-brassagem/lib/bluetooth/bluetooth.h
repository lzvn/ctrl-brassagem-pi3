#include "Arduino.h"
#include <SoftwareSerial.h>

#ifndef bluetooth_h
#define bluetooth_h

/**************************************************
O significado dos elementos de params:

OBS: booleanos são tratados como 0.0 para falso e 1.0 para verdadeiro

Cmd_codes:
  -ERROR_INVALID_CMD: todos os elementos são -1
  -CONNECTION: não tem importância
  -REQUEST: o primeiro elemento é o id do parâmetro requisitado, para PROC e PROC_READ
o segundo e o terceiros são os argumentos da função getControlProcess
  -UPDT_ALL: sem importância
  -START: sem importância
  -RESTART: sem importância
  -STOP: sem importância
  -RESET: sem importância
  -ACTIVATE: primeiro é o pino a ser ativado
  -DEACTIVATE: primeiro é o pino a ser desativado
  -SET_SLOPE: parâmetros de setSlope, na mesma ordem da declaração
  -RESET_SLOPE: parâmetros de resetSlope, na mesma ordem da declaração
  -RESET_ALL_SLOPES: parâmetros de resetAllSlope, na mesma ordem da declaração
  -RMV_SLOPE: primeiro é a posição da rampa a ser retirada
  -RMV_ALL_SLOPES: sem importância
  -ADD_PROC: parâmetros de addProc2Slope, na mesma ordem da declaração
  -RMV_PROC: parâmetros de rmvProc2Slope, na mesma ordem da declaração
  -CLR_MEM: sem importância

Param_codes
  -ERROR_INVALID_PRM: todos os elementos são -1
  -CMD_RETURN: o primeiro é o comando que foi executado, o segundo é o retorno
desse comando, ou -1 quando não há retorno nenhum
  -STATUS: primeiro é status do controlador
  -SLOPE_NUM: primeiro é o número da rampa atual
  -SLOPE_TEMP: primeiro é a temperatura da rampa atual
  -CURR_TEMP: temperatura lida no sensor principal
  -DURATION: duração da rampa atual em minutos
  -TIME_LEFT: tempo que falta para terminar a rampa atual em minutos (com fração)
  -MEM_LEFT: memória que falta em bytes
  -PIN_STATE: primeiro é o estado do pino (sua leitura em sensores e nível de saída
em atuadores) ou -1 se ele não está em uso e o segundo é 0 se o pino é sensor, 1
se é atuador e -1 se não está em uso
  -PROCS_NUM: primeiro é o número de processos extras da rampa atual
  -PROC: primeiro é o sensor do processo, segundo o atuador, terceiro valor de
referência e quarto a tolerância
  -PROC_READ: primeiro é a leitura do sensor
**************************************************/

#define CMD_MAX CLR_MEM
#define PARAM_MAX PROC_READ
#define VAR_SEPARATOR '|'
#define MSG_END '#'
#define MAX_MSG_PARAM 5
#define DEFAULT_RX 10
#define DEFAULT_TX 11
#define COMM_SPEED 9600

typedef struct {
	int id; //identifica comando ou atualização
	float params[MAX_MSG_PARAM]; //parâmetros possíveis
} Msg;

enum Cmd_codes {
	ERROR_INVALID_CMD = -1, //usado apenas para indicar erros
	CONNECTION,
	REQUEST,
	UPDT_ALL,
	START,
	RESTART,
	STOP,
	RESET,
	ACTIVATE,
	DEACTIVATE,
	SET_SLOPE,
	RESET_SLOPE,
	RESET_ALL_SLOPES,
	RMV_SLOPE,
	RMV_ALL_SLOPES,
	ADD_PROC,
	RMV_PROC,
	CLR_MEM
};

enum Param_codes {
	ERROR_INVALID_PRM = -1, //usado apenas para indicar erros
	CMD_RETURN,
	STATUS,
	SLOPE_NUM,
	SLOPE_TEMP, //temperatura da rampa
	CURR_TEMP, //temperatura lida
	DURATION,
	TIME_LEFT,
	MEM_LEFT,
	PIN_STATE,
	PROCS_NUM, //número de processos numa rampa
	PROC, //processo, com pinos de sensores e atuadores
	PROC_READ, //leitura do processo
};

#endif
