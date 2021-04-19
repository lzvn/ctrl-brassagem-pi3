#include <brewcontroller.h>
#include <timerds1307.h>
#include <sntmpntc10k.h>
#include <actuatoronoff.h>
#include <actuatorpidgas.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define UP 8 //tecla para confirmar ou avançar a depender do contexto
#define RIGHT 9
#define DOWN 10 //tecla para voltar ou cancelar a depender do contexto
#define LEFT 11
#define NO_KEY -1
#define CLR_ROW "                "
#define BTN_REST 100
#define REFRESH_TIME 500
#define PARAM_REFRESH_TIME 5 //em relação a REFRESH_TIME

#define MIN_INF -3.4028235E38
#define REST_STATE 0
#define BREW_STATE 1
#define STOP_BREW_STATE 2
#define ERROR_STATE 3

#define KP 1
#define KI 0.02
#define KD 0.01

const int HTR_PINS[2] = {4, 3}; //pinos dos aquecedores
const int NTC_PINS[2] = {A0, A1}; //pinos dos sensores ntc
const float RES_DIV[2] = {6.2, 5.7}; //resistências dos divisores de tensão usados com os sensores ntc
const int B_VALUE[2] = {3872, 3872}; //valures B dos sensores ntc

Timer* timer = new TimerDS1307();
SensorTempNTC10k* sensor1 = new SensorTempNTC10k(NTC_PINS[0], RES_DIV[0], B_VALUE[0]);
SensorTempNTC10k* sensor2 = new SensorTempNTC10k(NTC_PINS[1], RES_DIV[1], B_VALUE[1]);


ActuatorPIDGas* htr1 = new ActuatorPIDGas(HTR_PINS[0], 25, 1, KP, KI, KD);
ActuatorOnOff* htr2 = new ActuatorOnOff(HTR_PINS[1]);

BrewController brewer = BrewController(timer, NTC_PINS[0], sensor1, HTR_PINS[0], htr1);

LiquidCrystal_I2C lcd(0x27, 16, 2);

boolean error_state = false;
int print_time = 0;
int param_time = 0;
int param_index = 0;
boolean manual_mode = false;
boolean brewing = false;

///////////////// Declaração das funções

//funções do display
void print(String text, int row, boolean clr = false);
void printFullscreen(String up_text, String bottom_text);
void clearRow(int row);

//funções de interação com o usuário, menus e configurações
int checkInput(boolean wait_pressing = false);
void act(boolean stop_brew_state = false);
void activateManual();
void configSlopes();
void setSlope(int position);
Slope setParam(int position, int param_code);
void setProc(int position);
ControlProcess setProcParam(int position, int param_code);

//atualização de estados
void printState();
void printMemLeft();

void setup() {
  brewer.addSensor(NTC_PINS[1], sensor2);
  brewer.addActuator(HTR_PINS[1], htr2);

  pinMode(UP, INPUT);
  pinMode(RIGHT, INPUT);
  pinMode(DOWN, INPUT);
  pinMode(LEFT, INPUT);

  Serial.begin(9600);
  
  lcd.init();
  lcd.setBacklight(HIGH);
  printFullscreen(F("Iniciando..."), F("1, 2, 3..."));
  delay(1000);
  lcd.clear();
}

void loop() {
  int status = brewer.getStatus();
  int input = checkInput(true);

  if(input != NO_KEY) {
    print_time = REFRESH_TIME+1;
    param_time = PARAM_REFRESH_TIME+1;
  }
  if(input == LEFT) {
    param_index = (param_index>1)?param_index-2:4+param_index;
  }

  if(status == REST_STATE) {

      if(brewing) {
        printFullscreen(F("Brassagem"), F("encerrada"));
        while(checkInput(true) == NO_KEY);
        brewing = false;
      }

      htr1->closeValve();
    
      if(input == DOWN) configSlopes();

      if(input == UP) {
        act();
      }
      
  } else if(status == BREW_STATE) {
    if(!brewer.run()) {
      brewer.stop();
      printFullscreen(F("Erro no brassagem"), F("Parando..."));
      delay(2000);
      lcd.clear();
    }

    if(input == UP) {
      brewer.stop();
      printFullscreen(F("Parando..."), F(""));
      delay(1000);
      lcd.clear();
    }

    if(input == DOWN) {
      brewer.reset();
      printFullscreen(F("Resetando..."), F(""));
      delay(1000);
      lcd.clear();
    }
  } else if(status == STOP_BREW_STATE) {
    if(input == UP) {
      act(true);
    }
    if(input == DOWN) {
      brewer.reset();
      printFullscreen(F("Resetando..."), F(""));
      delay(1000);
      lcd.clear();
    }
  } else {
    if(!error_state) {
      printFullscreen(F("Erro"), F("Erro"));
      htr1->closeValve();
      error_state = true;
    }
    return;
  }

  printState();

}


