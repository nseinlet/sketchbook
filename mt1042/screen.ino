#define Logo_challenger_small_width 63
#define Logo_challenger_small_height 16
static const unsigned char Logo_challenger_small_bits[] U8X8_PROGMEM = {
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
uint32_t currentTime, displayTime;
int nbrdisp;
int lastdisp;
int draw_angle;
int endpointx;
int endpointy;

void setupScreen(){
  displayTime = 0;
  nbrdisp = 0;

  u8g2.begin();
}

void drawScreen(){
  currentTime = millis();
  if(currentTime > displayTime)
  {
    displayTime = currentTime + 5000;
    nbrdisp = nbrdisp + 1;
    if (nbrdisp>7) {
      nbrdisp=0;
    }
    u8g2.clearBuffer();
    if (nbrdisp==0) {
      drawScreen0();
    } else if (nbrdisp==1){
      drawScreen1();
    } else if (nbrdisp==2){
      drawScreen2();
    } else if (nbrdisp==3){
      drawScreen3();
    } else if (nbrdisp==4){
      drawScreen4();
    } else {
      drawScreen5();
    };
    u8g2.sendBuffer();
  };
};

void _drawServo(int pos, int angle) {
  draw_angle = angle-90;
  if (angle>90) {angle=90;};
  if (angle<-90) {angle=-90;};

  u8g2.drawLine(  pos,  1, 4+pos,  1);
  u8g2.drawLine(  pos, 23,   pos,  1);
  u8g2.drawLine(1+pos, 12, 1+pos, 12+(draw_angle/9));
  u8g2.drawLine(2+pos, 12, 2+pos, 12+(draw_angle/9));
  u8g2.drawLine(3+pos, 12, 3+pos, 12+(draw_angle/9));
  u8g2.drawLine(4+pos, 23, 4+pos,  1);
  u8g2.drawLine(  pos, 23, 4+pos, 23);
}

void drawScreen0() {
  //Just a logo :-)  
  u8g2.drawXBMP( 2, 4, Logo_challenger_small_width, Logo_challenger_small_height, Logo_challenger_small_bits);
  u8g2.setFont(u8g2_font_5x7_tf);
  u8g2.drawStr( 17, 30, "MT 1042");
};

void drawScreen1() {
  //Servo outputs 1->8
  int i;

  for (i=0;i<8;i++) {
    _drawServo(2+i*8, rec.channels[i].angle);
  };
    
  u8g2.setFont(u8g2_font_4x6_tf);
  u8g2.drawStr( 3, 32, "1 2 3 4 5 6 7 8");
};

void drawScreen2() {
  //Servo outputs 9->16
  int i;

  for (i=8;i<16;i++) {
    _drawServo(2+(i-8)*8, rec.channels[i].angle);
  };
    
  u8g2.setFont(u8g2_font_4x6_tf);
  u8g2.drawStr( 0, 32, "8 910111213141516");
};

void drawScreen3() {
  // Lights
  u8g2.setFont(u8g2_font_cursor_tf);
  if ((lm.lWarn || lm.warnings) && lm.blinkstate) {
    u8g2.drawStr( 1, 4, "\x8f");
  };
  if ((lm.rWarn || lm.warnings) && lm.blinkstate) {
    u8g2.drawStr(63, 4, "\x91");
  };
  if (rec.channels[1].angle>85){
    u8g2.drawStr(28, 17, "\x93");
  };
  if (rec.channels[1].angle<95){
    u8g2.drawStr(38, 32, "\x8b");
  };
  if (lm.turningWarn) {
    u8g2.drawStr(53, 23, "\x53");  
  };
  if (lm.lights) {
    u8g2.drawStr(5, 22, "\xae");
  };
  if (lm.highlights || lm.lightWarn) {
    u8g2.drawStr(5, 22, "\xaf");
  };
};

void drawScreen4() {
  u8g2.setFont(u8g2_font_5x7_tf);
  u8g2.drawStr( 1, 8, (String(fcs.getCurrent())+" A").c_str());
  u8g2.drawStr( 1, 16, (String(fcs.getVoltage())+" V").c_str());

  u8g2.drawStr( 1, 24, (String(receiver.getRxBatt())+" V").c_str());
  u8g2.drawStr( 1, 32, (String(receiver.getAdc2())+" V").c_str());

};

void drawScreen5() {
  u8g2.setFont(u8g2_font_5x7_tf);
  u8g2.drawStr( 18, 26, (String(fcs.getCurrent())+"A").c_str());

  //Power gauge
  u8g2.drawCircle(32, 32, 30, U8G2_DRAW_UPPER_RIGHT|U8G2_DRAW_UPPER_LEFT);
  u8g2.drawCircle(32, 32, 29, U8G2_DRAW_UPPER_RIGHT|U8G2_DRAW_UPPER_LEFT);
  u8g2.drawCircle(32, 32, 28, U8G2_DRAW_UPPER_RIGHT|U8G2_DRAW_UPPER_LEFT);
      
  angle = abs(rec.channels[1].angle-90)*2;
  if (angle>175) {angle=175;};
  if (angle<30) {angle=30;};
      
  endpointx = 32 - (int) (28 * cos((angle * 71) / 4068.0));
  endpointy = 32 - (int) (28 * sin((angle * 71) / 4068.0));
  u8g2.drawLine(31, 32, endpointx, endpointy);
  u8g2.drawLine(32, 32, endpointx, endpointy);
  u8g2.drawLine(33, 32, endpointx, endpointy);

  for(angle=10;angle<180;angle+=25){
    int startpointx = 32 - (int) (22 * cos((angle * 71) / 4068.0));
    int startpointy = 32 - (int) (22 * sin((angle * 71) / 4068.0));
    endpointx = 32 - (int) (28 * cos((angle * 71) / 4068.0));
    endpointy = 32 - (int) (28 * sin((angle * 71) / 4068.0));

    u8g2.drawLine(startpointx, startpointy, endpointx, endpointy);
  }

  u8g2.setFont(u8g2_font_cursor_tf);
  if ((lm.lWarn || lm.warnings) && lm.blinkstate) {
    u8g2.drawStr( 1, 4, "\x8f");
  };
  if ((lm.rWarn || lm.warnings) && lm.blinkstate) {
    u8g2.drawStr(63, 4, "\x91");
  };
};
