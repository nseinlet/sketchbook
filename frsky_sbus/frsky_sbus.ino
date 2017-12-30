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
#include "U8glib.h"

//Oled 0.96 SCL=A5, SDA=A4, VCC=3.3V
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_NO_ACK|U8G_I2C_OPT_FAST);
//U8GLIB_SSD1306_128X32 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_NO_ACK|U8G_I2C_OPT_FAST);

//Challenger Logos
#define Logo_challenger_width 128
#define Logo_challenger_height 40
static unsigned char Logo_challenger_bits[] U8G_PROGMEM = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0xc0, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x1c, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c,
   0x1c, 0x0e, 0x00, 0xc0, 0x61, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x0f, 0x0e, 0x07, 0x00, 0xe0, 0x79, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x03, 0x8e, 0x07, 0x00, 0x98,
   0x2c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x00,
   0xc7, 0x03, 0x00, 0x48, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x78, 0xc0, 0xc3, 0x01, 0x00, 0x24, 0x19, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0xc0, 0xe1, 0x01, 0x00, 0x92,
   0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x0f, 0xc0,
   0xf0, 0x00, 0x00, 0x49, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0xc0, 0x03, 0x00, 0x78, 0x00, 0x80, 0x64, 0x03, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x01, 0x00, 0xb8, 0x01, 0xc3, 0xb3,
   0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00,
   0xdc, 0xc1, 0xc7, 0xf9, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x3c, 0x00, 0x00, 0xee, 0xe1, 0xe7, 0x38, 0x0f, 0x00, 0x00, 0x80,
   0x03, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0xff, 0x30, 0x73, 0x9c,
   0xcd, 0xf9, 0x70, 0xc0, 0xe6, 0x03, 0x00, 0x00, 0x80, 0x07, 0x00, 0x00,
   0xef, 0x98, 0x39, 0xce, 0xec, 0x7f, 0x3c, 0x61, 0xf3, 0x03, 0x00, 0x00,
   0xc0, 0x07, 0x00, 0x80, 0x67, 0xcc, 0x1c, 0xc7, 0xf7, 0x37, 0x87, 0xb3,
   0xfb, 0x01, 0x00, 0x00, 0xe0, 0x03, 0x00, 0xc6, 0x31, 0xee, 0xde, 0xe3,
   0xb9, 0xbb, 0xe3, 0xf9, 0xec, 0x00, 0x00, 0x00, 0xe0, 0x8f, 0xff, 0xe7,
   0xf9, 0xff, 0xff, 0xf3, 0xdd, 0xd9, 0xfd, 0x7c, 0x77, 0x00, 0x00, 0x00,
   0xe0, 0xff, 0xff, 0xe3, 0xf8, 0xff, 0xbd, 0xff, 0xef, 0xfc, 0xef, 0xff,
   0xfb, 0x7f, 0x00, 0x00, 0x80, 0xff, 0x3f, 0x70, 0x78, 0xef, 0x9c, 0xef,
   0x03, 0xbc, 0xf3, 0xff, 0xf8, 0xff, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x01, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00 };
#define Logo_challenger_small_width 118
#define Logo_challenger_small_height 30
static unsigned char Logo_challenger_small_bits[] U8G_PROGMEM = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xdc, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0xce, 0x08, 0x00, 0x46, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0xc0, 0xc1, 0x0c, 0x80, 0x54, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x60, 0x0e, 0x80, 0x2a, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x30, 0x03,
   0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x1c, 0x10, 0x03, 0xa0, 0x1a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x07, 0x80, 0x05, 0x31, 0x07, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x03, 0xc0, 0xc6, 0xb3, 0x17, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x00, 0xe0, 0x23,
   0xcd, 0x94, 0x1d, 0x83, 0x7a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70,
   0x00, 0xe0, 0xb3, 0xcc, 0xd6, 0x9f, 0x41, 0x79, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x38, 0x00, 0x33, 0xd9, 0x37, 0xe7, 0x6d, 0xe6, 0x1e, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xfc, 0xbb, 0xff, 0x3f, 0xb7, 0xb5,
   0x77, 0x1b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x3f, 0x8c, 0xdf,
   0xf6, 0x0f, 0x6e, 0xff, 0xfc, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
