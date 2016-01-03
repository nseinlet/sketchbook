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
#include <Servo.h> 

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
  Servo outservo;
};
typedef struct serv Serv;
Serv sorties[13] = {0};

void setup() {
  for(uint8_t pin=0; pin<13; pin++){
    sorties[pin].angleactu=90;
    sorties[pin].tempo=0;
    sorties[pin].invert=false;
    sorties[pin].pinIn=0;
  }
  PCintPort::attachInterrupt(2, &calcInput, CHANGE); // retourner
  PCintPort::attachInterrupt(3, &calcInput, CHANGE); // roues

  sorties[7].pinIn=3;
  sorties[7].tempo=450;
  sorties[7].invert=true;
  sorties[8].pinIn=3;
  sorties[8].tempo=450;
  sorties[9].pinIn=2;
  sorties[9].tempo=900;
  
  for(uint8_t pin=0; pin<13; pin++){
    if (sorties[pin].pinIn!=0){
      sorties[pin].outservo.attach(pin);
      sorties[pin].outservo.write(90);
    };
    if (sorties[pin].tempo>900){
      sorties[pin].tempo=900;
    };
  };
}

//Read the signal
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
  //1100/1500/1900
  //->
  //0/90/180
  float tmp = pwmsig-1100.0;
  if (tmp<0){
    tmp=0;
  };
  if (tmp>800){
    tmp=800;
  };
  int res = (tmp/800.0)*180.0;
  return res;
};

int invertangle(int ang)
{
  return (ang-180)*(-1);
};

void loop(){
  int anglemax;
  delay(10);
  //Check which values to set on output
  for(uint8_t pin=0; pin<13; pin++){
    if (sorties[pin].pinIn!=0){
      if (signals[sorties[pin].pinIn].isOn){
        sorties[pin].anglerequis = PWM2deg(signals[sorties[pin].pinIn].len);
        if (sorties[pin].invert){
          sorties[pin].anglerequis = invertangle(sorties[pin].anglerequis);
        };
        if (! sorties[pin].tempo){
          sorties[pin].angleactu = sorties[pin].anglerequis;
        };
      };
    };
  };

  //Calc delaied outputs
  for(uint8_t pin=0; pin<13; pin++){
    if (sorties[pin].pinIn!=0){
      if (sorties[pin].tempo){
          anglemax = int(((180 * 5) / sorties[pin].tempo));
          if (anglemax>abs(sorties[pin].anglerequis-sorties[pin].angleactu)){
             anglemax = abs(sorties[pin].anglerequis-sorties[pin].angleactu);
          };
          if (sorties[pin].anglerequis>sorties[pin].angleactu){
            sorties[pin].angleactu = sorties[pin].angleactu + anglemax;
          } else {
            sorties[pin].angleactu = sorties[pin].angleactu - anglemax;
          };
      };
    };
  };
  
  //Set output values
  for(uint8_t pin=0; pin<13; pin++){
    if (sorties[pin].pinIn!=0){
        sorties[pin].outservo.write(sorties[pin].angleactu);
    };
  };
  //Reset values
  for(uint8_t pin=0; pin<13; pin++){
    if(signals[pin].isOn){
      signals[pin].isOn = false;
    }
  }
}

