/*********************************************
 * FrSky SmartPort Display                   *
 *                                           *
 * Read sensors values on SmartPort          *
 * Display them on a 1inch display           *
 *********************************************/

/* Display */
#include "U8glib.h"

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_NO_ACK|U8G_I2C_OPT_FAST);

/* Smartport */
#include "FrSkySportSensor.h"
#include "FrSkySportSensorFcs.h"
#include "FrSkySportSensorFlvss.h"
#include "FrSkySportSingleWireSerial.h"
#include "FrSkySportDecoder.h"
#include "SoftwareSerial.h"
 
FrSkySportSensorFcs fcs;                               // FCS-40A sensor with default ID
FrSkySportSensorFlvss flvss;                          // FLVSS sensor with default ID
FrSkySportDecoder decoder;                             // Create decoder object without polling

uint32_t currentTime, displayTime;
uint16_t decodeResult;
int nbrdisp;
int ledState = LOW;

void setup(void) {
  nbrdisp=0;
  /*SmartPort*/
  decoder.begin(FrSkySportSingleWireSerial::SOFT_SERIAL_PIN_12, &fcs, &flvss);
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
}

void drawScreen(String power, String voltage, float cell[], float rxbatt, int rssi, int swr) {
  String tmp;
  
  u8g.setFont(u8g_font_courB10);
  u8g.drawStr( 0, 12, power.c_str());
  u8g.drawStr( 64, 12, voltage.c_str());

  u8g.setFont(u8g_font_courR10);
  tmp = "1:"+String(cell[0])+"v";
  u8g.drawStr( 0, 26, tmp.c_str());
  tmp = "2:"+String(cell[1])+"v";
  u8g.drawStr( 64, 26, tmp.c_str());
  tmp = "3:"+String(cell[2])+"v";
  u8g.drawStr( 0, 40, tmp.c_str());
  tmp = "4:"+String(cell[3])+"v";
  u8g.drawStr( 64, 40, tmp.c_str());

  tmp = "Rx: "+String(rxbatt)+"v";
  u8g.drawStr( 0, 64, tmp.c_str());
}

void blinking()
{
  if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
    digitalWrite(LED_BUILTIN, ledState); 
};

void loop()
{
  decodeResult = decoder.decode();
  
  // Display data once a second to not interfeere with data decoding
  currentTime = millis();
  if(currentTime > displayTime)
  {
    displayTime = currentTime + 1000;
    nbrdisp = nbrdisp + 1;
    if (nbrdisp>10) {
      nbrdisp=0;
    }
    
    int rssi=0;
    float rxbatt = 0.0;
    int swr = 0;

    // Get current/voltage sensor (FCS) data
    String power=String(fcs.getCurrent())+"A";
    String voltage=String(fcs.getVoltage())+"V";
  
    // Get LiPo voltage sensor (FLVSS) data
    float c[4]={flvss.getCell1(), flvss.getCell2(), flvss.getCell3(), flvss.getCell4()};

    u8g.firstPage();  
     do {
      blinking();
        drawScreen(power, voltage, c, rxbatt, rssi, swr);
     } while( u8g.nextPage() );
  }
}