#define rwarn_width 26
#define rwarn_height 20
static unsigned char rwarn_bits[] U8G_PROGMEM = {
   0x00, 0x07, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x7f, 0x00, 0x00,
   0x00, 0xff, 0x01, 0x00, 0xff, 0xff, 0x07, 0x00, 0xff, 0xff, 0x1f, 0x00,
   0xff, 0xff, 0x3f, 0x00, 0xff, 0xff, 0x7f, 0x00, 0xff, 0xff, 0xff, 0x01,
   0xff, 0xff, 0xff, 0x03, 0xff, 0xff, 0xff, 0x03, 0xff, 0xff, 0xff, 0x01,
   0xff, 0xff, 0x7f, 0x00, 0xff, 0xff, 0x3f, 0x00, 0xff, 0xff, 0x1f, 0x00,
   0xff, 0xff, 0x07, 0x00, 0x00, 0xff, 0x01, 0x00, 0x00, 0x7f, 0x00, 0x00,
   0x00, 0x1f, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00 };
#define lwarn_width 26
#define lwarn_height 20
static unsigned char lwarn_bits[] U8G_PROGMEM = {
   0x00, 0x80, 0x03, 0x00, 0x00, 0xe0, 0x03, 0x00, 0x00, 0xf8, 0x03, 0x00,
   0x00, 0xfe, 0x03, 0x00, 0x80, 0xff, 0xff, 0x03, 0xe0, 0xff, 0xff, 0x03,
   0xf0, 0xff, 0xff, 0x03, 0xf8, 0xff, 0xff, 0x03, 0xfe, 0xff, 0xff, 0x03,
   0xff, 0xff, 0xff, 0x03, 0xff, 0xff, 0xff, 0x03, 0xfe, 0xff, 0xff, 0x03,
   0xf8, 0xff, 0xff, 0x03, 0xf0, 0xff, 0xff, 0x03, 0xe0, 0xff, 0xff, 0x03,
   0x80, 0xff, 0xff, 0x03, 0x00, 0xfe, 0x03, 0x00, 0x00, 0xf8, 0x03, 0x00,
   0x00, 0xe0, 0x03, 0x00, 0x00, 0x80, 0x03, 0x00 };
#define brake_width 30
#define brake_height 24
static unsigned char brake_bits[] U8G_PROGMEM = {
   0xff, 0xff, 0xff, 0x3f, 0xff, 0x0f, 0xf8, 0x3f, 0xff, 0xf1, 0xc7, 0x3f,
   0xef, 0xfe, 0xbf, 0x3d, 0x77, 0xff, 0x7f, 0x3b, 0xb7, 0x3f, 0x7f, 0x37,
   0xdb, 0x3f, 0xff, 0x36, 0xeb, 0x3f, 0xff, 0x36, 0xe9, 0x3f, 0xff, 0x2d,
   0xed, 0x3f, 0xff, 0x2d, 0xed, 0x3f, 0xff, 0x2d, 0xed, 0x3f, 0xff, 0x2d,
   0xed, 0x3f, 0xff, 0x2d, 0xed, 0x3f, 0xff, 0x2d, 0xed, 0x3f, 0xff, 0x2d,
   0xe9, 0xff, 0xff, 0x2e, 0xeb, 0xff, 0xff, 0x36, 0xdb, 0x3f, 0xff, 0x36,
   0xb7, 0x3f, 0x7f, 0x3b, 0x77, 0xff, 0xbf, 0x3b, 0xef, 0xfe, 0xdf, 0x3d,
   0xff, 0xf1, 0xe3, 0x3f, 0xff, 0x0f, 0xfc, 0x3f, 0xff, 0xff, 0xff, 0x3f };    
#define lights_width 39
#define lights_height 20
static unsigned char lights_bits[] U8G_PROGMEM = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x1e, 0xfe, 0x00,
   0x00, 0x80, 0x3f, 0xf8, 0x3c, 0x00, 0xdf, 0x0f, 0xe0, 0xfc, 0x80, 0x9f,
   0x03, 0x00, 0xfe, 0x80, 0x1f, 0x00, 0x00, 0xfe, 0xc1, 0x3f, 0x00, 0x00,
   0xef, 0xe1, 0x3d, 0x00, 0x3e, 0xcf, 0xe3, 0x79, 0x3e, 0x7f, 0xcf, 0xe3,
   0x79, 0x7f, 0x7f, 0xcf, 0xe3, 0x79, 0x7f, 0x3e, 0xcf, 0xe3, 0x79, 0x3e,
   0x00, 0xcf, 0xe3, 0x3d, 0x00, 0x00, 0xef, 0xe1, 0x3d, 0x00, 0x00, 0xfe,
   0xc0, 0x3f, 0x00, 0x00, 0xfe, 0x80, 0x1f, 0x00, 0xe0, 0x7c, 0x80, 0x9f,
   0x03, 0xfe, 0x00, 0x00, 0xc0, 0x3f, 0xfe, 0x00, 0x00, 0x80, 0x3f, 0x1c,
   0x00, 0x00, 0x00, 0x1c };
   
