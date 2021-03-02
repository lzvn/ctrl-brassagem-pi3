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

const Connection: () => React$Node = (props) => {
	const [peripherals, setPeripherals] = useState([]);
	const [btn_text, setBtnText] = useState("Atualizar");

	async function updtPeripherals() {
		let list = await props.blt.getPairedDevices();
		//console.log(list);
		setPeripherals(list);
	}
	
	async function connect(id) {
		setBtnText("Conectando...");
		let success = await props.blt.chooseDevice(id);
		if(success < 0) alert("Erro na conexão");
		else alert("Dispostivo conectado");
		console.log(id);
		setBtnText("Atualizar");
	}

	function makeListItem(name, id) {
		return (
			<>
				<TouchableOpacity style={styles.listItem} onPress={() => {connect(id)}}>
					<Text style={styles.listName}>Nome: {name}</Text>
					<Text style={styles.listId}>Endereço: {id}</Text>
				</TouchableOpacity>
				<View style={{borderBottomColor: '#333', borderBottomWidth: 2}}></View>
			</>
		)
	}

	return (
		<View style={styles.background}>
			<Text style={styles.listHeader}>Dispositivos pareados: </Text>
			<View style={{borderBottomColor: '#555', borderBottomWidth: 2}}></View>
			<FlatList
				data={peripherals}
				renderItem={ ( {item} ) => makeListItem(item.name, item.id) }	/>
			<Button onPress={() => {updtPeripherals()}} title={btn_text} />
		</View>
	)
}

const ConfigPage: () => React$Node = (props) => {
	
	return (
		<View style={styles.background}>
			<Connection blt={props.blt}/>
		</View>
	);
};
	
export default ConfigPage;
