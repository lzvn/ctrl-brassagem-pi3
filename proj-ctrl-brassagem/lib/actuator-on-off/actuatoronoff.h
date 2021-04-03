#include "Arduino.h"
#include <actuator.h>

#ifndef actuatoronoff_h
#define actuatoronoff_h

class ActuatorOnOff : public Actuator {
public:
	ActuatorOnOff(int pin, float ref_value = 25, float tolerance = 1);
	~ActuatorOnOff();

	int getPin();

	//métodos da interface
	boolean act(float input, boolean ignore_input = false);
	void deactivate();
	void setRefValue(float ref_value);
	void setTolerance(float tolerance);
	boolean isActive();
	float getRefValue();
	float getTolerance();
private:
	#define _ON LOW
	#define _OFF HIGH
	int _pin;
	boolean _working; //verdadeiro se a referência caiu abaixo da tolerância e o atuador deve retorná-la para seu valor
};

#endif
