#include "Arduino.h"
#include "EEPROM.h"

#include <sensor.h>
#include <timer.h>

#ifndef brewcontroller_h
#define brewcontroller_h

typedef struct {
	int pino;
	boolean active;
} Actuator

class BrewController {
public:
	BrewController(Timer *timer, int main_sensor_pin, Sensor *main_sensor, Actuator main_actuator);
	~BrewController();

	//controle do processo
	boolean start(); //saída verdadeira se não ocorrerem erros
	boolean stop(); //idem
	boolean reset(); //idem
	boolean activate(int output_pin); //ativa uma saída manualmente, o processo deve estar parado ou resetado
	boolean deactivate(int output_pin);
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
	void _activateActuator(int pin);
	void _deactivateActutor(int pin);
	void _indexOfPin(int pin); //retorna o índice de um pino na matriz _devices

	//constantes
	const int _MEMORY_SIZE = 1024; //tamanho da memória em bytes
	const int _CONF_END = 10; //endereço do fim da seção de configurações na memória (último endereço)
	const int _SLOPE_START_ID = 253; //código de identificação do início de uma rampa na memória
	const int _EXTRA_PROCS_ID = 254; //código que diz se haverão mais sensores que o principal
	const int _RECIPE_END_ID = 255; //código de identificação do fim da receita na memória
	const int _MAX_DEVICE_NUM = 6; /*Número máximo de sensores e atuadores disponível, 
									 lembrar de casar com a declaração da matriz de dispositivos*/

	//variáveis
	int _end_addr; //endereco do fim da receita na memória
	int _current_slope_addr; //endereço do início da rampa atual
	int _current_addr; //endereço atual ou o último acessado

	//timer, sensores e atuadores e pinos
	boolean _active; //true if in the recipe, false if stopped or other states
	Timer *_timer;
	int _devices[6][3]; /*
							  matriz que reune entradas e saídas do controlador. Seu formato é:
							  pino | 0 para entrada e 1 para saída | ponteiro pro o sensor ou atuador
							  Por primeiro vem os pinos digitais, ignorando-se o 0 e 1 por serem RX e TX e
							  depois vem os analógicos.
							*/
	//PS: Como não vou usar muitos pinos, resolvi reduzir o tamanho da matriz para 6 pinos possíveis, dois
	//dos quais são analógicos
	//PS2: Eu queria ter um nome melhor...
	
	int _main_sensor_index; //índice do sensor principal na _deviceMatrix
	int _main_actuator_index; //índice do atuador principal na _deviceMatrix
};

#endif
