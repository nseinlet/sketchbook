#include <SPI.h>
#include <WiFiNINA.h>
#include <Adafruit_SleepyDog.h>
#include "consts.h"
#include <WiFiUdp.h>
#include <DHT.h>

//NTP
unsigned int localPort = 2390;      // local port to listen for UDP packets
IPAddress timeServer(172, 16, 17, 253); // time.nist.gov NTP server
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
WiFiUDP Udp;
uint32_t udpDelayMS;
long lastUdpReadingTime = 0;
int hour,minutes,seconds;

//Web server and WiFi
int ledpin = 6;
int status = WL_IDLE_STATUS;
WiFiServer server(80);
IPAddress ip;
long rssi;
uint32_t delayMS;
long lastReadingTime = 0;

//Temp
#define DHTin 2
DHT dht(DHTin, DHT22);
float humidity, tempdht, tempreal;

//Motor
int motorSpeed, saltState;
#define SaltIn 1
#define SaltSpeed 80
#define MinSpeed 20

void setup () {
  Serial.begin(115200);
  Serial.println("Starting");
  delayMS=30000;
  udpDelayMS=3600000;

  Serial.print("WiFi101 shield: ");
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("NOT PRESENT");
    //return; // don't continue
  }
  Serial.println("DETECTED");
  check_wifi();
  updateTime();

  Watchdog.enable(4000);

  // DHT22 init
  dht.begin();
  readDHT22();

  //Salt
  pinMode(SaltIn, INPUT);
  motorSpeed=0;

  //Control led
  pinMode(ledpin, OUTPUT);
  digitalWrite(ledpin, LOW);
  Serial.println("Init Finished");
};

void loop () {
  Watchdog.reset();
  if (millis() - lastReadingTime > delayMS) {
    digitalWrite(6, HIGH);
    check_wifi();
    readDHT22();
    lastReadingTime = millis();
    digitalWrite(6, LOW);
  }; 
  if (millis() - lastUdpReadingTime > udpDelayMS) {
    updateTime();
    lastUdpReadingTime = millis();
  };
  listenForWifiClients();
  driveMotor();
}

void listenForWifiClients() {
  // listen for incoming clients
  WiFiClient client = server.available();
  if (client) {
    // an http request ends with a blank line
    bool currentLineIsBlank = true;
    while (client.connected()) {
      //Serial.println("Connected to client");
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
          client.print("{\"lastntptime\": \"");
          client.print(hour);
          client.print(":");
          client.print(minutes);
          client.print(":");
          client.print(seconds);
          client.print("\"");
          client.print(",\"wifirssi\": ");
          client.print(rssi);
          client.print(",\"temperature\": ");
          client.print(tempdht);
          client.print(",\"temperaturereal\": ");
          client.print(tempreal);
          client.print(",\"humidity\": ");
          client.print(humidity);
          client.print(",\"motor\": ");
          client.print(motorSpeed);
          client.print(",\"salt\": ");
          client.print(saltState);
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
  };
};

void check_wifi() {
  status = WiFi.status();
  Serial.println("Checking WiFi");
  
  if(status != WL_CONNECTED)
  {
    Serial.println("Not connected");
    Serial.println(status);
    int retries=0;
    while((status != WL_CONNECTED) && (retries<3))
    {
      WiFi.begin(ssid, pass); 
      retries++;
      Serial.println("\n Trying to Reconnect to WiFi Network ");
      delay(2000);
      status = WiFi.status();
      Serial.println(status);
      server.begin(); 
      Udp.begin(localPort);
    }
  }
  else
  {
    ip = WiFi.localIP();
    /*Serial.print("IP Address: ");
    Serial.println(ip);

    Serial.println();
    Serial.println("Network Information:");
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());*/

    // print the received signal strength:
    rssi = WiFi.RSSI();
    /*Serial.print("signal strength (RSSI):");
    Serial.println(rssi);*/
  }
  
};

void updateTime() {
  Serial.println("Try NTP");
  sendNTPpacket(timeServer); // send an NTP packet to a time server
  // wait to see if a reply is available
  delay(1000);
  if (Udp.parsePacket()) {
    Serial.println("packet received");
    // We've received a packet, read the data from it
    Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, extract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    Serial.print("Seconds since Jan 1 1900 = ");
    Serial.println(secsSince1900);

    // now convert NTP time into everyday time:
    Serial.print("Unix time = ");
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years and add 2 hours
    unsigned long epoch = secsSince1900 - seventyYears + 7200;
    hour=(epoch % 86400L) / 3600;
    minutes=(epoch % 3600) / 60;
    seconds=epoch % 60;

    // print the hour, minute and second:
    Serial.print("The UTC time is ");       // UTC is the time at Greenwich Meridian (GMT)
    Serial.print(hour);
    Serial.print(":");
    Serial.print(minutes);
    Serial.print(":");
    Serial.println(seconds);
  }
}

// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress& address) {
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

void readDHT22(){
  humidity = dht.readHumidity();
  tempdht = dht.readTemperature();
  tempreal = dht.computeHeatIndex(tempdht, humidity, false); // false -> Celsius
};

void driveMotor() {
  if (hour<10 or hour>18 or tempreal<18.0) {
    //Motor is stopped
    motorSpeed=0;
  };
  saltState = digitalRead(SaltIn);
  if (saltState == HIGH) {
    //Motor is running at salt speed
    motorSpeed=SaltSpeed;
  }
  else 
  {
    if (motorSpeed==0 and tempreal>19.5) {
      //Motor is stopped, and should start
      motorSpeed=MinSpeed;
    };
    if (motorSpeed>0)
    {
      //Motor is running
      //Speed is temperature dependent
      motorSpeed=MinSpeed+((tempreal-18))*10;
    };
  };
};