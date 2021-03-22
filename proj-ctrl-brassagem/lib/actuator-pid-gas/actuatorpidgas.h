#include "Arduino.h"
#include <actuator.h>
#include <Stepper.h>

#ifndef actuatorpidgas_h
#define actuatorpidgas_h

class ActuatorPIDGas : public Actuator {
public:
	ActuatorPIDGas(int first_pin, float ref_value, float kp, float ki, float kd);
    //first_pin é o primeiro pino do motor de passo, os outros são pegos a partir dele
	~ActuatorPIDGas();

	int getFirstPin();
	boolean isValveOpen(); //indica se a válvula e gás está aberta
	void closeValve(); //fecha a válvula

	//métodos da interface
	boolean act(float input, boolean ignore_input = false);
	void deactivate();
	void setRefValue(float ref_value);
	void setTolerance(float tolerance);
	boolean isActive();
	float getRefValue();
	float getTolerance();
	
private:
	#define _MIN_ANGLE 15 //graus
	#define _MAX_ANGLE 450 //graus
	#define _SPEED 75
	#define _TOTAL_STEPS 2000 //passos em uma volta completa
	
	int _pins[4];
	float _kp, _ki, _kd; //constantes do controlador
	int _current_angle;
	float _last_input;
	float _integral;
	long unsigned int _time_ref;
	boolean _valve_open; //verdadeiro se a válvula está aberta, isto é, com no mínimo o angulo de _MIN_ANGLE
	Stepper _motor = Stepper(_TOTAL_STEPS/4, 0, 1, 2, 3);

	void _setValveAngle(int angle);
	void _resetValve(); //rotina para resetar a válvula
};

#endif
