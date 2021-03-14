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

		props.blt.sendCmd(props.blt.makeMsg(props.blt.CMD_CODES.ACTIVATE, [ pin ]))
	}
}
let first_render = true;
let timeout_id = 0;
const REFRESH_TIME = 1000;

const MainPage: () => React$Node = (props) => {
	//const [ctrl, setCtrl] = useState(props.blt.getFullUpdt(true, props.devs));
	const [ctrl, setCtrl] = useState(props.blt.NO_CTRL);
	const [start_text, setText] = useState("Iniciar");

	let btn_ref = React.createRef();

	if(first_render) {
		first_render = false;
		//updtCtrl();
	}

	return (
		<SafeAreaView style={styles.background}>
			<ScrollView style={styles.background}>
				<Monitor blt={props.blt} ctrl={ctrl} devs={props.devs}/>
				<View style={styles.background}>
					<Button title={start_text} ref={btn_ref} style={styles.startBtn} onPress={ () => { handleStartStop() } }/>
					<Button title="Resetar" onPress={ () => { handleReset() } }/>
					<Text>Clique nas saídas dos acionadores acima quando fora da brassagem automática para acioná-los</Text>
				</View>
			</ScrollView>
		</SafeAreaView>
	);

	async function updtCtrl(repeat = false) {

		let is_connected = await props.blt.isConnected();
		if(!is_connected) {
			console.log("no connection to update")
			return;
		}
		//let new_ctrl = JSON.parse(Json.stringify(props.blt.NO_CTRL));
		let new_ctrl = JSON.parse(JSON.stringify(ctrl));
		
		await props.blt.getReadingsUpdt().then( (updt) => {

			if(updt !== props.blt.ERROR && updt.id === props.blt.PARAM_CODES.GNRL_UPDT) {
				console.log("updt", updt);
				Object.keys(updt).forEach((key) => {
					if(key === "sensors" || key === "actuators") {
						for(let i = 0; i < 1; i++) new_ctrl[key][i] = updt[key][i];
					} else {
						new_ctrl[key] = updt[key];
					}
				});
			}
			console.log("new ctrl", new_ctrl);
			setCtrl(new_ctrl);
		});

		if(new_ctrl.time_left < REFRESH_TIME) {
			console.log("nova rampa");
		}

		if(repeat) {
			timeout_id = setTimeout(() => {updtCtrl(true); console.log("setting next update")}, REFRESH_TIME);
		}
	}

	async function handleStartStop() {
		let result;
		let btn = btn_ref.current;

		let is_connected = await props.blt.isConnected();
		if(!is_connected) {
			alert("Nenhum dispositivo conectado");
			return;
		}
		
		if(ctrl.status===props.blt.STATUS.REST_STATE) {
			result = await props.blt.sendCmd(props.blt.makeMsg(props.blt.CMD_CODES.START));
			if(result) {
				await props.blt.sendCmd(props.blt.makeMsg(props.blt.CMD_CODES.GNRL_UPDT_REQ));
				updtCtrl(true);
				setText("Parar");
				btn.style = styles.stopBtn;
			} else {
				alert("Falha ao iniciar");
			}
		} else if(ctrl.status===props.blt.STATUS.BREW_STATE) {
			result = await props.blt.sendCmd(props.blt.makeMsg(props.blt.CMD_CODES.STOP, [0]));
			setText("Reiniciar");
			btn.style = styles.startBtn;
		} else if(props.ctrl.status===props.blt.STATUS.STOP_BREW_STATE) {
			result = await props.blt.sendCmd(props.blt.makeMsg(props.blt.CMD_CODES.RESTART, [0]));
			if(result) {
				setText("Parar");
				btn.style = styles.stopBtn;

			} else {
				alert("Falha ao reiniciar");
			}
		}
	}

	async function handleReset() {
		let is_connected = await props.blt.isConnected();
		if(!is_connected) {
			alert("Nenhum dispositivo conectado");
			return;
		}
		//props.blt.sendCmd(props.blt.makeMsg(props.blt.CMD_CODES.RESET, [0]));
		props.blt.sendCmd(props.blt.makeMsg(props.blt.CMD_CODES.GNRL_UPDT_REQ));
		clearTimeout(timeout_id);
		setText("Iniciar");
		btn.style = styles.startBtn;
	}
}

export default MainPage;
