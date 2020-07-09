/*
  Read PWM from BLE and write them to servos

  The circuit:
  - Arduino Nano 33 BLE, or Arduino Nano 33 BLE Sense board.
*/

#include <ArduinoBLE.h>
#include <Servo.h>

#define pwm_peripheral_name "PWM_Emitter"
#define service_uuid "fd6db617-daf2-4502-823c-3d9ae8915948"
#define pwmchannels_uuid "c9724225-18b5-4e34-81bc-0293b7cbddd4"
#define first_pin 2

const char *pwm_channels_uuids[] = {"1f16f2dc-3e06-4217-af6d-e68010f6ced0","3b5baf05-2931-4da2-bc78-b9a141abf658","006dd860-7678-40da-b15e-c7f968017764","230598d7-99e2-46c2-b2b2-839a0cce27d4","e046d73f-6cf3-4b36-a078-d4b14b6e3a22","98901c11-4aaa-4db5-81fc-6db19024287f","81d975dc-44b6-4450-80b5-8cbbbb7f6827","7086616f-7854-4b5e-a84f-eee31ab16426","c682bcf1-0e40-4777-bbaa-ccb23561099c","98e4e078-0ba0-473a-9eef-bfcf46e9d1f4"};

const int MAX_CHANNEL=10;
unsigned int nbr_channels;
BLECharacteristic nbr_channels_charact;
BLECharacteristic tmp_channels_charact[MAX_CHANNEL];
unsigned int pwm_values[MAX_CHANNEL];
Servo myservo[MAX_CHANNEL];

union ArrayToInteger {
  byte array[4];
  unsigned int integer;
};

unsigned int byteArrayToInt(const byte data[], int length) {
  ArrayToInteger converter;
  
  for (int i = 0; i < min(length,4); i++) {
        converter.array[i] = data[i];   
  };
  return converter.integer ;
};

void setup() {
  Serial.begin(115200);
  while (!Serial);

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");
    while (1);
  }

  Serial.println("BLE Central - Peripheral Explorer");

  BLE.scanForName(pwm_peripheral_name);

  for (int i=0;i<MAX_CHANNEL;i++){
    myservo[i].attach(first_pin+i);
  }
}

void loop() {
  // check if a peripheral has been discovered
  BLEDevice peripheral = BLE.available();

  if (peripheral) {
    // see if peripheral is a PWM emitter
    if (peripheral.localName() == "PWM_Emitter") {
      // stop scanning
      BLE.stopScan();

      explorerPeripheral(peripheral);
    }
    BLE.scanForName(pwm_peripheral_name);
  }
}

void explorerPeripheral(BLEDevice peripheral) {
  // connect to the peripheral
  Serial.println("Connecting ...");

  if (peripheral.connect()) {
    Serial.println("Connected");
  } else {
    Serial.println("Failed to connect!");
    return;
  }
  if (peripheral.connected()) {
    // discover peripheral attributes
    Serial.println("Discovering attributes ...");
    if (peripheral.discoverAttributes()) {
      Serial.println("Attributes discovered");
    } else {
      Serial.println("Attribute discovery failed!");
      peripheral.disconnect();
      return;
    }

    BLEService pwmService = peripheral.service(service_uuid);
    if (pwmService) {
      nbr_channels=0;
      exploreService(pwmService);
    };
    Serial.println();
  };
  while (peripheral.connected()){readValues();delay(1);};
  
};

void exploreService(BLEService service) {
  // print the UUID of the service
  Serial.print("Service ");
  Serial.println(service.uuid());
  if(nbr_channels==0){
    nbr_channels_charact = service.characteristic(pwmchannels_uuid);
    if(nbr_channels_charact){
      nbr_channels_charact.read();
      nbr_channels = byteArrayToInt(nbr_channels_charact.value(), nbr_channels_charact.valueLength());
    };
    Serial.print("Channels :");
    Serial.println(nbr_channels);
  };
  
  for (int i = 0; i < min((int)nbr_channels, MAX_CHANNEL); i++) {
    tmp_channels_charact[i] = service.characteristic(pwm_channels_uuids[i]);
    readValues();
  }
}

void readValues() {
  for (int i = 0; i < min((int)nbr_channels, MAX_CHANNEL); i++) {
    tmp_channels_charact[i].read();
    pwm_values[i] = byteArrayToInt(tmp_channels_charact[i].value(), tmp_channels_charact[i].valueLength());
    
    Serial.print(i);
    Serial.print(" : ");
    Serial.println(pwm_values[i]);
    myservo[i].write(map(pwm_values[i], 1100, 1900, 45, 135));
  }
};

  
