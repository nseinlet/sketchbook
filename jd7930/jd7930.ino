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
  volatile int actualPWM;
  volatile int requiredPWM;
  volatile int pinIn;
  volatile boolean invert;
  volatile unsigned long tempo;
  volatile unsigned long lastMove;
  volatile boolean anglemode;
  volatile int prcRatio;
  Servo outservo;
};
typedef struct serv Serv;
Serv outputs[13] = {0};

void setup() {
  Serial.begin(115200);
  for(uint8_t pin=0; pin<13; pin++){
    outputs[pin].actualPWM=1500;
    outputs[pin].tempo=0;
    outputs[pin].invert=false;
    outputs[pin].pinIn=0;
    outputs[pin].anglemode=false;
    outputs[pin].prcRatio = 100;
  }
  PCintPort::attachInterrupt(2 , &computeInput, CHANGE); // motors
  PCintPort::attachInterrupt(3 , &computeInput, CHANGE); // rear lift
  PCintPort::attachInterrupt(4 , &computeInput, CHANGE); // steering
  
  outputs[5].pinIn=4;
  outputs[5].tempo=30;
  outputs[6].pinIn=2;
  outputs[6].tempo=0;
  outputs[6].prcRatio = 81;
  outputs[9].pinIn=2;
  outputs[9].tempo=0;
  outputs[10].pinIn=3;
  outputs[10].tempo=50;
  outputs[10].anglemode=true;
  outputs[11].pinIn=3;
  outputs[11].tempo=50;
  outputs[11].invert=true;
  outputs[11].anglemode=true;
  
  for(uint8_t pin=0; pin<13; pin++){
    if (outputs[pin].pinIn!=0){
      outputs[pin].outservo.attach(pin);
      outputs[pin].outservo.writeMicroseconds(1500);
    };
    if (outputs[pin].tempo>500){
      outputs[pin].tempo=500;
    };
  };
}

//Read the signal
void computeInput(){
  int pin = PCintPort::arduinoPin;
  if(digitalRead(pin) == HIGH){
    signals[pin].start = micros();
  }else{
    if(signals[pin].start && (signals[pin].isOn == false)){
      if (abs(signals[pin].len - (int)(micros() - signals[pin].start))>20){
        signals[pin].len = (int)(micros() - signals[pin].start);
      };
      signals[pin].start = 0;
      signals[pin].isOn = true;
    } 
  }  
}

int invertPWM(int PWMsignal)
{
  return (-1)*(PWMsignal-1500)+1500;
};

int PWM2deg(int pwmsig)
{
  //Convert PWM signal to an angle in degrees
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
};

void electronicDiff(int steer, int rightMotor, int leftMotor)
{
  //ElectronicalDiff
  int steerAngle;
  float angle;
  steerAngle = invertPWM(outputs[steer].actualPWM); //Steering servo is head down
  angle = (1500-steerAngle)/15.0;

  outputs[leftMotor].actualPWM = outputs[leftMotor].actualPWM + int((outputs[leftMotor].actualPWM-1500.0) * float(angle/100.0));
  outputs[rightMotor].actualPWM = outputs[rightMotor].actualPWM - int((outputs[rightMotor].actualPWM-1500.0) * float(angle/100.0));
};

void loop(){
  float tmpPWM;
  int PWMmax;
  delay(10);
  //Check which values to set on output
  for(uint8_t pin=0; pin<13; pin++){
    if (outputs[pin].pinIn!=0){
      if (signals[outputs[pin].pinIn].isOn){
        tmpPWM = float(signals[outputs[pin].pinIn].len);
        outputs[pin].requiredPWM = 1500 + int((tmpPWM-1500.0) * float(outputs[pin].prcRatio/100.0));
        if (outputs[pin].invert){
          outputs[pin].requiredPWM = invertPWM(outputs[pin].requiredPWM);
        };
        if (! outputs[pin].tempo){
          outputs[pin].actualPWM = outputs[pin].requiredPWM;
        };
      };
    };
  };

  //Compute delayed outputs
  for(uint8_t pin=0; pin<13; pin++){
    if (outputs[pin].pinIn!=0){
      if (outputs[pin].tempo){
          PWMmax = int(((500) / outputs[pin].tempo));
          if (PWMmax>abs(outputs[pin].requiredPWM-outputs[pin].actualPWM)){
             PWMmax = abs(outputs[pin].requiredPWM-outputs[pin].actualPWM);
          };
          if (abs(outputs[pin].requiredPWM-outputs[pin].actualPWM)>18)
          {
            if (outputs[pin].requiredPWM>outputs[pin].actualPWM){
              outputs[pin].actualPWM = outputs[pin].actualPWM + PWMmax;
            } else {
              outputs[pin].actualPWM = outputs[pin].actualPWM - PWMmax;
            };
          };
      };
    };
  };

  electronicDiff(5, 6, 9);
  
  //Set output values
  for(uint8_t pin=0; pin<13; pin++){
    if (outputs[pin].pinIn!=0){
        if (outputs[pin].anglemode){
          outputs[pin].outservo.write(PWM2deg(outputs[pin].actualPWM));
        }else{
          outputs[pin].outservo.writeMicroseconds(outputs[pin].actualPWM);
        };
    };
  };
  
  //Reset values
  for(uint8_t pin=0; pin<13; pin++){
    if(signals[pin].isOn){
      signals[pin].isOn = false;
    }
  }
}

