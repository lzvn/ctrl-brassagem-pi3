'use strict';

import React from 'react';
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
		fontSize: 15,
		color: '#000',
		textAlign: 'left',
		marginLeft: '15%'
	},
	procText: {
		backgroundColor: '#bbb',
		fontSize: 10,
		color: '#111',
		textAlign: 'left',
		marginLeft: '10%'
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

const SlopePage: () => React$Node = (props) => {
	let slopes = [];
	for(let i = 0; i < props.recipe.slopes_num; i++) {
		let slope = recipe[i];
		let procs = "";
		if(slope.extra_procs.length > 0) {
			procs = slope.extra_procs.map((proc) => {
				<TouchableOpacity>
					<Text style={styles.procText}>Sensor: {proc.sensor} Atuador: {proc.actuator}</Text>
					<Text style={styles.procText}>Referência: {proc.ref_value} Tolerância: {proc.tolerance}</Text>
				</TouchableOpacity>
			})
		}
		slopes.push(
			<View>
				<TouchableOpacity>
					<Text style={styles.title}>Rampa {i+1}</Text>
					<Text style={styles.slopeText}>Duração: {slope.duration} min</Text>
					<Text style={styles.slopeText}>Temperatura: {slope.temp} ºC</Text>
					<Text style={styles.slopeText}>Tolerância: {slope.tolerance} ºC</Text>
				</TouchableOpacity>
				<Text style={styles.slopeText}>Processos</Text>
				{ procs }
				<Button style={styles.addProcBtn} title="Adicionar processo" />
			</View>
		)
	}

	return (
		<SafeAreaView style={styles.background} >
			<ScrollView>
				{ slopes }
				<Button style={styles.addSlopeBtn} title="Adicionar rampa"/>
				<Button style={styles.saveBtn} title="Salvar e enviar" />
				<Button style={styles.deleteBtn} title="Deletar tudo" />
			</ScrollView>
		</SafeAreaView>
	);
}

export default SlopePage;
