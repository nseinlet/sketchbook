#include <Wire.h>
#include <Servo.h>
#include <PinChangeInt.h>

#define PWMMin   900
#define PWMMax   2100
#define AngleMin 5
#define AngleMax 175

#define WHEEL_PIN 6
#define MOTOR_PIN 7

#define INPUT_PWM_SIZE 4
#define INPUT_ANGLE_LEFT 9
#define INPUT_ANGLE_RIGHT 10
#define INPUT_POSITION 11
#define INPUT_WHEEL_ANGLE 12

const int MPU_addr=0x68;
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
int read_id;const int read_interval=3;

int minVal=265;
int maxVal=402;

double x; 
double y; 
double z;
const double z_correction=90;
double plough_angle;
double motor_control;
const int max_angle_adjust=50;
int pos_left;
int pos_right;

double levelwheelsetting;
double levelwheelangle;
double requiredangle;
int requiredzone;
int currentzone;

const int motorright=90-25;
const int motorleft=90+30;

const int PWM_PIN[INPUT_PWM_SIZE]={INPUT_ANGLE_LEFT,INPUT_ANGLE_RIGHT,INPUT_POSITION,INPUT_WHEEL_ANGLE};
const int bPinMap[14] = {0,0,0,0,0,0,0,0,0,0,1,2,3,0};
volatile unsigned long pwm_value[INPUT_PWM_SIZE];
volatile unsigned long pwm_time[INPUT_PWM_SIZE];
int last_pwm_pin_read;

Servo wheelservo;
Servo motorservo;

int pwmToAngle(int pwmvalue, int angleMin, int angleMax){
  return map(pwmvalue, PWMMin, PWMMax, angleMin, angleMax);
};

void rising()
{
  last_pwm_pin_read=PCintPort::arduinoPin;
  PCintPort::attachInterrupt(last_pwm_pin_read, &falling, FALLING);
  pwm_time[bPinMap[last_pwm_pin_read]] = micros();
}

void falling() {
  last_pwm_pin_read=PCintPort::arduinoPin;
  PCintPort::attachInterrupt(last_pwm_pin_read, &rising, RISING);
  pwm_value[bPinMap[last_pwm_pin_read]] = micros()-pwm_time[bPinMap[last_pwm_pin_read]];
}

void read_inputs(){
  //0 : required angle left
  //1 : required angle right
  //2 : required position
  //3 : Wheel position

  pos_left=pwmToAngle(pwm_value[0], 90-(max_angle_adjust/2), 90+(max_angle_adjust/2));
  pos_right=pwmToAngle(pwm_value[1], 270-(max_angle_adjust/2), 270+(max_angle_adjust/2));
  if (pwmToAngle(pwm_value[2], AngleMin, AngleMax)>110){
    requiredangle=pos_left;
  } else {
    if (pwmToAngle(pwm_value[2], AngleMin, AngleMax)<70){
      requiredangle=pos_right;
    } else {
      requiredangle=180;
    };  
  };
  levelwheelsetting=pwmToAngle(pwm_value[3], AngleMin/2, AngleMax/2);
}

void read_gyro(){
  read_id+=1;
  if (read_id>read_interval){
    read_id=0;
    //Use I2c to read angles
    Wire.beginTransmission(MPU_addr);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_addr,14,true);
    AcX=Wire.read()<<8|Wire.read();
    AcY=Wire.read()<<8|Wire.read();
    AcZ=Wire.read()<<8|Wire.read();
    int xAng = map(AcX,minVal,maxVal,-90,90);
    int yAng = map(AcY,minVal,maxVal,-90,90);
    int zAng = map(AcZ,minVal,maxVal,-90,90);

    x= RAD_TO_DEG * (atan2(-yAng, -zAng)+PI);
    y= RAD_TO_DEG * (atan2(-xAng, -zAng)+PI);
    z= RAD_TO_DEG * (atan2(-yAng, -xAng)+PI);
  }
}

int get_zone(double angle) {
  //0 no go zone
  //1 right
  //2 middle
  //3 left
  if (angle>=45 && angle<135)
      return 1;
  if (angle<225)
      return 2;
  if (angle<=315)
      return 3;
  return 0;
}

void setup(){ 
  Wire.begin(); 
  Wire.beginTransmission(MPU_addr); 
  Wire.write(0x6B); 
  Wire.write(0); 
  Wire.endTransmission(true); 
  //Serial.begin(115200);
  wheelservo.attach(WHEEL_PIN);
  motorservo.attach(MOTOR_PIN);
  levelwheelsetting=65;
  requiredangle=270;
  requiredzone=3;
  levelwheelangle=0;
  motor_control=90;
  for (last_pwm_pin_read=0;last_pwm_pin_read<INPUT_PWM_SIZE;last_pwm_pin_read++){
    pinMode(PWM_PIN[last_pwm_pin_read], INPUT); digitalWrite(PWM_PIN[last_pwm_pin_read], HIGH);
    PCintPort::attachInterrupt(PWM_PIN[last_pwm_pin_read], &rising, RISING);
  };
  read_id=0;
} 

void loop(){ 
  read_gyro();
  //Convert z to usable angle
  plough_angle=360-z+z_correction;
  if (plough_angle>360) {
    plough_angle=plough_angle-360;
  }
  if (plough_angle<0) {
    plough_angle=plough_angle+360;
  }
  currentzone=get_zone(plough_angle);

  //Read pwn signal
  //Except when in no go zone
  if (currentzone>0){
    read_inputs();
    requiredzone=get_zone(requiredangle);
  };
  
  //Control leveling wheel
  switch (currentzone) {
    case 1:
      levelwheelangle=90+(levelwheelsetting);
      break;
    case 2:
      levelwheelangle=90;
      break;
    case 3:
      levelwheelangle=90+(levelwheelsetting*(-1));
      break;
    default:
      // In no go zone, keep existing value
      break;
  }

  motor_control=90;
  if (requiredzone!=0 and requiredzone!=2){
    if (requiredzone==currentzone){
      //Just need to adjust
      //Avoid micro adjustment
      if (abs(requiredangle-plough_angle)>5){
        if (requiredangle>plough_angle){
          motor_control=motorleft;
        } else {
          motor_control=motorright;
        };
      };
    } else {
      //Go to required zone
      if (requiredzone==1) {
        if (currentzone==0){
          motor_control=motorleft;
        } else {
          motor_control=motorright;
        };
      } else {
        if (currentzone==0){
          motor_control=motorright;
        } else {
          motor_control=motorleft;
        };
      };
    };
  };

    //Serial.print(plough_angle);
    //Serial.print(" | ");
    //Serial.print(requiredangle);
    //Serial.print(" | ");
    //Serial.print(motor_control);
    //Serial.print(" | ");
    //Serial.print(currentzone);
    //Serial.print(" | ");
    //Serial.print(requiredzone);
    //Serial.print(" | ");
    //Serial.println(levelwheelangle);
  
  wheelservo.write(levelwheelangle);
  motorservo.write(motor_control);
  
}
