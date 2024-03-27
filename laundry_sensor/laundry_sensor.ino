#include <DHT.h>
#include <SPI.h>
#include <WiFiNINA.h>
#include <Adafruit_SleepyDog.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSerif9pt7b.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "consts.h"

#define serialdebug true
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

// ************************
// Temp
// ************************
#define DHTin 7
DHT dht(DHTin, DHT22);

//Temperature sensor
#define MAX_SENSORS 10
#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);  
DallasTemperature sensors(&oneWire);
int deviceCount = 0;
float temperatures[MAX_SENSORS];
String temperaturesAddr[MAX_SENSORS];

int8_t tempmhz;
float humidity, tempdht, tempreal;
int ledpin = 6;
int status = WL_IDLE_STATUS;
int pingResult;
WiFiServer server(80);
IPAddress ip;
int displayedScreen;
uint32_t delayMS;
uint32_t delayPing;
long lastReadingTime = 0;
long lastPingTime = 0;
long lastSrvTime = 0;
bool pingIsOk;

void setup () {
  if (serialdebug){Serial.begin(115200);};

  delayMS = 30000;
  pingIsOk = true;
  delayPing = 600000;
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(5, 5);  
  display.println("init DHT22");
  display.display();
  
  // DHT22 init
  dht.begin();

  //Dallas Temperature sensors
  //T° sensor
  sensors.begin();
  // locate devices on the bus
  if (serialdebug){
    Serial.print("Locating devices...");
    Serial.print("Found ");
  };  
  deviceCount = sensors.getDeviceCount();
  if (serialdebug){  
    Serial.print(deviceCount, DEC);
  };
  
  //WiFi
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(5, 5);  
  display.println("init WiFi");
  display.display();
  
  if (serialdebug){  
    Serial.print("WiFi101 shield: ");
    if (WiFi.status() == WL_NO_SHIELD) {
      Serial.println("NOT PRESENT");
      return; // don't continue
    }
    Serial.println("DETECTED");
  }  
  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) {
    digitalWrite(ledpin, LOW);
    if (serialdebug){
      Serial.print("Attempting to connect to Network named: ");
      Serial.println(ssid);                   // print the network name (SSID);
    }    
    digitalWrite(ledpin, HIGH);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
    // print the SSID of the network you're attached to:
    if (serialdebug){    
      Serial.print("SSID: ");
      Serial.println(WiFi.SSID());
    };
 
    // print your WiFi shield's IP address:
    ip = WiFi.localIP();
    if (serialdebug){    
      Serial.print("IP Address: ");
      Serial.println(ip);
    };      
 
    // print the received signal strength:
    long rssi = WiFi.RSSI();
    if (serialdebug){    
      Serial.print("signal strength (RSSI):");
      Serial.print(rssi);
      Serial.println(" dBm");
      // print where to go in a browser:
      Serial.print("To see this page in action, open a browser to http://");
      Serial.println(ip);
    };    
  }
  
  Watchdog.enable(600000);
  
  server.begin(); 

  //Control led
  pinMode(6, OUTPUT);
  digitalWrite(6, LOW);
  displayedScreen = 0;
};

void readDHT22(){
  humidity = dht.readHumidity();
  tempdht = dht.readTemperature();
  tempreal = dht.computeHeatIndex(tempdht, humidity, false); // false -> Celsius
};

void listenForWifiClients() {
  // listen for incoming clients
  WiFiClient client = server.available();
  if (client) {
    // an http request ends with a blank line
    bool currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        digitalWrite(6, HIGH);
        lastSrvTime = millis();
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
          client.print(tempdht);
          client.print(",\"temperaturereal\":");
          client.print(tempreal);
          client.print(",\"humidity\":");
          client.print(humidity);
          client.print(",\"temperaturedht\":");
          client.print(tempdht);
          client.print(",\"wifirssi\":");
          client.print(WiFi.RSSI());
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
        digitalWrite(6, LOW);
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
  }
};

void show_info() {
  int currentline=0;  
  int maxscreens;

  maxscreens = deviceCount / 4;
  if ((deviceCount % 4)>0) {maxscreens = maxscreens + 1;}
  
  displayedScreen = displayedScreen + 1;
  if (displayedScreen>maxscreens){displayedScreen=0;};
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);  
  display.setFont();
  if (displayedScreen==0){  
    display.setCursor(0, 0);  
    display.println("T");
    display.setCursor(10, 0);  
    display.println(tempreal);
    display.setCursor(50, 0);
    display.println("H");
    display.setCursor(60, 0);  
    display.println(humidity);
    display.setCursor(0, 12);
    display.println("IP");
    display.setCursor(30, 12);
    display.println(ip);
    display.setCursor(0, 24);
    display.println("RSSI");
    display.setCursor(30, 24);
    display.println(WiFi.RSSI());
  };
  if (displayedScreen!=0){
    for (int i=((displayedScreen - 1) * 4); i<deviceCount && i<MAX_SENSORS && i<4*displayedScreen;  i++)
    {
      display.setCursor(0, currentline);
      display.println("Sensor ");
      display.setCursor(40, currentline);
      display.println(i+1);
      display.setCursor(55, currentline);
      display.println(temperatures[i]);      
      currentline = currentline + 9;
    }
  };    
  display.display();

  if (serialdebug){
    Serial.print("Temp. (C) : ");
    Serial.println(tempreal);
    Serial.print("Humidite : ");
    Serial.println(humidity);  
  };    
};


void check_wifi() {
  status = WiFi.status(); 

    if(status != WL_CONNECTED)
    {
      display.clearDisplay();
      display.setTextColor(WHITE);
      display.setTextSize(1);
      display.setCursor(5, 5);
      display.println("Reconnect WiFi");
      display.display();
      
    int retries=0;
    if(WiFi.status() != WL_CONNECTED)
    WiFi.begin(ssid, pass); 
    delay(1000);
    status = WiFi.status();
    if (serialdebug){    
      Serial.println(status);
    };      
    while((status != WL_CONNECTED) && (retries<2))
    {
      WiFi.begin(ssid, pass); 
      retries++;
      if (serialdebug){
        Serial.println("\n Trying to Reconnect to WiFi Network ");
      };      
      delay(2000);
      status = WiFi.status();
      if (serialdebug){
        Serial.println(status);
      };
      server.begin(); 
    }
    }
};

void readTempSensorsValues()
{
  DeviceAddress Thermometer;
  float tempC;
  String addr;
  
  sensors.requestTemperatures();
  deviceCount = sensors.getDeviceCount();  
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

void loop () {
  
  if (millis() - lastReadingTime > delayMS) {
    digitalWrite(6, HIGH);
    readDHT22();
    readTempSensorsValues();
    show_info();
    check_wifi();
    
    lastReadingTime = millis();
    
    digitalWrite(6, LOW);
    ip = WiFi.localIP();
    if (serialdebug){    
      Serial.print("IP Address: ");
      Serial.println(ip);
    };
  };   
     
  listenForWifiClients();
  
  if (pingIsOk){Watchdog.reset();};
  if (millis() - lastPingTime > delayPing) {
    if (serialdebug){Serial.println("Ping..."); };
    pingResult = WiFi.ping("172.16.17.253");
    if (pingResult >= 0) {if (serialdebug){Serial.println("Ping done"); };} else {pingIsOk=false; };
    lastPingTime = millis();
    if ((lastPingTime-lastSrvTime)>900000) {pingIsOk=false; };
  };  
}
