#include <Arduino.h>

#ifndef sensor_h
#define sensor_h

class Sensor {
public:
	Sensor() {};
	~Sensor() {};
	virtual float read() = 0; //método padrão para leitura do mensurando
	virtual void calibrate() = 0; //método padrão de calibração do sensor
};

#endif
