/**
 * @format
 */

import React, { useState } from 'react';
import {
	AppRegistry,
	SafeAreaView,
	StyleSheet,
	ScrollView,
	View,
	Text,
	StatusBar,
	Button
} from 'react-native';

import {name as appName} from './app.json';
import { TopBar, Menu, MainPage, SlopePage, ConfigPage, BltComm, Storage } from './lib';
BltComm.start();

const styles = StyleSheet.create({
	body: {
		backgroundColor: '#fff',
		height: '100%',
		width: '100%'
	},
	title: {
		backgroundColor: '#ffcc00',
		fontSize: 24,
		fontWeight: 'bold',
		color: '#663300',
		textAlign: 'center'
	},
	text: {
		fontSize: 18,
		color: '#000'
	}
});

//dispositivos tirados diretamente da disposição do hardware
//indices 0 e 1 são para o sensor e aquecedor principal, restantes para os demais
const devices = {
	sensors: ['A0', 'A1'],
	actuators: [2, 3]
};

let recipe = Storage.getRecipe();
recipe = (recipe===undefined)?{}:recipe;

let controller = BltComm.getFullUpdt(true, devices);

const REFRESH_RATE = 1000;

const DEFAULT_STATE = {
	page: "MAIN",
	menu_on: false,
	recipe: recipe,
	devices: devices,
	controller: controller
};



const ErrorText: () => React$Node = (props) => {
	return (
		<View style={styles.text}>
			<Text style={styles.text}>ERRO!</Text>
		</View>
	);
}

const PageSelector: () => React$Node = (props) => {
	switch(props.page) {
	case "MAIN": return ( <MainPage blt={BltComm} ctrl={props.ctrl} devices={props.devices} /> );
	case "SLOPES": return ( <SlopePage blt={BltComm} strg={Storage}ctrl={props.ctrl} recipe={props.recipe}/> );
	case "CONFIG": return ( <ConfigPage blt={BltComm} strg={Storage}/> );
	default: return ( <ErrorText />);
	}
}

const App: () => React$Node = () => {
	const [state, setState] = useState(DEFAULT_STATE);

	let title_text = "Monitoramento";
	
	switch(state.page) {
	case "MAIN": title_text = "Controle"; break;
	case "SLOPES": title_text = "Rampas"; break;
	case "CONFIG": title_text = "Configurações"; break;
	default: title_text = "ERRO!";
	}

	setInterval(() => {
		let new_ctrl = {};
		if(state.ctrl.time_left <= REFRESH_RATE) {
			new_ctrl = getFullUpdt(true, state.devices);
		} else {
			new_ctrl = state.ctrl;
			let pin_updt = getReadingsUpdt(state.devices);
			new_ctrl.sensors = pin_updt.sensors;
			new_ctrl.actuators = pin_updt.actuators;
		}
		setState({ page: page, menu_on: menu_on, recipe: recipe, devices, devices, ctrl: new_ctrl });
	}, REFRESH_RATE);
	
	return (
		<>
			<View style={styles.body}>
				<TopBar text={title_text} callMenu={switchMenu}/>
				<Menu show={state.menu_on} selectItem={changePage}/>
				<PageSelector page={state.page} ctrl={state.controller} recipe={state.recipe} devices={state.devices}/>
			</View>
		</>
	);

	function switchMenu() {
		setState({page: state.page, menu_on: !state.menu_on, recipe: state.recipe, devices: state.devices});
	}

	function changePage(selectedText) {
		let next_page;
		
		switch(selectedText) {
		case "Controle": next_page = "MAIN"; break;
		case "Rampas": next_page = "SLOPES"; break;
		case "Configurações": next_page = "CONFIG"; break;
		default: next_page = "ERRO"
		}
		//console.log(next_page);
		setState({page: next_page, menu_on: false, recipe: state.recipe, devices: state.devices});
	}
};

AppRegistry.registerComponent(appName, () => App);
