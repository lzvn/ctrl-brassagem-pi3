#include <Arduino.h>
#include <sensor.h>

#ifndef fakesor1_h
#define fakesor1_h

class Fakesor1 : public Sensor {
public:
	Fakesor1();
	~Fakesor1();
	float read(); //default method for reading the input
	void calibrate(); //default method for callibration, whatever it means for an specific sensor
	//ps: offsets and corrections are to be applied to specific implementations since this is just an interface
};

#endif
