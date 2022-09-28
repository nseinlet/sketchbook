#include <DHT.h>
#include <MHZ19.h>
#include <SPI.h>
#include <WiFi101.h>
#include <Adafruit_SleepyDog.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSerif9pt7b.h>
#include "consts.h"

#define serialdebug true
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

// ************************
// Temp
// ************************
#define DHTin 7    // La ligne de communication du DHT22 sera donc branchée sur la pin D6 de l'Arduino
DHT dht(DHTin, DHT22);

// ************************
// CO2
// ************************
// pin for pwm reading
#define CO2_IN 10
// pin for uart reading
#define MH_Z19_RX 13
#define MH_Z19_TX 14
MHZ19 myMHZ19;

int ppm;
int8_t tempmhz;
float humidity, tempdht, tempreal;
int ledpin = 6;
int status = WL_IDLE_STATUS;
WiFiServer server(80);
IPAddress ip;

uint32_t delayMS;
long lastReadingTime = 0;

void setup () {
  if (serialdebug){Serial.begin(115200);};

  delayMS=30000;

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(5, 5);  
  display.println("init DHT22");
  display.display();
  
  // DHT22 init
  dht.begin();
  
  //CO2 init
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(5, 5);  
  display.println("init MH-Z19B");
  display.display();
  
  pinMode(CO2_IN, INPUT);
  delay(100);
  if (serialdebug){
    Serial.println("MHZ 19B");
  };
  Serial1.begin(9600);                               // (Uno example) device to MH-Z19 serial start       
  myMHZ19.begin(Serial1);                                // *Serial(Stream) refence must be passed to library begin(). 

  myMHZ19.autoCalibration();
  delay(3000);
  if (serialdebug){  
    Serial.print("Range: ");
    Serial.println(myMHZ19.getRange());   
    Serial.print("Background CO2: ");
    Serial.println(myMHZ19.getBackgroundCO2());
    Serial.print("Temperature Cal: ");
    Serial.println(myMHZ19.getTempAdjustment());
    Serial.print("ABC Status: "); myMHZ19.getABC() ? Serial.println("ON") :  Serial.println("OFF");
  }  
  char myVersion[4];          
  myMHZ19.getVersion(myVersion);

  if (serialdebug){
    Serial.print("\nFirmware Version: ");
    for(byte i = 0; i < 4; i++){
      Serial.print(myVersion[i]);
      if(i == 1)
        Serial.print(".");    
    }
    Serial.println("");
  }
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
  
  server.begin(); 
  Watchdog.enable(4000);

  //Control led
  pinMode(6, OUTPUT);
  digitalWrite(6, LOW);
};

void readDHT22(){
  humidity = dht.readHumidity();
  tempdht = dht.readTemperature();
  tempreal = dht.computeHeatIndex(tempdht, humidity, false); // false -> Celsius
};

void readMHZ19() {
  ppm=myMHZ19.getCO2();
  tempmhz = myMHZ19.getTemperature();
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
          client.print(",\"co2\":");
          client.print(ppm);
          client.print(",\"temperaturedht\":");
          client.print(tempdht);
          client.print(",\"wifirssi\":");
          client.print(WiFi.RSSI());
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
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(0, 0);  
  display.println("CO");
  display.setTextSize(1);
  display.setCursor(24, 9);  
  display.println("2");
  display.setTextSize(2);
  display.setCursor(52, 0);  
  if (ppm>900) {display.setTextColor(BLACK, WHITE);};
  display.println(ppm);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setFont();
  display.setCursor(0, 16);  
  display.println("Temp.(C)");
  display.setCursor(52, 16);  
  display.println(tempreal);
  display.setCursor(0, 25);
  display.println("Humidite");
  display.setCursor(52, 25);  
  display.println(humidity);
  display.display();

  if (serialdebug){
    Serial.print("Temp. (C) : ");
    Serial.println(tempreal);
    Serial.print("Humidite : ");
    Serial.println(humidity);
    Serial.print("CO2 : ");
    Serial.println(ppm);    
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

void loop () {
  Watchdog.reset();
  if (millis() - lastReadingTime > delayMS) {
    digitalWrite(6, HIGH);
    readDHT22();
    readMHZ19();
    show_info();
    check_wifi();
    
    lastReadingTime = millis();
    
    digitalWrite(6, LOW);
  };   
     
  listenForWifiClients();
}