/////////////////////////// Funções


/////////////////////////// funções do display

void print(String text, int row, boolean clr = false) {
  clearRow(row);
  lcd.setCursor(0, row);
  lcd.print(text);
}

void printFullscreen(String up_text, String bottom_text) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(up_text);
  lcd.setCursor(0, 1);
  lcd.print(bottom_text);
}

void clearRow(int row) {
    lcd.setCursor(0, row);
    lcd.print(CLR_ROW);
    lcd.setCursor(0, row);
}

/////////////////////////// funções de interação com o usuário, menus e configurações

int checkInput(boolean wait_pressing = false) {
  int input = NO_KEY;
  int keys[4] = {UP, RIGHT, DOWN, LEFT};

  for(int i = 0; i < 4; i++) if(digitalRead(keys[i])) input = keys[i];

  if(input != NO_KEY) {
    if(wait_pressing) while(checkInput()!= NO_KEY);
    else delay(BTN_REST);
  }

  return input;
}

void act(boolean stop_brew_state = false) {

  boolean first_render = true;
  boolean manual = false;
  
  while(1) {
    int input = checkInput(true);

    if(input == RIGHT || input == LEFT) {
      manual = !manual;
    } else if(input == DOWN) {
      break;
    } else if(input == UP) {
      if(manual) {
        activateManual();
        break;
      } else {
        for(int i = 0; i < 1; i++) brewer.deactivate(HTR_PINS[i]);
        manual_mode = false;
        printFullscreen(stop_brew_state?F("Voltando"):F("Iniciando"), (""));
        boolean success = brewer.start(stop_brew_state);
        if(success) {
          printFullscreen(stop_brew_state?F("Voltando"):F("Iniciando"), (""));
          delay(1000);
          lcd.clear();
          brewing = true;
        } else {
          printFullscreen(F("Falha ao iniciar"), (""));
          delay(1000);
          lcd.clear();
        }
        print_time = 0;
        param_index = 0;
        param_time = 0;
        break;
      }
    } else if(input == NO_KEY && !first_render) {
      continue;
    }

    if(manual) printFullscreen(F("Modo manual"), F(""));
    else printFullscreen((stop_brew_state)?F("Reiniciar"):F("Iniciar"), F("brassagem"));

    first_render = false;
  }
}

void activateManual() {
  int current_actuator = 0;
  boolean first_render =  true;
  boolean activated = brewer.isActuatorOn(HTR_PINS[current_actuator]);
  while(1) {
    int input = checkInput(true);
    if(input == RIGHT || input == LEFT) {
      current_actuator = 1 - current_actuator;
    } else if(input == DOWN) {
      break;
    } else if(input == UP) {
      if(!activated) {
        brewer.activate(HTR_PINS[current_actuator]);
      } else {
        brewer.deactivate(HTR_PINS[current_actuator]);
        if(current_actuator == 0) htr1->closeValve();
      }
      manual_mode = true;
    } else if(input == NO_KEY && !first_render) {
      continue;
    }
    activated = brewer.isActuatorOn(HTR_PINS[current_actuator]);
    printFullscreen((current_actuator)?F("Auxiliar"):F("Principal"), activated?F("Ativado"):F("Desativado"));
    first_render = false;
  }

}

void configSlopes() {
  delay(BTN_REST);
  int current_slope = 1;
  boolean set_slope = false;
  boolean first_render = true;
  int total_slopes = brewer.getNumberOfSlopes();
  lcd.clear();

  if(total_slopes <= 0) {
    brewer.clearAllMemory();
    total_slopes = 1;
  }
  
  while(1) {
    int input = checkInput(true);
    
    if(input == RIGHT) {
      current_slope = (current_slope+1 <= total_slopes+1)?current_slope+1:total_slopes+1;
      clearRow(1);
    } else if(input == LEFT) {
      current_slope = (current_slope-1 >= 1)?current_slope-1:1;
      clearRow(1);
    } else if(input == DOWN) {
      set_slope = true;
    } else if(input == UP) {
      printFullscreen(F("Salvando"), F("1, 2, 3..."));
      delay(1000);
      break;
    } else if(input == NO_KEY && !first_render) {
      continue;
    }

    if(set_slope) {
      setSlope(current_slope);
      total_slopes = brewer.getNumberOfSlopes();
      set_slope = false;
      first_render = true;
      continue;
    }

    print(F("Configurar rampas"), 0);
    
    if(current_slope <= total_slopes) {
      print("Rampa " + String(current_slope), 1);
    } else {
      print("Adicionar rampa " + String(current_slope), 1 );
    }

    first_render = false;
  }
}

