bool debug = true;
int incomingByte = 0;
byte res[65];

void debugSetup() {
  if (debug){
    Serial.begin(115200);
    //Serial1.begin(100000);
  };
}

void debugLoop() {
  if (debug){
    for(int i=0;i<16;i++){
      Serial.print(rec.channels[i].angle);
      Serial.print('\t');
    };
    Serial.println();

    //if (Serial1.available() > 0) {
    //  // read the incoming byte:
    //  incomingByte = Serial1.read();
    //  // say what you got:
    //  Serial.print("I received: ");
    //  Serial.println(incomingByte, DEC);
    //}

    //Serial1.readBytes(res, 25);
    //for(int i=0;i<25;i++) Serial.print(res[i]);
    //Serial.println();

  }
}