Receiver rec;
LightManager lm;

Servo myservo[6];

//Display management
int currentTime;
int displayTime;
int nbrdisp=0;
int displayCounter=0;

void setup(){
  rec.setup();
  lm.setup(2, 3, 4, 5, 6, 7, 13);
  myservo[0].attach(8);
  myservo[1].attach(9);
  myservo[2].attach(10);
  myservo[3].attach(11);
  myservo[4].attach(12);
}

void loop(){
  delay(75);
  if (rec.read()==1){
    myservo[0].write(rec.channels[4].angle);  
    myservo[1].write(rec.channels[5].angle);
    myservo[2].write(rec.channels[6].angle);
    myservo[3].write(rec.channels[7].angle);
    myservo[3].write(rec.channels[8].angle);
  };
  
  //Manage ligths
  lm.checkLights(rec.channels[3].angle, rec.channels[1].angle, rec.channels[4].angle);

  //Display
  displayCounter++;
  if (displayCounter>4){
    displayCounter=0;
    draw();
  }
}

void drawingScreenChooser(){
  if (millis()<10000){
    nbrdisp=0;
  } else if (rec.channels[14].angle<30){
    nbrdisp=0;
  } else if (rec.channels[14].angle<60){
    nbrdisp=1;
  } else if (rec.channels[14].angle<90){
    nbrdisp=2;
  } else if (rec.channels[14].angle<120){
    nbrdisp=3;
  } else if (rec.channels[14].angle<150){
    nbrdisp=4;
  } else {
    nbrdisp=5;
  }
  nbrdisp=1;
}

