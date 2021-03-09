import React from 'react';
import { NativeModules, NativeEventEmitter } from 'react-native';

import BluetoothSerial from '@tecsinapse/react-native-serial-bt';
import RNBluetoothClassic from 'react-native-bluetooth-classic';

//OBS: no final do arquivo, há partes do código de comunicação do controlador para referência

const ERROR = -1;
let CMD_MAX = 0;
let PARAM_MAX = 0;
const VAR_SEPARATOR = '|';
const MSG_END = '#';
const MAX_MSG_PARAM = 5;

const ANALOG_PINS = {
	"A0": 14,
	"A1": 15,
	"A2": 16,
	"A3": 17,
	"A4": 18,
	"A5": 19
}

const updt_keys = [
	"status",
	"slope_num",
	"slope_temp",
	"curr_temp",
	"duration",
	"time_left",
	"mem_left",
	"pin_read",
	"procs_num",
	"proc",
	"proc_read"
];

const proc_keys = [
	"sensor",
	"actuator",
	"ref_value",
	"tolerance"
];

let NO_CTRL = {};
for(let i = 0; i < updt_keys.length; i++) {
	if(updt_keys[i] === "pin_read") {
	} else if(updt_keys[i] === "proc" || updt_keys[i] === "proc_read") {
		NO_CTRL[updt_keys[i]] = [];
	} else {
		NO_CTRL[updt_keys[i]] = 0;
	}
}
NO_CTRL.sensors = new Array(2).fill(0);
NO_CTRL.actuators = new Array(2).fill(0);

let CMD_CODES = {};
let PARAM_CODES = {};

let MSG = {
	id: 0,
	params: new Array(MAX_MSG_PARAM).fill(0)
};

let controller = {};
let started = false;

async function start() {
	let enabled = await RNBluetoothClassic.isBluetoothEnabled();
	if(!enabled) await RNBluetoothClassic.requestBluetoothEnabled();
	BluetoothSerial.withDelimiter(MSG_END);
	if(BluetoothSerial.isConnected()) BluetoothSerial.disconnect();

	let cmd_keys = [
		"ERROR_INVALID_CMD",
		"CONNECTION",
		"REQUEST",
		"UPDT_ALL",
		"START",
		"RESTART",
		"STOP",
		"RESET",
		"ACTIVATE",
		"DEACTIVATE",
		"SET_SLOPE",
		"RESET_SLOPE",
		"RESET_ALL_SLOPES",
		"RMV_SLOPE",
		"RMV_ALL_SLOPES",
		"ADD_PROC",
		"RMV_PROC",
		"CLR_MEM"
	];
	let param_keys =[
		"ERROR_INVALID_PRM",
		"CMD_RETURN",
		"STATUS",
		"SLOPE_NUM",
		"SLOPE_TEMP",
		"CURR_TEMP",
		"DURATION",
		"TIME_LEFT",
		"MEM_LEFT",
		"PIN_STATE",
		"PROCS_NUM",
		"PROC",
		"PROC_READ",
	]
	
	let i = -1;
	cmd_keys.forEach((key) => {
		CMD_CODES[key] = i;
		i++
	});
	i = -1;
	param_keys.forEach((key) => {
		PARAM_CODES[key] = i;
		i++;
	});
	CMD_MAX = CMD_CODES[cmd_keys[cmd_keys.length -1]];
	PARAM_MAX = PARAM_CODES[param_keys[param_keys.length -1]];
	
	started = true;
	console.log("Dispositivo iniciado");
}

async function isConnected() {
	return await BluetoothSerial.isConnected();
}

async function getPairedDevices() {
	if(!started) return ERROR;
	
	let devices = await BluetoothSerial.list();
	if(devices.length === 0) devices = -1;
	
	return devices;
}

