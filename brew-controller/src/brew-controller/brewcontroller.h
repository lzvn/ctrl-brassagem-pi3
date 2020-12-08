#include "Arduino.h"
#include "EEPROM.h"

#include <sensor.h>
#include <timer.h>

#ifndef brewcontroller_h
#define brewcontroller_h

/*
Estrutura da memória:

Completa:
[seção de configurações 0-10][Seção de rampas 11-1022][código de fim da receita 15-1023]

Configurações:
[pino do sensor principal 0][pino do atuador principal 1]...
ps: provavelmente não vou implementar os dois primeiros, mas os endereços ficam reservados

Rampa:
[código início de rampa 1 byte][tempo em minutos 1 byte][temperatura do mosto 1 byte]...
[tolerancia 1 byte][identificação se há mais processos ou não 1 byte]...
[pino do sensor][valor de referência][tolerância][pino do atuador]
Se o identificador dizer que não há mais processos, a rampa acaba ali e começa no próximo endereço,
do contrário, continua até encontrar o código de início da próxima rampa

Códigos numéricos
Negativo: [código 1 byte][número 1 ou mais bytes]
Número maior que 249 e decimais: [código 1 byte][vezes de 250 1 byte][restante 0 ou 1 byte]...
[decimal de 00 até 99 0 ou 1 byte][código]
exemplo: 250 = [cdg][1][cdg]
         350 = [cdg][1][100][cdg]
		 567 = [cdg][2][67][cdg]
		 1,5 = [cdg][0][1][50][cdg]
ps: não pretendo implementar esses, mas fica aqui caso necessite deles
*/

typedef struct {
	int pin;
	boolean active;
} Actuator;

class BrewController {
public:
	BrewController(Timer *timer, int main_sensor_pin, Sensor *main_sensor, Actuator *main_actuator);
	~BrewController();

	//controle do processo
	boolean start(); //saída verdadeira se não ocorrerem erros
	boolean stop(); //idem
	boolean reset(); //idem
	boolean activate(int output_pin); //ativa uma saída manualmente, o processo deve estar parado ou resetado
	boolean deactivate(int output_pin);
	boolean run(); //verifica as entradas e atua sobre as saídas de forma automática

	//manipulação de receitas e rampas
	//position inicia em 0
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
	unsigned int getStatus(); //return the status of the controller
		
private:

	void _readFromMemory(int addr);
	void _clearMemory(boolean clear_config = true);
	void _activateActuator(int pin);
	void _deactivateActuator(int pin);
	int _indexOfPin(int pin); //retorna o índice de um pino na matriz _devices
	void _setConfig(); //faz a configuração com os ítens da memória
	void _setErrorState();
	int _getAddrOfSlope(int position);

	//constantes
	#define _MIN_INF (std::numeric_limits<float>::infinity()) //minus infinity
    #define _MEMORY_SIZE 1024 //tamanho da memória em bytes
	#define _CONF_END 10 //endereço do fim da seção de configurações na memória (último endereço)
	#define _SLOPE_START_ID 252 //código de identificação do início de uma rampa na memória
	#define _EXTRA_PROCS_ID 253 //código que diz se haverão mais sensores que o principal
    #define _NO_EXTRA_PROCS_ID 254 //código que diz que não haverão mais sensores que o principal
	#define _RECIPE_END_ID 255 //código de identificação do fim da receita na memória
	#define _MAX_DEVICE_NUM 6 /*Número máximo de sensores e atuadores disponível, 
									 lembrar de casar com a declaração da matriz de dispositivos*/
	#define _PIN_COL 0
	#define _TYPE_COL 1
	#define _DEV_COL 2
	#define _REST_STATE 0 //State of being on, but not brewing
	#define _BREW_STATE 1 //In brewing and not stopped
	#define _STOP_BREW_STATE 2 //In brewing, but the process was stopped
	#define _ERROR_STATE 3 //An error ocurred and the controller is inoperand until turned off

	//variáveis
	int _end_addr; //endereco do fim da receita na memória
	int _current_slope_addr; //endereço do início da rampa atual

	//timer, sensores e atuadores e pinos
	unsigned int _status = _REST_STATE; //status of the controller
	Timer *_timer;
	int _devices[_MAX_DEVICE_NUM][3]; /*
							  matriz que reune entradas e saídas do controlador. Seu formato é:
							  pino | 0 para entrada e 1 para saída | ponteiro pro o sensor ou atuador
							  Por primeiro vem os pinos digitais, ignorando-se o 0 e 1 por serem RX e TX e
							  depois vem os analógicos.
							*/
	//PS: Como não vou usar muitos pinos, resolvi reduzir o tamanho da matriz para 6 pinos possíveis, dois
	//dos quais são analógicos
	//PS2: Eu queria ter um nome melhor...
	
	int _main_sensor_index; //índice do sensor principal na matriz _devices
	int _main_actuator_index; //índice do atuador principal na matriz _devices
};

#endif
