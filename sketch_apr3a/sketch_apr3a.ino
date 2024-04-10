// кнопки наше всё
// сделать UART те же 0 и 1 
// Использовать энеогосберегающую память запомненные позиции

#include "GyverStepper.h"
#include <EEPROM.h>
#include "EEManager.h"
//#include "GyverPlanner.h"

#define sensorMax1 30
#define sensorMax2 31
#define sensorMax3 32
#define sensorMax4 33
#define catcher 2
#define buttonRotationPlusA1 11
#define buttonRotationMinusA1 12
#define buttonRotationPlusA2 13
#define buttonRotationMinusA2 14
#define buttonRotationPlusA3 15
#define buttonRotationMinusA3 16
#define buttonRotationPlusA4 17
#define buttonRotationMinusA4 18
#define UART 19



int pins[4][3] = {
  { 0, 1, 6 },
  { 2, 3, 7 },
  { 3, 4, 9 },
  { 11, 5, 10 },
};  // {{dir,en,step}}
int positions[64][4];

int counterCoordinates = 0;




int readyDrivers = 0;

int parsingData[257];
EEManager memory(parsingData);

GStepper<STEPPER2WIRE> stepper1(8192, pins[0][0], pins[0][0], pins[0][1]);
GStepper<STEPPER2WIRE> stepper2(8192, pins[1][3], pins[1][0], pins[1][1]);
GStepper<STEPPER2WIRE> stepper3(8192, pins[2][3], pins[2][0], pins[2][1]);
GStepper<STEPPER2WIRE> stepper4(8192, pins[3][3], pins[3][0], pins[3][1]);

//GPlanner< STEPPER2WIRE, 4 > planner;

void setup() {
  Serial.begin(115200);
  stepper1.setMaxSpeed(400);
  stepper1.setAcceleration(300);
  stepper1.enable();
  stepper1.setRunMode(FOLLOW_POS);
  stepper2.setMaxSpeed(400);
  stepper2.setAcceleration(300);
  stepper2.enable();
  stepper2.setRunMode(FOLLOW_POS);
  stepper3.setMaxSpeed(400);
  stepper3.setAcceleration(300);
  stepper3.enable();
  stepper3.setRunMode(FOLLOW_POS);
  stepper4.setMaxSpeed(400);
  stepper4.setAcceleration(300);
  stepper4.enable();
  stepper4.setRunMode(FOLLOW_POS);
  
  /*planner.addStepper(0,stepper1);
  planner.addStepper(1,stepper2);
  planner.addStepper(2,stepper3);
  planner.addStepper(3,stepper4);
  planner.setAcceleration(100);
  planner.setMaxSpeed(300);
  planner.reset();
  */
}

void loop() {
   parse();
   if (command == 1){
    memory.updateNow(parsingData);
   }
   else if (command == 2){
    rotationAuto();
   }
}




// ручное
void rotation() {
  int buttonsActive = (digitalRead(buttonRotationPlusA1) == HIGH) ? 1 : 0
    + (digitalRead(buttonRotationMinusA1) == HIGH) ? 1 : 0
    + (digitalRead(buttonRotationPlusA2) == HIGH) ? 1 : 0
    + (digitalRead(buttonRotationMinusA2) == HIGH) ? 1 : 0 
    + (digitalRead(buttonRotationPlusA3) == HIGH) ? 1 : 0
    + (digitalRead(buttonRotationMinusA3) == HIGH) ? 1 : 0
    + (digitalRead(buttonRotationPlusA4) == HIGH) ? 1 : 0
    + (digitalRead(buttonRotationMinusA4) == HIGH) ? 1 : 0;
  if (buttonsActive > 1) {
    return;
  }
  if (digitalRead(buttonRotationPlusA1) == HIGH) {
    stepper1.reverse(false);
    stepper1.setTarget(1, RELATIVE);
  }
  if (digitalRead(buttonRotationMinusA1) == HIGH) {
    stepper1.reverse(true);
    stepper1.setTarget(1, RELATIVE);
  }
  if (digitalRead(buttonRotationPlusA2) == HIGH) {
    stepper2.reverse(false);
    stepper2.setTarget(1, RELATIVE);
  }
  if (digitalRead(buttonRotationMinusA2) == HIGH) {
    stepper2.reverse(true);
    stepper2.setTarget(1, RELATIVE);
  }
 if (digitalRead(buttonRotationPlusA3) == HIGH) {
    stepper3.reverse(false);
    stepper3.setTarget(1, RELATIVE);
  }
  if (digitalRead(buttonRotationMinusA3) == HIGH) {
    stepper3.reverse(true);
    stepper3.setTarget(1, RELATIVE);
  }
 if (digitalRead(buttonRotationPlusA4) == HIGH) {
    stepper4.reverse(false);
    stepper4.setTarget(1, RELATIVE);
  }
  if (digitalRead(buttonRotationMinusA4) == HIGH) {
    stepper4.reverse(true);
    stepper4.setTarget(1, RELATIVE);
  }
}

