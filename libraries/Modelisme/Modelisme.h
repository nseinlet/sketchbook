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

#include "Arduino.h"
#include <FUTABA_SBUS.h>

class ReceiverCanalHistory {
  public:
    int state;
    unsigned long timing;
    ReceiverCanalHistory();
};

class ReceiverCanal {
  public:
    unsigned long when;
    int pwmvalue;
    int angle;
    bool manageHistory;
    ReceiverCanalHistory ligthHistory[6];

    ReceiverCanal();
    int pwmToDeg();
    int canalToHighLow();
    void manageTheHistory();
    void resetHistory();
    unsigned long getMaxHistoryTime();
    int getMaxHistoryLength();
    int getHistoryState();

};

class Receiver {
  public:
    ReceiverCanal channels[16];

    void setup();
    int read();

  private:
    FUTABA_SBUS sBus;
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

    LightManager(ReceiverCanal*);
    LightManager(ReceiverCanal*, ReceiverCanal*);
    LightManager(ReceiverCanal*, ReceiverCanal*, ReceiverCanal*);
    void checkLights();

  private:
    ReceiverCanal* canal;
    ReceiverCanal* throttleCanal;
    ReceiverCanal* steeringCanal;
    unsigned long blinktime;

    void _blinking();
};

#endif
