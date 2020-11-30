#include <EEPROM.h>
#define eeprom EEPROM

//rampa exemplo
int id_rampa = 1023;
int tempo_rampa = 15;
float temp_mosto = 68.0;
int id_procs_extras = 1;
int pino_sensores[2] = {11, 6};
float valores_ideais[2] = {100.0, 135.0};
int id_fim_receita = 253;

void setup() {
  Serial.begin(9600);
  Serial.println("Início do teste da memória EEPROM:");

  Serial.print("Tamanho da memória: ");
  Serial.println(eeprom.length());

  Serial.println("Colocando a receita na memória");

  int endr_atual = 0;
  while(endr_atual<4) {
    switch(endr_atual) {
      case 0:
        eeprom.write(endr_atual, id_rampa);
        break;
      case 1:
        eeprom.write(endr_atual, tempo_rampa);
        break;
      case 2:
        eeprom.write(endr_atual, temp_mosto);
        break;
      case 3:
        eeprom.write(endr_atual, id_procs_extras);
        break;
    }
    endr_atual++;
  }
  for(int i = 0; i<2; i++) {
    eeprom.write(endr_atual, pino_sensores[i]);
    endr_atual++;
    eeprom.write(endr_atual, valores_ideais[i]);
    endr_atual++;
  }
  eeprom.write(endr_atual, id_fim_receita);

  Serial.print("Espaco disponível depois da receita: ");
  Serial.println(eeprom.length() - (endr_atual +1));
}

boolean n_escrever = false;
void loop() {
  if(n_escrever) return;
  
  Serial.println("Variáveis na memória");
  int endr_atual = 0;
  while(eeprom.read(endr_atual) != id_fim_receita) {
    Serial.print("Variavel no endereco ");
    Serial.println(endr_atual);
    Serial.println(eeprom.read(endr_atual));
    endr_atual++;
    delay(500);
  }
  n_escrever = true;
}
