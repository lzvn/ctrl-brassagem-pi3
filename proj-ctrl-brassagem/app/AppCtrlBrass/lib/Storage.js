import React from 'react';
import { NativeModules, NativeEventEmitter } from 'react-native';

//por hora, desisto de fazer memória permanente
//tento de novo se der tempo

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

//exemplo do objetos de dispositivos
const devices = {
	sensors: [2, 10], //pinos, o índice 0 é sempre para o principal
	actuators: [3, 11] //pino, o índice 0 é sempre para o principal
};

*/

//////////////////////////////////////////////////////////
//retirar se conseguir implementar uma memória permanente

const ERROR = -1;

const KEYS = {
	RECIPE: "recipe",
	PERIPHERAL: "peripheral"
};

const SLOPE = {
	duration: 0,
	temp: 0,
	tolerance: 0,
	extra_procs: []
};

const PROC = {
	sensor: 0,
	actuator: 0,
	ref_value: 0,
	tolerance: 0
};

const DEFAULT_SLOPE = Object.freeze({
	duration: 0,
	temp: 25,
	tolerance: 1,
	extra_procs: []
});

const DEFAULT_RECIPE = Object.freeze({
	slopes_num: 1,
	'0': DEFAULT_SLOPE
});

const NO_DEVICE = {
	name: "NO DEVICE",
	id: "00:00:00:00:00:00"
}

const DEFAULT_DEVICE = {
	name: 'HC05',
	id: '98:D3:32:F5:9F:3D'
}

class Storage {

	constructor() {
		this.current_recipe = JSON.parse(JSON.stringify(DEFAULT_RECIPE));
		this.default_peripheral = JSON.parse(JSON.stringify(DEFAULT_DEVICE));
	}

	/////////////////////// receitas e processos

	async setDefaultRecipe() {
		await this._storeObj(KEYS.RECIPE, DEFAULT_RECIPE);
		return this.current_recipe;
	}

	async setSlope(position, duration, slope_temp, tolerance) {
		let recipe = await this.getRecipe();
		position -= 1;

		if(position < 0) return ERROR;

		if(position < recipe.slopes_num) {
			recipe[position].duration = duration;
			recipe[position].temp = slope_temp;
			recipe[position].tolerance = tolerance;
		} else {
			for(let i = recipe.slopes_num; i <= position; i++ ) recipe[i] = JSON.parse(JSON.stringify(DEFAULT_SLOPE));
			recipe[position].duration = duration;
			recipe[position].temp = slope_temp;
			recipe[position].tolerance = tolerance;
			recipe.slopes_num = position+1;
		}

		await this._storeObj(KEYS.RECIPE, recipe);
		return recipe;
	}

	async rmvSlope(position) {
		let recipe = await this.getRecipe();
		position -= 1;

		if(position < 0 || position >= recipe.slope_num) return ERROR;

		let new_recipe = Object.create(RECIPE);
		new_recipe.slopes_num = recipe.slopes_num-1;

		for(let i = 0; i < recipe.slopes_num; i++) {
			if(i < position) new_recipe[i] = recipe[i];
			else if(i > position) new_recipe[i-1] = recipe[i];
		}

		await this._storeObj(KEYS.RECIPE, new_recipe);

		return new_recipe;
	}

	async addProcToSlope(slope_pos, sensor, actuator, ref_value, tolerance) {
		let recipe = await this.getRecipe();
		slope_pos -= 1;

		if(slope_pos < 0 || slope_pos >= recipe.slopes_num) return ERROR;

		let procs = Object.create(recipe[slope_pos].extra_procs);
		let proc_length = procs.length;
		let already_exists = false;

		for(let i = 0; i < proc_length; i++) {
			if(procs[i].sensor === sensor && procs[i].actuator === actuator && procs[i].ref_value === ref_value && procs[i].tolerance === tolerance)
				already_exists = true;
		}
		
		if(!already_exists) {
			let new_proc = JSON.parse(JSON.stringify(PROC));
			new_proc.sensor = sensor;
			new_proc.actuator = actuator;
			new_proc.ref_value = ref_value;
			new_proc.tolerance = tolerance;
			recipe[slope_pos].extra_procs.push(new_proc);
		}

		await this._storeObj(KEYS.RECIPE, recipe);
		return recipe;
	}

