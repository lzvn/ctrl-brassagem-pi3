#include <timerds1307.h>

#define FREQ_SERIAL 9600
#define TEST_TIME 60000

//OBS: SDA -> A4, SCL -> A5

TimerDS1307 timer;

void boolprintln(boolean var);
void timeprintln(float time);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  if(timer.verifyClock()) Serial.println("DS1307 está funcionando");
  else Serial.println("DS1307 não está funcionando");
  
  timer.start(TEST_TIME/TEST_TIME);
  
  int i = 0;
  boolean stop = false;
  
  while(!timer.isTimeOver() || i < 50) {
    Serial.print(i);
    Serial.println(" segundos se passarão");
    Serial.print("Tempo que falta: ");
    timeprintln(timer.timeLeft());
    Serial.print("Se a contagem acabou: ");
    boolprintln(timer.isTimeOver());
    
    if(!stop) i++;

    //para parar ou retomar a contagem
    if(Serial.available()) {
      while(Serial.available()) Serial.read();
      timer.stopRestart();
      stop = !stop;
    }
    delay(1000);
  }

  Serial.println("Intervalo de contagem antes do reset: ");
  Serial.println(timer.timeSet());
  Serial.println("Depois do reset (deveria ser 0)");
  timer.reset();
  Serial.println(timer.timeSet());
  
  Serial.println("Fim do teste");
}

void loop() {
  // put your main code here, to run repeatedly:
}

void boolprintln(boolean var) {
  Serial.println((var)?"true":"false");
}

void timeprintln(float time) {
  int minutes = (int) time;
  int seconds = (int) (60*(time - minutes));
  
  Serial.print(minutes);
  Serial.print(":");
  Serial.println(seconds);
}
