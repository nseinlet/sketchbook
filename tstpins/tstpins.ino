#include <PinChangeInt.h>
 
#define MY_PIN 2 // we could choose any pin
#define MY_PIN2 4 // we could choose any pin

volatile int pwm_value[12];
volatile int prev_time[12];
uint8_t latest_interrupted_pin;
 
void rising()
{
  latest_interrupted_pin=PCintPort::arduinoPin;
  PCintPort::attachInterrupt(latest_interrupted_pin, &falling, FALLING);
  prev_time[latest_interrupted_pin] = micros();
}
 
void falling() {
  latest_interrupted_pin=PCintPort::arduinoPin;
  PCintPort::attachInterrupt(latest_interrupted_pin, &rising, RISING);
  pwm_value[latest_interrupted_pin] = micros()-prev_time[latest_interrupted_pin];
  
}
 
void setup() {
  pinMode(MY_PIN, INPUT); digitalWrite(MY_PIN, HIGH);
  pinMode(MY_PIN2, INPUT); digitalWrite(MY_PIN2, HIGH);
  Serial.begin(115200);
  PCintPort::attachInterrupt(MY_PIN, &rising, RISING);
  PCintPort::attachInterrupt(MY_PIN2, &rising, RISING);
}
 
void loop() { 
delayMicroseconds(5000000);
Serial.print(pwm_value[2]);
Serial.print("|");
Serial.print(pwm_value[3]);
Serial.println("------------");
}
