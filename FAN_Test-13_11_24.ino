#include <ESP32Servo.h>
#include<MobaTools.h>

static const int servoPin = 12;
Servo servo1;

#define stepPin 25
#define dirPin 26
#define limitPin 18

const int stepsPerRev = 1600;   // Steps per Revolution ( example with 1/4 microsteps )
const long  targetPos = 3000;         // stepper moves between 0 and targetpos
long nextPos;
MoToStepper myStepper ( stepsPerRev, STEPDIR );
MoToTimer stepperPause;                    // Pause between stepper moves
bool stepperRunning;
bool hfun;
int lim = 0;
unsigned long startTime = 0; // Timer variable
bool timerActive = false;     // Boolean to track timer status


int seqcount = 0;

// the loop2 function also runs forver but as a parallel task
void loop2 (void* pvParameters) {
  while (1) {

    for (int posDegrees = 60; posDegrees >= 0; posDegrees--) {
      servo1.write(posDegrees);
      Serial.println("SERVO_UPWARDS");
      delay(50);
    }
    for (int posDegrees = 0; posDegrees <= 60; posDegrees++) {
      servo1.write(posDegrees);
      Serial.println("SERVO_DOWNWARDS");
      delay(50);
    }
  }
}

void homeFun() {
  while (digitalRead(limitPin) == 1) {
    myStepper.setSpeed(50);
    myStepper.setRampLen(60);
    Serial.println("HOMING");
    myStepper.rotate(-1);

  }
  if (digitalRead(limitPin) != 1) {
    myStepper.stop();
    myStepper.setZero();
    Serial.println("HOME_SET");
    hfun = true;

    // Start the timer for 10 seconds
    startTime = millis();
    timerActive = true; // Activate the timer
  }
}



void setup() {
  pinMode(limitPin, INPUT_PULLUP);

  servo1.attach(servoPin);

  myStepper.attach( stepPin, dirPin );

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
  homeFun();
}

// the loop function runs over and over again forever
void loop() {
  if (hfun) {
    if (timerActive) {
      // Check if 10 seconds have passed
      if (millis() - startTime >= 5000) { // 10000 ms = 10 seconds
        timerActive = false;
        stepperRunning = false;// Stop the timer
      } else {
        return; // Exit loop until 10 seconds have passed                   (   THISLOOPGIVES5SecondDelayNONBLOCKING)
      }
    }
    while (seqcount <= 4) {
      if (!stepperRunning)
      {
        // Move forward
        myStepper.setSpeed(400);
        myStepper.move(6000);

        Serial.println("Moving forward");
        stepperRunning = true;

        while (myStepper.moving())
        {
          delay(10); // Wait until the forward movement is complete
        }

        // Move backward
        myStepper.setSpeed(400);
        myStepper.move(-6000);
        Serial.println("Moving backward");

        while (myStepper.moving())
        {
          delay(10); // Wait until the backward movement is complete
        }

        stepperRunning = false; // Reset the flag when the complete oscillation is done
      }
      seqcount++;
    }
    while ((seqcount>4)&&(seqcount <= 10)) {
      if (!stepperRunning)
      {
        // Move forward
        myStepper.setSpeed(250);
        myStepper.move(targetPos);

        Serial.println("Moving forward");
        stepperRunning = true;

        while (myStepper.moving())
        {
          delay(10); // Wait until the forward movement is complete
        }

        // Move backward
        myStepper.setSpeed(250);
        myStepper.move(-targetPos);
        Serial.println("Moving backward");

        while (myStepper.moving())
        {
          delay(10); // Wait until the backward movement is complete
        }

        stepperRunning = false; // Reset the flag when the complete oscillation is done
      }
      seqcount++;
    }
  }
  
}
