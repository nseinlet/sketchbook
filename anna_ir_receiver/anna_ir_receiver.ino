// IR Transmitter ...
#include <IRremote.h>
#include <Servo.h>

int receiverPin=9;
int val;
int pot;
int servo_max_angle;
unsigned long data;
decode_results results;
IRrecv irrecv(receiverPin);
Servo myservo[4];
int servo_vals[4];

void setup() {
  Serial.begin(115200);
  irrecv.enableIRIn();
  irrecv.blink13(true);
  myservo[0].attach(6); 
  myservo[1].attach(5);
  myservo[2].attach(4);
  myservo[3].attach(3);
}

void loop() {
  delay(200);
  if (irrecv.decode(&results)) {
    if (results.decode_type == NEC) {
      //Serial.print("NEC: ");
    } else if (results.decode_type == SONY) {
      //Sony is the servo value
      //Serial.print("SONY: ");
      val=( int ) results.value;
    } else if (results.decode_type == RC5) {
      //Serial.print("RC5: ");
    } else if (results.decode_type == RC6) {
      //Serial.print("RC6: ");
    } else if (results.decode_type == UNKNOWN) {
      //Serial.print("UNKNOWN: ");
    }
    //Serial.println(results.value, HEX);
    
    //Serial.println(val);
    delay(200);
    irrecv.resume(); // Receive the next value
    
  }

  //Servo 0 = input
  delay(1);
  servo_vals[0]=map(val, 0, 1023, 0, 180);
  myservo[0].write(map(val, 0, 1023, 0, 180)); 
  //Servo 1 =avg(input, potentiometer)

  for (int i = 1; i < 4; i++) {
      //Potentiometer control max steering
      pot = analogRead(i);
      //Max angle is a map between pot values (0->1023) and servo angle values in a given direction (0->90) 
      servo_max_angle=map(pot, 0, 1023, 0, 90);
      servo_vals[i]=map(val, 0, 1023, servo_max_angle, 180-servo_max_angle);
      myservo[1].write(servo_vals[i]); 
  }
  
  Serial.print(val);
  for (int i = 0; i < 4; i++) {
     Serial.print(" | ");  
     Serial.print(servo_vals[i]);
  }
  Serial.println();
}
