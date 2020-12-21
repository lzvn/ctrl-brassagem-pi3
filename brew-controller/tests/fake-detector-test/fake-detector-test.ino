#include <brewcontroller.h>
#include <timerds1307.h>
#include <fakedetector.h>
#include <actuatoronoff.h>

#define LED_PIN 2
#define DETECTOR_PIN 3

ActuatorOnOff* led = new ActuatorOnOff(LED_PIN, 1, 0);
FakeDetector* detector = new FakeDetector(DETECTOR_PIN);
TimerDS1307* timer = new TimerDS1307();
BrewController controller(timer, DETECTOR_PIN, detector, LED_PIN, led);

void setup() {
  
}

void loop() {
  
}
