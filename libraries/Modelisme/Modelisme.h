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
#include <FRSKY_SBUS.h>

#if defined(ACCESS_24)
  #define RECEIVER_CHANNELS 24
#else
  #define RECEIVER_CHANNELS 16
#endif

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
    ReceiverCanal channels[RECEIVER_CHANNELS];

    void setup();
    void setup(int);
    int read();

  private:
    FRSKY_SBUS sBus;
    int max_channels;
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
    ChannelHistoryLine history[12];

    ChannelHistory();
    void manageTheHistory(int);
    int getMinAngle();
    int getMaxAngle();
    bool isIncreasing();
    bool idDecreasing();
    bool isAllEqual();
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
    bool lightWarn;
    bool blinkstate;
    int rwPIN;
    int lwPIN;
    int bPIN;
    int rPIN;
    int lPIN;
    int hlPIN;
    int twPIN;
    unsigned long blinktime;
    unsigned long lightWarnTime;
    LightManagerHistory lightHistory;
    ChannelHistory steerHistory;
    ChannelHistory throttleHistory;
    ChannelHistory throttleHistory2;

    LightManager();
    void setup(int, int, int, int, int, int, int);
    void _checkLights(int, int, int);
    void checkLights(int, int, int);
    void checkLights(int, int, int, int);
    void powerLights();
    void _setup();
    void _blinking();
    void _breaking();
    void _breaking_dual();
};

#endif
