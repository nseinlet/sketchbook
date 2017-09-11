#include <FUTABA_SBUS.h>
#include <SoftwareSerial.h>

FUTABA_SBUS sBus;
SoftwareSerial * port = new SoftwareSerial(10,11);
int counter=0;

void setup(){
  sBus.begin(port);
  Serial.begin(115200);
}

void loop(){
  
  delay(300);
  sBus.FeedLine();
  if (sBus.toChannels == 1){
    counter++;
    sBus.UpdateServos();
    sBus.UpdateChannels();
    sBus.toChannels = 0;
    if (counter==4) {
      Serial.print(sBus.channels[0]);
      Serial.print("  ");
      Serial.print(sBus.channels[1]);
      Serial.print("  ");
      Serial.print(sBus.channels[2]);
      Serial.print("  ");
      Serial.print(sBus.channels[3]);
      Serial.print("  ");
      Serial.print(sBus.channels[4]);
      Serial.print("  ");
      Serial.print(sBus.channels[5]);
      Serial.print("  ");
      Serial.print(sBus.channels[6]);
      Serial.print("  ");
      Serial.print(sBus.channels[7]);
      Serial.print("  ");
//      Serial.print(sBus.channels[8]);
//      Serial.print("  ");
//      Serial.print(sBus.channels[9]);
//      Serial.print("  ");
//      Serial.print(sBus.channels[10]);
//      Serial.print("  ");
//      Serial.print(sBus.channels[11]);
//      Serial.print("  ");
//      Serial.print(sBus.channels[12]);
//      Serial.print("  ");
//      Serial.print(sBus.channels[13]);
//      Serial.print("  ");
//      Serial.print(sBus.channels[14]);
//      Serial.print("  ");
//      Serial.print(sBus.channels[15]);
      Serial.println("");
      counter=0;
    };
  }
}
