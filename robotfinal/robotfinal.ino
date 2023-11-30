#include <Servo.h>


class UltraSonicSensor {
private:
  byte triggerPin;
  byte echoPin;
  //For TinkerCad, use max = 11
  //For physical use, use max = 13
  int max = 11;
  const byte MAX_DURATION = max*12*74*2;
  
public:
  
  ////////////////|Constructors|/////////////////////
  UltraSonicSensor() {//base constructor
    triggerPin = 0;
    echoPin = 0;
  }
  
  UltraSonicSensor(byte pin) {//3 pin constructor
    triggerPin = pin;
    echoPin = pin;
  }
  
  //4 pin constructor
  UltraSonicSensor(byte tPin, byte ePin) {
    triggerPin = tPin;
    echoPin = ePin;
  }
  
  ////////////////|Methods|/////////////////////
  //main measurement method
  //conducts a measurement of duration it takes to 
  //send a signal and have it return to US Sensor
  unsigned long measure(){
    unsigned long duration = 0;//initalize duration
    
    //turn off interrupts so measurement isn't disrupted
    noInterrupts();
    
    //echo pin to output and set to low while sending out signal
    pinMode(echoPin, OUTPUT);
    digitalWrite(echoPin, LOW);
    
    //trigger to output, start at low to prepare to send signal
    pinMode(triggerPin, OUTPUT);
    digitalWrite(triggerPin, LOW);
    delayMicroseconds(5);
    
    //send out a signal for 10 microseconds, then stop signal
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(triggerPin, LOW);
    
    //echo to input to record measurement of duration
    pinMode(echoPin, INPUT);
    duration = pulseIn(echoPin, MAX_DURATION);
    
    //now that measurement is done, interrupts can be turned back on
    interrupts();
    
    //divide by 2 to make sure it returns 1-way duration
    if(duration > 0)
      duration /= 2;
    
    return duration;
  }
  
  
  //convert measure duration from microseconds to inches
  double measureIn() {
    double inches = 0;
    unsigned long duration = measure();
    if(duration > 0)
      inches = static_cast<double>(duration/74.0);
    
    return inches;
  }
  
  //convert measure duration from microseconds to centimeters
  double measureCm() {
    double centimeters = 0;
    unsigned long duration = measure();
    if(duration > 0)
      centimeters = static_cast<double>(duration/29.1);
    
    return centimeters;
  }
  
};

// define class Motor
//    properties: pin1, pin2, enable, speed
//    methods: constructors, getter/setter for speed, run
class Motor {
private:
  byte _pin1;
  byte _pin2;
  byte _enable;
  byte _speed;
  
public:
  enum MotorDir {
    MotorStop,
    MotorForward,
    MotorReverse
  }; // enum MotorDir
  
  Motor() : _pin1{0}, _pin2{0}, _enable{0}, _speed{0} { }
  
  Motor(byte pin1, byte pin2, byte enable)
    : _pin1{pin1}, _pin2{pin2}, _enable{enable}, _speed{0} 
  { 
  	pinMode(_pin1, OUTPUT);
    pinMode(_pin2, OUTPUT);
    pinMode(_enable, OUTPUT);
  } // end Motor prop constructor
  
  byte getSpeed() { return _speed; }
  void setSpeed(byte speed) { _speed = speed; }
  
  void run(Motor::MotorDir dir) {
    switch(dir) {
    case MotorStop:
      digitalWrite(_pin1,LOW);
      digitalWrite(_pin2,LOW);
      digitalWrite(_enable,LOW);
      break;
    case MotorForward:
      digitalWrite(_pin1,HIGH);
      digitalWrite(_pin2,LOW);
      analogWrite(_enable,_speed);
      break;
    case MotorReverse:
      digitalWrite(_pin1,LOW);
      digitalWrite(_pin2,HIGH);
      analogWrite(_enable,_speed);
    } // which dir
  } // end run
  
}; // end Motor








/////////////|Pins|/////////////////
const byte LMBack = 2;
const byte LMFor = 4;
const byte LMEnable = 5;
const byte RMBack = 7;
const byte RMFor = 8;
const byte RMEnable = 6;

byte speed = 100;

Motor leftMotor;
Motor rightMotor;
Servo myServo;
UltraSonicSensor ultraSonic(A1, A0);

void setup() {
  // put your setup code here, to run once:
  leftMotor = Motor(LMBack,LMFor,LMEnable);
  rightMotor = Motor(RMBack,RMFor,RMEnable);
  myServo.attach(A2, 500, 2500);
}

void loop() {
  // put your main code here, to run repeatedly:
  leftMotor.setSpeed(speed);
  rightMotor.setSpeed(speed);
  leftMotor.run(Motor::MotorForward);
  rightMotor.run(Motor::MotorForward);
  delay(10000);
}



























































