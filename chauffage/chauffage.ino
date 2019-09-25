#include <Ethernet.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_Sensor.h>
#include <OneWire.h>
#include <DallasTemperature.h>

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

  //T° sensor
  sensors.begin();
  // locate devices on the bus
  Serial.print("Locating devices...");
  Serial.print("Found ");
  deviceCount = sensors.getDeviceCount();
  Serial.print(deviceCount, DEC);

  for (int i = 0;  i < MAX_SENSORS;  i++)
  {
    temperatures[i] = 11.4+i;
    temperaturesAddr[i] = String("12345");
  }
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
  EthernetClient client = server.available();
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

  dht.humidity().getEvent(&event);
  if (not (isnan(event.relative_humidity)))
    h=event.relative_humidity;
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
  }
  
}

String sensorAddressToString(DeviceAddress deviceAddress)
{ 
  String toReturn="";
  for (uint8_t i = 0; i < 8; i++)
  {
    toReturn+=String("0x");
    if (deviceAddress[i] < 0x10) toReturn+="0";
    toReturn+=String(deviceAddress[i], HEX);
    if (i < 7) toReturn+="-";
  }
  return toReturn;
}
