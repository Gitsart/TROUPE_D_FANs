#include<MobaTools.h>
#define stepPin 25
#define dirPin 26
#define limitPin 34
const int stepsPerRev = 1000;
const long  targetPos = -750;
long nextPos;
MoToStepper myStepper ( stepsPerRev, STEPDIR );
MoToTimer stepperPause;                    // Pause between stepper moves
bool stepperRunning;
bool hfun;
int lim = 0;
unsigned long startTime = 0; // Timer variable
bool timerActive = false;     // Boolean to track timer status
int count = 0;
void setup() {
  pinMode(limitPin, INPUT_PULLUP);
  myStepper.attach( stepPin, dirPin );
  Serial.begin (115200);
  stepperRunning = true;
  homeFun();
}

void loop() {
  if (hfun) {
    if (timerActive) {
      // Check if 10 seconds have passed
      if (millis() - startTime >= 10000) { // 10000 ms = 10 seconds
        timerActive = false; // Stop the timer
      } else {
        return; // Exit loop until 10 seconds have passed                   (   THISLOOPGIVES10SecondDelayNONBLOCKING)
      }
    }
  }
  if ((count >= 0) && (count <= 2)) {
    motorBasic();
  }
  if ( (3 <= count) && (count < 7) ) {
    motor2();
  }
  if (count >= 7 && count <= 10) {
    motorLast();
  }
  if (count > 10) {
    motorBasic();
  }
  // The sketch is not blocked while the stepper is moving nor while it is stopped.
  // Other nonblocking tasks can be added here
  Serial.print("COUNT:");
  Serial.println(count);
}

void homeFun() {
  while (digitalRead(limitPin) == 0) {
    myStepper.rotate(1);
  }
  if (digitalRead(limitPin) != 0) {
    myStepper.stop();
    myStepper.setZero();
    hfun = true;

    // Start the timer for 10 seconds
    startTime = millis();
    timerActive = true; // Activate the timer
  }
}
void motorBasic() {
  if (stepperRunning) {
    myStepper.setRampLen(10);
    myStepper.setSpeed(200);
    // Wait till stepper has reached target, then set pause time
    if (!myStepper.moving()) {
      // Stepper has reached target, start pause
      stepperPause.setTime(1000);
      stepperRunning = false;
    }
  } else {
    // Stepper doesn't move, wait till stepperPause time expires
    if (stepperPause.expired()) {
      // StepperPause time expired. Start stepper in opposite direction
      if (nextPos == 0) {
        nextPos = targetPos;
      } else {
        nextPos = 0;
      }
      myStepper.moveTo(nextPos);
      count = count + 1;
      stepperRunning = true;
    }
  }
  if (count > 3) {
    myStepper.setSpeed(80);
    myStepper.setRampLen(3);
    myStepper.moveTo(2100);
    myStepper.moveTo(0);
    count = count + 1;
  }
  if(count>7) {
    myStepper.setSpeed(140);
  myStepper.setRampLen(5);
  myStepper.moveTo(500);
  myStepper.moveTo(-500);
  myStepper.moveTo(1400);
  myStepper.moveTo(-1000);
  myStepper.moveTo(0);
  count = count + 1;
  }
}
