#include <FUTABA_SBUS.h>
#include <Servo.h>

FUTABA_SBUS sBus;
Servo myservo[6];
int PWMMin = 1500-768;
int PWMMax = 1500+768;
int AngleMin = 10;
int AngleMax = 180;

void setup(){
  sBus.begin();
  myservo[0].attach(3);
  myservo[1].attach(5);
  myservo[2].attach(6);
  myservo[3].attach(9);
  myservo[4].attach(10);
  myservo[5].attach(11);
}

void loop(){
  delay(300);
  sBus.FeedLine();
  if (sBus.toChannels == 1){
    sBus.UpdateServos();
    sBus.UpdateChannels();
    sBus.toChannels = 0;

    myservo[0].write(map(sBus.channels[0], PWMMin, PWMMax, AngleMin, AngleMax));  
    myservo[1].write(map(sBus.channels[0], PWMMin, PWMMax, AngleMin, AngleMax));
    myservo[2].write(map(sBus.channels[3], PWMMin, PWMMax, AngleMin, AngleMax));
    myservo[3].write(map(sBus.channels[12], PWMMin, PWMMax, AngleMin, AngleMax));
  }
}