void setSlope(int position) {
  int total_slopes = brewer.getNumberOfSlopes();
  int default_duration = 0;
  float default_moist = 25.0;
  float default_tolerance = 1.0;
  
  if(position == total_slopes + 1) brewer.setSlope(position, default_duration, default_moist, default_tolerance);
  
  Slope slope = brewer.getSlope(position);
  ControlProcess proc;
  if(slope.extra_procs) proc = brewer.getControlProcess(position, 1);

  boolean first_render = true;
  int param_code = 0;
  boolean set_param = false; //0 para não nenhuma mudança, 1 para parâmetros simples e 2 para processo extra
  
  while(1) {
    int input = checkInput(true);
    if(input == RIGHT) {
      param_code = (param_code < 4)?param_code+1:4;
    } else if(input == LEFT) {
      param_code = (param_code > 0)?param_code-1:0;
    } else if(input == DOWN) {
      set_param = true;
    } else if(input == UP) {
      printFullscreen("Salvando", "1, 2, 3...");
      delay(1000);
      printMemLeft();
      break;
    } else if(input == NO_KEY && !first_render) {
      continue;
    }

    switch(param_code) {
      case 0:
        printFullscreen(F("Temperatura"), String(slope.temp));
        break;
      case 1:
        printFullscreen(F("Duracao"), String(slope.duration));
        break;
      case 2:
        printFullscreen(F("Tolerancia"), String(slope.tolerance));
        break;
      case 3:
        printFullscreen(F("Processo extra"), (slope.extra_procs)?(String(proc.ref_value) + ", " + String(proc.tolerance)):"Adicionar processo");
        break;
      case 4:
        print(F("Remover rampa"), 0, true);
        clearRow(1);
        break;
      default:
        printFullscreen(F("Erro"), F(""));
    }

    if(set_param) {
      if(param_code == 4) {
        brewer.removeSlope(position);
        if(brewer.getNumberOfSlopes() <= 0) brewer.clearAllMemory();
        break;
      } else if(param_code == 3) {
        setProc(position);
        proc = brewer.getControlProcess(position, 1);
        slope = brewer.getSlope(position);
      } else {
        
        Slope new_slope = setParam(position, param_code);
        boolean success = brewer.setSlope(position, new_slope.duration, new_slope.temp, new_slope.tolerance);
        if(success) {
          printFullscreen(F("Sucesso ao salvar"), F(""));
          slope = new_slope;
        } else {
          printFullscreen(F("Falha ao salvar"), F(""));
        }
        delay(1000);
      }
      printMemLeft();
      set_param = false;
      first_render = true;
      continue;
    }


    first_render = false;
  }
}

Slope setParam(int position, int param_code) {
  Slope slope = brewer.getSlope(position);
  float new_param = 0;
  float step = 1;

  switch(param_code) {
    case 0: 
      new_param = slope.temp; 
      //step = 0.5;
      break;
    case 1: 
      new_param = slope.duration;
      //step = 1;
      break;
    case 2: 
      new_param = slope.tolerance;
      //step = 0.1;
      break;
    default: return slope;
  }

  boolean first_render = true;
  while(1) {
    int input = checkInput();
    
    if(input == RIGHT) {
      new_param+=step;
      if(new_param > 249) new_param = 249;
    } else if(input == LEFT) {
      new_param -= step;
      if(new_param < 0 && (param_code == 1 || param_code == 2) ) new_param = 0;
    } else if(input == UP) {
      break;
    } else if(!first_render) {
      continue;
    }

    printFullscreen("Novo valor", String( (param_code==1)?((int) new_param):(new_param) ));

    first_render = false;
  }

  switch(param_code) {
    case 0: 
      slope.temp = new_param; 
      break;
    case 1: 
      slope.duration = new_param;
      break;
    case 2: 
      slope.tolerance = new_param;
      break;
  }

  return slope;
  
}

