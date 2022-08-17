int arduinoPort;

void setup() {
  Serial.begin(9600);
  for (arduinoPort = 2; arduinoPort <= 9; arduinoPort++) {
    pinMode(arduinoPort, INPUT_PULLUP);
  }
}

byte byteout=0;

void loop() {
  for (arduinoPort = 2; arduinoPort <= 9; arduinoPort++)
  {
    byteout |= digitalRead(arduinoPort) << (arduinoPort-2);
  }
  Serial.write(byteout);
  byteout = 0;
}
