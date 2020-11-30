#include <Arduino.h>
#include <sensor.h>

#ifndef sntmpntc10k_h
#define sntmpntc10k_h

//todas as resistências estão em kilo ohms e em todos os cálculos as constantes estão ajustadas para isso
/* o código espera que o sensor NTC seja ligado a um divisor de tensão de tal forma que a variável de entrada
do arduino seja a tensão, pela qual a resistência do sensor será obtida e, por ela, a temperatura
*/
//_pin deve ser um pino analógico do arduino

class SensorTempNTC10k : public Sensor {
public:
	SensorTempNTC10k(int pin, float res_div, float b_value);
	~SensorTempNTC10k();

	//métodos da interface
	float read();
	void calibrate(); //ignorado por hora

private:
	int _pin;
	float _temp_ref;
	float _res_ref;
	float _res_div;
	float _b_value;
};

#endif
