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

#include "Arduino.h"
#include <PinChangeInt.h>
#include <Servo.h>

struct sig {
  volatile boolean isOn;
  volatile unsigned long start;
  volatile int len;
};
typedef struct sig Sig;

struct serv {
  volatile int angleactu;
  volatile int anglerequis;
  volatile int pinIn;
  volatile boolean invert;
  volatile unsigned long tempo;
  volatile unsigned long lastMove;
  Servo outservo;
};
typedef struct serv Serv;

class Modelisme
{
  public:
      Serv sorties[13] = {0};
  private:
      Sig signals[13] = {0};
};

#endif
