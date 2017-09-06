#include <FUTABA_SBUS.h>

FUTABA_SBUS sBus;

void setup(){
  sBus.begin();
  Serial.begin(115200);
}

void loop(){
  sBus.FeedLine();
  if (sBus.toChannels == 1){
    sBus.UpdateServos();
    sBus.UpdateChannels();
    sBus.toChannels = 0;
    Serial.println(sBus.channels[0]);
    Serial.println(sBus.channels[1]);
    Serial.println(sBus.channels[2]);
    Serial.println(sBus.channels[3]);
    Serial.println(sBus.channels[4]);
    Serial.println(sBus.channels[5]);
    Serial.println(sBus.channels[6]);
  }
}
