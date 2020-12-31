#include <TimerOne.h>

#define PILOT_PIN 10
#define SIGNAL_PIN A0
#define SERIAL_BAUD 115200

#define LIVE_ENABLE_PIN 2

#define STATE_A 0 //ready state                       +12 V CP-PE
#define STATE_B 1 //connected but not charging        +9±1 V CP-PE
#define STATE_C 2 //charging                          +6±1 V
#define STATE_D 3 //charging ventilation required     +3±1 V
#define STATE_E 4 //No Power                          0 V
#define STATE_F 5 //EVSE Error                        −12 V

#define OUTPUT_12V 1023
#define OUTPUT_MINUS_12V 0

#define MAX_AMPS 6
#define DUTY 100
#define PILOT_READ_COUNT 10

//ADC values before this are taken to be the negative reading of the pilot signal
#define PILOT_LOW_TRESHOLD 250

//Tweak as needed
#define CUT_POINTS {890, 790, 690, 590}

//current state
int state;

void setup() {
  Serial.begin(SERIAL_BAUD);
  state = 0;
  pinMode(SIGNAL_PIN, INPUT);
  pinMode(PILOT_PIN, OUTPUT);
  pinMode(LIVE_ENABLE_PIN, OUTPUT);

  Timer1.initialize(1000);  // Frequency, 1000us = 1khz
  stateA();
}

void changeToState(int s) {
  state = s;
  if (state == STATE_A) {
    stateA();
  } else if (state == STATE_B) {
    stateB();
  } else if (state == STATE_C) {
    stateC();
  } else if (state == STATE_D) {
    stateD();
  } else if (state == STATE_E) {
    stateE();
  } else if (state == STATE_F) {
    stateF();
  }
}

int getRequiredState() {

 int reading = readPilot();
 int cutpoints[] = CUT_POINTS;
 for (int i=0;i < 4; i++) {
    int cutoff = cutpoints[i];
    if (reading > cutoff) {
      if (state != i) {
         changeToState(i);
      }
      break;
    }
 }

}

int readPilot() {
 int reading; 
 for (int i=0;i < PILOT_READ_COUNT; i++) {
    reading = analogRead(SIGNAL_PIN);  // read the input pin
    
    if (reading > PILOT_LOW_TRESHOLD) {
      break;
    }
  }
  return reading;
}

void stateA() {
  //Standby CP should be +12V
  Serial.println("Changing to State A");
  Timer1.pwm(PILOT_PIN, OUTPUT_12V);
  digitalWrite(LIVE_ENABLE_PIN, LOW);
}

void stateB() {
  //CP should be square wave with duty cycle
  Serial.println("Changing to State B");
  Timer1.pwm(PILOT_PIN, DUTY);
  digitalWrite(LIVE_ENABLE_PIN, LOW);
}

void stateC() {
  //CP should be square wave with duty cycle
  Serial.println("Changing to State C");
  Timer1.pwm(PILOT_PIN, DUTY);
  digitalWrite(LIVE_ENABLE_PIN, HIGH);

}

void stateD() {
  //CP should be square wave with duty cycle
  Serial.println("Changing to State D");
  Timer1.pwm(PILOT_PIN, DUTY);
  digitalWrite(LIVE_ENABLE_PIN, HIGH);

}

void stateE() {

}
void stateF() {
  Serial.println("Changing to State F");
  Timer1.pwm(PILOT_PIN, OUTPUT_MINUS_12V);
  digitalWrite(LIVE_ENABLE_PIN, LOW);

}

void loop() {
  getRequiredState();
  delay(1000);
}
