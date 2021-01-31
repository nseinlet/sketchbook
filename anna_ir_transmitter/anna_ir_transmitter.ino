#include <ArduinoBLE.h>

int val;
int last_val;
unsigned long data;

//Bluetooth part
BLEService PWMService("fd6db617-daf2-4502-823c-3d9ae8915948");  // BLE PWM service
BLEIntCharacteristic PWMchannelsCharacteristic("c9724225-18b5-4e34-81bc-0293b7cbddd4", BLERead | BLENotify);

void setup()
{
  val = 0;
  last_val = 0;
  Serial.begin(115200);

    //Start bluetooth emitter
  if (!BLE.begin()) {   // initialize BLE
    Serial.println("starting BLE failed!");
    while (1);
  }

  BLE.setLocalName("PWM_Emitter");  // Set name for connection
  BLE.setAdvertisedService(PWMService); // Advertise service
  PWMService.addCharacteristic(PWMchannelsCharacteristic); // Add characteristic to service
  BLE.addService(PWMService); // Add service
  PWMchannelsCharacteristic.setValue(512);

  BLE.advertise();  // Start advertising
  Serial.print("Peripheral device MAC: ");
  Serial.println(BLE.address());
  Serial.println("Waiting for connections...");
}

void loop() {   
  read_input();
  
  Serial.println(val);
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

    while (central.connected()){
      read_input();
    };
    // when the central disconnects, turn off the LED:
    digitalWrite(LED_BUILTIN, LOW);
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}

void read_input() {
  //A2 is the trailer angle.
  //A3 is the trim
  val = analogRead(A2)+analogRead(A3)-512;
  Serial.println(val);
  if (last_val!=val){
    PWMchannelsCharacteristic.writeValue(val);
    last_val=val;
  };
};
