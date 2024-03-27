#include <Ethernet.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_Sensor.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFiClient.h>

//Cheap sensor T°/hmidity
#define DHT_PIN 7
#define DHT_TYPE DHT11

DHT_Unified dht(DHT_PIN, DHT_TYPE);
uint32_t delayMS;
float h,t;
long lastReadingTime = 0;

//Temperature sensor
#define MAX_SENSORS 10
#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);  
DallasTemperature sensors(&oneWire);
int deviceCount = 0;
float temperatures[MAX_SENSORS];
String temperaturesAddr[MAX_SENSORS];

//Pressure_sensor
#define MAX_PRESSURE_SENSORS 2
float pressure[MAX_PRESSURE_SENSORS];

//Web server
byte mac[] = { 0x90, 0xA2, 0xDA, 0x00, 0xF1, 0xDE };
IPAddress  ip;
EthernetServer server(80);

void setup() {
  Serial.begin(115200);
  //Ethernet
  char erreur = Ethernet.begin(mac);
  if (erreur == 0) {
    Serial.println("Parametrage avec ip fixe...");
  };
  ip=Ethernet.localIP();
  Serial.println(ip);
  server.begin();
  
  //Cheap Sensor
  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  delayMS = sensor.min_delay / 1000;
  if (delayMS<5000){delayMS=5000;};

  //T° sensor
  sensors.begin();
  // locate devices on the bus
  Serial.print("Locating devices...");
  Serial.print("Found ");
  deviceCount = sensors.getDeviceCount();
  Serial.print(deviceCount, DEC);
}

void loop() { 
  if (millis() - lastReadingTime > delayMS) {
      readCheapSensor();
      readSensorsValues();
  
      lastReadingTime = millis();
  };
  listenForEthernetClients();
}

void listenForEthernetClients() {
  // listen for incoming clients
  WiFiClient client = server.available(); 
  if (client) {
    // an http request ends with a blank line
    bool currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          // print the current readings, in HTML format:
          client.print("{\"temperature\":");
          client.print(t);
          client.print(",\"humidity\":" + String(h));
          if (deviceCount>0)
          {
            client.print(",\"temperatures\":[");
            for (int i = 0;  i<deviceCount && i<MAX_SENSORS;  i++)
            {
              if (i > 0) client.print(",");
              client.print("{\"temperature\":" + String(temperatures[i]) + ",\"address\":\"" + temperaturesAddr[i] + "\"}");
            }
            client.println("]");
          };
          client.print(",\"pressures\":[");
          for (int i = 0;  i < MAX_PRESSURE_SENSORS;  i++){
            if (i > 0) client.print(",");
            client.print("{\"id\": ");
            client.print(i+1);
            client.print(", \"pressure\": ");
            client.print(pressure[i]);
            client.print("}");
          };
          client.println("],");
          client.println("}");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
  }
}

void readCheapSensor()
{
  sensors_event_t event;

  dht.temperature().getEvent(&event);
  if (not (isnan(event.temperature))) 
    t=event.temperature;
  Serial.println(t);
  dht.humidity().getEvent(&event);
  if (not (isnan(event.relative_humidity)))
    h=event.relative_humidity;
  Serial.println(h);
}

void readSensorsValues()
{
  DeviceAddress Thermometer;
  float tempC;
  String addr;
  
  sensors.requestTemperatures();
  for (int i = 0;  i<deviceCount && i<MAX_SENSORS;  i++)
  {
    sensors.getAddress(Thermometer, i);
    temperatures[i] = sensors.getTempC(Thermometer);
    temperaturesAddr[i] = sensorAddressToString(Thermometer);
    Serial.print(temperaturesAddr[i]);
    Serial.print(": ");
    Serial.println(temperatures[i]);
  }
  
}

String sensorAddressToString(DeviceAddress deviceAddress)
{ 
  String toReturn="";
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 0x10) toReturn+="0";
    toReturn+=String(deviceAddress[i], HEX);
    if (i < 7) toReturn+=":";
  }
  return toReturn;
}

void readPreasureGauge()
{
   float tmp_pressure;

   tmp_pressure=readPreasureGaugeLoop(A0);
   if (tmp_pressure>0.0){
      pressure[0]=tmp_pressure;
   };
   tmp_pressure=readPreasureGaugeLoop(A1);
   if (tmp_pressure>0.0){
      pressure[1]=tmp_pressure;
   };
   
}

float readPreasureGaugeLoop(int pin)
{
   int count = 16;
   float raw = 0;
   float tmp;
   int valid_measure=0;
   
   for (int i=0; i< count; i++){
     tmp=readPreasureGaugeReal(pin);
     if (tmp>0) {
      raw+=tmp;
      valid_measure+=1;
     }
   };
   if (valid_measure>0){
    Serial.print(raw/valid_measure);
    Serial.println(" Bars");
    return raw/valid_measure;
   }
   Serial.print(raw);
    Serial.print(" Bars for invalids: ");
    Serial.println(valid_measure);
   return 0;
}

float readPreasureGaugeReal(int pin)
{
  // MEASUREMENT
  int raw = analogRead(pin);  // return 0..1023 representing 0..5V

  // CONVERT TO VOLTAGE
  float voltage = 5.0 * raw / 1023; // voltage = 0..5V;  we do the math in millivolts!!

  // INTERPRET VOLTAGES
  if (voltage < 0.5)
  {
     //Serial.print(voltage);
     //Serial.println("v :open circuit");
     return 0,0;
  }
  else if (voltage <= 4.5)  // between 0.5 and 4.5 now...
  {
    float psi = mapFloat(voltage, 0.5, 4.5, 0.0, 300.0);    // variation on the Arduino map() function
    //Serial.println(psi);
    return psi/14.504;
  }
  else
  {
    //Serial.println("Signal too high!!");
    return 0,0;
  }
}

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
