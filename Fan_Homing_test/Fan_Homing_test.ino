#include<MobaTools.h>
#define stepPin 25
#define dirPin 26
#define limitPin 34

const int stepsPerRev = 1000;
const long  targetPos = 600;
long nextPos;
MoToStepper myStepper ( stepsPerRev, STEPDIR );
MoToTimer stepperPause;                    // Pause between stepper moves
bool stepperRunning;
bool movingForward;
bool clothDetected;
int count = 0;
int limit_count = 0;

bool limswitch = 0;

int hOme = 0;
int pos = 0;
int sTart = 0;

void setup() {
  pinMode(limitPin, INPUT_PULLUP);
  myStepper.attach( stepPin, dirPin );
  Serial.begin (115200);
  stepperRunning = true;
  movingForward = true;
  clothDetected = false;


  pinMode(limitPin, INPUT_PULLUP);
  homing();
}

void homing() {
  myStepper.setSpeed(1500);
  while (digitalRead(limitPin) == HIGH && limit_count == 0) {
    myStepper.rotate(1);
    Serial.print(limswitch);
    Serial.println("Finding END");
  }

  if (digitalRead(limitPin) != HIGH && limit_count == 0) {
    myStepper.stop();
    Serial.println("END POSITION");
    limit_count += 1;
    limswitch = HIGH;
    hOme = 1;
  }
  myStepper.setZero();
  delay(2000);
  myStepper.setSpeed(200); myStepper.moveTo(-400);
  limswitch = LOW;
  delay(2000);
  while (digitalRead(limitPin) == HIGH && limit_count == 1) {
    myStepper.rotate(1);
    Serial.print(limswitch);
    Serial.println("Finding END2");
  }
  if (digitalRead(limitPin) != HIGH && limit_count == 1) {
    myStepper.stop();
    Serial.println("END POSITION2");
    limit_count += 1;
    limswitch = HIGH;
    hOme = 2;
  }
  Serial.print("limit_count:"); Serial.println(limit_count);
  Serial.print("limswitch:"); Serial.println(limswitch);
  Serial.print("hOme:"); Serial.println(hOme);
  Serial.print("POS:"); Serial.println(pos);
  myStepper.setZero();
  delay(2000);
}

void loop() {}
