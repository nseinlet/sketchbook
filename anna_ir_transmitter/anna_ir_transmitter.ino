// IR Transmitter ...
#include <IRremote.h>

int val;
unsigned long data;
IRsend irsend;

void setup()
{
  val = 0;
}

void loop() {   
  val = analogRead(2);
  data = ( unsigned long ) val;
  delay(50);
  //data=0xa90;
  irsend.sendSony(data, 12);
  delay(200);
}