async function chooseDevice(id) {
	if(!started) return ERROR;
	
	let peripherals = await getPairedDevices();
	let success = ERROR;
	let peripheral_index = 0;

	if(peripherals === []) {
		success = ERROR;
		alert("Erro ao validar o dispositivo escolhido");
	}
	if(typeof id !== "string") success = ERROR;

	peripherals.forEach((peripheral) => {
		if(peripheral.id === id) {
			success = true;
			return;
		} else {
			peripheral_index++;
		}
	})

	if(success !== ERROR) {
		controller = peripherals[peripheral_index];

		if(BluetoothSerial.isConnected()) await BluetoothSerial.disconnect();
		await BluetoothSerial.connect(controller.id)
			.then(() => {
				success = true;
				sendCmd({id: CMD_CODES.CONNECTION, params: new Array(0).fill(0)});
				console.log("Dispotivo conectado");
			})
			.catch((error) => {
				success = ERROR;
				console.log("Erro ao conectar-se ", error);
			});
	}
	return success;
}

//faz um objeto do tipo mensagem e o retorna
function makeMsg(id, params=[0]) {
	if(id < -1 || id > CMD_MAX) return ERROR;
	//if(params.length === 0) return ERROR;
	
	let msg = Object.create(MSG);
	msg.id = id;
	for(let i = 0; i < MAX_MSG_PARAM; i++) {
		if(params[i] !== undefined) msg.params[i] = params[i];
		else msg.params[i] = 0;
	}

	return msg;
}

//envia um comando e retorna a saída dada pelo controlador
async function sendCmd(msg, return_cmplt_msg = false) {
	if(!started) return ERROR;

	let cmd_return = ERROR;
	let msg_str = _stringifyMsg(msg);
	
	//por ser uma atualização, vou processar o UPDT_ALL por uma função própria
	if(msg.id === CMD_CODES.UPDT_ALL) {
		msg_str = ERROR;
	}

	if(msg_str !== ERROR) {
		await BluetoothSerial.write(msg_str);
		let return_msg = await _getIncomingMsg();

		if(return_msg !== ERROR) {
			cmd_return = _extractMsg(return_msg);
		} else {
			cmd_return === ERROR;
		}
	}

	console.log("cmd return ", cmd_return);

	if(cmd_return === undefined) cmd_return = ERROR;
	
	if(cmd_return !== ERROR && return_cmplt_msg) {
		if(cmd_return.params[0]) cmd_return = cmd_return.params[0];
		else cmd_return === ERROR;
	}

	return cmd_return;
}

/*igual a sendCmd, porém especializada para requisição de informações
assim, não é necessária uma mensagem como argumento e ela retorna um objeto
próprio com as variáveis atualizadas
params deve ser conforme o REQUEST nas informações no fim*/
async function request(params) {
	if(!started) return ERROR;

	//por problemas de sincronia e lentidão na comunicação bluetooth
	//(essa por parte do hc-05), o comando UPDT_ALL foi substituído por
	//várias requisições que abordam todos os parâmetros

	let updt = {};

	let msg = makeMsg(CMD_CODES.REQUEST, params);
	let updt_msg = await sendCmd(msg);
	let key = "";

	switch(updt_msg.id) {
	case PARAM_CODES.STATUS:
	case PARAM_CODES.SLOPE_NUM:
	case PARAM_CODES.SLOPE_TEMP:
	case PARAM_CODES.CURR_TEMP:
	case PARAM_CODES.DURATION:
	case PARAM_CODES.TIME_LEFT:
	case PARAM_CODES.MEM_LEFT:
	case PARAM_CODES.PROCS_NUM:
	case PARAM_CODES.PROC_READ:
		updt[updt_keys[updt_msg.id-1]]  = updt_msg.params[0];
		break;
	case PARAM_CODES.PIN_STATE:
		key = updt_keys[updt_msg.id-1];
		updt[key] = {};
		updt[key].state = updt_msg.params[0];
		updt[key].type = updt_msg.params[1];
		break;			
	case PARAM_CODES.PROC:
		key = updt_keys[updt_msg.id-1];
		updt[key] = {};
		for(let i = 0; i < keys.length; i++)
			updt[key][keys[i]] = updt_msg.params[i];
		break;
	default:
		updt = -1;
	}
	
	return updt;
}

