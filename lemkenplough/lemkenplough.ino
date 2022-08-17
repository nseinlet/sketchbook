
/*Copyright (C) 2015  Seinlet Nicolas

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>*/

#include "Wire.h"
#include <MPU6050_light.h>
#define EI_ARDUINO_INTERRUPTED_PIN
#include <EnableInterrupt.h>
#include <Servo.h>

#define SerialDebug false
#define PWM_FIRST_PIN 9
#define PWM_NUMBER_OF_PINS 4
#define PWM_AVG_VALUES 50
#define PWM_MIN 800
#define PWM_MAX 2200

#define POS_INPUT 0
#define RIGHT_POS 1
#define LEFT_POS 2
#define WHEEL_POS 3

#define GY512_AVG_VALUES 10
#define FRONT_REAR_ANGLE 1
#define RIGHT_LEFT_ANGLE 2
#define MIN_ADJUSTMENT_ANGLE 3
#define ADJUSTMENT_SPEED 20
#define MIN_WHEEL_ADJUSTMENT 3
#define WHEEL_SERVO_MIN 45
#define WHEEL_SERVO_MAX 135
#define MOTOR_SPEED_MIN 45
#define MOTOR_SPEED_MAX 135

#define WHEEL_PIN 5
#define WHEEL_PIN_INVERTED true
#define MOTOR_PIN 6
#define MOTOR_PIN_INVERTED true

MPU6050 mpu(Wire);
float Angles[3], AngHist[3][GY512_AVG_VALUES];
double sumAngle;
int AngLastInput;
float requiredLeftAngle, requiredRightAngle;

volatile unsigned long pwm_prev_time[PWM_NUMBER_OF_PINS];
volatile int pwm_last_input[PWM_NUMBER_OF_PINS];
int pwm_timings[PWM_NUMBER_OF_PINS][PWM_AVG_VALUES];
int pwm_inputs[PWM_NUMBER_OF_PINS];

int wheel_position, motor_speed;
Servo wheel_servo, motor_controller;

void gy_512_setup() {
  Wire.begin();

  byte status = mpu.begin();
  if (SerialDebug) {
    Serial.print(F("MPU6050 status: "));
    Serial.println(status);
  };
  AngLastInput = 0;
  if (SerialDebug) {Serial.println(F("While calculating the offsets value, do not move the MPU6050 sensor"));};
  delay(1000);
  mpu.calcOffsets();
  if (SerialDebug) {Serial.println("Done!\n");};
}

void gy_512_read() {
  mpu.update();
  AngHist[0][AngLastInput] = mpu.getAngleX();
  AngHist[1][AngLastInput] = mpu.getAngleY();
  AngHist[2][AngLastInput] = mpu.getAngleZ();

  if (SerialDebug) {
      Serial.print("aX = "); Serial.print((AngHist[0][AngLastInput]));Serial.print("90\xc2\xb0");
      Serial.print("\t aY = "); Serial.print((AngHist[1][AngLastInput]));Serial.print("90\xc2\xb0");
      Serial.print("\t aZ = "); Serial.print((AngHist[2][AngLastInput]));Serial.print("90\xc2\xb0");
      Serial.println();
  };
  AngLastInput++;
  if (AngLastInput>=GY512_AVG_VALUES) {
    AngLastInput = 0;
  }

  for (int ang=0;ang<3;ang++){
      sumAngle = 0;
      for (int i=0;i<GY512_AVG_VALUES;i++) {
        sumAngle += AngHist[ang][i];
      }
      Angles[ang] = sumAngle/GY512_AVG_VALUES;
  }
}

void compute_required_angles(){
  requiredLeftAngle = 180 - map(pwm_inputs[LEFT_POS], PWM_MIN, PWM_MAX, -20, 20);
  requiredRightAngle = map(pwm_inputs[RIGHT_POS], PWM_MIN, PWM_MAX, -20, 20);
}

void pwm_changing()
{
  uint8_t pwm_latest_interrupted_pin=arduinoInterruptedPin;
  int pwm_arrayid = pwm_latest_interrupted_pin-PWM_FIRST_PIN;

  if (arduinoPinState != 0) {
      // HIGH
      pwm_prev_time[pwm_arrayid] = micros();
  } else {
      // LOW
      pwm_timings[pwm_arrayid][pwm_last_input[pwm_arrayid]] = int(micros() - pwm_prev_time[pwm_arrayid]);
      pwm_last_input[pwm_arrayid]++;
      if (pwm_last_input[pwm_arrayid]>=PWM_AVG_VALUES) {
          pwm_last_input[pwm_arrayid]=0;
      };
  }
}

void pwm_setup() {
  for (int pin=PWM_FIRST_PIN; pin<PWM_FIRST_PIN + PWM_NUMBER_OF_PINS; pin++){
    pinMode(pin, INPUT_PULLUP);
    pwm_last_input[pin-PWM_FIRST_PIN]=0;
    for (int i=0; i<PWM_AVG_VALUES; i++){
      pwm_timings[pin-PWM_FIRST_PIN][i]=1500;
    }
    enableInterrupt(pin, pwm_changing, CHANGE);
  }
}

