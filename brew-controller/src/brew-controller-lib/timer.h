#include <Arduino.h>

#ifndef timer_h
#define timer_h

class Timer {
public:
	Timer() {};
	~Timer() {};
	virtual void start(unsigned int time_set) = 0; //time_set é em minutos
	virtual void stopRestart() = 0; //para a contagem ou a retoma de onde ela parou
	virtual void reset() = 0; //reseta a contagem por completo
	virtual float timeLeft() = 0; //em minutos com os segundos como decimais
	virtual boolean isTimeOver() = 0; //verifica se a contagem terminou ou não
	virtual unsigned int timeSet() = 0; //tempo para qual o cronômetro iniciado
protected:
	boolean _time_over; //verdadeiro se a contagem chegou ao seu fim
	boolean _active; //verdadeiro se o cronômetro está ativo, podendo ter sua contagem pausada
	boolean _stopped; //verdadeiro se a contagem foi parada
	unsigned int _ref_time; //referência para cálculos de intervalos de tempo, em segundos
	unsigned int _time_set; //intervalo de contagem do cronômetro
	unsigned int _time_when_stopped; //tempo em que o cronômetro parou, para que seja continuado
};

#endif
