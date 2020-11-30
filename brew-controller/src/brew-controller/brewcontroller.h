#include "Arduino.h"

#ifndef brewcontroller_h
#define brewcontroller_h

class BrewController {
public:
	BrewController(Timer *timer, int main_sensor_pin, Sensor *main_sensor, int main_output_pin);
	~BrewController();

	//controle do processo
	boolean start(); //saída verdadeira se não ocorrerem erros
	boolean stop(); //idem
	boolean reset(); //idem
	boolean activate(int output_pin); //ativa uma saída manualmente, o processo deve estar parado ou resetado
	boolean run(); //verifica as entradas e atua sobre as saídas de forma automática

	//manipulation of recipes and slopes
	boolean setSlope(int position, unsigned int duration, float moist_temp, float tolerance); //verdadeiro se tudo ocorrer bem
	boolean addProc2Slope(int position, int input_pin, int output_pin, float ref_value, float tolerance); //idem
	void resetSlope(int position);
	void resetAllSlopes();
	float getSlopeTemp(int position);
	int getCurrentSlopeNumber();
	//float getCurrentSlopeTemp(); //provalmente não será necessaŕio dados os métodos acima

	//manipulação de sensores e atuadores
	boolean addSensor(int pin, Sensor *sensor); //verdadeiro se tudo ir bem
	boolean addActuator(int pin, Actuator *actuator); //idem
	boolean clear(int pin); //idem, e limpa todos os pinos com excessão dos dois principais de entrada e saída
	boolean isPinInUse(int pin); //verdadeiro se o pino é usado como entrada ou saída
	float getSensorReading(int pin); //retorna menos infinito como código de erros
	boolean isActuatorOn(int pin); //returna verdadeiro se um atuador está ativo e falso caso contrário or se o pino é um sensor

	//Outros métodos (cronômetro, parte pública da memória, etc)
	unsigned int getTimeLeft(); //em minutos
	unsigned int getCurrentSlopeDuration(); //em minutos
	unsigned int getMemoryLeft(); //em bytes
		
private:

	void _clearMemory();

	//constantes
	int _MEMORY_SIZE; //tamanho da memória em bytes
	int _CONF_END; //endereço do fim da seção de configurações na memória
	int _SLOPE_START_ID; //código de identificação do início de uma rampa na memória
	int _EXTRA_PROCS_ID; //código que diz se haverão mais sensores que o principal
	int _RECIPE_END_ID; //código de identificação do fim da receita na memória

	//variáveis
	int _end_addr //endereco do fim da receita na memória
	int _current_slope_addr; //endereço do início da rampa atual
	int _current_addr; //endereço atual ou o último acessado

	//timer, sensores e atuadores e pinos
	Timer *_timer;
	int _main_sensor_pin;
	Sensor *_main_sensor;
	int _main_output_pin;
};

#endif