//работа через запарашенные коорды
int rotationIndex = 0;

void rotationAUTO(int coordinates[4]) {
  if (rotationIndex == 4){
    rotationIndex = 0;
  }
  if (rotationIndex == 0) {
      stepper1.setTarget(coordinates[0], ABSOLUTE);
      rotationIndex++;
  } else if (rotationIndex == 1 && !stepper1.tick() ) {
      stepper2.setTarget(coordinates[1], ABSOLUTE);
      rotationIndex++;
  } else if (rotationIndex == 2 && !stepper2.tick()) {
      stepper3.setTarget(coordinates[2], ABSOLUTE);
      rotationIndex++;
  } else if (rotationIndex == 3 && !stepper3.tick()) {
      stepper4.setTarget(coordinates[3], ABSOLUTE);
      rotationIndex++;
  }
}


// заданная программа похвастаться
void Program(){

}


void moveToHome() {
  while (true) {
    if (digitalRead(sensorMax1) == 0) {
        stepper1.reverse(true);
        stepper1.setTarget(1, RELATIVE);
    }
    else if (digitalRead(sensorMax2) == 0) {
        stepper2.reverse(true);
        stepper2.setTarget(1, RELATIVE);
    }
    else if (digitalRead(sensorMax3) == 0) {
        stepper3.reverse(true);
        stepper3.setTarget(1, RELATIVE);
    }
    else if (digitalRead(sensorMax1) == 1 && digitalRead(sensorMax2) == 1 && digitalRead(sensorMax3) == 1 ) {
      stepper1.setCurrent(0);
      stepper2.setCurrent(0);
      stepper3.setCurrent(0);
      break;
    }
  }
}



    // число значений в массиве, который хотим получить
     // массив численных значений после парсинга
bool recievedFlag = false;
bool getStarted = false;
int index = 0;
String string_convert = "";
int command = 0; 


void parse() {
  if (Serial.available() > 0) {
    char incomingByte = Serial.read();        // обязательно ЧИТАЕМ входящий символ
    if (getStarted) {                         // если приняли начальный символ (парсинг разрешён)
      if (incomingByte != ' ' && incomingByte != ';') {   // если это не пробел И не конец
        string_convert += incomingByte;       // складываем в строку
      } else {
        if (index == 0){
          command = string_convert.toInt();
        } 
        else{                     
          parsingData[index-1] = string_convert.toInt();  // преобразуем строку в int и кладём в массив
        }
        string_convert = "";                  // очищаем строку
        index++;                              // переходим к парсингу следующего элемента массива
      }
    }
    if (incomingByte == '$') {                // если это $
      getStarted = true;                      // поднимаем флаг, что можно парсить
      index = 0;                              // сбрасываем индекс
      string_convert = "";                    // очищаем строку
    }
    if (incomingByte == ';') {                // если таки приняли ; - конец парсинга
      getStarted = false;                     // сброс
      recievedFlag = true;                    // флаг на принятие
    }
  }
}

int counter=0;
int counterDriver=0;
void positions(){

  positions[counter][counterDriver]
  counterDriver++;
  if (counterDriver==4){
  counter++
  counterDriver=0;
  }
}

