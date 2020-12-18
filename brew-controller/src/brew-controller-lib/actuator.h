#include "Arduino.h"

#ifndef actuator_h
#define actuator_h

class Actuator {
public:
	Actuator() {};
	~Actuator() {};

	virtual boolean act(float input, boolean ignore_input) = 0; //controla a saída do atuador conforme sua entrada
	virtual void deactivate() = 0; //desliga o atuador, zerando a saída
	virtual void setRefValue(float ref_value) = 0;
	virtual void setTolerance(float tolerance) = 0;
	virtual float getRefValue() = 0;
	virtual float getTolerance() = 0;
	virtual boolean isActive() = 0;
	//virtual float getOutputValue(float input) = 0; //dá o valor de saída do controlador para uma determinada entrada
protected:
	boolean _active; //verdadeiro se o controlador tem valor não nulo, independente de qual seja
	float _ref_value; //valor de referencia do processo de controle
	float _tolerance; //tolerância
};

#endif
