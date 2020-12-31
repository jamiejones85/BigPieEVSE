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
#define PILOT_READ_COUNT 10

#define DIODE_CHECK true

//ADC values before this are taken to be the negative reading of the pilot signal
// -12 to 12 1023 / 24 ~42 points per volt on ADC 
#define PILOT_LOW_TRESHOLD 500

// -12v to -8v threshold 8 * 42.6 + a bit
#define DIODE_PRESENT_TRESHOLD 360

//Tweak as needed   
#define CUT_POINTS {890, 790, 690, 590}

//current state
int state;
bool diodePresent = false;

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

  //diode check fails, drop to F
  if (s > STATE_A && diodePresent == false) {
    s = STATE_F;
  }
  if (s == STATE_A) {
    stateA();
  } else if (s == STATE_B) {
    stateB();
  } else if (s == STATE_C) {
    stateC();
  } else if (s == STATE_D) {
    stateD();
  } else if (s == STATE_E) {
    stateE();
  } else if (s == STATE_F && state != STATE_F) {
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
 int pilotHigh; 

 if (DIODE_CHECK) {
  diodePresent = false;
 } else {
  diodePresent = true;
 }
 
 for (int i=0;i < PILOT_READ_COUNT; i++) {
    int reading = analogRead(SIGNAL_PIN);  // read the input pin
    if (reading > PILOT_LOW_TRESHOLD) {
      pilotHigh = reading;
    } else {
      //negaive must remain below -8v for a diode present
      if (reading <= DIODE_PRESENT_TRESHOLD) {
        diodePresent = true;
      }
    }
  }
  return pilotHigh;
}

void stateA() {
  state = STATE_A;
  //Standby CP should be +12V
  Serial.println("Changing to State A");
  Timer1.pwm(PILOT_PIN, OUTPUT_12V);
  digitalWrite(LIVE_ENABLE_PIN, LOW);
}

void stateB() {
  state = STATE_B;
  //CP should be square wave with duty cycle
  Serial.println("Changing to State B");
  Timer1.pwm(PILOT_PIN, getDuty(MAX_AMPS));
  digitalWrite(LIVE_ENABLE_PIN, LOW);
}

void stateC() {
  state = STATE_C;
  //CP should be square wave with duty cycle
  Serial.println("Changing to State C");
  Timer1.pwm(PILOT_PIN, getDuty(MAX_AMPS));
  digitalWrite(LIVE_ENABLE_PIN, HIGH);

}

void stateD() {
  state = STATE_D;
  //CP should be square wave with duty cycle
  Serial.println("Changing to State D");
  Timer1.pwm(PILOT_PIN, getDuty(MAX_AMPS));
  digitalWrite(LIVE_ENABLE_PIN, HIGH);

}

void stateE() {
    state = STATE_E;

}

void stateF() {
  state = STATE_F;
  Serial.println("Changing to State F");
  Timer1.pwm(PILOT_PIN, OUTPUT_MINUS_12V);
  digitalWrite(LIVE_ENABLE_PIN, LOW);

}

int getDuty(int amps) {

  int duty = 0;
   if ((amps >= 6) && (amps <= 51)) {
      // amps = (duty cycle %) X 0.6
      duty = amps * (OUTPUT_12V/60);
   } else if ((amps > 51) && (amps <= 80)) {
      // amps = (duty cycle % - 64) X 2.5
      duty = (amps * (OUTPUT_12V/250)) + (64*(OUTPUT_12V/100));
   }

   return duty;
}

void loop() {
  getRequiredState();
  delay(500);
}
