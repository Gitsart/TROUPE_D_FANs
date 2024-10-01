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

void setup() {
  pinMode(limitPin, INPUT_PULLUP);
  myStepper.attach( stepPin, dirPin );
  myStepper.setSpeed( 150 );  // 60 Rev/Min ( if stepsPerRev is set correctly )
  myStepper.setRampLen(5);
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

    if (stepperRunning) {
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
        stepperRunning = true;
      }
    }
  }
  // The sketch is not blocked while the stepper is moving nor while it is stopped.
  // Other nonblocking tasks can be added here
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
