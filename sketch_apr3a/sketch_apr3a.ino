// кнопки наше всё
// сделать UART те же 0 и 1 
#include "GyverStepper.h"
//#include "GyverPlanner.h"
#define sensorMax0 30
#define sensorMax1 31
#define sensorMax2 32
#define sensorMax3 33
#define catcher 2
#define buttonRotationPlusA0 11
#define buttonRotationMinusA0 12
#define buttonRotationPlusA1 13
#define buttonRotationMinusA1 14
#define buttonRotationPlusA2 15
#define buttonRotationMinusA2 16
#define buttonRotationPlusA3 17
#define buttonRotationMinusA3 18
#define UART 19

int pins[4][3] = {
  { 0, 1, 6 },
  { 2, 3, 7 },
  { 3, 4, 9 },
  { 4, 5, 10 },
};  // {{dir,en,step}}
long positionsDrivers[4] = { 0, 0, 0, 0 };
int coordinatesProgram[5][4]{
  { 0, 0, 0, 0 },
  { 3453, 2353, 5345, -5424 },
  { 4654, -4658, 7825, 9972 },
  { 1024, 2048, -3060, 1 },
  { -2342, 2, 4234, 8768 },
};

int coordinates[4]={0,0,0,0};

int counterCoordinates = 0;

int readyDrivers = 0;

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
  
}




// ручное
void rotation() {
  if (digitalRead(buttonRotationPlusA0) == HIGH) {
    stepper1.reverse(false);
    stepper1.setTarget(1, RELATIVE);
  }
  if (digitalRead(buttonRotationMinusA0) == HIGH) {
    stepper1.reverse(true);
    stepper1.setTarget(1, RELATIVE);
  }
  if (digitalRead(buttonRotationPlusA1) == HIGH) {
    stepper2.reverse(false);
    stepper2.setTarget(1, RELATIVE);
  }
  if (digitalRead(buttonRotationMinusA1) == HIGH) {
    stepper2.reverse(true);
    stepper2.setTarget(1, RELATIVE);
  }
 if (digitalRead(buttonRotationPlusA2) == HIGH) {
    stepper3.reverse(false);
    stepper3.setTarget(1, RELATIVE);
  }
  if (digitalRead(buttonRotationMinusA2) == HIGH) {
    stepper3.reverse(true);
    stepper3.setTarget(1, RELATIVE);
  }
 if (digitalRead(buttonRotationPlusA3) == HIGH) {
    stepper4.reverse(false);
    stepper4.setTarget(1, RELATIVE);
  }
  if (digitalRead(buttonRotationMinusA3) == HIGH) {
    stepper4.reverse(true);
    stepper4.setTarget(1, RELATIVE);
  }
  positionsDrivers[0]=stepper1.getTarget();
  positionsDrivers[1]=stepper1.getTarget();
  positionsDrivers[2]=stepper1.getTarget();
  positionsDrivers[3]=stepper1.getTarget();
}

//работа через запарашенные коорды
void rotationAUTO() {
  if (!stepper1.tick()) {
    stepper1.setTarget(coordinates[0], ABSOLUTE);
    readyDrivers += 1;
    positionsDrivers[0] = stepper1.getCurrent();
  } else if (!stepper2.tick()) {
    stepper2.setTarget(coordinates[1], ABSOLUTE);
    readyDrivers += 1;
    positionsDrivers[1] = stepper2.getCurrent();
  } else if (!stepper3.tick()) {
    stepper3.setTarget(coordinates[2], ABSOLUTE);
    readyDrivers += 1;
    positionsDrivers[2] = stepper3.getCurrent();
  } else if (!stepper4.tick()) {
    stepper4.setTarget(coordinates[3], ABSOLUTE);
    readyDrivers += 1;
    positionsDrivers[3] = stepper4.getCurrent();
  }
  if (readyDrivers == 4) {
    counterCoordinates += 1;
    readyDrivers = 0;
  }
}


// заданная программа похвастаться
void Program(){

}


void moveToHome() {
  while (digitalRead(sensorMax1) == LOW) {

  }
}

int counterDrivers = 0;
int counterBit = 0;
int timer=0;
bool parsing = false;

void parseUART(){
  if (digitalRead(UART)==HIGH && parsing == false){
    timer = millis();
    parsing = true;
    counterDrivers = 0;
  }
  else if (millis()-timer > 1000) {
    if (digitalRead(UART) == HIGH){
      coordinates[counterDrivers] += 2^counterBit;
    }
    counterBit +=1;
    timer = millis();
    if (counterBit == 8){
      counterDrivers+=1;
    }
    if (counterDrivers == 4){
      parsing = false;
      counterDrivers = 0;      
    }
  }
}