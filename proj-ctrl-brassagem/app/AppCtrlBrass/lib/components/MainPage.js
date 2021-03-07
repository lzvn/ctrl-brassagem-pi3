'use strict';

import React, { useState } from 'react';
import {
	StyleSheet,
	View,
	Text,
	StatusBar,
	TouchableOpacity,
	Button, 
	SafeAreaView,
	ScrollView
} from 'react-native';

const styles = StyleSheet.create({
	background: {
		backgroundColor: '#fff',
	},
	text: {
		backgroundColor: '#fff',
		fontSize: 20,
		color: '#000',
		textAlign: 'left'
	},
	startBtn: {
		backgroundColor: '#1f1'
	},
	stopBtn: {
		backgroundColor: '#f11'
	}
});

const Monitor: () => React$Node = (props) => {

	//rampa

	let slope_num = (props.ctrl.slope_num===undefined)?0:props.ctrl.slope_num;

	//tempo
	let minutes_left;
	let time;

	if(props.ctrl.time_left===undefined) {
		minutes_left = 0;
		time = "00:00";
	} else {
		minutes_left = Math.ceil(props.ctrl.time_left);
		time = String(minutes_left) + ":" + String(Math.ceil(60*(props.ctrl.time_left - minutes_left)));
	}
	
	//duração
	let duration = (props.ctrl.duration===undefined)?"00":props.ctrl.duration;

	//leituras

	let sensors = [];
	let actuators = [];
	for(let i = 0; i < 2; i++) {
		let sensor_reading = 0;
		let actuator_reading = 0;
		if(props.ctrl.sensors !== undefined && props.ctrl.sensors[i]===undefined) 
			sensor_reading = props.ctrl.sensors[i];
		if(props.ctrl.actuators !== undefined && props.ctrl.actuators[i]===undefined)
			actuator_reading = props.ctrl.actuators[i];

		sensors.push(sensor_reading);
		actuators.push(actuator_reading);
	}

	//temperatura alvo

	let slope_temp = (props.ctrl.slope_temp===undefined)?0:props.ctrl.slope_temp;

	return (
		<View style={styles.background}>
			<Text style={styles.text}>Rampa: {slope_num}</Text>
			<Text style={styles.text}>Tempo: </Text>
			<Text style={styles.text}>{time} / {duration}</Text>
			<Text style={styles.text}>Temperatura do mosto:</Text>
			<Text style={styles.text}>{sensors[0]} / {slope_temp} ºC</Text>
			<TouchableOpacity onPress={ () => { activate(props.devs.actuators[0]) } }>
				<Text style={styles.text}>Aquecedor do mosto: </Text>
				<Text style={styles.text}>{(actuators[0])?"Ligado":"Desligado"}</Text>
			</TouchableOpacity>
			<Text style={styles.text}>Temperatura da água de lavagem: </Text>
			<Text style={styles.text}>{sensors[1]} ºC</Text>
			<TouchableOpacity onPress={ () => { activate(props.devs.actuators[1]) } }>
				<Text style={styles.text}>Aquecedor da água de lavagem: </Text>
				<Text style={styles.text}>{(actuators[1])?"Ligado":"Desligado"}</Text>
			</TouchableOpacity>
		</View>
	);

	async function activate(pin) {
		if(pin===undefined) return;

		props.blt.sendCmd(makeMsg(props.blt.CMD_CODES.ACTIVATE, [ pin ]))
	}
}

const Controls: () => React$Node = (props) => {
	const [start_text, setText] = useState("Iniciar");

	let btn_ref = React.createRef();

	async function handleStartStop() {
		let result;
		let btn = btn_ref.current;
		if(ctrl.status===props.blt.STATUS.REST_STATE) {
			result = await props.blt.sendCmd(makeMsg(props.blt.CMD_CODES.START, [0]));
			if(result) {
				setText("Parar");
				btn.style = styles.stopBtn;
			} else {
				alert("Falha ao iniciar");
			}
		} else if(ctrl.status===props.blt.STATUS.BREW_STATE) {
			result = await props.blt.sendCmd(makeMsg(props.blt.CMD_CODES.STOP, [0]));
			setText("Reiniciar");
			btn.style = styles.startBtn;
		} else if(props.ctrl.status===props.blt.STATUS.STOP_BREW_STATE) {
			result = await props.blt.sendCmd(makeMsg(props.blt.CMD_CODES.RESTART, [0]));
			if(result) {
				setText("Parar");
				btn.style = styles.stopBtn;

			} else {
				alert("Falha ao reiniciar");
			}
		}
	}

	async function handleReset() {
		props.blt.sendCmd(makeMsg(props.blt.CMD_CODES.RESET, [0]));
		setText("Iniciar");
		btn.style = styles.startBtn;
	}

	return (
		<View style={styles.background}>
			<Button title={start_text} ref={btn_ref} style={styles.startBtn} onPress={ () => { handleStartStop() } }/>
			<Button title="Resetar" onPress={ () => { handleReset() } }/>
			<Text>Clique nas saídas dos acionadores acima quando fora da brassagem automática para acioná-los</Text>
		</View>
	);
}

const MainPage: () => React$Node = (props) => {
	//const [ctrl, setCtrl] = useState(props.blt.getFullUpdt(true, props.devs));
	const [ctrl, setCtrl] = useState(props.blt.NO_CTRL);

	return (
		<SafeAreaView style={styles.background}>
			<ScrollView style={styles.background}>
				<Monitor blt={props.blt} ctrl={ctrl} />
				<Controls blt={props.blt} ctrl={ctrl} />		
			</ScrollView>
		</SafeAreaView>
	);
}

export default MainPage;
