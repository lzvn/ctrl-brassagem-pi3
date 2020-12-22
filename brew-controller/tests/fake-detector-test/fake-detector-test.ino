#include <brewcontroller.h>
#include <timerds1307.h>
#include <fakedetector.h>
#include <actuatoronoff.h>

#include <EEPROM.h>

#define LED_PIN 3
#define DETECTOR_PIN 2
#define DETECTOR_ON 1
#define TOLERANCE 0
#define DURATION1 1
#define DURATION2 2

ActuatorOnOff* led = new ActuatorOnOff(LED_PIN, DETECTOR_ON, TOLERANCE);
FakeDetector* detector = new FakeDetector(DETECTOR_PIN);
TimerDS1307* timer = new TimerDS1307();
//OBS: SDA -> A4, SCL -> A5
BrewController brewer;



boolean start_brewing = true;

void setup() {
  Serial.begin(9600);

  //for(int i = 0; i < EEPROM.length(); i++) EEPROM.write(i, 0);

  brewer = BrewController(timer, DETECTOR_PIN, detector, LED_PIN, led);
  
  brewer.removeAllSlopes();
  Serial.print("Status do controlador (deve ser 0): ");
  Serial.println(brewer.getStatus());
  Serial.print("Memoria disponível (deve ser 1008): ");
  Serial.println(brewer.getMemoryLeft());
  
  Serial.print("Adicionando uma rampa, deve retornar true se der tudo certo: ");
  Serial.println(brewer.setSlope(1,DURATION1, DETECTOR_ON, TOLERANCE));

  Serial.print("Status do controlador (deve ser 0): ");
  Serial.println(brewer.getStatus());
  Serial.print("Veficando memória (deve restar 1008): ");
  Serial.println(brewer.getMemoryLeft());

  Serial.print("Adicionando mais uma rampa, deve retornar true se der tudo certo: ");
  Serial.println(brewer.setSlope(2,DURATION2, DETECTOR_ON, TOLERANCE));

  Serial.print("Status do controlador (deve ser 0): ");
  Serial.println(brewer.getStatus()); 
  Serial.print("Veficando memória (deve restar 1003): ");
  Serial.println(brewer.getMemoryLeft()); 
  
  Serial.println("Digite s para iniciar o teste ou n para cancelar:");
  while(1) {
    if(Serial.available()) {
      char answer = Serial.read();
      start_brewing = (answer=='s')?true:false;
      
      if(start_brewing) {
        Serial.print("Iniciando a brassagem (deve ser true): ");
        Serial.println(brewer.start());
        Serial.print("Status do controlador (deve ser 1): ");
        Serial.println(brewer.getStatus());
        Serial.println("Durante a execução, digite p para parar a brassagem e r para retomá-la");
        Serial.println("Quando pausado, digite q para ligar e w para desligar a saída (so funciona quando pausado)");
      } else {
        Serial.print("Processo cancelado");
      }
      break;
    }
    delay(200);
  }
}

void loop() {
  if(!start_brewing) return;
  
  while(brewer.getStatus()!=0 && brewer.getStatus()!=3) {
    Serial.print("Resultado de atualizar o controlador (deve ser 1): ");
    Serial.println(brewer.run());

    Serial.println("--------------------------------");
    Serial.print("Número da rampa atual: ");
    Serial.println(brewer.getCurrentSlopeNumber());
    Serial.print("Temperatura da rampa atual (deve ser 1): ");
    Serial.println(brewer.getSlopeTemp(brewer.getCurrentSlopeNumber()));
    Serial.print("Tempo restante em minutos: ");
    Serial.println(brewer.getTimeLeft());
    Serial.print("Duração da rampa atual: ");
    Serial.println(brewer.getCurrentSlopeDuration());
    Serial.print("Status do controlador (deve ser 1 ativo e 2 para pausado): ");
    Serial.println(brewer.getStatus());
    Serial.println("--------------------------------");

    if(Serial.available()) {
      char command = Serial.read();
      Serial.println("************************************");
      if(command == 'p') Serial.println(brewer.stop());
      else if(command == 'r') Serial.println(brewer.start(true));
      else if(command == 'q') Serial.println(brewer.activate(LED_PIN));
      else if(command == 'w') Serial.println(brewer.deactivate(LED_PIN));
      else Serial.println("Comando invalido");
      Serial.println("************************************");
    }
    
    delay(1000);
    
  }
  
  Serial.println("Teste finalizado");
  start_brewing = false;
}
