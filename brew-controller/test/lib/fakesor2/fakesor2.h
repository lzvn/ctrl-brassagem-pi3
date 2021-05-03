#include <Arduino.h>
#include <sensor.h>

#ifndef fakesor2_h
#define fakesor2_h

class Fakesor2 : public Sensor {
public:
	Fakesor2();
	~Fakesor2();
	float read(); //default method for reading the input
	void calibrate(); //default method for callibration, whatever it means for an specific sensor
	//ps: offsets and corrections are to be applied to specific implementations since this is just an interface
};

#endif
