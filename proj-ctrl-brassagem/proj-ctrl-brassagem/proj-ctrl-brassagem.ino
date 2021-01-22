#include <brewcontroller.h>
#include <timerds1307.h>
#include <sntmpntc10k.h>
#include <actuatoronoff.h>
#include <bluetooth.h>

#define MIN_INF -3.4028235E38
#define REST_STATE 0
#define BREW_STATE 1
#define STOP_BREW_STATE 2
#define ERROR_STATE 3

#define TX_PIN 10
#define RX_PIN 11

const int HTR_PINS[2] = {2, 3}; //pinos dos aquecedores
const int NTC_PINS[2] = {A0, A1}; //pinos dos sensores ntc
const int RES_DIV[2] = {6, 6}; //resistências dos divisores de tensão usados com os sensores ntc
const int B_VALUE[2] = {1482, 1482}; //valures B dos sensores ntc

Msg execute(Msg cmd); //executa um comando e retorna uma mensagem com o resultado da execução
Msg getUpdate(float params[MAX_MSG_PARAM]);
Msg getUpdate(int param_code);
void updateReadings(); //atualiza a leitura dos sensores em uso no processo
void updateAll(); //atualiza todas as variáveis
float bl2flt(boolean arg);
Msg invalidUpdt(); //retorna uma mensagem inválida


Bluetooth bluetooth = Bluetooth(RX_PIN, TX_PIN);

Timer* timer = new TimerDS1307();
SensorTempNTC10k* sensor1 = new SensorTempNTC10k(NTC_PINS[1], RES_DIV[1],B_VALUE[1]);
SensorTempNTC10k* sensor2 = new SensorTempNTC10k(NTC_PINS[2], RES_DIV[2], B_VALUE[2]);
ActuatorOnOff* htr1 = new ActuatorOnOff(HTR_PINS[1]);
ActuatorOnOff* htr2 = new ActuatorOnOff(HTR_PINS[2]);

BrewController brewer = BrewController(timer, NTC_PINS[2], sensor1, HTR_PINS[1], htr1);

boolean phone_connected = false;


void setup() {
  brewer.addSensor(NTC_PINS[2], sensor2);
  brewer.addActuator(HTR_PINS[2], htr2);
}

void loop() {
  //rotina normal
  int status = brewer.getStatus();

  if(status == REST_STATE) {
    updateReadings();
  } else if(status == BREW_STATE) {
    if(!brewer.run()) {
      brewer.stop();
      bluetooth.sendUpdate( getUpdate(STATUS) ); //o app deve entender que houve um erro pela mudança não requisitada de status
    } else {
      updateReadings();
    }
  } else if(status == STOP_BREW_STATE) {
    updateReadings();    
  } else {
    bluetooth.sendUpdate( getUpdate(STATUS) );
    return;
  }

  //interação do usuário
  if(bluetooth.cmdAvailable()) {
    Msg result = execute(bluetooth.getCmd());
    bluetooth.sendUpdate(result);
  }
}




Msg execute(Msg cmd) {
  Msg result;
  result.id = CMD_RETURN;
  result.params[0] = cmd.id;

  switch(cmd.id) {
    case CONNECTION:
      if(!phone_connected) {
        phone_connected = true;
        result.params[1] = 1;
      } else {
        result.params[1] = 0;
      }
      break;
    case REQUEST:
      result = getUpdate(cmd.params);
      break;
    case UPDT_ALL:
      updateAll();
      break;
    case START:
      result.params[1] = bl2flt(brewer.start());
      break;
    case RESTART:
      result.params[1] = bl2flt(brewer.start(true));
      break;
    case STOP:
      result.params[1] = bl2flt(brewer.stop());
      break;
    case RESET:
      result.params[1] = bl2flt(brewer.reset());
      break;
    case ACTIVATE:
      result.params[1] = bl2flt(brewer.activate((int) cmd.params[0]));
      break;
    case DEACTIVATE:
      result.params[1] = bl2flt(brewer.activate((int) cmd.params[0]));
      break;
    case SET_SLOPE:
      result.params[1] = bl2flt(brewer.setSlope((int) cmd.params[0], (int) cmd.params[1], cmd.params[2], cmd.params[3]));
      break;
    case RESET_SLOPE:
      result.params[1] = -1;
      brewer.resetSlope( (int) cmd.params[0], (boolean) cmd.params[1] );
      break;
    case RESET_ALL_SLOPES:
      result.params[1] = -1;
      brewer.resetAllSlopes( (boolean) cmd.params[0] );
      break;
    case RMV_SLOPE:
      result.params[1] = -1;
      brewer.removeSlope( (int) cmd.params[0] );
      break;
    case RMV_ALL_SLOPES:
      result.params[1] = -1;
      brewer.removeAllSlopes();
      break;
    case ADD_PROC:
      result.params[1] = bl2flt(brewer.addProc2Slope( (int) cmd.params[0], (int) cmd.params[1], (int) cmd.params[2], cmd.params[3], cmd.params[4]) );
      break;
    case RMV_PROC:
      result.params[1] = bl2flt(brewer.rmvProc2Slope( (int) cmd.params[0], (int) cmd.params[1], (int) cmd.params[2], cmd.params[3], cmd.params[4]) );
      break;
    case CLR_MEM:
      result.params[1] = -1;
      brewer.clearAllMemory();
      break;
    default:
      result = invalidUpdt();
  }

  return result;
}

