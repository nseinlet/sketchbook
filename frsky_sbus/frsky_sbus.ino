#include <FUTABA_SBUS.h>

FUTABA_SBUS sBus;
int CH1Pin = 3;
int CH2Pin = 5;
int CH3Pin = 6;
int CH4Pin = 9;
int CH5Pin = 10;
int CH6Pin = 11;

void setup(){
  sBus.begin();
  pinMode(CH1Pin, OUTPUT);
  pinMode(CH2Pin, OUTPUT);
  pinMode(CH3Pin, OUTPUT);
  pinMode(CH4Pin, OUTPUT);
  pinMode(CH5Pin, OUTPUT);
  pinMode(CH6Pin, OUTPUT);
}

void loop(){
  
  delay(300);
  sBus.FeedLine();
  if (sBus.toChannels == 1){
    sBus.UpdateServos();
    sBus.UpdateChannels();
    sBus.toChannels = 0;
    analogWrite(CH1Pin, sBus.channels[0] / 4);
    analogWrite(CH2Pin, sBus.channels[0] / 4);
    analogWrite(CH3Pin, sBus.channels[3] / 4);
    analogWrite(CH4Pin, sBus.channels[12] / 4);
  }
}
