#include <SoftwareSerial.h>

SoftwareSerial phone(2, 3); //rx, tx, (trocado na conexão)
String command = "";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  phone.begin(9600);
  delay(2000);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available()) {
    command = "";
    while(Serial.available()) command += Serial.readString();
    
    Serial.print("Comando: ");
    Serial.println(command);
    int tries = 0;
    int sent_bytes = 0;
    
    while(sent_bytes < command.length()) {
      sent_bytes = phone.println(command);
      tries++;

      if(tries>5) break;
    }
    
    Serial.print("Bytes enviados: ");
    Serial.println(sent_bytes);
    //Serial.println(command.length());
  }
  /*
  if(phone.available()) {
    command = "";
    while(phone.available()) command += phone.readString();
    
    phone.write("Received");
    Serial.println(command);
  }
  */
}

/*
 * 5*R2/(R1+R2) = 5/(R1/R2 + 1) = 3.3
 * ==> R1/R2 = 5/3.3 - 1 = 17/33
*/
