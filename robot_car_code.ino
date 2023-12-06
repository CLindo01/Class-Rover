#include <Servo.h>

#define RightEnable 6
#define RightForward 8
#define RightBackward 7
#define LeftEnable 5
#define LeftForward 4
#define LeftBackward 2


Servo servoLook; // create an object to control servos

const byte trig = A1; // assigns US pins
const byte echo = A0;
const byte stopDist = 50; // min distance from an object to stop in cm
const float timeOut = 2 * (stopDist + 10) / 100 / 340 * 1000000; // time to wait for signal

const byte motorSpeed = 125; // max motor speed
const int turnSpeed = 50;   // amount to replace when turning

void setup() {
  pinMode(RightEnable, OUTPUT); // set the motors to the motor speed
  pinMode(RightForward, OUTPUT);
  pinMode(RightBackward, OUTPUT);
  pinMode(LeftEnable, OUTPUT);
  pinMode(LeftForward, OUTPUT);
  pinMode(LeftBackward, OUTPUT);

  servoLook.attach(A2); // assigns servo pin
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  stopMove();
  servoLook.write(90); // Set sensor to 90 degrees
  delay(750); // Delay for servo to reach position
}

void loop() {
  unsigned long startTime = millis();
  unsigned long obstacleCheckTime = millis(); // Timer for obstacle checks
  bool obstacleDetected = false;

  while (millis() - startTime < 4000) { // Stop if robot travels for 4 seconds without stopping
    int distance = getDistance(); // check for obstacles

    if (distance < stopDist) {
      obstacleDetected = true;
      stopMove(); // Stop if an obstacle is detected
      delay(1000); // Wait for 1 second after stopping

      int turnDir = checkDirection(); // Check surroundings in all directions
      Serial.print(turnDir);
      switch (turnDir) { // Turn left, reverse, or turn right
        case 0: // turn left
          turnLeft(400);
          delay(100); // additional delay after turning left
          break;
        case 1: // reverse
          moveBackward();
          delay(2000); // reverse for 2 seconds
          stopMove(); // stop after reversing
          break;
        case 2: // turn right
          turnRight(400);
          delay(100); // additional delay after turning right
          break;
      }

      obstacleDetected = false; // Reset obstacle detection for the next cycle
      obstacleCheckTime = millis(); // Reset the timer after taking action
    }

    // Check for obstacles every 2 seconds
    if (millis() - obstacleCheckTime >= 2000) {
      obstacleCheckTime = millis(); // Reset the timer
      obstacleDetected = false; // Reset obstacle detection
      moveForward(); // Resume moving forward
    }
  }
  // If the robot has traveled for 4 seconds without stopping, end program
}

void moveForward() {
  digitalWrite(RightBackward, LOW);
  digitalWrite(LeftBackward, LOW);
  analogWrite(RightEnable, motorSpeed + 20);
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
  analogWrite(RightEnable, motorSpeed);
  analogWrite(LeftEnable, 100);
  digitalWrite(RightForward, HIGH);
  digitalWrite(LeftBackward, HIGH);
  delay(duration);
  stopMove();
}

void moveBackward() {
  digitalWrite(RightForward, LOW);
  digitalWrite(LeftForward, LOW);
  analogWrite(RightEnable, motorSpeed);
  analogWrite(LeftEnable, motorSpeed);
  digitalWrite(RightBackward, HIGH);
  digitalWrite(LeftBackward, HIGH);
}

void turnRight(int duration) {
  digitalWrite(RightForward, LOW);
  digitalWrite(LeftBackward, LOW);
  analogWrite(RightEnable, 100);
  analogWrite(LeftEnable, motorSpeed);
  digitalWrite(RightBackward, HIGH);
  digitalWrite(LeftForward, HIGH);
  delay(duration);
  stopMove();
}

int getDistance() {
  unsigned long pulseTime;
  int distance;
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  pulseTime = pulseIn(echo, HIGH, timeOut);
  distance = (float)pulseTime * 340 / 2 / 10000;
  return distance;
}

int checkDirection() {
  int distances[2] = {0, 0};
  servoLook.write(180);
  delay(500);
  distances[0] = getDistance();
  servoLook.write(0);
  delay(1000);
  distances[1] = getDistance();
  
  int turnDir = 1; // Default turnDir value

  if (distances[0] >= 200 && distances[1] >= 200) {
    turnDir = 0;
  } else if (distances[0] <= stopDist && distances[1] <= stopDist) {
    turnDir = 1;
  } else if (distances[0] >= distances[1]) {
    turnDir = 0;
  } else if (distances[0] < distances[1]) {
    turnDir = 2;
  }
  servoLook.write(90); // Return servo to 90-degree position
  delay(500); // Delay to ensure servo reaches the desired position
  return turnDir;
}
