#include <Arduino.h>
#include <sensor.h>

#ifndef fakedetector_h
#define fakedetector_h

class FakeDetector : public Sensor {
public:
	FakeDetector(int pin);
	~FakeDetector();
	float read(); //default method for reading the input
	void calibrate(); //default method for callibration, whatever it means for an specific sensor
private:
	int _pin;
};

#endif
