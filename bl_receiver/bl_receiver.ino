/*
  Peripheral Explorer

  This example scans for BLE peripherals until one with a particular name ("LED")
  is found. Then connects, and discovers + prints all the peripheral's attributes.

  The circuit:
  - Arduino MKR WiFi 1010, Arduino Uno WiFi Rev2 board, Arduino Nano 33 IoT,
    Arduino Nano 33 BLE, or Arduino Nano 33 BLE Sense board.

  You can use it with another board that is compatible with this library and the
  Peripherals -> LED example.

  This example code is in the public domain.
*/

#include <ArduinoBLE.h>

#define pwm_peripheral_name "PWM_Emitter"
#define service_uuid "fd6db617-daf2-4502-823c-3d9ae8915948"
#define pwmchannels_uuid "c9724225-18b5-4e34-81bc-0293b7cbddd4"
#define first_pin 2
#define last_pin 6

const char *pwm_channels_uuids[] = {"1f16f2dc-3e06-4217-af6d-e68010f6ced0","3b5baf05-2931-4da2-bc78-b9a141abf658","006dd860-7678-40da-b15e-c7f968017764","230598d7-99e2-46c2-b2b2-839a0cce27d4","e046d73f-6cf3-4b36-a078-d4b14b6e3a22","98901c11-4aaa-4db5-81fc-6db19024287f","81d975dc-44b6-4450-80b5-8cbbbb7f6827","7086616f-7854-4b5e-a84f-eee31ab16426","c682bcf1-0e40-4777-bbaa-ccb23561099c","98e4e078-0ba0-473a-9eef-bfcf46e9d1f4"};

const int MAX_CHANNEL=10;

union ArrayToInteger {
  unsigned int integer;
  byte array[sizeof(integer)];
};

BLECharacteristic tmp_channels_charact[MAX_CHANNEL];
ArrayToInteger pwm_values[MAX_CHANNEL];
unsigned int debug_pwm;

// using a macro to avoid function call overhead
#define WAIT_FOR_PIN_STATE(state) \
  while (digitalRead(pin) != (state)) { \
    if (micros() - timestamp > timeout) { \
      return 0; \
    } \
  }

static unsigned int newPulseIn(const byte pin, const byte state, const unsigned long timeout) {
  unsigned long timestamp = micros();
  WAIT_FOR_PIN_STATE(!state);
  WAIT_FOR_PIN_STATE(state);
  timestamp = micros();
  WAIT_FOR_PIN_STATE(!state);
  return (unsigned int)(micros() - timestamp);
}

void read_pwm(){
//  for(int i=first_pin;i<=last_pin;i++){
//    pwm_value[i] = newPulseIn(i, HIGH, default_timeout);
//  }
  //Debug mode
  for(int i=first_pin;i<=last_pin;i++){
    debug_pwm=debug_pwm+10;
    if(debug_pwm>1900){debug_pwm=1100;};
    pwm_values[i].integer=debug_pwm;
    Serial.println(tmp_channels_charact[i].uuid());
    tmp_channels_charact[i].writeValue(pwm_values[i].array, sizeof(pwm_values[i].integer));
  }
}


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
  debug_pwm=1500;
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
      exploreService(pwmService);
    };

  }
  while (peripheral.connected()){
    read_pwm();
  }
};

void exploreService(BLEService service) {
  // print the UUID of the service
  Serial.print("Service ");
  Serial.println(service.uuid());

  for (int i = 0; i < min(MAX_CHANNEL, service.characteristicCount()); i++) {
    tmp_channels_charact[i] = service.characteristic(pwm_channels_uuids[i]);
    Serial.println(tmp_channels_charact[i].uuid());
  };
  
}
  
