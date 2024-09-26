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
  if (debug==2){
    //Serial.print(rec.get_sBus_failsafe_status());
    Serial.print('\t');
    Serial.print(SBUS_SIGNAL_OK);
    Serial.print('\t');
    Serial.print(SBUS_SIGNAL_LOST);
    Serial.print('\t');
    Serial.print(SBUS_SIGNAL_FAILSAFE);
    Serial.print('\t');
    Serial.print(SBUS_NO_SIGNAL);
    Serial.println();
  }
}
