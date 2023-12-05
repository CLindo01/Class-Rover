#include <Servo.h> 

#define RightEnable 6
#define RightForward 8
#define RightBackward 7
#define LeftEnable 5
#define LeftForward 4
#define LeftBackward 2

Servo servoLook;     //create an object to control servos

const byte trig = A1;     //assigns US pins
const byte echo = A0;
const byte maxDist = 150;   //max seeing distance
const byte stopDist = 50;   // min dustance from an object to stop in cm
const float timeOut = 2 * (maxDist + 10) / 100 / 340 * 1000000;     //time to wait for signal

const byte motorSpeed = 150;     //max motor speed
const int motorOffset = 50;     // adjusts speed
const int turnSpeed = 100;       //amount to replace when turning

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

  stopMove();
}

void loop() {
  Serial.println("Inside Loop");
  // Move forward for 2 seconds
  moveForward(2000);

  // Check for obstacles while moving forward
  int distance;
  for (int i = 0; i < 200; i++) {
    distance = getDistance();
    delay(10);
    if (distance < stopDist) {
      stopMove();
      break;
    }
  }

  // Reset the servo to the front position
  servoLook.write(90);

  // Check for obstacles every 5 seconds
  delay(2000);
  Serial.print("Distance: ");
  Serial.println(distance);
  if (distance >= stopDist) {
    moveForward(2000);
  }

  while (distance >= stopDist) {
    // Keep checking the object distance until it is within the minimum stopping distance
    distance = getDistance();
    delay(250);
  }

  stopMove();  // Stops motors
  int turnDir = checkDirection();  // Checks left and right
  servoLook.write(90);
  Serial.print("Turning: ");
  Serial.println(turnDir);

  switch (turnDir) {
    case 0:  // Turn left
      turnLeft(400);
      break;
    case 1:  // Turn around
      turnLeft(700);
      break;
    case 2:  // Turn right
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

void decelerate() {
  for (int i = motorSpeed; i != 0; i--) {
    analogWrite(RightEnable, i);
    analogWrite(LeftEnable, i);
    delay(10);
  }
}

void moveForward(int duration) {
  digitalWrite(RightBackward, LOW);
  digitalWrite(LeftBackward, LOW);
  analogWrite(RightEnable, motorSpeed);
  analogWrite(LeftEnable, motorSpeed);
  digitalWrite(RightForward, HIGH);
  digitalWrite(LeftForward, HIGH);

  
}

void stopMove() {
  Serial.println("Stopping Motors");
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
  analogWrite(RightEnable, motorSpeed + turnSpeed + motorOffset);
  analogWrite(LeftEnable, motorSpeed + turnSpeed);
  digitalWrite(RightForward, HIGH);
  digitalWrite(LeftBackward, HIGH);
  delay(duration);
  stopMove();
}

void turnRight(int duration) {
  digitalWrite(RightForward, LOW);
  digitalWrite(LeftBackward, LOW);
  analogWrite(RightEnable, motorSpeed + turnSpeed);
  analogWrite(LeftEnable, motorSpeed + turnSpeed + motorOffset);
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