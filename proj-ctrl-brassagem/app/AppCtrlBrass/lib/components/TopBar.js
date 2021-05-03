'use strict';

import React from 'react';
import {
	StyleSheet,
	View,
	Text,
	StatusBar,
	Image,
	TouchableOpacity,
} from 'react-native';

const styles = StyleSheet.create({
	title: {
		backgroundColor: '#ffcc00',
		flexDirection: 'row',
		textAlign: 'center',
		height: '8%'
	},
	titleText: {
		fontSize: 24,
		fontWeight: 'bold',
		color: '#663300',
		textAlign: 'center',
		marginTop: '2%'
	},
	btn: {
		height: '100%',
		width: '15%',
		marginRight: '18%',
		marginLeft: '2%'
	},
	image: {
		height: '100%',
		width: '100%',
	}
});

const TopBar: () => React$Node = (props) => {
	let callMenu = (event) => {
		props.setMenu(true);
		event.preventDefault();
	}
	
	return (
		<View style={styles.title}>
			<TouchableOpacity style={styles.btn} onPress={(event) => callMenu(event, props)}>
				<Image source={require('./menu-icon.png')} style={styles.image}></Image>
			</TouchableOpacity>
			<Text style={styles.titleText}>{props.text}</Text>
		</View>
	);
}
export default TopBar;
