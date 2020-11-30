#include "Arduino.h"
#include <timer.h>
#include <Wire.h>
#include <RTClib.h>

#ifndef timerds1307_h
#define timerds1307_h

// _ref_time e _time_when_stopped são em segundos, _time_set é em minutos e todos calculados com tempo unix

class TimerDS1307 : public Timer {
public:
	TimerDS1307();
	~TimerDS1307();

	boolean verifyClock(); //verfica se o relógio está funcionando corretamente

	//métodos da interface
	void start(unsigned int time_set);
	void stopRestart();
	void reset();
	float timeLeft();
	boolean isTimeOver();
	unsigned int timeSet();
private:
	long unsigned int _secs2min(long unsigned int time); //converte de segundos para minutos ignorando decimais
	float _secs2minsec(long unsigned int time); //mesmo que o acima, porém com decimais. Esse método buga em entradas muito altas
	
	RTC_DS1307 _clock;
};

#endif