void setProc(int position) {
  int procs_num = brewer.getProcsNum(position);
  ControlProcess proc;
  float default_temp = 25.0;
  float default_tolerance = 1.0;
  
  if(procs_num <= 0) {
    proc.sensor_pin = NTC_PINS[1];
    proc.actuator_pin = HTR_PINS[1];
    proc.ref_value = default_temp;
    proc.tolerance = default_tolerance;
    brewer.addProc2Slope(position, proc.sensor_pin, proc.actuator_pin, proc.ref_value, proc.tolerance);
  } else {
    proc = brewer.getControlProcess(position, 1);
  }

  boolean first_render = true;
  int param_code = 0;
  boolean set_param = false;
  while(1) {
    int input = checkInput(true);
    if(input == RIGHT) {
      param_code = (param_code < 2)?param_code+1:2;
    } else if(input == LEFT) {
      param_code = (param_code > 0)?param_code-1:0;
    } else if(input == DOWN) {
      set_param = true;
    } else if(input == UP) {
      printFullscreen(F("Salvando"), F("1, 2, 3..."));
      delay(1000);
      printMemLeft();
      break;
    } else if(input == NO_KEY && !first_render) {
      continue;
    }

    switch(param_code) {
      case 0:
        printFullscreen(F("Temperatura"), String(proc.ref_value));
        break;
      case 1:
        printFullscreen(F("Tolerancia"), String(proc.tolerance));
        break;
      case 2:
        print(F("Remover processo"), 0, true);
        clearRow(1);
        break;
      default:
        printFullscreen(F("Erro"), F(""));
    }

    if(set_param) {
      if(param_code == 2) {
        brewer.rmvProc2Slope(position, proc.sensor_pin, proc.actuator_pin, proc.ref_value, proc.tolerance);
        break;
      } else {
        ControlProcess new_proc = setProcParam(position, param_code);
        boolean success = brewer.rmvProc2Slope(position, proc.sensor_pin, proc.actuator_pin, proc.ref_value, proc.tolerance);
        success = success&&brewer.addProc2Slope(position, new_proc.sensor_pin, new_proc.actuator_pin, new_proc.ref_value, new_proc.tolerance);
        if(success) {
          printFullscreen(F("Sucesso ao salvar"), F(""));
          proc = new_proc;
        } else {
          printFullscreen(F("Falha ao salvar"), F(""));
        }
      }
      printMemLeft();
      set_param = false;
      first_render = true;
      continue;
    }


    first_render = false;
    
  }
}

ControlProcess setProcParam(int position, int param_code) {
  ControlProcess proc = brewer.getControlProcess(position, 1);
  float new_param = 0;
  float step = 1;

  switch(param_code) {
    case 0: 
      new_param = proc.ref_value; 
      //step = 0.5;
      break;
    case 1: 
      new_param = proc.tolerance;
      //step = 0.1;
      break;
  }

  boolean first_render = true;
  while(1) {
    int input = checkInput();
    
    if(input == RIGHT) {
      new_param+=step;
      if(new_param > 249) new_param = 249;
    } else if(input == LEFT) {
      new_param -= step;
      if(new_param < 0 && (param_code == 1) ) new_param = 0;
    } else if(input == UP) {
      break;
    } else if(!first_render) {
      continue;
    }

    printFullscreen(F("Novo valor"), String(new_param));

    first_render = false;
  }

  switch(param_code) {
    case 0: 
      proc.ref_value = new_param; 
      break;
    case 1: 
      proc.tolerance = new_param;
      break;
  }

  return proc;
}

/////////////////////////// atualização de estados

void printState() {
  const String param_texts[6] = {
    "Rampa ",
    "Temp. mosto",
    "Tempo",
    "Res. mosto",
    "Temp. auxiliar",
    "Res. auxiliar"
  };
  
  if(print_time >= REFRESH_TIME) {
    if(param_time == 0) print(param_texts[param_index], 0, true);
    int time_left = (int) brewer.getTimeLeft();
        
    switch(param_index) {
      case 0:
        printFullscreen(param_texts[0] + String(brewer.getCurrentSlopeNumber()), F(""));
        break;
      case 1:
        print(String(brewer.getSensorReading(NTC_PINS[0])) + "/" + String( brewer.getSlopeTemp(brewer.getCurrentSlopeNumber()) ) + " C", 1, true);
        break;
      case 2:
		    if(time_left == 0) time_left = brewer.getCurrentSlopeDuration();
        print(String(time_left) + "/" + String(brewer.getCurrentSlopeDuration()) + " min", 1, true);
        break;
      case 3:
        print(brewer.isActuatorOn(HTR_PINS[0])?F("Ativado"):F("Desativado"), 1, true);
        break;
      case 4:
          print(String(brewer.getSensorReading(NTC_PINS[1])) + "/" + String(brewer.getControlProcess(brewer.getCurrentSlopeNumber(), 1).ref_value ) + " C", 1, true);
        break;
      case 5:
        print(brewer.isActuatorOn(HTR_PINS[1])?F("Ativado"):F("Desativado"), 1, true);
        break;
      default: print(F("ERRO"), 1, true);
    }
    
    print_time = 0;
    param_time++;
    
  } else {
    print_time++;
    delay(1);
  }

  if(param_time >= PARAM_REFRESH_TIME) {
    param_index = (param_index < 5)?param_index+1:0;
    param_time = 0;
  }
}

void printMemLeft() {
  printFullscreen(F("Memoria restante: "), String(brewer.getMemoryLeft()) + " bytes");
  delay(2000);
  lcd.clear();
}
