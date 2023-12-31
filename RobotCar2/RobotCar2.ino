#include <IRremote.h>

#include <Servo.h> 

#define RightEnable 6
#define RightForward 8
#define RightBackward 7
#define LeftEnable 5
#define LeftForward 4
#define LeftBackward 2

#define IR_PIN 12

enum BTN {
  OK
};



Servo servoLook;     //create an object to control servos

const byte trig = A1;     //assigns US pins
const byte echo = A0;
const byte maxDist = 150;   //max seeing distance
const byte stopDist = 50;   // min dustance from an object to stop in cm
const float timeOut = 2 * (maxDist + 10) / 100 / 340 * 1000000;     //time to wait for signal

const byte motorSpeed = 110;     //max motor speed
const int motorOffset = 50;     // adjusts speed
const int turnSpeed = 25;       //amount to replace when turning

void setup() {
  pinMode(RightEnable, OUTPUT);       //set the motors to the motor speed
  pinMode(RightForward, OUTPUT);
  pinMode(RightBackward, OUTPUT);
  pinMode(LeftEnable, OUTPUT);
  pinMode(LeftForward, OUTPUT);
  pinMode(LeftBackward, OUTPUT);
  

  servoLook.attach(A2);       //assigns servo pin
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  
  IrReceiver.begin(IR_PIN, true);
  
  stopMove();

  Serial.begin(9600);
}

void loop() {
  /*if (IrReceiver.decode()) {
    switch (IrReceiver.decodedIRData.command) {
      case OK:
        Serial.println("OK");
        break;
      default:
        Serial.println("Unknown command");
        break;
    }
    IrReceiver.resume();
  }*/
  servoLook.write(90);        //set servo to look straight ahead
  delay(750);
  int distance = getDistance();   //check that there are no objects

  if (distance >= stopDist) {
    moveForward();
  }

  while (distance >= stopDist) {       //keep checking the object distance until it is within the minimum stopping distance
    distance = getDistance();
    delay(250);
  }

  stopMove();             // Stops motors
  int turnDir = checkDirection();              //checks left and right
  Serial.print(turnDir);
  switch (turnDir) {                //Turn left, turn around, or turn right
    case 0:               //turn left
      turnLeft(400);
      break;
    case 1:    //turn around
      turnLeft(700);
      break;
    case 2:
      turnRight(400);
      break;
  }
}

void accelerate() {
  for (int i = 0; i < motorSpeed; i++) {
    analogWrite(RightEnable, i);
    analogWrite(LeftEnable, i);
    delay(10);
  }
}


void moveForward() {
  digitalWrite(RightBackward, LOW);
  digitalWrite(LeftBackward, LOW);
  analogWrite(RightEnable, motorSpeed + 15);
  analogWrite(LeftEnable, motorSpeed);
  digitalWrite(RightForward, HIGH);
  digitalWrite(LeftForward, HIGH);
}

void stopMove() {
  digitalWrite(RightBackward, LOW);
  digitalWrite(LeftBackward, LOW);
  digitalWrite(RightForward, LOW);
  digitalWrite(LeftForward, LOW);
  analogWrite(RightEnable, 0);
  analogWrite(LeftEnable, 0);
}

void turnLeft(int duration) {
  digitalWrite(RightBackward, LOW);
  digitalWrite(LeftForward, LOW);
  analogWrite(RightEnable, motorSpeed + turnSpeed);
  analogWrite(LeftEnable, motorSpeed);
  digitalWrite(RightForward, HIGH);
  digitalWrite(LeftBackward, HIGH);
  delay(duration);
  stopMove();
}

void turnRight(int duration) {
  digitalWrite(RightForward, LOW);
  digitalWrite(LeftBackward, LOW);
  analogWrite(RightEnable, motorSpeed);
  analogWrite(LeftEnable, motorSpeed + turnSpeed);
  digitalWrite(RightBackward, HIGH);
  digitalWrite(LeftForward, HIGH);
  delay(duration);
  stopMove();
}


int getDistance()
{
  unsigned long pulseTime;
  int distance;
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  pulseTime = pulseIn(echo, HIGH, timeOut);
  distance = (float)pulseTime * 340 / 2 / 10000;
  return distance;
}

int checkDirection()
{
  int distances [2] = {0,0};
  int turnDir = 1;
  servoLook.write(180);
  delay(500);
  distances [0] = getDistance();
  servoLook.write(0);
  delay(1000);
  distances[1] = getDistance();
  if (distances[0] >= 200 && distances [1] >= 200)
    turnDir = 0;
  else if (distances[0] <= stopDist && distances [1] <= stopDist)
    turnDir = 1;
  else if (distances[0] >= distances [1])
    turnDir = 0;
  else if (distances[0] < distances [1])
    turnDir = 2;
  return turnDir;
}
