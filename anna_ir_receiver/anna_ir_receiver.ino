// IR Transmitter ...
#include <IRremote.h>
#include <Servo.h>

int receiverPin=9;
int val;
unsigned long data;
decode_results results;
IRrecv irrecv(receiverPin);
Servo myservo;

void setup() {
  Serial.begin(9600);
  irrecv.enableIRIn();
  irrecv.blink13(true);
  myservo.attach(6); 
}

void loop() {
  delay(200);
  if (irrecv.decode(&results)) {
    if (results.decode_type == NEC) {
      Serial.print("NEC: ");
    } else if (results.decode_type == SONY) {
      //Sony is the servo value
      Serial.print("SONY: ");
      val=( int ) results.value;
    } else if (results.decode_type == RC5) {
      Serial.print("RC5: ");
    } else if (results.decode_type == RC6) {
      Serial.print("RC6: ");
    } else if (results.decode_type == UNKNOWN) {
      Serial.print("UNKNOWN: ");
    }
    //Serial.println(results.value, HEX);
    
    Serial.println(val);
    delay(200);
    irrecv.resume(); // Receive the next value
    
  }
  delay(1);
  myservo.write(map(val, 0, 1023, 0, 180)); 

}
