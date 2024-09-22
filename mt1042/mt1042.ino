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
#include <Servo.h>

/* Display */
#include "U8g2lib.h"

U8G2_SSD1306_64X32_1F_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

int angle=45;

Receiver rec;
LightManager lm;

Servo myservo[12];

void setup(void) {
  debugSetup();

  //Manage lights
  lm.setup(2, 3, 4, 5, 6, 7, 8);
  //Screen
  setupScreen();
  //Sbus Decoder
  rec.setup(19);
  myservo[0].attach(9);
  myservo[1].attach(10);
  myservo[2].attach(11);
  myservo[3].attach(12);
  myservo[4].attach(13);
  myservo[5].attach(A0);
  myservo[6].attach(A1);
  myservo[7].attach(A2);
  myservo[8].attach(A3);
  myservo[9].attach(A6);
  myservo[10].attach(A7);
}

void loop()
{ 
  delay(50);
  if (rec.read()==1){
    myservo[0].write(rec.channels[6].angle);
    myservo[1].write(rec.channels[7].angle);
    myservo[2].write(rec.channels[8].angle);
    myservo[3].write(rec.channels[9].angle);
    myservo[4].write(rec.channels[10].angle);
    myservo[5].write(rec.channels[12].angle);
    myservo[6].write(rec.channels[13].angle);
    myservo[7].write(rec.channels[14].angle);
    myservo[8].write(rec.channels[15].angle);
    myservo[9].write(rec.channels[16].angle);
    myservo[10].write(rec.channels[17].angle);
  };
  //Manage ligths
  lm.checkLights(rec.channels[11].angle, rec.channels[2].angle, rec.channels[4].angle, rec.channels[1].angle);
  //Manage screen
  drawScreen();

  debugLoop();
}
