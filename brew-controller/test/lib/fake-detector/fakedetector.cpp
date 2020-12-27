#include "fakedetector.h"

FakeDetector::FakeDetector(int pin) {
	_pin = pin;
	pinMode(_pin, INPUT);
}

FakeDetector::~FakeDetector() {
	
}

float FakeDetector::read() {
	float output = 0;
	if(digitalRead(_pin) == HIGH) output = 1.0;
	return output;
}

void FakeDetector::calibrate() {	
}
