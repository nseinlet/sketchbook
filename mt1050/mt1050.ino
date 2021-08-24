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
 * Display them on a 1inch display             *
 ***********************************************/
 
#include <Modelisme.h>
#include <Servo.h>
/* Display */
#include "U8glib.h"

U8GLIB_SSD1306_64X48 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_NO_ACK|U8G_I2C_OPT_FAST);

uint32_t currentTime, displayTime;
int nbrdisp;
int angle=45;
int endpointx;
int endpointy;

Receiver rec;
LightManager lm;

Servo myservo[10];

#define Logo_challenger_small_width 63
#define Logo_challenger_small_height 16
static unsigned char Logo_challenger_small_bits[] U8G_PROGMEM = {
   0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xb0, 0x01, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x8c, 0x09, 0xc0, 0x06, 0x00, 0x00, 0x00,
   0x00, 0xc3, 0x0c, 0x40, 0x01, 0x00, 0x00, 0x00, 0xc0, 0x61, 0x06, 0xa0,
   0x02, 0x00, 0x00, 0x00, 0x60, 0x00, 0x03, 0x58, 0x01, 0x00, 0x00, 0x00,
   0x18, 0x80, 0xc5, 0xed, 0x02, 0x00, 0x00, 0x00, 0x0c, 0xc0, 0xa7, 0xb6,
   0xea, 0x0c, 0x3b, 0x00, 0x06, 0xc0, 0x57, 0xd2, 0xfe, 0x96, 0x3a, 0x00,
   0x07, 0x68, 0xfa, 0xcf, 0x7e, 0xd9, 0x1d, 0x00, 0xff, 0x27, 0x7f, 0xfb,
   0xef, 0xfb, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

void setup(void) {
  //Serial.begin(115200);
  displayTime = 0;
  nbrdisp = 0;
  
  rec.setup();
  lm.setup(2, 3, 4, 5, 6, 7, 8);
  //Sbus Decoder
  myservo[0].attach(9);
  myservo[1].attach(10);
  myservo[2].attach(11);
  myservo[3].attach(12);
  myservo[4].attach(13);
  myservo[5].attach(A0);
  myservo[6].attach(A1);
  myservo[7].attach(A2);
  myservo[8].attach(A3);

  //Optional sound
  //pinMode(1, OUTPUT);
  //myservo[9].attach(1);
  
}

void drawScreen1() {
  //Just a logo :-)
  String tmp;
  
  u8g.drawXBMP( 2, 19, Logo_challenger_small_width, Logo_challenger_small_height, Logo_challenger_small_bits);
  u8g.setFont(u8g_font_4x6);
  tmp = "MT 1050";
  u8g.drawStr( 18, 42, tmp.c_str());
};

void drawScreen2() {
  //Servo outputs 1->8
  String tmp;
  int i;

  for (i=0;i<8;i++) {
    u8g.drawLine(2+i*8,  2, 6+i*8, 2);
    u8g.drawLine(2+i*8, 38, 2+i*8, 2);
    u8g.drawLine(3+i*8, 38, 3+i*8, 38-(rec.channels[i].angle/5));
    u8g.drawLine(4+i*8, 38, 4+i*8, 38-(rec.channels[i].angle/5));
    u8g.drawLine(5+i*8, 38, 5+i*8, 38-(rec.channels[i].angle/5));
    u8g.drawLine(6+i*8, 38, 6+i*8, 2);
  };
    
  u8g.setFont(u8g_font_4x6);
  tmp = " 1 2 3 4 5 6 7 8";
  u8g.drawStr( 0, 47, tmp.c_str());
};

void drawScreen3() {
  //Servo outputs 9->16
  String tmp;
  int i;

  for (i=8;i<16;i++) {
    u8g.drawLine(2+(i-8)*8,  2, 6+(i-8)*8, 2);
    u8g.drawLine(2+(i-8)*8, 38, 2+(i-8)*8, 2);
    u8g.drawLine(3+(i-8)*8, 38, 3+(i-8)*8, 38-(rec.channels[i].angle/5));
    u8g.drawLine(4+(i-8)*8, 38, 4+(i-8)*8, 38-(rec.channels[i].angle/5));
    u8g.drawLine(5+(i-8)*8, 38, 5+(i-8)*8, 38-(rec.channels[i].angle/5));
    u8g.drawLine(6+(i-8)*8, 38, 6+(i-8)*8, 2);
  };
    
  u8g.setFont(u8g_font_4x6);
  tmp = "8 910111213141516";
  u8g.drawStr( 0, 47, tmp.c_str());
};

void drawScreen4() {
  //Power gauge
  u8g.drawCircle(32, 47, 30, U8G_DRAW_UPPER_RIGHT|U8G_DRAW_UPPER_LEFT);
  u8g.drawCircle(32, 47, 29, U8G_DRAW_UPPER_RIGHT|U8G_DRAW_UPPER_LEFT);
  u8g.drawCircle(32, 47, 28, U8G_DRAW_UPPER_RIGHT|U8G_DRAW_UPPER_LEFT);
      
  angle = abs(rec.channels[1].angle-90)*2;
  if (angle>175) {angle=175;};
  if (angle<30) {angle=30;};
      
  endpointx = 32 - (int) (28 * cos((angle * 71) / 4068.0));
  endpointy = 47 - (int) (28 * sin((angle * 71) / 4068.0));
  u8g.drawLine(31, 47, endpointx, endpointy);
  u8g.drawLine(32, 47, endpointx, endpointy);
  u8g.drawLine(33, 47, endpointx, endpointy);

  // Lights
  u8g.setFont(u8g_font_cursor);
  if ((lm.lWarn || lm.warnings) && lm.blinkstate) {
    u8g.drawStr( 1, 20, "\x8f");
  };
  if ((lm.rWarn || lm.warnings) && lm.blinkstate) {
    u8g.drawStr(63, 20, "\x91");
  };
  if (rec.channels[1].angle>85){
    u8g.drawStr(27, 23, "\x93");
  };
  if (rec.channels[1].angle<95){
    u8g.drawStr(37, 38, "\x8b");
  };
  if (lm.turningWarn) {
    u8g.drawStr(32, 38, "\xaf");  
  };
  if (lm.lights || lm.lightWarn) {
    u8g.drawStr(14, 40, "\x40");
  };
  if (lm.highlights || lm.lightWarn) {
    u8g.drawStr(50, 40, "\x41");
  };
};

void loop()
{ 
  delay(50);
  if (rec.read()==1){
    myservo[0].write(rec.channels[6].angle);
    myservo[1].write(rec.channels[7].angle);
    myservo[2].write(rec.channels[8].angle);
    myservo[3].write(rec.channels[9].angle);
    myservo[4].write(rec.channels[10].angle);
    myservo[5].write(rec.channels[11].angle);
    myservo[6].write(rec.channels[13].angle);
    myservo[7].write(rec.channels[14].angle);
    myservo[8].write(rec.channels[15].angle);
    //sound extra
//    if (rec.channels[0].angle>100 or rec.channels[0].angle<80 or rec.channels[3].angle>100 or rec.channels[3].angle<80){
//      //Any lift is working -> Hydraulic sound
//      myservo[9].write(45);
//    } else {
//      myservo[9].write(90);
//    };
    
  };

  //Manage ligths
  lm.checkLights(rec.channels[11].angle, rec.channels[2].angle, rec.channels[4].angle, rec.channels[1].angle);
  
  currentTime = millis();
  if(currentTime > displayTime)
  {
    displayTime = currentTime + 5000;
    nbrdisp = nbrdisp + 1;
    if (nbrdisp>6) {
      nbrdisp=0;
    }
    u8g.firstPage();  
    do {
      if (nbrdisp==0) {
        drawScreen1();
      } else if (nbrdisp==1){
        drawScreen2();
      } else if (nbrdisp==2){
        drawScreen3();
      } else {
        drawScreen4();
      };
    } while( u8g.nextPage() );
  };
}
