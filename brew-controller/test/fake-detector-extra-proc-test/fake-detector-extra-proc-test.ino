#include <brewcontroller.h>
#include <timerds1307.h>
#include <fakedetector.h>
#include <actuatoronoff.h>
#include <interface.h>

#define MAIN_LED_PIN 3
#define MAIN_DTCR_PIN 2

#define PROC_LED_PIN 5
#define PROC_DTCR_PIN 4

#define DTCR_ON 1
#define TOLERANCE 0

#define DURATION1 1
#define DURATION2 2

TimerDS1307* timer = new TimerDS1307();
//OBS: SDA -> A4, SCL -> A5

ActuatorOnOff* main_led = new ActuatorOnOff(MAIN_LED_PIN, DTCR_ON, TOLERANCE);
FakeDetector* main_dtcr = new FakeDetector(MAIN_DTCR_PIN);

ActuatorOnOff* proc_led = new ActuatorOnOff(PROC_LED_PIN, DTCR_ON, TOLERANCE);
FakeDetector* proc_dtcr = new FakeDetector(PROC_DTCR_PIN);

BrewController brewer(timer, MAIN_DTCR_PIN, main_dtcr, MAIN_LED_PIN, main_led);

boolean start_brewing = true;

void printMemToEnd();

void setup() {
  Serial.begin(9600);

  ///////////////////////////////////////////////////////////////////////
  //adição das duas rampas

  //for(int i = 0; i < EEPROM.length(); i++) EEPROM.write(i, 0);

  brewer = BrewController(timer, MAIN_DTCR_PIN, main_dtcr, MAIN_LED_PIN, main_led);

  Serial.println(F("Tabela de pinos: "));
  brewer.getPinMatrix();
  
  brewer.removeAllSlopes();
  Serial.print(F(status_msg));
  Serial.print(F(" (deve ser 0): "));
  Serial.println(brewer.getStatus());
  Serial.print(F(mem_left_msg));
  Serial.print(F(" 1008): "));
  Serial.println(brewer.getMemoryLeft());
  
  Serial.print(F(new_slope_msg));
  Serial.println(brewer.setSlope(1,DURATION1, DTCR_ON, TOLERANCE));
  
  Serial.print(F(status_msg));
  Serial.print(" (deve ser 0): ");
  Serial.println(brewer.getStatus());
  Serial.print(F(mem_left_msg));
  Serial.print(" 1008): ");
  Serial.println(brewer.getMemoryLeft());

  Serial.print(F(new_slope_msg));
  Serial.println(brewer.setSlope(2,DURATION2, DTCR_ON, TOLERANCE));

  Serial.print(F(status_msg));
  Serial.print(" (deve ser 0): ");
  Serial.println(brewer.getStatus());
  Serial.print(F(mem_left_msg));
  Serial.print(" 1003): ");
  Serial.println(brewer.getMemoryLeft()); 

  ///////////////////////////////////////////////////////////////////////
  //adição de processos extras

  Serial.print(F("Adicionando um sensor: "));
  Serial.println(brewer.addSensor(PROC_DTCR_PIN, proc_dtcr));
  Serial.print(F("Adicionando um atuador: "));
  Serial.println(brewer.addActuator(PROC_LED_PIN, proc_led));

  brewer.getPinMatrix();

  Serial.print(F(new_proc_msg));
  Serial.println(brewer.addProc2Slope(1, PROC_DTCR_PIN, PROC_LED_PIN, DTCR_ON, TOLERANCE));
  Serial.print(F(rmv_proc_msg));
  Serial.println(brewer.rmvProc2Slope(1, PROC_DTCR_PIN, PROC_LED_PIN, DTCR_ON, TOLERANCE));

  Serial.print(F(new_proc_msg));
  Serial.println(brewer.addProc2Slope(2, PROC_DTCR_PIN, PROC_LED_PIN, DTCR_ON, TOLERANCE));

  ////////////////////////////////////////////////////////////////////////
  //início da brassagem
  
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
      else if(command == 'q') Serial.println(brewer.activate(MAIN_LED_PIN));
      else if(command == 'w') Serial.println(brewer.deactivate(MAIN_LED_PIN));
      else Serial.println(F("Comando invalido"));
      Serial.println(F(stars));
    }
    
    delay(1000);
  }
  
  Serial.println(F(end_msg));
  start_brewing = false;
}

void printMemToEnd() {
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
