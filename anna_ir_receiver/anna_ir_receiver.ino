#include <Servo.h>
#include <ArduinoBLE.h>

#define pwm_peripheral_name "PWM_Emitter"
#define service_uuid "fd6db617-daf2-4502-823c-3d9ae8915948"
#define pwmchannels_uuid "c9724225-18b5-4e34-81bc-0293b7cbddd4"

int receiverPin=9;
//int val;
int pot;
int pot_trim;
int servo_max_angle;
unsigned long data;
Servo myservo[4];
int servo_ins[4];
int servo_trims[4];
int servo_vals[4];
BLECharacteristic readingchan;
uint32_t val;

void setup() {
  Serial.begin(115200);

  for (int i = 0; i < 4; i++) {
    myservo[i].attach(i+3);
  };
  servo_ins[0]=A0;
  servo_ins[1]=A1;
  servo_ins[2]=A2;
  servo_ins[3]=A3;
  servo_trims[0]=A4;
  servo_trims[1]=A5;
  servo_trims[2]=A6;
  servo_trims[3]=A7;
  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");
    while (1);
  }

  Serial.println("BLE Central - Peripheral Explorer");

  BLE.scanForName(pwm_peripheral_name);
}

void loop() {
  // check if a peripheral has been discovered
  BLEDevice peripheral = BLE.available();

  if (peripheral) {
    // see if peripheral is a PWM emitter
    if (peripheral.localName() == "PWM_Emitter") {
      // stop scanning
      BLE.stopScan();
      // turn on the LED to indicate the connection:
      digitalWrite(LED_BUILTIN, HIGH);

      explorerPeripheral(peripheral);
      digitalWrite(LED_BUILTIN, LOW);
    }
    BLE.scanForName(pwm_peripheral_name);
  }
};

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
      readingchan = pwmService.characteristic(pwmchannels_uuid);
      while (peripheral.connected()) {
        readingchan.readValue(val);
        Serial.println(val);
        move_servos();
      };
    };
    peripheral.disconnect();

  }
  
};

void move_servos(){

  for (int i = 0; i < 4; i++) {
      //Potentiometer control max steering
      pot = analogRead(servo_ins[i]);
      pot_trim = analogRead(servo_trims[i]);
      
      //Max angle is a map between pot values (0->1023) and servo angle values in a given direction (0->90) 
      servo_max_angle=map(pot, 0, 1023, 0, 90);
      servo_vals[i]=map(val+pot_trim-512, 0, 1023, servo_max_angle, 180-servo_max_angle);
      myservo[i].write(servo_vals[i]); 
  }
  
  Serial.print(val);
  for (int i = 0; i < 4; i++) {
     Serial.print(" | ");  
     Serial.print(servo_vals[i]);
  }
  Serial.println();
};
