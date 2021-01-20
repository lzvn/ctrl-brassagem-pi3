#include <SoftwareSerial.h>

#include <brewcontroller.h>
#include <timerds1307.h>
#include <sntmpntc10k.h>
#include <actuatoronoff.h>

#define MIN_INF -3.4028235E38
#define REST_STATE 0
#define BREW_STATE 1
#define STOP_BREW_STATE 2
#define ERROR_STATE 3

/*old code to review

#define RES_DIV1 6
#define RES_DIV2 6
#define B_VALUE1 1482
#define B_VALUE2 1482
#define NTC_SENSOR_PIN1 A0
#define NTC_SENSOR_PIN2 A1

#define HEATER_RES_PIN1 2
#define HEATER_RES_PIN2 3

Timer* timer = new TimerDS1307();

SensorTempNTC10k* sensor1 = new SensorTempNTC10k(NTC_SENSOR_PIN1, RES_DIV1, B_VALUE1);
SensorTempNTC10k* sensor2 = new SensorTempNTC10k(NTC_SENSOR_PIN2, RES_DIV2, B_VALUE2);

ActuatorOnOff* heater_res1 = new ActuatorOnOff(HEATER_RES_PIN1);
ActuatorOnOff* heater_res2 = new ActuatorOnOff(HEATER_RES_PIN2);

BrewController controlador = BrewController(timer, NTC_SENSOR_PIN1, sensor1, HEATER_RES_PIN1, heater_res1);

*/

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
