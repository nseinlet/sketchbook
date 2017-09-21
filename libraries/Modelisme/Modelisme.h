/*Copyright (C) 2015  Seinlet Nicolas

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>*/

#ifndef Modelisme_h
#define Modelisme_h

#define PWMMin   170
#define PWMMax   1800
#define AngleMin 5
#define AngleMax 175

#define MAX_LM_HISTORY 5

#include "Arduino.h"
#include <FUTABA_SBUS.h>

class ReceiverCanal {
  public:
    unsigned long when;
    int pwmvalue;
    int angle;

    ReceiverCanal();
    int pwmToDeg();
};

class Receiver {
  public:
    ReceiverCanal channels[16];

    void setup();
    int read();

  private:
    FUTABA_SBUS sBus;
};

class LightManagerHistoryLine {
  public:
    int state;
    unsigned long timing;

    LightManagerHistoryLine();
};

class LightManagerHistory {
  public:
    LightManagerHistoryLine history[MAX_LM_HISTORY];

    void manageTheHistory(int);
    void resetHistory();
    unsigned long getMaxHistoryTime();
    int getMaxHistoryLength();
    int getHistoryState();

  private:
    int canalToHighLow(int);
};

class ChannelHistoryLine {
  public:
    int angle;
    unsigned long timing;
    ChannelHistoryLine();
};

class ChannelHistory {
  public:
    ChannelHistoryLine history[MAX_LM_HISTORY];

    void manageTheHistory(int);
    int getMinAngle();
    int getMaxAngle();
};

class LightManager {
  public:
    bool rWarn;
    bool lWarn;
    bool brake;
    bool rear;
    bool lights;
    bool highlights;
    bool warnings;
    bool turningWarn;
    bool blinkstate;
    ReceiverCanal* canal;
    ReceiverCanal* steeringCanal;
    ReceiverCanal* throttleCanal;
    unsigned long blinktime;
    LightManagerHistory lightHistory;
    ChannelHistory steerHistory;
    ChannelHistory throttleHistory;

    LightManager(ReceiverCanal*);
    LightManager(ReceiverCanal*, ReceiverCanal*);
    LightManager(ReceiverCanal*, ReceiverCanal*, ReceiverCanal*);
    void checkLights();

  private:
    void _setup();
    void _blinking();
};

#endif