void drawScreen(int screenNumber){
  if (screenNumber==0){
    //u8g.drawXBMP( 4, 12, Logo_challenger_width, Logo_challenger_height, Logo_challenger_bits);
  } else if (screenNumber==1){
    u8g.drawXBMP( 6, 34, Logo_challenger_small_width, Logo_challenger_small_height, Logo_challenger_small_bits);
    if ((lm.lWarn || lm.warnings) && lm.blinkstate) {
      u8g.drawXBMP( 1, 1, lwarn_width, lwarn_height, lwarn_bits);
    };
    if (lm.brake) {
      u8g.drawXBMP( 31, 1, brake_width, brake_height, brake_bits);
    };
    if (lm.lights) {
      u8g.drawXBMP( 63, 1, lights_width, lights_height, lights_bits);
    };
    if ((lm.rWarn || lm.warnings) && lm.blinkstate) {
      u8g.drawXBMP( 102, 1, rwarn_width, rwarn_height, rwarn_bits);
    };
  } else if (screenNumber==2){
    //Servos values
    u8g.setFont(u8g_font_courR10);
    u8g.drawStr(0, 12, String(rec.channels[0].angle).c_str());
    u8g.drawStr(0, 26, String(rec.channels[1].angle).c_str());
    u8g.drawStr(0, 40, String(rec.channels[2].angle).c_str());
    u8g.drawStr(0, 54, String(rec.channels[3].angle).c_str());
    u8g.drawStr(32, 12, String(rec.channels[4].angle).c_str());
    u8g.drawStr(32, 26, String(rec.channels[5].angle).c_str());
    u8g.drawStr(32, 40, String(rec.channels[6].angle).c_str());
    u8g.drawStr(32, 54, String(rec.channels[7].angle).c_str());
    u8g.drawStr(64, 12, String(rec.channels[8].angle).c_str());
    u8g.drawStr(64, 26, String(rec.channels[9].angle).c_str());
    u8g.drawStr(64, 40, String(rec.channels[10].angle).c_str());
    u8g.drawStr(64, 54, String(rec.channels[11].angle).c_str());
    u8g.drawStr(96, 12, String(rec.channels[12].angle).c_str());
    u8g.drawStr(96, 26, String(rec.channels[13].angle).c_str());
  } else if (screenNumber==3){
    //PWM values
    u8g.setFont(u8g_font_courR10);
    u8g.drawStr(0, 12, String(rec.channels[0].pwmvalue).c_str());
    u8g.drawStr(0, 26, String(rec.channels[1].pwmvalue).c_str());
    u8g.drawStr(0, 40, String(rec.channels[2].pwmvalue).c_str());
    u8g.drawStr(0, 54, String(rec.channels[3].pwmvalue).c_str());
    u8g.drawStr(32, 12, String(rec.channels[4].pwmvalue).c_str());
    u8g.drawStr(32, 26, String(rec.channels[5].pwmvalue).c_str());
    u8g.drawStr(32, 40, String(rec.channels[6].pwmvalue).c_str());
    u8g.drawStr(32, 54, String(rec.channels[7].pwmvalue).c_str());
    u8g.drawStr(64, 12, String(rec.channels[8].pwmvalue).c_str());
    u8g.drawStr(64, 26, String(rec.channels[9].pwmvalue).c_str());
    u8g.drawStr(64, 40, String(rec.channels[10].pwmvalue).c_str());
    u8g.drawStr(64, 54, String(rec.channels[11].pwmvalue).c_str());
    u8g.drawStr(96, 12, String(rec.channels[12].pwmvalue).c_str());
    u8g.drawStr(96, 26, String(rec.channels[13].pwmvalue).c_str());
    u8g.drawStr(96, 40, String(rec.channels[14].pwmvalue).c_str());
    u8g.drawStr(96, 54, String(rec.channels[15].pwmvalue).c_str());
//  } else if (screenNumber==4){
//    //Light canal history
//    u8g.setFont(u8g_font_courR10);
//    u8g.drawStr(0, 12, String(lm.lightHistory.history[0].state).c_str());
//    u8g.drawStr(0, 26, String(lm.lightHistory.history[0].timing).c_str());
//    u8g.drawStr(0, 40, String(lm.lightHistory.history[1].state).c_str());
//    u8g.drawStr(0, 54, String(lm.lightHistory.history[1].timing).c_str());
//    u8g.drawStr(40, 12, String(lm.lightHistory.history[2].state).c_str());
//    u8g.drawStr(40, 26, String(lm.lightHistory.history[2].timing).c_str());
//    u8g.drawStr(40, 40, String(lm.lightHistory.history[3].state).c_str());
//    u8g.drawStr(40, 54, String(lm.lightHistory.history[3].timing).c_str());
//    u8g.drawStr(80, 12, String(lm.lightHistory.history[4].state).c_str());
//    u8g.drawStr(80, 26, String(lm.lightHistory.history[4].timing).c_str());
//
//    u8g.drawStr(80, 40, String(lm.lwPIN).c_str());
//    u8g.drawStr(80, 54, String(lm.rwPIN).c_str());
  } else if (screenNumber==4){
    //Light canal history
    u8g.setFont(u8g_font_courR10);
    u8g.drawStr(0, 12, String(lm.throttleHistory.history[0].angle).c_str());
    u8g.drawStr(0, 26, String(lm.throttleHistory.history[0].timing).c_str());
    u8g.drawStr(0, 40, String(lm.throttleHistory.history[1].angle).c_str());
    u8g.drawStr(0, 54, String(lm.throttleHistory.history[1].timing).c_str());
    u8g.drawStr(40, 12, String(lm.throttleHistory.history[2].angle).c_str());
    u8g.drawStr(40, 26, String(lm.throttleHistory.history[2].timing).c_str());
    u8g.drawStr(40, 40, String(lm.throttleHistory.history[3].angle).c_str());
    u8g.drawStr(40, 54, String(lm.throttleHistory.history[3].timing).c_str());
    u8g.drawStr(80, 12, String(lm.throttleHistory.history[4].angle).c_str());
    u8g.drawStr(80, 26, String(lm.throttleHistory.history[4].timing).c_str());

    u8g.drawStr(80, 40, "Throttle");
    u8g.drawStr(80, 54, String(lm.throttleHistory.idDecreasing()).c_str());  
  } else if (screenNumber==5){
    //Light canal history
    u8g.setFont(u8g_font_courR10);
    u8g.drawStr(0, 12, String(lm.steerHistory.history[0].angle).c_str());
    u8g.drawStr(0, 26, String(lm.steerHistory.history[0].timing).c_str());
    u8g.drawStr(0, 40, String(lm.steerHistory.history[1].angle).c_str());
    u8g.drawStr(0, 54, String(lm.steerHistory.history[1].timing).c_str());
    u8g.drawStr(40, 12, String(lm.steerHistory.history[2].angle).c_str());
    u8g.drawStr(40, 26, String(lm.steerHistory.history[2].timing).c_str());
    u8g.drawStr(40, 40, String(lm.steerHistory.history[3].angle).c_str());
    u8g.drawStr(40, 54, String(lm.steerHistory.history[3].timing).c_str());
    u8g.drawStr(80, 12, String(lm.steerHistory.history[4].angle).c_str());
    u8g.drawStr(80, 26, String(lm.steerHistory.history[4].timing).c_str());

    u8g.drawStr(80, 40, "Steer");
    u8g.drawStr(80, 54, String(lm.steerHistory.idDecreasing()).c_str());
  };
}

void draw() {
  drawingScreenChooser();
  u8g.firstPage(); 
    do {
        drawScreen(nbrdisp);
    } while( u8g.nextPage() );
}