//retorna um objeto com uma atualização completa
//se ignore_procs for falso, o método pula os parâmetros procs e proc_read,
//se for dado um argumento devices, ele também retorna os estados dos pinos 
//neste devices
async function getFullUpdt(ignore_procs = true, devices = undefined) {
	let updt = undefined;

	let msg = makeMsg(CMD_CODES.UPDT_ALL, [0]);
	await sendCmd(msg);

	let proc_pos = 0;
	let procs_num = 0;
	
	let updt_msg = await _getIncomingMsg();
	while(updt_msg !== ERROR) {
		updt = {};
		switch(updt_msg.id) {
		case PARAM_CODES.PROCS_NUM:
			procs_num = updt_msg.params[0];
		case PARAM_CODES.STATUS:
		case PARAM_CODES.SLOPE_NUM:
		case PARAM_CODES.SLOPE_TEMP:
		case PARAM_CODES.CURR_TEMP:
		case PARAM_CODES.DURATION:
		case PARAM_CODES.TIME_LEFT:
		case PARAM_CODES.MEM_LEFT:
			updt[updt_keys[updt_msg.id-1]]  = updt_msg.params[0];
			break;
		case PARAM_CODES.PROC_READ:
			if(ignore_procs) {
				updt = ERROR;
				updt_msg = ERROR;
				break;
			}
			if(proc_pos === 0) updt[updt_keys[updt_msg.id-1]] = [];
			updt[updt_keys[updt_msg.id-1]].push(updt_msg.params[0]);
			proc_pos = (proc_pos >= procs_num)?0:procs_pos+1;
		case PARAM_CODES.PROC:
			if(ignore_procs) {
				updt = ERROR;
				updt_msg = ERROR;
				break;
			}
			let key = updt_keys[updt_msg.id-1];
			if(procs_pos === 0 ) updt[key] = [];

			let proc_obj = {}
			for(let i = 0; i < keys.length; i++)
				proc_obj[key][keys[i]] = updt_msg.params[i];
			updt[key].push(proc_obj);
			break;
		default:
			updt = ERROR;
			
		}
		updt_msg = (updt===ERROR)?ERROR:(await _getIncomingMsg());
	}

	if(devices !== undefined && updt !== ERROR && updt !== undefined) {
		let pin_updt = await getReadingsUpdt(devices);
		if(pin_updt !== ERROR) {
			updt.sensors = pin_updt.sensors;
			updt.actuators = pin_updt.actuators;
		}
	}

	if(updt === ERROR) updt = NO_CTRL;

	return updt;
}

async function getReadingsUpdt(devices) {
	let updt = { sensors: [], actuators: []};

	if(devices.sensors === undefined || devices.actuators === undefined) return ERROR;
	if(devices.sensors.length === 0 || devices.actuators.length === 0) return ERROR;

	let max_length = 0;
	let keys = [];

	if(devices.sensors.length > devices.actuators.length){
		max_length = devices.sensors.length;
		keys = ["sensors", "actuators"];

	} else {
		max_length = devices.actuators.length;
		keys = ["actuators", "sensors"];
	}

	for(let i = 0; i < max_length; i++) {
		for(let j = 0; j < (i < devices[keys[1]].length)?2:1; j++) {
			let pin = (devices[keys[j]][i] < ANALOG_PINS["A0"])?devices[keys[j]][i]:ANALOG_PINS[[keys[j]]];
			let state = await request([CMD_CODES.PIN_STATE, pin]);
			updt[keys[j]].push(state.state);
		}
	}

	return updt;
}

