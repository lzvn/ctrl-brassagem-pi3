#include <Arduino.h>
#include <brewcontroller.h>

#ifndef interface_h
#define interface_h

const PROGMEM char status_msg[25] = "Status do controlador";

const PROGMEM char mem_left_msg[30] = "Memoria disponível (deve ser ";

const PROGMEM char new_slope_msg[62] = "Adicionando uma rampa, deve retornar true se der tudo certo: ";
const PROGMEM char new_proc_msg[54] = "Adicionando processo a uma rampa, true se der certo: ";
const PROGMEM char rmv_proc_msg[46] = "Removendo processo extra, true se der certo: ";

const PROGMEM char cmd_start_msg[50] = "Digite s para iniciar o teste ou n para cancelar:";
const PROGMEM char start_msg[40] = "Iniciando a brassagem (deve ser true): ";

const PROGMEM char pause_cmd_msg[82] = "Durante a execução, digite p para parar a brassagem e r para retomá-la";
const PROGMEM char manual_ctrl_msg[92] = "Quando pausado, digite q para ligar e w para desligar a saída (so funciona quando pausado)";

const PROGMEM char ctrl_update_msg[52] = "Resultado de atualizar o controlador (deve ser 1): ";

const PROGMEM char current_slope_msg[24] = "Numero da rampa atual: ";
const PROGMEM char current_temp_msg[42] = "Temperatura da rampa atual (deve ser 1): ";
const PROGMEM char time_left_msg[28] = "Tempo restante em minutos: ";
const PROGMEM char duration_msg[25] = "Duracao da rampa atual: ";

const PROGMEM char invalid_cmd_msg[17] = "Comando invalido";

const PROGMEM char cancel_msg[20] = "Processo cancelado";
const PROGMEM char end_msg[15] = "O teste acabou";

const PROGMEM char lines[30] = "----------------------------";
const PROGMEM char stars[30] = "*****************************";

#endif
