'use strict';

import React, { useState } from 'react';
import {
	StyleSheet,
	View,
	Text,
	StatusBar,
	TouchableOpacity,
	Button
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
	let minutes_left = Math.ceil(props.ctrl.time_left);
	let time = String(minutes_left) + String(Math.ceil(60*(props.ctrl.time_left - minutes_left)))
	return (
		<View style={styles.background}>
			<Text style={styles.text}>Rampa: {props.ctrl.slope_num}</Text>
			<Text style={styles.text}>Tempo: </Text>
			<Text style={styles.text}>{time} / {props.ctrl.duration}</Text>
			<Text style={styles.text}>Temperatura do mosto:</Text>
			<Text style={styles.text}>{props.ctrl.sensors[0]} / {props.ctrl.slope_temp} ºC</Text>
			<TouchableOpacity onPress={ () => { props.blt.sendCmd(makeMsg(blt.CMD_CODES.ACTIVATE, [props.devices.actuators[0]])) } }>
				<Text style={styles.text}>Aquecedor do mosto: </Text>
				<Text style={styles.text}>{(props.ctrl.actuators[0])?"Ligado":"Desligado"}</Text>
			</TouchableOpacity>
			<Text style={styles.text}>Temperatura da água de lavagem: </Text>
			<Text style={styles.text}>{props.ctrl.sensors[1]} ºC</Text>
			<TouchableOpacity onPress={ () => { props.blt.sendCmd(makeMsg(blt.CMD_CODES.ACTIVATE, [props.devices.actuators[1]])) } }>
				<Text style={styles.text}>Aquecedor da água de lavagem: </Text>
				<Text style={styles.text}>{(props.ctrl.actuators[1])?"Ligado":"Desligado"}</Text>
			</TouchableOpacity>
		</View>
	);
}

const Controls: () => React$Node = (props) => {
	const [start_text, setText] = useState("Iniciar");

	btn_ref = React.createRef();

	async function handleStartStop() {
		let result;
		let btn = btn_ref.current;
		if(props.ctrl.status===blt.STATUS.REST_STATE) {
			result = await blt.sendCmd(makeMsg(blt.CMD_CODES.START, [0]));
			if(result) {
				setText("Parar");
				btn.style = styles.stopBtn;
			} else {
				alert("Falha ao iniciar");
			}
		} else if(props.ctrl.status===blt.STATUS.BREW_STATE) {
			result = await blt.sendCmd(makeMsg(blt.CMD_CODES.STOP, [0]));
			setText("Reiniciar");
			btn.style = styles.startBtn;
		} else if(props.ctrl.status===blt.STATUS.STOP_BREW_STATE) {
			result = await blt.sendCmd(makeMsg(blt.CMD_CODES.RESTART, [0]));
			if(result) {
				setText("Parar");
				btn.style = styles.stopBtn;

			} else {
				alert("Falha ao reiniciar");
			}
		}
	}

	async function handleReset() {
		props.blt.sendCmd(makeMsg(blt.CMD_CODES.RESET, [0]));
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
	return (
		<View style={styles.background}>
			<Monitor blt={props.blt} ctrl={props.ctrl} />
			<Controls blt={props.blt} ctrl={props.ctrl} />
		</View>
	);
}

export default MainPage;
