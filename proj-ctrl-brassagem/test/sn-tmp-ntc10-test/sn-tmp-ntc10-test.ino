#include <sntmpntc10k.h>

#define DELAY_TIME 1000
#define SENSOR_PIN A0
#define B_VALUE 1482.8
#define RES_DIV 6.1 //em kiloohms (lembrar que calculei 6.353kohms para meio em 55C)

SensorTempNTC10k sensor = SensorTempNTC10k(SENSOR_PIN, RES_DIV, B_VALUE);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Início do teste do termistor NTC:");
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("Temperatura lida: ");
  Serial.print(sensor.read());
  Serial.println(" C");
  delay(DELAY_TIME);
}
