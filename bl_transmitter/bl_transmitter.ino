/*
  Read PWM from receiver
  and write them on BLE services that can be read

  The circuit:
  - Arduino Nano 33 BLE, or Arduino Nano 33 BLE Sense board.
*/

#include <ArduinoBLE.h>

#define default_timeout 10000
#define first_pin 2
#define last_pin 6

unsigned int debug_pwm;
unsigned int pwm_value[last_pin+1];
int increment;

//Bluetooth part
BLEService PWMService("fd6db617-daf2-4502-823c-3d9ae8915948");  // BLE PWM service
BLEUnsignedIntCharacteristic PWMchannelsCharacteristic("c9724225-18b5-4e34-81bc-0293b7cbddd4", BLERead | BLENotify);
BLEUnsignedIntCharacteristic pwm_channels[]= {
  BLEUnsignedIntCharacteristic("1f16f2dc-3e06-4217-af6d-e68010f6ced0", BLERead | BLENotify),
  BLEUnsignedIntCharacteristic("3b5baf05-2931-4da2-bc78-b9a141abf658", BLERead | BLENotify),
  BLEUnsignedIntCharacteristic("006dd860-7678-40da-b15e-c7f968017764", BLERead | BLENotify),
  BLEUnsignedIntCharacteristic("230598d7-99e2-46c2-b2b2-839a0cce27d4", BLERead | BLENotify),
  BLEUnsignedIntCharacteristic("e046d73f-6cf3-4b36-a078-d4b14b6e3a22", BLERead | BLENotify),
  BLEUnsignedIntCharacteristic("98901c11-4aaa-4db5-81fc-6db19024287f", BLERead | BLENotify),
  BLEUnsignedIntCharacteristic("81d975dc-44b6-4450-80b5-8cbbbb7f6827", BLERead | BLENotify),
  BLEUnsignedIntCharacteristic("7086616f-7854-4b5e-a84f-eee31ab16426", BLERead | BLENotify),
  BLEUnsignedIntCharacteristic("c682bcf1-0e40-4777-bbaa-ccb23561099c", BLERead | BLENotify),
  BLEUnsignedIntCharacteristic("98e4e078-0ba0-473a-9eef-bfcf46e9d1f4", BLERead | BLENotify)
};

void setup() {
  Serial.begin(115200);

  //Set pins in read pwm mode
  for(int i=first_pin;i<=last_pin;i++){
    pinMode(i, INPUT);
  }

  //Start bluetooth emitter
  if (!BLE.begin()) {   // initialize BLE
    Serial.println("starting BLE failed!");
    while (1);
  }

  BLE.setLocalName("PWM_Emitter");  // Set name for connection
  BLE.setAdvertisedService(PWMService); // Advertise service
  PWMService.addCharacteristic(PWMchannelsCharacteristic); // Add characteristic to service
  PWMchannelsCharacteristic.setValue(1+last_pin-first_pin); // # of channels
  for(int i=0; i<=last_pin-first_pin; i++){
    PWMService.addCharacteristic(pwm_channels[i]);
    pwm_channels[i].setValue(1500);
  };

  BLE.addService(PWMService); // Add service

  BLE.advertise();  // Start advertising
  Serial.print("Peripheral device MAC: ");
  Serial.println(BLE.address());
  Serial.println("Waiting for connections...");
  debug_pwm=1500;
  increment=10;
}

// function prototype to define default timeout value
static unsigned int newPulseIn(const byte pin, const byte state, const unsigned long timeout = 1000000L);

// using a macro to avoid function call overhead
#define WAIT_FOR_PIN_STATE(state) while (digitalRead(pin) != (state));

static unsigned int newPulseIn(const byte pin, const byte state, const unsigned long timeout) {
  unsigned long timestamp = micros();
  WAIT_FOR_PIN_STATE(!state);
  WAIT_FOR_PIN_STATE(state);
  timestamp = micros();
  WAIT_FOR_PIN_STATE(!state);
  return (unsigned int)(micros() - timestamp);
}

void read_pwm(){
  for(int i=first_pin;i<=last_pin;i++){
    pwm_value[i] = newPulseIn(i, HIGH, default_timeout);
    Serial.println(pwm_value[i]);
    delay(1);
  }
  //Debug mode
//  for(int i=first_pin;i<=last_pin;i++){
//    debug_pwm=debug_pwm+increment;
//  if (debug_pwm>1900){increment=-10;};
//  if (debug_pwm<1100){increment=10;};
//    pwm_value[i] = debug_pwm;
//  }

  //Set values to channels
  for(int i=0; i<=last_pin-first_pin; i++){
    pwm_channels[i].setValue(pwm_value[i+first_pin]);
  }
}

void loop(){
  Serial.print("Peripheral device MAC: ");
  Serial.println(BLE.address());

  // listen for BLE peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to the peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's BT address:
    Serial.println(central.address());
    // turn on the LED to indicate the connection:
    digitalWrite(LED_BUILTIN, HIGH);

    // check the battery level every 200ms
    // while the central is connected:
    while (central.connected()){read_pwm();};
    // when the central disconnects, turn off the LED:
    digitalWrite(LED_BUILTIN, LOW);
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }

  //Read PWM values and store them in ble service
  read_pwm();
  
};