	async rmvProcOfSlope(slope_pos, proc_pos) {
		let recipe = await this.getRecipe();
		slope_pos -= 1;
		proc_pos -= 1;
		
		if(slope_pos < 0 || slope_pos >= recipe.slopes_num) return ERROR;
		if(proc_pos < 0 || proc_pos >= recipe[slope_pos].extra_procs.length) return ERROR;

		let procs = [];
		let proc_length = recipe[slope_pos].extra_procs.length;

		for(let i = 0; i < proc_length; i++) {
			if(i !== proc_pos) procs.push(recipe[slope_pos].extra_procs[i]);
		}
		recipe[slope_pos].extra_procs = procs;

		await this._storeObj(KEYS.RECIPE, recipe);
		return recipe;
	}

	async getRecipe() {
		if(this.current_recipe === undefined) await this.setDefaultRecipe();
		return this.current_recipe;
		
	}

	async getSlope(position) {
		let recipe = await this._getObj(KEYS.RECIPE);
		position -= 1;

		if(position >= recipe.slopes_num || position < 0) return ERROR;

		return recipe[position];	
	}

	async getProc(slope_pos, proc_pos) {
		let slope = await this.getSlope(slope_pos);
		proc_pos -= 1;
		
		if(slope === ERROR) return ERROR;
		if(proc_pos >= slope.extra_procs.length || proc_pos < 0) return ERROR;

		return slope.extra_procs[proc_pos];	
	}

	async getNumOfSlopes() {
		let recipe = await this.getRecipe();
		return recipe.slopes_num;
	}

	async getNumOfProcs(slope_pos) {
		let slope = await this.getSlope(slope_pos);
		if(slope === ERROR) return ERROR;

		return slope.extra_procs.length;
	}

	/////////////////////// configurações

	async setDefaultPeripheral(peripheral) {
		if(peripheral.id===undefined || peripheral.name === undefined) return ERROR;

		await this._storeObj(KEYS.PERIPHERAL, peripheral);
	}

	async getDefaultPeripheral() {
		let peripheral =  await this._getObj(KEYS.PERIPHERAL);
		
		if(peripheral===undefined) peripheral = ERROR;

		return peripheral;
	}

	/////////////////////// outros

	async clearStorage() {
		let success = false;

		this.current_recipe = DEFAULT_RECIPE;
		this.default_peripheral = NO_DEVICE;
		success = true;
		
		return success;
	}

	async _storeObj(key, obj) {
		let success = false;
		success = true;
		if(key===KEYS.RECIPE) {
			this.current_recipe = JSON.parse(JSON.stringify(obj));
		}
		else if(key === KEYS.PERIPHERAL) {
			this.default_peripheral = JSON.parse(JSON.stringify(obj));
		}
		
		return success;
	}

	async _getObj(key) {
		let obj = ERROR;
		if(key === KEYS.RECIPE) obj = this.current_recipe;
		else if(key === KEYS.PERIPHERAL) obj = this.default_peripheral;
		return obj;
	}

	async _rmvObj(key) {
		let success = false;

		if(key === KEYS.RECIPE) this.this.current_recipe = undefined;
		else if(key === KEYS.PERIPHERAL) this.default_peripheraln
			= undefined;
		success = true;

		return success;
	}
	
}

/*
const Storage = {	
	ERROR: ERROR,
	DEFAULT_SLOPE: DEFAULT_SLOPE,
	DEFAULT_RECIPE: DEFAULT_RECIPE,
	setSlope: setSlope,
	rmvSlope: rmvSlope,
	addProcToSlope: addProcToSlope,
	rmvProcOfSlope: rmvProcOfSlope,
	getRecipe: getRecipe,
	getSlope: getSlope,
	getProc: getProc,
	getNumOfSlopes: getNumOfSlopes,
	getNumOfProcs: getNumOfProcs,
	setDefaultPeripheral: setDefaultPeripheral,
	getDefaultPeripheral: getDefaultPeripheral,
	clearStorage: clearStorage,
};
*/

export default Storage;
