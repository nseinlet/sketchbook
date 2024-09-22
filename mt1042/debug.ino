int debug = 0;

void debugSetup() {
  if (debug>0){
    Serial.begin(115200);
  };
}

void debugLoop() {
  if (debug==1){
    for(int i=0;i<24;i++){
      Serial.print(rec.channels[i].angle);
      Serial.print('\t');
    };
    Serial.println();
  }
}
