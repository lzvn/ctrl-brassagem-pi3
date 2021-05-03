#include <sensor.h>
#include <fakesor1.h>
#include <fakesor2.h>

Fakesor1 fake1;
Fakesor2 fake2;

void printSensorRead(Sensor *sensor);

void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.println("Fakesor1");
  printSensorRead(&fake1);
  Serial.println("Fakesor2");
  printSensorRead(&fake2);
  delay(1000);
}

void printSensorRead(Sensor *sensor) {
  Serial.print("Leitura do sensor: ");
  Serial.println(sensor->read());
}
