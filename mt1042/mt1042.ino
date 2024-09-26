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

/***********************************************
 * FrSky Sbus decoder, light management        *
 * and screen display                          *
 *                                             *
 * Read channels values on SBus port (rx port) *
 * Manage ligths based on 1 channel            *
 * Patterns are 1,2 or 3 times up or down      *
 * Display them on a 0.49inch display          *
 * Drive screen with channel 18                *
 ***********************************************/
 
#include <Modelisme.h>
#include <ReceiverDisplay.h>

Receiver rec;
LightManager lm;
ReceiverDisplay screen("MT 1042", 24);


void setup(void) {
  debugSetup();
  //Manage lights
  lm.setup(2, 3, 4, 5, 6, 7, 8);
  //Screen
  screen.setup(&rec, &lm);
  //Sbus Decoder
  rec.setup(19);
  //Servos
  rec.servoPins[0] = 9; rec.servoChannels[0] = 6;
  rec.servoPins[1] = 10;rec.servoChannels[1] = 7;
  rec.servoPins[2] = 11;rec.servoChannels[2] = 8;
  rec.servoPins[3] = 12;rec.servoChannels[3] = 9;
  rec.servoPins[4] = 13;rec.servoChannels[4] = 10;
  rec.servoPins[5] = A0;rec.servoChannels[5] = 12;
  rec.servoPins[6] = A1;rec.servoChannels[6] = 13;
  rec.servoPins[7] = A2;rec.servoChannels[7] = 14;
  rec.servoPins[8] = A3;rec.servoChannels[8] = 15;
  rec.servoPins[9] = A6;rec.servoChannels[9] = 16;
  rec.servoPins[10]= A7;rec.servoChannels[10]= 17;
}

void loop()
{ 
  delay(50);
  rec.read();
  //Manage ligths
  lm.checkLights(rec.channels[11].angle, rec.channels[2].angle, rec.channels[4].angle, rec.channels[1].angle);
  //Manage screen
  screen.loop();

  debugLoop();
}
