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
#define CHANNELS_SIZE 16

Receiver rec;
LightManager lm;
ReceiverDisplay screen("MT 1042");
long lastcheck;

void setup(void) {
  debugSetup();
  //Manage lights
  lm.setup(2, 3, 4, 5, 6, 7, 8);
  //Screen
  screen.setup(&rec, &lm);
  //Sbus Decoder
  rec.setup(CHANNELS_SIZE);
  //Servos
  //No off-by-one for the channel number. The first channel=1 !
  rec.servoChannels[0] = 7;
  rec.servoChannels[1] = 8;
  rec.servoChannels[2] = 9;
  rec.servoChannels[3] = 10;
  rec.servoChannels[4] = 11;
  rec.servoChannels[5] = 13;
  rec.servoChannels[6] = 14;
  rec.servoChannels[7] = 15;
  rec.servoChannels[8] = 16;
  rec.servoChannels[9] = 17;
  rec.servoChannels[10]= 18;
  lastcheck=0;
}

void loop()
{ 
  //delay(50);
  rec.read();

  if ((millis()-lastcheck) > 50){
    lastcheck=millis();
    //Manage ligths
    //On the receiver, there's an off-by-one for the channels. first channel=0 !
    lm.checkLights(rec.channels[11].angle, rec.channels[2].angle, rec.channels[4].angle, rec.channels[1].angle);
    //Manage screen
    screen.loop();

    debugLoop();
  };
}