Msg getUpdate(float params[MAX_MSG_PARAM]) {
  Msg updt;
  int unused = 0;
  updt.id = (int) params[0];
  ControlProcess proc = brewer.getControlProcess((int) params[1], (int) params[2]); //aqui pois causa problema de redeclaração dentro do switch

  switch(updt.id) {
  case STATUS:
    updt.params[0] = (float) brewer.getStatus();
    unused = 1;
  case CMD_RETURN:
    break;
  case SLOPE_NUM:
    updt.params[0] = (float)brewer.getCurrentSlopeNumber();
    unused = 1;
    break;
  case SLOPE_TEMP:
    updt.params[0] = brewer.getSlopeTemp(brewer.getCurrentSlopeNumber());
    unused = 1;
    break;
  case CURR_TEMP:
    updt.params[0] = brewer.getSensorReading(HTR_PINS[0]);
    unused = 1;
    break;
  case DURATION:
    updt.params[0] = (float) brewer.getCurrentSlopeDuration();
    unused = 1;
    break;
  case TIME_LEFT:
    updt.params[0] = brewer.getTimeLeft();
    unused = 1;
    break;
  case MEM_LEFT:
    updt.params[0] = (float) brewer.getMemoryLeft();
    unused = 1;
    break;
  case PROCS_NUM:
    ///////////////////////////////////////////////////////////////////
    //LEMBRAR DE COLOCAR UM MÉTODO PARA SABER O NÚMERO DE PROCESSOS NA BREW CONTROLLER
    ///////////////////////////////////////////////////////////////////

    //updt.params[0] = brewer.getProcsNum(brewer.getCurrentSlopeNumber());
    unused = 1;
    break;
  case PROC:
    if(proc.sensor_pin < 0) {
      updt = invalidUpdt();
      unused = MAX_MSG_PARAM;
    } else {
      updt.params[0] = proc.sensor_pin;
      updt.params[1] = proc.actuator_pin;
      updt.params[2] = proc.ref_value;
      updt.params[3] = proc.tolerance;
      unused = 4;
    }
    break;
  case PROC_READ:
    if(proc.sensor_pin < 0) {
      updt = invalidUpdt();
      unused = MAX_MSG_PARAM;
    } else {
      updt.params[0] = brewer.getSensorReading(proc.sensor_pin);
      unused = 1;
    }
    break;
  default:
    updt = invalidUpdt();
    unused = MAX_MSG_PARAM;
  }

  while(unused < MAX_MSG_PARAM) {
    updt.params[unused] = 0;
    unused++;
  }

  return updt;
}

Msg getUpdate(int param_code) {
  float params[MAX_MSG_PARAM];
  params[0] = param_code;
  return getUpdate(params);
}

void updateReadings() {
  
}

////////////////////////////////////////////////////
//TO BE REMADE
////////////////////////////////////////////////////
void updateAll() {
  for(int i = 0; i <= PARAM_MAX; i++) {
    //bluetooth.sendUpdate(getUpdate(i));
    delay(100);
  }
}

float bl2flt(boolean arg) {
  return arg?1:0;
}

Msg invalidUpdt() {
  Msg msg;
  msg.id = ERROR_INVALID_PRM;
  for(int i = 0; i < MAX_MSG_PARAM; i++) msg.params[i] = -1;
  return msg;
}
