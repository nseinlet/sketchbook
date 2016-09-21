/*********************************************
 * FrSky SmartPort Display                   *
 *                                           *
 * Read sensors values on SmartPort          *
 * Display them on a 1inch display           *
 *********************************************/

/* Display */
#include "U8glib.h"

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_FAST);

/* Smartport */
#include "FrSkySportSensor.h"
#include "FrSkySportSensorFcs.h"
#include "FrSkySportSensorFlvss.h"
#include "FrSkySportSensorRpm.h"
#include "FrSkySportSingleWireSerial.h"
#include "FrSkySportDecoder.h"
#include "SoftwareSerial.h"

FrSkySportSensorFcs fcs;                               // FCS-40A sensor with default ID
FrSkySportSensorFlvss flvss;                          // FLVSS sensor with default ID
FrSkySportSensorRpm rpm1;                               // RPM sensor with default ID
FrSkySportSensorRpm rpm2(FrSkySportSensor::ID6);      // RPM sensor with given ID
FrSkySportDecoder decoder;                             // Create decoder object without polling

uint32_t currentTime, displayTime;
uint16_t decodeResult;
int nbrdisp;

void setup(void) {
  nbrdisp = 0;
  
  /*Display */
  // assign default color value
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255,255,255);
  }
  
  /*SmartPort*/
  decoder.begin(FrSkySportSingleWireSerial::SOFT_SERIAL_PIN_12, &fcs, &flvss, &rpm1, &rpm2);
}

void drawScreen(String power, String voltage, float cell[], uint32_t rpm[], int32_t temp[], float rxbatt, int rssi, int swr) {
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

void drawScreen2(String power, String voltage, float cell[], uint32_t rpm[], int32_t temp[], float rxbatt, int rssi, int swr) {
  String tmp;
  
  u8g.setFont(u8g_font_courB10);
  u8g.drawStr( 0, 12, power.c_str());
  u8g.drawStr( 64, 12, voltage.c_str());

  u8g.setFont(u8g_font_courR10);
  tmp=String(temp[0])+" C";
  u8g.drawStr(0,  26, tmp.c_str());
  tmp=String(temp[0])+" C";
  u8g.drawStr(64,  26, tmp.c_str());
  tmp=String(temp[0])+" C";
  u8g.drawStr(0,  40, tmp.c_str());
  tmp=String(temp[0])+" C";
  u8g.drawStr(64,  40, tmp.c_str());

  u8g.setFont(u8g_font_courB10);
  tmp = String(rpm[0])+"T/m";
  u8g.drawStr( 0, 63, tmp.c_str());
  tmp = String(rpm[1])+"T/m";
  u8g.drawStr( 64, 63, tmp.c_str());
}

void loop()
{
  // Read and decode the telemetry data, note that the data will only be decoded for sensors
  // that that have been passed to the begin method. Print the AppID of the decoded data.
  decodeResult = decoder.decode();
  if(decodeResult != SENSOR_NO_DATA_ID) { Serial.print("Decoded data with AppID 0x"); Serial.println(decodeResult, HEX); }

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
   
    // Get RPM/temperature sensor data
    uint32_t rpm[2] = {rpm1.getRpm(), rpm2.getRpm()};
    int32_t temp[4] = {rpm1.getT1(), rpm1.getT2(), rpm2.getT1(), rpm2.getT2()}; // Temperature #1 in degrees Celsuis (can be negative, will be rounded)

    u8g.firstPage();  
     do {

      if (nbrdisp<=5) {
        drawScreen(power, voltage, c, rpm, temp, rxbatt, rssi, swr);
      } else {
        drawScreen2(power, voltage, c, rpm, temp, rxbatt, rssi, swr);
      };

     } while( u8g.nextPage() );
  }
}
