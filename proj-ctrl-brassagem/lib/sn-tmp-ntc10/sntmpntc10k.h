#include <Arduino.h>
#include <sensor.h>

#ifndef sntmpntc10k_h
#define sntmpntc10k_h

//all resistances are in kiloohms and all calculations and constants adjusted to it
/*this sensor is designed with a voltage divisor as means to calculate the resistance of the
termistor and this is used in the calculations, hence the _res_div variable
*/
//_pin is supposed to be an analog pin of the arduino

class SensorTempNTC10k : public Sensor {
public:
	SensorTempNTC10k(int pin, float res_div, float b_value);
	~SensorTempNTC10k();

	//methods from the interface
	float read();
	void calibrate(); //I'll just ignore this for now

private:
	int _pin;
	float _temp_ref;
	float _res_ref;
	float _res_div;
	float _b_value;
};

#endif
