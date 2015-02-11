#include <PinChangeInt.h>
// Detecting PWM signal's in arduino
struct sig {
  volatile boolean isOn;        // signal LOW->HIGH detected
  volatile unsigned long start; // time in microseconds when "isOn" was detected
  volatile int len;             // the length of the signal on HIGH (NOW-start)
};
typedef struct sig Sig;
Sig signals[20] = {0};

void setup() {
  Serial.begin(115200);
  PCintPort::attachInterrupt(3, &calcInput, CHANGE); // throttle (1ms)->2ms
  PCintPort::attachInterrupt(4, &calcInput, CHANGE); // ailerons 1ms<-(1.5ms)->2ms
  PCintPort::attachInterrupt(5, &calcInput, CHANGE); // elevator 1ms<-(1.5ms)->2ms
  PCintPort::attachInterrupt(2, &calcInput, CHANGE); // rudder   1ms<-(1.5ms)->2ms
}

void loop(){
  delay(1000);
  for(uint8_t pin=0; pin<20; pin++){
    if(signals[pin].isOn){
      printStruct(pin);
      signals[pin].isOn = false;
    }
  }
  Serial.println();
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

void printStruct(int pin){
    Serial.print(pin);
    Serial.print(":");
    Serial.print(signals[pin].isOn);
    Serial.print(":");
    Serial.print(signals[pin].start);
    Serial.print(":");
    Serial.print(signals[pin].len);
    Serial.print(" ");
}
