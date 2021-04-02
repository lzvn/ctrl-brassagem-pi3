#include "Arduino.h"
#include <actuator.h>
#include <AccelStepper.h>

#ifndef actuatorpidgas_h
#define actuatorpidgas_h

class ActuatorPIDGas : public Actuator {
public:
	ActuatorPIDGas(int first_pin, float ref_value, float tolerance, float kp, float ki, float kd);
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
	#define _TOTAL_STEPS 1024 //passos em uma volta completa
	#define _SPEED (_TOTAL_STEPS/2) //passos por segundo
	#define _ACCEL (2*_TOTAL_STEPS) //passos por segundo por segundo
	#define _MOTOR_INTERFACE 4 //interface do motor para a biblioteca
	#define _MIN_ANGLE 0.083 //passos/volta completa
	#define _MAX_ANGLE 1.5 //passos/volta completa
	
	int _pins[4];
	float _kp, _ki, _kd; //constantes do controlador
	float _current_angle; //passos por volta
	float _last_input;
	float _integral;
	long unsigned int _time_ref;
	boolean _valve_open; //verdadeiro se a válvula está aberta, isto é, com no mínimo o angulo de _MIN_ANGLE
	AccelStepper _motor;

	void _setValveAngle(float angle);
	void _resetValve(); //rotina para resetar a válvula
};

#endif
