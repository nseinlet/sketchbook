/*Copyright (C) 2015  Seinlet Nicolas

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>*/

#include <PinChangeInt.h>

struct sig {
  volatile boolean isOn;
  volatile unsigned long start;
  volatile int len;
};
typedef struct sig Sig;
Sig signals[13] = {0};

struct serv {
  volatile int angleactu;
  volatile int anglerequis;
  volatile int pinIn;
  volatile boolean invert;
  volatile unsigned long tempo;
  volatile unsigned long lastMove;
};
typedef struct serv Serv;
Serv sorties[13] = {0};

void setup() {
  Serial.begin(115200);
  for(uint8_t pin=0; pin<13; pin++){
    sorties[pin].angleactu=90;
    sorties[pin].tempo=0;
    sorties[pin].invert=false;
    sorties[pin].pinIn=0;
  }
  PCintPort::attachInterrupt(2, &calcInput, CHANGE); // retourner
  PCintPort::attachInterrupt(3, &calcInput, CHANGE); // position
  PCintPort::attachInterrupt(4, &calcInput, CHANGE); // packomat
  sorties[6].pinIn=2;
  sorties[7].pinIn=2;
  sorties[7].invert=true;
  sorties[8].pinIn=2;
  sorties[8].tempo=500;
}

void calcInput(){
  int pin = PCintPort::arduinoPin;
  if(digitalRead(pin) == HIGH){
    signals[pin].start = micros();
  }else{
    if(signals[pin].start && (signals[pin].isOn == false)){
      signals[pin].len = (int)(micros() - signals[pin].start);
      signals[pin].start = 0;
      signals[pin].isOn = true;
    } 
  }  
}

int PWM2deg(int pwmsig)
{
  //Convertir les signaux PWM en angles
  //1000/1500/2000
  //->
  //0/90/180
  float tmp = pwmsig-1000.0;
  if (tmp<0){
    tmp=0;
  };
  if (tmp>1000){
    tmp=1000;
  };
  int res = (tmp/1000.0)*180.0;
  return res;
}

void loop(){
  delay(1000);
  anglesrequis[0] = PWM2deg(signals[2].len);
  anglesrequis[1] = PWM2deg(signals[3].len);
  anglesrequis[2] = PWM2deg(signals[4].len);
  
  for(uint8_t pin=0; pin<3; pin++){
    if(anglesactu[pin]<anglesrequis[pin]){
      anglesactu[pin]++;
    };
    if(anglesactu[pin]>anglesrequis[pin]){
      anglesactu[pin]--;
    };
  }
  
  //Reset values
  for(uint8_t pin=0; pin<5; pin++){
    if(signals[pin].isOn){
      signals[pin].isOn = false;
    }
  }
}

