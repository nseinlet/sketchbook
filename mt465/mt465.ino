/***********************************************
 * FrSky Sbus decoder, light management        *
 * and screen display                          *
 *                                             *
 * Read channels values on SBus port (rx port) *
 * Manage ligths based on 1 channel            *
 * Patterns are 1,2 or 3 times up or down      *
 * Display them on a 1inch display             *
 ***********************************************/

#include <Modelisme.h>
#include <Servo.h>
  
Receiver rec;
LightManager lm;

Servo myservo[6];

//Display management
int currentTime;
int displayTime;
int nbrdisp=0;
int displayCounter=0;

void setup(){
  rec.setup();
  lm.setup(2, 3, 4, 5, 6, 7, 13);
  myservo[0].attach(8);
  myservo[1].attach(9);
  myservo[2].attach(10);
  myservo[3].attach(11);
  myservo[4].attach(12);
}

void loop(){
  delay(75);
  if (rec.read()==1){
    myservo[0].write(rec.channels[4].angle);  
    myservo[1].write(rec.channels[5].angle);
    myservo[2].write(rec.channels[6].angle);
    myservo[3].write(rec.channels[7].angle);
    myservo[3].write(rec.channels[8].angle);
  };
  
  //Manage ligths
  lm.checkLights(rec.channels[12].angle, rec.channels[1].angle, rec.channels[4].angle);

}


