'use strict';

import React from 'react';
import {
	StyleSheet,
	View,
	Text,
	StatusBar,
	TouchableOpacity,
} from 'react-native';

const styles = StyleSheet.create({
	background: {
		backgroundColor: '#fff',
		position: 'absolute',
		top: '8%',
		left: -5,
		zIndex: 1,
		height: "92%",
		width: "80%",
		borderWidth: 5,
		borderStyle: 'solid',
		borderColor: '#999'
	},
	text: {
		backgroundColor: '#fff',
		fontSize: 20,
		color: '#000',
		textAlign: 'left'
	},
	btn: {
		marginBottom: 2,
	},
	hidden: {
		height: 0,
		width: 0
	}
});

const Item: () => React$Node = (props) => {
	function handlePress(event) {
		props.selectItem(props.text);
		event.preventDefault();
	}
	return (
		<TouchableOpacity style={styles.btn} onPress={(event) => handlePress(event)}>
			<Text style={styles.text}>{props.text}</Text>
		</TouchableOpacity>
	);
}

const Menu: () => React$Node = (props) => {
	function selectItem(selected) {
		props.selectItem(selected);
		//console.log(selected);
	}
	
	if(props.show) {
		return (
			<View style={styles.background}>
				<Item text="Controle" selectItem={selectItem} />
				<Item text="Rampas" selectItem={selectItem} />
				<Item text="Configurações" selectItem={selectItem} />
			</View>
		);
	} else {
		return(
			<View style={styles.hidden}></View>
		);
	}
}
export default Menu;
