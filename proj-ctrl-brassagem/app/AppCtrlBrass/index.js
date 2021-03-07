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
let strg = new Storage();

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
const DEVICES = {
	sensors: ['A0', 'A1'],
	actuators: [2, 3]
};

const REFRESH_RATE = 1000;

const ErrorText: () => React$Node = (props) => {
	return (
		<View style={styles.text}>
			<Text style={styles.text}>ERRO!</Text>
		</View>
	);
}

const PageSelector: () => React$Node = (props) => {
	switch(props.page) {
	case "MAIN": return ( <MainPage blt={BltComm} devs={DEVICES} /> );
	case "SLOPES": return ( <SlopePage blt={BltComm} strg={strg} devs={DEVICES} /> );
	case "CONFIG": return ( <ConfigPage blt={BltComm} strg={strg} devs={DEVICES} /> );
	default: return ( <ErrorText />);
	}
}

const App: () => React$Node = () => {
	const [page, setPage] = useState("SLOPES");
	const [menu, setMenu] = useState(false);

	let title_text = "Monitoramento";
	
	switch(page) {
	case "MAIN": title_text = "Controle"; break;
	case "SLOPES": title_text = "Rampas"; break;
	case "CONFIG": title_text = "Configurações"; break;
	default: title_text = "ERRO!";
	}
	
	return (
		<>
			<View style={styles.body}>
				<TopBar text={title_text} setMenu={setMenu}/>
				<Menu show={menu} selectItem={changePage}/>
				<PageSelector page={page} />
			</View>
		</>
	);

	function changePage(selectedText) {
		let next_page;
		
		switch(selectedText) {
		case "Controle": next_page = "MAIN"; break;
		case "Rampas": next_page = "SLOPES"; break;
		case "Configurações": next_page = "CONFIG"; break;
		default: next_page = "ERRO"
		}
		setPage(next_page);
		setMenu(false);
	}
};

AppRegistry.registerComponent(appName, () => App);
