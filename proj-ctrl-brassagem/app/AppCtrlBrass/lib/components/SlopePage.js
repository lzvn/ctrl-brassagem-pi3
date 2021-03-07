'use strict';

import React , { useState, useEffect }from 'react';
import {
	StyleSheet,
	View,
	Text,
	StatusBar,
	TouchableOpacity,
	Button,
	SafeAreaView,
	ScrollView,
	Modal,
	TextInput,
	Picker
} from 'react-native';

const styles = StyleSheet.create({
	background: {
		backgroundColor: '#fff',
		paddingBottom: 50
	},
	title: {
		backgroundColor: '#fff',
		fontSize: 20,
		color: '#000',
		textAlign: 'center',
		fontWeight: 'bold',
		borderWidth: 1,
		borderColor: '#000',
		borderStyle: 'solid'
	},
	slopeText: {
		backgroundColor: '#ddd',
		fontSize: 16,
		color: '#000',
		textAlign: 'left',
		marginLeft: '10%'
	},
	slopeBackground: {
		backgroundColor: '#ddd'
	},
	procText: {
		backgroundColor: '#bbb',
		fontSize: 14,
		color: '#111',
		textAlign: 'left',
		marginLeft: '12%'
	},
	procBackground: {
		backgroundColor: '#bbb',
	},
	addSlopeBtn: {
	},
	addProcBtn: {
		height: '90%',
		width: '70%',
		marginLeft: '15%'
	},
	saveBtn: {
		backgroundColor: '#1f1'
	},
	deleteBtn: {
		backgroundColor: '#f11'
	}
});

let first_render = true;
let render_slopes = false;
let mdl_var = new Array(7).fill(0); //gambiarra

