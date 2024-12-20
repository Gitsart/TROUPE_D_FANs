#include <ESP32Servo.h>
#include<MobaTools.h>

static const int servoPin = 12;
Servo servo1;

#define stepPin 25
#define dirPin 26
const int stepsPerRev = 2000;   // Steps per Revolution ( example with 1/4 microsteps )
const long  targetPos = 800;         // stepper moves between 0 and targetpos
long nextPos;
MoToStepper myStepper ( stepsPerRev, STEPDIR );
MoToTimer stepperPause;                    // Pause between stepper moves
bool stepperRunning;

void setup() {


  servo1.attach(servoPin);

  myStepper.attach( stepPin, dirPin );
  myStepper.setSpeed( 100 );  // 60 Rev/Min ( if stepsPerRev is set correctly )
  myStepper.setRampLen(20 );
  stepperRunning = true;
  
  Serial.begin (115200);

  xTaskCreatePinnedToCore (
    loop2,     // Function to implement the task
    "loop2",   // Name of the task
    1000,      // Stack size in bytes
    NULL,      // Task input parameter
    1,         // Priority of the task
    NULL,      // Task handle.
    0          // Core where the task should run
  );
}

// the loop function runs over and over again forever
void loop() {
  if ( stepperRunning ) {
    // Wait till stepper has reached target, then set pause time
    if ( !myStepper.moving() ) {
      // stepper has reached target, start pause
      stepperPause.setTime( 1000 );
      stepperRunning = false;
    }
  } else {
    // stepper doesn't move, wait till stepperPause time expires
    if ( stepperPause.expired() ) {
      // stepperPause time expired. Start stepper in opposite direction
      if ( nextPos == 0 ) {
        nextPos = targetPos;
      } else {
        nextPos = 0;
      }
      myStepper.moveTo( nextPos );
      stepperRunning = true;
    }
  }
}

// the loop2 function also runs forver but as a parallel task
void loop2 (void* pvParameters) {
  while (1) {
    for (int posDegrees = 80; posDegrees >= 0; posDegrees--) {
      servo1.write(posDegrees);
      Serial.println(posDegrees);
      delay(20);
    }
    for (int posDegrees = 0; posDegrees <= 80; posDegrees++) {
      servo1.write(posDegrees);
      Serial.println(posDegrees);
      delay(20);
    }
  }
}
