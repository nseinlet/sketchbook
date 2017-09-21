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
    };
    return 1;
  };
  return 0;
}

ReceiverCanal::ReceiverCanal()
{
  when=0;
  pwmvalue=0;
  angle=0;
}

int ReceiverCanal::pwmToDeg(){
  return map(pwmvalue, PWMMin, PWMMax, AngleMin, AngleMax);
};

LightManager::LightManager(ReceiverCanal* rcan){
  canal = rcan;
  throttleCanal = NULL;
  steeringCanal = NULL;
  _setup();
};

LightManager::LightManager(ReceiverCanal* rcan, ReceiverCanal* tcan){
  canal = rcan;
  throttleCanal = tcan;
  steeringCanal = NULL;
  _setup();
};

LightManager::LightManager(ReceiverCanal* rcan, ReceiverCanal* tcan, ReceiverCanal* scan){
  canal = rcan;
  throttleCanal = tcan;
  steeringCanal = scan;
  _setup();
};

void LightManager::_setup(){
  blinktime = 0;
  rWarn = false;
  lWarn = false;
  brake = false;
  rear = false;
  lights = false;
  highlights = false;
  warnings = false;
  turningWarn = false;
  blinkstate = false;
};

void LightManager::checkLights() {
  lightHistory.manageTheHistory(canal->angle);
  if (throttleCanal) {throttleHistory.manageTheHistory(throttleCanal->angle);};
  if (steeringCanal) {steerHistory.manageTheHistory(steeringCanal->angle);};

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
  if (lightHistory.history[0].timing>0 && (actualTime-lightHistory.getMaxHistoryTime()>3000 || actualTime-lightHistory.history[0].timing>1300)) {
    int state=lightHistory.getHistoryState();
    int pattern=lightHistory.getMaxHistoryLength();
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
    lightHistory.resetHistory();
  };
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
  };
};

int LightManagerHistory::canalToHighLow(int angle) {
  //Check if a canal is high, low or neutral, to help managing history of ligth canal
  if (angle>115){
    return 1;
  } else if (angle<75){
    return -1;
  } else {
    return 0;
  };
};


void LightManagerHistory::manageTheHistory(int angle) {
   //Manage history of the canal
   //Check sequences of type 3x High, 2x Low, ... to manage multiple functions on 1 canal
   int state = canalToHighLow(angle);
   if (state!=history[0].state){
      //Do not store the middle values
      if (history[0].state!=0) {
       for (int i=MAX_LM_HISTORY;i>0;i--){
         history[i].state=history[i-1].state;
         history[i].timing=history[i-1].timing;
       };
      };
     history[0].state = state;
     history[0].timing = millis();
   };
};

void LightManagerHistory::resetHistory(){
  for (int i=0;i<MAX_LM_HISTORY;i++){
    history[i].state=0;
    history[i].timing=0;
  }
};

unsigned long LightManagerHistory::getMaxHistoryTime(){
  unsigned long res=history[0].timing;
  for (int i=1;i<MAX_LM_HISTORY;i++){
    if (history[i].timing>0 and history[i].timing<res){
      res=history[i].timing;
    }
  };
  return res;
};

int LightManagerHistory::getMaxHistoryLength(){
  int res=0;
  bool toContinue=true;

  //If history[0]==0, means switch is neutral, need to check from pos 1
  int startpos=0;
  if (history[0].state==0){startpos=1;};

  if (history[startpos].state!=0){
    for (int i=startpos;i<MAX_LM_HISTORY && toContinue;i++){
      if (history[i].state==history[startpos].state && history[i].timing>0){
        res++;
      }
    }
  }
  return res;
};

int LightManagerHistory::getHistoryState(){
  if (history[0].state==0) {
    return history[1].state;
  };
  return history[0].state;
};

LightManagerHistoryLine::LightManagerHistoryLine()
{
  state=0;
  timing=0;
}

void ChannelHistory::manageTheHistory(int angle) {
   //Manage history of the canal
   for (int i=MAX_LM_HISTORY;i>0;i--){
     history[i].angle=history[i-1].angle;
     history[i].timing=history[i-1].timing;
   };
   history[0].angle = angle;
   history[0].timing = millis();
}

ChannelHistoryLine::ChannelHistoryLine()
{
  angle=0;
  timing=0;
}
