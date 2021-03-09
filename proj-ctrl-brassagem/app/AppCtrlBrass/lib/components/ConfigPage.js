'use strict';

import React, {useState} from 'react';
import {
	StyleSheet,
	View,
	Text,
	StatusBar,
	TouchableOpacity,
	Button,
	FlatList
} from 'react-native';

const styles = StyleSheet.create({
	background: {
		backgroundColor: '#fff',
		fontSize: 20,
		color: '#000',
		textAlign: 'left'
	},
	text: {
		backgroundColor: '#fff',
		fontSize: 20,
		color: '#000',
		textAlign: 'left'
	},
	btn: {
	},
	listHeader: {
		backgroundColor: '#bbb',
		fontSize: 22,
		fontWeight: 'bold'
	},
	listItem: {
		backgroundColor: '#ddd',
		paddingTop: 5,
		paddingBottom: 5
	},
	listId: {
		fontSize: 16,
		color: '#444',
		textAlign: 'left',
	},
	listName: {
		fontSize: 18,
		color: '#000',
		textAlign: 'left',
	}
});

let first_render = true;

const Connection: () => React$Node = (props) => {
	const [peripherals, setPeripherals] = useState({available: props.blt.getPairedDevices(), connected: props.strg.getDefaultPeripheral()});
	const [btn_text, setBtnText] = useState("Atualizar");

	async function updtPeripherals() {
		let list = await props.blt.getPairedDevices();
		//console.log(list);
		setPeripherals({available: list, connected: peripherals.connected});
	}
	
	async function connect(peripheral) {
		setBtnText("Conectando...");
		let success = await props.blt.chooseDevice(peripheral.id);
		if(success === props.blt.ERROR) {
			alert("Erro na conexão");
		} else {
			alert("Dispostivo conectado");
			props.strg.setDefaultPeripheral(peripheral);
			setPeripherals({available: peripherals.available, connected: peripheral});
		}
		setBtnText("Atualizar");

	}

	function makeListItem(peripheral) {
		return (
			<>
				<TouchableOpacity style={styles.listItem} onPress={() => {connect(peripheral)}}>
					<Text style={styles.listName}>Nome: {peripheral.name}</Text>
					<Text style={styles.listId}>Endereço: {peripheral.id}</Text>
				</TouchableOpacity>
				<View style={{borderBottomColor: '#333', borderBottomWidth: 2}}></View>
			</>
		)
	}

	let connected = (peripherals.connected.name === undefined || peripherals.connected.id === undefined)?"Nenhum dispositivo conectado":("Conectado: "+peripherals.connected.name+", "+peripherals.connected.id);

	if(first_render) {
		first_render = false;
		updtPeripherals();
	}
	return (
		<View style={styles.background}>
			<Text style={styles.listHeader}>Dispositivos pareados: </Text>
			<View style={{borderBottomColor: '#555', borderBottomWidth: 2}}></View>
			<FlatList
				data={peripherals.available}
				renderItem={ ( {item} ) => makeListItem(item) }	/>
			<Button onPress={() => {updtPeripherals()}} title={btn_text} disabled={btn_text!=="Atualizar"}/>
			<Text style={styles.text}>{connected}</Text>
		</View>
	)
}

const ConfigPage: () => React$Node = (props) => {
	
	return (
		<View style={styles.background}>
			<Connection blt={props.blt} strg={props.strg}/>
		</View>
	);
};
	
export default ConfigPage;
