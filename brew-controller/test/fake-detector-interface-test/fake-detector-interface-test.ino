#include <brewcontroller.h>
#include <timerds1307.h>
#include <fakedetector.h>
#include <actuatoronoff.h>
#include <interface.h>

#define LED_PIN 3
#define DETECTOR_PIN 2
#define DETECTOR_ON 1
#define TOLERANCE 0
#define DURATION1 1
#define DURATION2 2

ActuatorOnOff* led = new ActuatorOnOff(LED_PIN, DETECTOR_ON, TOLERANCE); //150 bytes (7%)
FakeDetector* detector = new FakeDetector(DETECTOR_PIN); //158 bytes (7%)
TimerDS1307* timer = new TimerDS1307(); //277 bytes (13%)
//OBS: SDA -> A4, SCL -> A5
BrewController brewer(timer, DETECTOR_PIN, detector, LED_PIN, led); //329 bytes (16%)

boolean start_brewing = true;

void printToEnd();

void setup() {
  Serial.begin(9600);

  //for(int i = 0; i < EEPROM.length(); i++) EEPROM.write(i, 0);

 brewer = BrewController(timer, DETECTOR_PIN, detector, LED_PIN, led);
  
  brewer.removeAllSlopes();
  Serial.print(F(status_msg));
  Serial.print(F(" (deve ser 0): "));
  Serial.println(brewer.getStatus());
  Serial.print(F(mem_left_msg));
  Serial.print(F(" 1008): "));
  Serial.println(brewer.getMemoryLeft());
  
  Serial.print(F(new_slope_msg));
  Serial.println(brewer.setSlope(1,DURATION1, DETECTOR_ON, TOLERANCE));

  Serial.print(F(status_msg));
  Serial.print(" (deve ser 0): ");
  Serial.println(brewer.getStatus());
  Serial.print(F(mem_left_msg));
  Serial.print(" 1008): ");
  Serial.println(brewer.getMemoryLeft());

  Serial.print(F(new_slope_msg));
  Serial.println(brewer.setSlope(2,DURATION2, DETECTOR_ON, TOLERANCE));

  Serial.print(F(status_msg));
  Serial.print(" (deve ser 0): ");
  Serial.println(brewer.getStatus());
  Serial.print(F(mem_left_msg));
  Serial.print(" 1003): ");
  Serial.println(brewer.getMemoryLeft()); 

  
  start_brewing = false;
  return;

  Serial.println(F(cmd_start_msg));
  while(1) {
    if(Serial.available()) {
      char answer = Serial.read();
      start_brewing = (answer=='s')?true:false;
      
      if(start_brewing) {
        Serial.print(F(start_msg));
        Serial.println(brewer.start());
        Serial.print(F(status_msg));
        Serial.print(" (deve ser 1): ");
        Serial.println(brewer.getStatus());
        Serial.println(F(pause_cmd_msg));
        Serial.println(F(manual_ctrl_msg));
      } else {
        Serial.print(F(cancel_msg));
      }
      break;
    }
    delay(200);
  }
}

void loop() {
    if(!start_brewing) return;
  
  while(brewer.getStatus()!=0 && brewer.getStatus()!=3) {
    Serial.print(F(ctrl_update_msg));
    Serial.println(brewer.run());

    Serial.println(F(lines));
    Serial.print(F(current_slope_msg));
    Serial.println(brewer.getCurrentSlopeNumber());
    Serial.print(F(current_temp_msg));
    Serial.println(brewer.getSlopeTemp(brewer.getCurrentSlopeNumber()));
    Serial.print(F(time_left_msg));
    Serial.println(brewer.getTimeLeft());
    Serial.print(F(duration_msg));
    Serial.println(brewer.getCurrentSlopeDuration());
    Serial.print(F(status_msg));
    Serial.print(F(" (deve ser 1 ativo e 2 para pausado): "));
    Serial.println(brewer.getStatus());
    Serial.println(F(lines));

    if(Serial.available()) {
      char command = Serial.read();
      Serial.println(F(stars));
      if(command == 'p') Serial.println(brewer.stop());
      else if(command == 'r') Serial.println(brewer.start(true));
      else if(command == 'q') Serial.println(brewer.activate(LED_PIN));
      else if(command == 'w') Serial.println(brewer.deactivate(LED_PIN));
      else Serial.println(F("Comando invalido"));
      Serial.println(F(stars));
    }
    
    delay(1000);
  }
  
  Serial.println(F(end_msg));
  start_brewing = false;
}

void printToEnd() {
    int i = 0;
  int num = 0;
  while(1) {
    num = EEPROM.read(i);
    Serial.print("Endereco ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(num);
    i++;
    if(num==255) break;
    if(i>255) break;
  }
}
