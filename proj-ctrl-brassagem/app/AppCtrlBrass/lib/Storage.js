import React from 'react';
import { NativeModules, NativeEventEmitter, AsyncStorage } from 'react-native';

/*
  things that will be done with the storage:
      -storage of recipes, composed of slopes (duration and temp) and extra processes (sensor, ref value, tolerance and actuator)
	  -storage of configs, which are the connected peripheral, sensors and actuators and their pins (there may be more)
*/

/*
//informações sobre como as receitas e sensores serão armazenados: 

exemplo de receita:
const example = {
	slopes_num: 4
	'0': {
		duration: 0,
		temp: 68,
		tolerance: 1,
		extra_procs: []
	},
	'1' : {
		duration: 15,
		temp: 78,
		tolerance: 1,
		extra_procs: [
			{
				sensor: 1, //sensor da lista de sensores e não o pino
				actuator: 1, //atuador da lista de atuadores e não o pino
				ref_value: 100,
				tolerance: 1
			}
		]
	},
	'2': {
		duration: 30,
		temp: 78,
		tolerance: 1,
		extra_procs: [
			{
				sensor: 1,
				actuator: 1,
				ref_value: 100,
				tolerance: 1
			}
		]
	},
	'3': {
		duration: 60,
		temp: 100,
		tolerance: 1,
		extra_procs: []
	}
};

//exemplo do objetos de dispositovos

const devices = {
	sensors: [2, 10], //pinos, o índice 0 é sempre para o principal
	actuators: [3, 11] //pino, o índice 0 é sempre para o principal
};

*/

const ERROR = -1;

const KEYS = {
	RECIPE: "recipe",
	DEVICES: "devices",
	PERIPHERAL: "peripheral"
};

const SLOPE = Object.freeze({
	duration: 0,
	temp: 0,
	tolerance: 0,
	extra_procs: []
});

const PROC = Object.freeze({
	sensor: 0,
	actuator: 1,
	ref_value: 0,
	tolerance: 0
});

/////////////////////// receitas e processos

async function setRecipe(recipe) {
	
}

async function addSlope(position, duration, slope_temp) {
	
}

async function rmvSlope(position) {
	
}

async function setSlope(position, duration, slope_temp) {
	
}

async function addProcToSlope(slope_pos, sensor, actuator, ref_value, tolerance) {
	
}

async function rmvProcOfSlope(slope_pos, proc_pos) {
	
}

async function getRecipe() {
	
}

async function getSlope(position) {
	
}

async function getProc(slope_pos, proc_pos) {
	
}

async function getNumOfSlopes() {
	
}

async function getNumOfProcs(slope_pos) {
	
}

/////////////////////// configurações

async function setDefaultPeripheral(peripheral) {
	
}

async function setControllerDevices(device_obj) {
	
}

async function getDefaultPeripheral() {
	
}

async function getControllerDevices() {
	
}

/////////////////////// outros

async function clearStorage() {
	let success = false;
	
	await AsyncStorage.clear()
		.then(() => {
			success = true;
			console.log("Memória limpa");
		})
		.catch((error) => {
			success = false;
			console.log("Erro ao limpar a memória: ");
			console.log(error);
		})

	return success;
}

async function _storeObj(key, obj) {
	let success = false;
	
	let obj_str = JSON.stringify(obj);

	//adicionar verificacoes depois

	await AsyncStorage.setItem(key, obj_str)
		.then(() => {
			success = true;
		})
		.catch(() => {
			success = false;
		})

	return success;
}

async function _getObj(key) {
	let obj = ERROR;
	let obj_str = "";

	obj_str = await AsyncStorage.getItem(key)
		.then(() => {
			obj = JSON.parse(obj_str);
		})
		.catch((error) => {
			obj = ERROR;
			console.log("Erro ao consultar objeto: " + error)
		})

	return obj;
}

async function _rmvObj(key) {
	let success = false;

	await AsyncStorage.removeItem(key)
		.then(() => {
			success = true;
		})
		.catch((error) => {
			success = false;
			console.log("Erro ao remover objeto: " + error);
		})

	return success;
}



const Storage = {	
};

export default Storage;
