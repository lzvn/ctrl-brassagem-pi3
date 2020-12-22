#include "Arduino.h"
#include "timerds1307.h"

TimerDS1307::TimerDS1307() {
	reset();
}

TimerDS1307::~TimerDS1307() {	
}

boolean TimerDS1307::verifyClock() {
	return !_clock.isrunning(); //lembrar de verificar se isrunning tem uma saida invertida ou não
}

void TimerDS1307::start(unsigned int time_set) {
	if(_active) return;

	_clock = RTC_DS1307();
	_clock.begin();
	//_clock.adjust(DateTime((uint32_t) ABSOLUTE_REFTIME)); //ajusta o ds1307 para uma data em tempo unix
	_clock.adjust(DateTime(2000, 01, 01, 0, 0, 0));
	
	reset();
	_ref_time = _getTimeNow(); //em tempo unix
	_time_when_stopped = _ref_time;
	_time_set = time_set;
	_active = true;
	_stopped = false;
	_time_over = false;
}

void TimerDS1307::stopRestart() {
	if(isTimeOver() || !_active) return;
	
	if(_stopped) {
		_stopped = false;
		_ref_time += _getTimeNow() - _time_when_stopped;
		//_clock.adjust(DateTime((uint32_t) _time_when_stopped));
		_time_when_stopped = _ref_time;
	} else {
		_stopped = true;
		_time_when_stopped = _getTimeNow();
	}
}

void TimerDS1307::reset() {
	_ref_time = 0;
	_time_when_stopped = 0;
	_time_set = 0;
	_active = false;
	_stopped = false;
	_time_over = true;
}

float TimerDS1307::timeLeft() {
	float time_left = 0;

	if(!isTimeOver() && _active) {
		unsigned int time_now = (_stopped)?_time_when_stopped:_getTimeNow();
		time_left = (float) _time_set - _secs2minsec(time_now - _ref_time);
		if(time_left < 0) {
			_time_over = true;
			time_left = 0;
		}
	}
	
	return time_left;
}

boolean TimerDS1307::isTimeOver() {
	if(!_time_over && _active && !_stopped) _time_over = _secs2min(_getTimeNow() - _ref_time) > _time_set;
	return _time_over;
}

unsigned int TimerDS1307::timeSet() {
	return _time_set;
}

unsigned int TimerDS1307::_secs2min(long unsigned int time) {
	return (unsigned int)(time/60);
}

float TimerDS1307::_secs2minsec(long unsigned int time) {
	//esse método pode bugar se a entrada for muito grande (não procurei saber a partir de quanto)
	return ((float) time)/60.0;
}

unsigned int TimerDS1307::_getTimeNow() {
	//return _clock.now().unixtime() - ABSOLUTE_REFTIME;
	DateTime now = _clock.now();
	return now.hour()*3600 + now.minute()*60 + now.second();
}