void pwm_compute_inputs() {
  long sum_val;
  for (int pin=PWM_FIRST_PIN; pin<PWM_FIRST_PIN + PWM_NUMBER_OF_PINS; pin++){
      sum_val = 0;
      for (int i=0; i<PWM_AVG_VALUES; i++){
        sum_val += pwm_timings[pin - PWM_FIRST_PIN][i];
      }
      pwm_inputs[pin-PWM_FIRST_PIN] = sum_val / PWM_AVG_VALUES;
      if (SerialDebug) {
        Serial.print(pwm_inputs[pin-PWM_FIRST_PIN]);
        Serial.print(" | ");
      }
  }
  if (SerialDebug) {
    Serial.println("");
  }
}

void setup() {
  if (SerialDebug) {Serial.begin(115200);};
  
  wheel_servo.attach(WHEEL_PIN);
  motor_controller.attach(MOTOR_PIN);
  motor_controller.write(90);
  
  gy_512_setup();
  pwm_setup();
}

int compute_motor_speed () {
  if (SerialDebug) {
    Serial.print(pwm_inputs[POS_INPUT]);
    Serial.print(" | ");
    Serial.print(requiredRightAngle);
    Serial.print(" | ");
    Serial.print(Angles[RIGHT_LEFT_ANGLE]);
    Serial.print(" | ");
    Serial.print(requiredLeftAngle);
    Serial.println("");
  }
  if (pwm_inputs[POS_INPUT]>1200 and pwm_inputs[POS_INPUT]<1800) {
    //Set in no move position
    if (SerialDebug) {Serial.println("No motor move!");};
    return 90;
  }
  if (pwm_inputs[POS_INPUT]<1200 and Angles[RIGHT_LEFT_ANGLE] < requiredLeftAngle - 10) {
    //Should be left oriented (angle ~= 180) , but is not
    if (SerialDebug) {Serial.println("Move to left");};
    return MOTOR_SPEED_MIN;
  }
  if (pwm_inputs[POS_INPUT]<1200 and abs(Angles[RIGHT_LEFT_ANGLE] - requiredLeftAngle) > MIN_ADJUSTMENT_ANGLE ) {
    //Should be left oriented (angle ~= 180) , it is, but need adjustment
    if (SerialDebug) {Serial.println("Adjust Left");};
    if (Angles[RIGHT_LEFT_ANGLE] < requiredLeftAngle) {
      return MOTOR_SPEED_MIN + ADJUSTMENT_SPEED;
    } else {
      return MOTOR_SPEED_MAX - ADJUSTMENT_SPEED;
    }
  }
  if (pwm_inputs[POS_INPUT]>1800 and Angles[RIGHT_LEFT_ANGLE] > requiredRightAngle + 10) {
    if (SerialDebug) {Serial.println("Move to right");};
    //Should be right oriented (angle ~= 0) , but is not
    return MOTOR_SPEED_MAX;
  }
  if (pwm_inputs[POS_INPUT]>1800 and abs(Angles[RIGHT_LEFT_ANGLE] - requiredRightAngle) > MIN_ADJUSTMENT_ANGLE ) {
    //Should be right oriented (angle ~= 0) , it is, but need adjustment
    if (SerialDebug) {Serial.println("Adjust right");};
    if (Angles[RIGHT_LEFT_ANGLE] < requiredRightAngle) {
      return MOTOR_SPEED_MIN + ADJUSTMENT_SPEED;
    } else {
      return MOTOR_SPEED_MAX - ADJUSTMENT_SPEED;
    }
  }
  if (SerialDebug) {Serial.println("No motor move required");};
  return 90;
};

int compute_wheel_pos(){
  if (Angles[RIGHT_LEFT_ANGLE]<45) {
    if (abs(map(pwm_inputs[WHEEL_POS], PWM_MIN, PWM_MAX, WHEEL_SERVO_MAX, 90) - wheel_position) > MIN_WHEEL_ADJUSTMENT) {
      return map(pwm_inputs[WHEEL_POS], PWM_MIN, PWM_MAX, WHEEL_SERVO_MAX, 90);
    }
    return wheel_position;
  }
  if (Angles[RIGHT_LEFT_ANGLE]>135) {
    if (abs(map(pwm_inputs[WHEEL_POS], PWM_MIN, PWM_MAX, WHEEL_SERVO_MIN, 90) - wheel_position) > MIN_WHEEL_ADJUSTMENT) {
      return map(pwm_inputs[WHEEL_POS], PWM_MIN, PWM_MAX, WHEEL_SERVO_MIN, 90);
    }
    return wheel_position;
  }
  return 90;
}

void loop(){
  gy_512_read();
  pwm_compute_inputs();
  compute_required_angles();
  
  motor_speed = compute_motor_speed();
  if (MOTOR_PIN_INVERTED) {
    motor_controller.write(map(motor_speed, 0, 180, 180, 0));
  } else {
    motor_controller.write(motor_speed);
  }
  
  wheel_position = compute_wheel_pos();
  if (WHEEL_PIN_INVERTED) {
    wheel_servo.write(map(wheel_position, 0, 180, 180, 0));
  } else {
    wheel_servo.write(wheel_position);  
  }
  
  if (SerialDebug) {
    Serial.print(motor_speed);
    Serial.print(" |  ");
    Serial.println(wheel_position);
  }
}
