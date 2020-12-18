#include <Arduino.h>
#include "sntmpntc10k.h"

SensorTempNTC10k::SensorTempNTC10k(int pin, float res_div, float b_value) {
	_pin = pin;
	_temp_ref = 25.0 + 273.5; //kelvin
	_res_ref = 10; //kiloohms
	_res_div = res_div;
	_b_value = b_value;
}

SensorTempNTC10k::~SensorTempNTC10k() {
	
}

float SensorTempNTC10k::read() {
	float res_sn = analogRead(_pin); //reading of an analog pin
	res_sn = 5.0*res_sn/1023.0; //the reading converted to a voltage value
	res_sn = _res_div/(5.0/res_sn - 1.0); //finally, the resistance of the sensor is calculated
	
	float log_res = 0;
	//série de taylor de ln(x)
	for(int i = 1; i <= 5; i++) {
		float exp = 1;
		for(int j = 0; j < i; j++) exp *= (res_sn/_res_div-1);
		exp = (i%2==0)?-exp:exp;
		log_res += exp/i;
	}
	//assume-se que res_sn/_res_div ficará entre 0 e 1
	
	float output_temp = _b_value*_temp_ref/(_b_value + _temp_ref*log_res) - 273.5;
	
	return output_temp;
}

void SensorTempNTC10k::calibrate() {
	
}