async function testConnection() {
	if(!started) return false;
	if(!BluetoothSerial.isConnected()) return false;

	let success = false;

	let msg = "Kirk to Enterprise. Spock, do you read me?";
	let expected_answer = "Captain Kirk, this is Spock. We are listening";

	BluetoothSerial.write(msg + MSG_END).then(() => {
		console.log("Mensagem enviada: " + msg);
	});

	let received = await _getIncomingMsg();
	if(received === ERROR) {
		console.log("Houve um erro");
	} else if(received) {
		console.log("Mensagem recebida: " + received);
		if(received === expected_answer) success = true;
	} else {
		console.log("Nada recebido");
	}

	return success;
}

function _stringifyMsg(msg) {
	if(msg.id === undefined || msg.params === undefined) return ERROR;
	
	let msg_str = "";
	msg_str += String(msg.id) + VAR_SEPARATOR;

	for(let i = 0; i < MAX_MSG_PARAM; i++) {
		msg_str += (msg.params[i] !== undefined)?String(msg.params[i]):"0";
		msg_str += VAR_SEPARATOR;
	}

	return msg_str;
}

//extrai uma mensagem de uma string
function _extractMsg(msg_str) {
	let separators = 0;
	for(let i = 0; i < msg_str.length; i++) {
		if(msg_str[i] === VAR_SEPARATOR) separators++;
	};
	if(separators < 5) return ERROR;
	
	let msg = JSON.parse(JSON.stringify(MSG));
	let value = "";
	let aux = -1;

	for(let i = 0; i < msg_str.length; i++) {
		let character = msg_str[i];
		if(character === VAR_SEPARATOR) {
			if(aux < 0) msg.id = Number(value);
			else if(aux >= 0 && aux < MAX_MSG_PARAM) msg.params[aux] = Number(value);
			value = "";
			aux++;
		} else {
			value+=character;
		}
	};

	return msg;
}

//recebe uma mensagem esperada
async function _getIncomingMsg() {
	if(!started) return ERROR;

	let msg_string = "";
	
	let available = await BluetoothSerial.available();
	let min_msg_size = 25;
	let tries = 0;
	let max_tries = 1000;

	while(tries < max_tries) {
		if(available > 0) {
			tries = 0;
			await BluetoothSerial.readFromDevice()
				.then((data) => {
					msg_string += data;
					console.log("msg_string ", msg_string);
				})
			.catch((error) => {
				console.log("Mensagem não recebida por " + error);
				msg_string = ERROR;
			});
		}

		if(msg_string === ERROR) break;
		available = await BluetoothSerial.available();
		tries++;
	}
	
	if(msg_string.length < min_msg_size || msg_string === ERROR) {
		console.log("TIMEOUT");
		msg_string = ERROR;
	}

	BluetoothSerial.clear();
	return msg_string;
}

const BltComm = {
	start: start,
	isConnected: isConnected,
	getPairedDevices: getPairedDevices,
	chooseDevice: chooseDevice,
	makeMsg: makeMsg,
	sendCmd: sendCmd,
	request, request,
	getFullUpdt: getFullUpdt,
	getReadingsUpdt: getReadingsUpdt,
	testConnection: testConnection,
	CMD_CODES: CMD_CODES,
	PARAM_CODES: PARAM_CODES,
	ANALOG_PINS: ANALOG_PINS,
	NO_CTRL: NO_CTRL
}

export default BltComm;
/*****************************************************************
O significado dos elementos de params:

OBS: booleanos são tratados como 0.0 para falso e 1.0 para verdadeiro

Cmd_codes:
  -ERROR_INVALID_CMD: todos os elementos são -1
  -CONNECTION: não tem importância
  -REQUEST: o primeiro elemento é o id do parâmetro requisitado, para PROC e PROC_READ
o segundo e o terceiros são os argumentos da função getControlProcess, para PIN_STATE,
o segundo é o valor do pino
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

*****************************************************************
Partes do código que são úteis: 

#define CMD_MAX CLR_MEM
#define PARAM_MAX UPDT_ALL
#define VAR_SEPARATOR '|'
#define MSG_END '-'
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
*****************************************************************/
