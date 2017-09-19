/*
  Mpodelisme.cpp - Various usefull libs for PWM.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/

#include "Arduino.h"
#include "Modelisme.h"
#include <FUTABA_SBUS.h>

void Receiver::setup()
{
  sBus.begin();
}

int Receiver::read()
{
  sBus.FeedLine();
  if (sBus.toChannels == 1){
    sBus.UpdateServos();
    sBus.UpdateChannels();
    sBus.toChannels = 0;

    for (int x; x<16; x++) {
      channels[x].when = millis();
      channels[x].pwmvalue = sBus.channels[x];
      channels[x].angle = channels[x].pwmToDeg();
      if (channels[x].manageHistory) {
        channels[x].manageTheHistory();
      };
    };
    return 1;
  };
  return 0;
}

ReceiverCanal::ReceiverCanal()
{
  manageHistory=false;
}

int ReceiverCanal::pwmToDeg(){
  return map(pwmvalue, PWMMin, PWMMax, AngleMin, AngleMax);
};

int ReceiverCanal::canalToHighLow() {
  //Check if a canal is high, low or neutral, to help managing history of ligth canal
  if (angle>115){
    return 1;
  } else if (angle<75){
    return -1;
  } else {
    return 0;
  }
};

void ReceiverCanal::manageTheHistory() {
   //Manage history of the canal
   //Check sequences of type 3x High, 2x Low, ... to manage multiple functions on 1 canal
   int state = canalToHighLow();
   if (state!=ligthHistory[0].state){
     //Do not store the middle values
     if (ligthHistory[0].state!=0) {
       for (int i=6;i>0;i--){
         ligthHistory[i].state=ligthHistory[i-1].state;
         ligthHistory[i].timing=ligthHistory[i-1].timing;
       };
     };
     ligthHistory[0].state = state;
     ligthHistory[0].timing = millis();
   };
}

void ReceiverCanal::resetHistory(){
  for (int i=0;i<6;i++){
    ligthHistory[i].state=0;
    ligthHistory[i].timing=0;
  }
}

unsigned long ReceiverCanal::getMaxHistoryTime(){
  unsigned long res=ligthHistory[0].timing;
  for (int i=1;i<6;i++){
    if (ligthHistory[i].timing>0 and ligthHistory[i].timing<res){
      res=ligthHistory[i].timing;
    }
  };
  return res;
};

int ReceiverCanal::getMaxHistoryLength(){
  int res=0;
  bool toContinue=true;

  //If history[0]==0, means switch is neutral, need to check from pos 1
  int startpos=0;
  if (ligthHistory[0].state==0){startpos=1;};

  if (ligthHistory[startpos].state!=0){
    for (int i=startpos;i<6 && toContinue;i++){
      if (ligthHistory[i].state==ligthHistory[startpos].state && ligthHistory[i].timing>0){
        res++;
      }
    }
  }
  return res;
};

int ReceiverCanal::getHistoryState(){
  if (ligthHistory[0].state==0) {
    return ligthHistory[1].state;
  };
  return ligthHistory[0].state;
};

ReceiverCanalHistory::ReceiverCanalHistory()
{
  state=0;
  timing=0;
}

LightManager::LightManager(ReceiverCanal* rcan){
  canal = rcan;
  throttleCanal = NULL;
  steeringCanal = NULL;
  blinktime = 0;
  canal->manageHistory = true;
}

LightManager::LightManager(ReceiverCanal* rcan, ReceiverCanal* tcan){
  canal = rcan;
  throttleCanal = tcan;
  steeringCanal = NULL;
  blinktime = 0;
  canal->manageHistory = true;
}

LightManager::LightManager(ReceiverCanal* rcan, ReceiverCanal* tcan, ReceiverCanal* scan){
  canal = rcan;
  throttleCanal = tcan;
  steeringCanal = scan;
  blinktime = 0;
  canal->manageHistory = true;
}

void LightManager::checkLights() {
  if (throttleCanal){
    if (throttleCanal->angle < 80){
      rear = true;
    } else {
      rear = false;
    };
  };
  //Check lightHistoryCanal for a pattern
  //First, check if we reached the timing
  unsigned long actualTime = millis();
  //Max 3s, and at least one hit / 1.3sec
  if (canal->ligthHistory[0].timing>0 && (actualTime-canal->getMaxHistoryTime()>3000 || actualTime-canal->ligthHistory[0].timing>1300)) {
    int state=canal->getHistoryState();
    int pattern=canal->getMaxHistoryLength();
    if (state!=0 && pattern>0){
      //Check the pattern
      if (state==1 && pattern==1){
        rWarn=not rWarn;
        if (rWarn) lWarn=false;
      } else if (state==-1 && pattern==1){
        lWarn=not lWarn;
        if (lWarn) rWarn=false;
      } else if (state==1 && pattern==2){
        warnings=not warnings;
        if (warnings){
          rWarn=false;
          lWarn=false;
        }
      } else if (state==-1 && pattern==2){
        turningWarn=not turningWarn;
      } else if (state==1 && pattern==3){
        lights=not lights;
      } else if (state==-1 && pattern==3){
        highlights=not highlights;
      };
    };
    canal->resetHistory();
  }
  _blinking();
};

void LightManager::_blinking() {
  //For the warnings ;-)
  if (millis()-blinktime >= 750) {
    if (blinkstate) {
      blinkstate=false;
    } else {
      blinkstate=true;
    }
    blinktime = millis();
  }
};