const SlopePage: () => React$Node = (props) => {
	const [slope_view, setSlopeView] = useState( <Text></Text> );
	const [slopeModalVisible, setSlopeModalVisible] = useState(false);
	const [procModalVisible, setProcModalVisible] = useState(false);
	const [memory_left, setMemoryLeft] = useState(1024);
	const [save_btn_text, setSaveBtnText] = useState("Salvar e enviar");

	if(first_render) {
		setTimeout(() => { sendAllToCtrl() }, 1000);
		updtSlopeView();
		first_render = false;
	}

	useEffect(() => {
		if(render_slopes) {
			updtSlopeView();
			render_slopes = false;
		} else {
			render_slopes = true;
		}
	})

	return (
		<SafeAreaView style={styles.background} >
			<ScrollView>
				<Modal
					animationType="fade"
					transparent={false}
					visible={slopeModalVisible}
				>
					<View style={styles.slopeBackground}>
						<Text style={styles.title}>
							{mdl_var[0]?"Nova Rampa:":("Modificar rampa "+(mdl_var[1]+1)+":" )}
						</Text>
						<Text style={styles.slopeText}>Duração: </Text>
						<TextInput style={styles.slopeText}
								   autoFocus={true} clearTextOnFocus={true} keyboardType={'numeric'} defaultValue={String(mdl_var[2])}
								   onChangeText={(text) => mdl_var[2] = Number(text)}/>
						<Text style={styles.slopeText}>Temperatura: </Text>
						<TextInput style={styles.slopeText}
								   clearTextOnFocus={true} keyboardType={'numeric'} defaultValue={String(mdl_var[3])}
								   onChangeText={(text) => mdl_var[3] = Number(text)}/>
						<Text style={styles.slopeText}>Tolerância: </Text>
						<TextInput style={styles.slopeText}
								   clearTextOnFocus={true} keyboardType={'numeric'} defaultValue={String(mdl_var[4])}
								   onChangeText={(text) => mdl_var[4] = Number(text)}/>
						<Button title="Confirmar" onPress={() => {setSlope(mdl_var.slice(1, mdl_var.length))}} />
						<Button title={(mdl_var[0])?"Cancelar":"deletar"} onPress={() => {rmvSlope(mdl_var)}}/>
					</View>
				</ Modal>
				<Modal
					animationType="fade"
					transparent={false}
					visible={procModalVisible}
				>
					<View style={styles.slopeBackground}>
						<Text style={styles.title}>
							{mdl_var[0]?"Novo Processo:":("Modificar processo "+(mdl_var[1]+1)+":" )}
						</Text>
						<Text style={styles.slopeText}>Sensor: 1</Text>
						<Text style={styles.slopeText}>Atuador: 1</Text>
						<Text style={styles.slopeText}>Valor de referência: </Text>
						<TextInput style={styles.slopeText}
								   autoFocus={true} clearTextOnFocus={true} keyboardType={'numeric'} defaultValue={String(mdl_var[5])}
							onChangeText={(text) => mdl_var[5] = Number(text)}/>
						<Text style={styles.slopeText}>Tolerância: </Text>
						<TextInput style={styles.slopeText}
								   clearTextOnFocus={true} keyboardType={'numeric'} defaultValue={String(mdl_var[6])}
								   onChangeText={(text) => mdl_var[6] = Number(text)}/>
						<Button title="Confirmar" onPress={() => {setProc(mdl_var.slice(1, mdl_var.length))}} />
						<Button title={(mdl_var[0])?"Cancelar":"deletar"} onPress={() => {rmvProc(mdl_var)}}/>
					</View>
				</Modal>
		
				
				{ slope_view }
				<Button title="Adicionar rampa" style={styles.addSlopeBtn}
						onPress={() => { setModalNewSlope() }} />
				<Button style={styles.saveBtn} onPress={() => { sendAllToCtrl() }}
						title={save_btn_text} disabled={save_btn_text!=="Salvar e enviar"}/>
				<Button style={styles.deleteBtn} onPress={() => { deleteAll() }} title="Deletar tudo" />
				<Text>Memória disponível no controlador: {memory_left} bytes</Text>
			</ScrollView>
		</SafeAreaView>
	)

	async function updtSlopeView() {
		let recipe = await props.strg.getRecipe();
		let new_slopes = [];	
		if(recipe===undefined || recipe.slopes_num === undefined) {
			new_slopes = <Text style={styles.title}> Erro ao buscar a receita</Text>;
		} else {
			for(let i = 0; i < recipe.slopes_num; i++) {
				let slope = recipe[i];
				let procs = [];
				if(slope.extra_procs.length > 0) {
					let j = 0;
					slope.extra_procs.forEach((proc) => {
						procs.push(
							<View style={styles.procBackground} key={10*i + j}>
								<TouchableOpacity style={styles.procBackground}
												  onPress={() => { setModalSetProc(i+1, j+1, proc.sensor, proc.actuator, proc.ref_value, proc.tolerance) }}>
									<Text style={styles.procText}>Sensor: {proc.sensor} Atuador: {proc.actuator}</Text>
									<Text style={styles.procText}>Referência: {proc.ref_value} Tolerância: {proc.tolerance}</Text>
								</TouchableOpacity>
							</ View>
						)
						j++;
					});
				}
				new_slopes.push(
					<View style={styles.slopeBackground} key={i}>
						<TouchableOpacity onPress={() => { setModalSetSlope(i+1, slope.duration, slope.temp, slope.tolerance) }}>
							<Text style={styles.title}>Rampa {i+1}</Text>
							<Text style={styles.slopeText}>Duração: {slope.duration} min</Text>
							<Text style={styles.slopeText}>Temperatura: {slope.temp} ºC</Text>
							<Text style={styles.slopeText}>Tolerância: {slope.tolerance} ºC</Text>
						</TouchableOpacity>
						<Text style={styles.slopeText}>Processos</Text>
						{ procs }
						<Button style={styles.addProcBtn} onPress={() => { setModalNewProc(i+1) }} title="Adicionar processo" />
					</View>
				)
			}
		}
		setSlopeView(new_slopes);
	}

	async function setModalSetSlope(position, duration, temp, tolerance) {
		let is_connected = await props.blt.isConnected();
		if(!is_connected) {
			alert("Nenhum dispositivo conectado");
			setSlopeModalVisible(false);
			return;
		}
		mdl_var[0] = false;
		mdl_var[1] = position;
		mdl_var[2] = duration;
		mdl_var[3] = temp;
		mdl_var[4] = tolerance;
		setSlopeModalVisible(true);
	}
	
	async function setModalNewSlope() {
		let is_connected = await props.blt.isConnected();
		if(!is_connected) {
			alert("Nenhum dispositivo conectado");
			setSlopeModalVisible(false);
			return;
		}
		let slopes_num = await props.strg.getNumOfSlopes();
		mdl_var[0] = true;
		mdl_var[1] = slopes_num + 1;
		for(let i = 2; i < 7; i++) mdl_var[i] = 0;
		setSlopeModalVisible(true);
	}

	async function setModalSetProc(slope_pos, proc_pos, sensor, actuator, ref_value, tolerance) {
		let is_connected = await props.blt.isConnected();
		if(!is_connected) {
			alert("Nenhum dispositivo conectado");
			setProcModalVisible(false);
			return;
		}
		mdl_var[0] = false;
		mdl_var[1] = slope_pos;
		mdl_var[2] = proc_pos;
		mdl_var[3] = sensor;
		mdl_var[4] = actuator;
		mdl_var[5] = ref_value;
		mdl_var[6] = tolerance;
		setProcModalVisible(true);
	}

	async function setModalNewProc(slope_pos) {
		let is_connected = await props.blt.isConnected();
		if(!is_connected) {
			alert("Nenhum dispositivo conectado");
			setProcModalVisible(false);
			return;
		}
		let procs_num = await props.strg.getNumOfProcs(slope_pos);
		let max_procs_num = 1;
		if(procs_num !== props.strg.ERROR || procs_num < max_procs_num) {
			mdl_var[0] = true;
			mdl_var[1] = slope_pos;
			mdl_var[2] = procs_num;
			mdl_var[3] = 1;
			mdl_var[4] = 1;
			for(let i = 5; i < 7; i++) mdl_var[i] = 0;
			setProcModalVisible(true);
		}
	}
	
	async function setSlope(params) {
		let is_connected = await props.blt.isConnected();
		if(!is_connected) {
			alert("Nenhum dispositivo conectado");
			setSlopeModalVisible(false);
			return;
		}
		await props.strg.setSlope(params[0], params[1], params[2], params[3]);
		await props.blt.sendCmd(props.blt.makeMsg(props.blt.CMD_CODES.SET_SLOPE, [params[0], params[1], params[2], params[3]]));
		let mem_left = await props.blt.sendRequest([props.blt.PARAM_CODES.MEM_LEFT]);
		setMemoryLeft(mem_left);
		await updtSlopeView();
		setSlopeModalVisible(false);
	}

	async function rmvSlope(params) {
		let is_connected = await props.blt.isConnected();
		if(!is_connected) {
			alert("Nenhum dispositivo conectado");
			setSlopeModalVisible(false);
			return;
		}
		if(!params[0]) {
			let recipe = await props.strg.getRecipe();
			if(params[1] === recipe.slopes_num) {
				deleteAll();
			} else {
				await props.strg.rmvSlope(params[1]);
				await props.blt.sendCmd(props.blt.makeMsg(props.blt.CMD_CODES.RMV_SLOPE, [params[1]]));
				let mem_left = await props.blt.sendRequest([props.blt.PARAM_CODES.MEM_LEFT]);
				setMemoryLeft(mem_left);
			}
			await updtSlopeView();
		}
		setSlopeModalVisible(false);
	}

	async function setProc(params) {
		let is_connected = await props.blt.isConnected();
		if(!is_connected) {
			alert("Nenhum dispositivo conectado");
			setProcModalVisible(false);
			return;
		}
		await props.strg.addProcToSlope(params[0], params[2], params[3], params[4], params[5]);
		let sensor = props.devs.sensors[params[2]];
		let actuator = props.devs.actuators[params[3]]
		await props.blt.sendCmd(props.blt.makeMsg(props.blt.CMD_CODES.ADD_PROC, [params[0], sensor, actuator, params[4], params[5]]));
		let mem_left = await props.blt.sendRequest([props.blt.PARAM_CODES.MEM_LEFT]);
		setMemoryLeft(mem_left);
		await updtSlopeView();
		setProcModalVisible(false);
	}

	async function rmvProc(params) {
		let is_connected = await props.blt.isConnected();
		if(!is_connected) {
			alert("Nenhum dispositivo conectado");
			setProcModalVisible(false);
			return;
		}
		if(!params[0]) {
			props.strg.rmvProcOfSlope(params[1], params[2]);
			let sensor = props.devs.sensors[params[3]];
			let actuator = props.devs.actuator[params[4]];
			let msg = props.blt.makeMsg(RMV_PROC,
										params[1], params[2], sensor, actuator, params[5], params[6]);
			await props.blt.sendCmd(msg);
			let mem_left = await props.blt.sendRequest([]);
			setMemoryLeft(mem_left);
			await updtSlopeView();
		}
		setProcModalVisible(false);
	}

	async function sendAllToCtrl() {
		let recipe = await props.strg.getRecipe();
		let cmd_return = {};
		setSaveBtnText("Em processo...");

		let is_connected = await props.blt.isConnected();
		
		if(is_connected){
			cmd_return = await props.blt.sendCmd( props.blt.makeMsg(props.blt.CMD_CODES.RMV_ALL_SLOPES), true );
			//checking the return of it

			for(let i = 0; i < recipe.slopes_num; i++) {
				let slope = recipe[i];
				let msg = props.blt.makeMsg(props.blt.CMD_CODES.SET_SLOPE, [i+1, slope.duration, slope.temp, slope.tolerance]);
				cmd_return = await props.blt.sendCmd(msg, true);
				console.log(cmd_return);

				if(slope.extra_procs.length > 0) {
					let procs = slope.extra_procs;
					
					for(let j = 0; j < procs.length; j++) {
						let proc_msg = makeMsg(props.blt.CMD_CODES.ADD_PROC,
											   [i+1, devs.sensors[procs[j].sensor], devs.actuators[procs[j].actuator], procs[j].ref_value, procs[j].tolerance]);
						cmd_return = await props.blt.sendCmd(proc_msg, true);
						//checking each answer
					}
				}
			}
			
			let mem_left = await props.blt.request([props.blt.PARAM_CODES.MEM_LEFT]);
			setMemoryLeft(mem_left);
		} else {
			alert("Nenhum dispositivo conectado");
		}
		
		setSaveBtnText("Salvar e enviar");
	}

	async function deleteAll() {
		let is_connected = await props.blt.isConnected();
		if(!is_connected) {
			alert("Nenhum dispositivo conectado");
			return;
		}
		await props.strg.setDefaultRecipe()
		await updtSlopeView();
		await props.blt.sendCmd(props.blt.makeMsg(RMV_ALL_SLOPES));
	}
}

export default SlopePage;
