#include <sensor.h>
#include <fakesor1.h>
#include <fakesor2.h>

Fakesor1 fake1;
Fakesor2 fake2;

void printSensorRead(Sensor *sensor);
int pointers[2];

void setup() {
  Serial.begin(9600);
  pointers[0] = (int)&fake1;
  pointers[1] = (int)&fake2;
}

void loop() {
  int i = 0;
  for(i = 0; i < 2; i++) {
    Serial.print("Fakesor");
    Serial.println(i+1);
    printSensorRead((Sensor*) pointers[i]);
  }
  delay(1000);
}

void printSensorRead(Sensor *sensor) {
  Serial.print("Leitura do sensor: ");
  Serial.println(sensor->read());
}
