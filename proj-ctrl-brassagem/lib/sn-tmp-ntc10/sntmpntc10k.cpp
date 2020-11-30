#include <Arduino.h>
#include "sntmpntc10k.h"

SensorTempNTC::SensorTempNTC10k(int pin, float res_div, float b_value) {
	_pin = pin;
	_temp_ref = 25.0 + 273.5; //em kelvin
	_res_ref = 10; //em kiloohms
	_res_div = res_div;
	_b_value = b_value;
}

SensorTempNTC::~SensorTempNTC10k() {
	
}

float SensorTempNTC::read() {
	float res_sn = analogRead(_pin); //leitura do pino analógico
	res_sn = 5.0*res_sn/1023.0; //sua conversão em um valor de tensão
	res_sn = _res_div/(5.0/res_sn - 1.0); //cálculo da resistência por meio da lei de ohm
	
	float log_res = 1; //LEMBRAR DE ACHAR UMA FÓRMULA APROXIMADA PARA LN PARA O CÁLCULO DE LN(RSN/RES_DIV)
	float output_temp = _b_value*_temp_ref/(_b_value + _temp_ref*log_res) - 273.5;
	
	return output_temp;
}

void SensorTempNTC::calibrate() {
	return;
}
