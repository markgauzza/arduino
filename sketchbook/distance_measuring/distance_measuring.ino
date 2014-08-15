#include <Servo.h>

#define VOLTS_PER_UNIT .0049F

#define LEFT 0
#define RIGHT 1
#define BACK 3
#define FRONT 4

const int rightForwardPin = 4;
const int rightBackwardPin = 2;
const int leftBackwardPin = 7;
const int leftForwardPin = 5;


Servo myservo;  // create servo object to control a servo 
                // a maximum of eight servo objects can be created 
 
int servoPos = 0;    // variable to store the servo position 
const int servoPin = 9;
const int servoDegrees = 180;
const int forwardDistanceSensorPin = 0;
const int aftDistanceSensorPin = 1;

void setup() 
{
  Serial.begin(9600);
  myservo.attach(servoPin);  // attaches the servo on pin 9 to the servo object 
  
  pinMode(rightForwardPin, OUTPUT);
  pinMode(rightBackwardPin, OUTPUT);
  pinMode(leftForwardPin, OUTPUT);
  pinMode(leftBackwardPin, OUTPUT);
}

void loop()
{
  int nextDirection = getDirection();
}

int getDirection()
{
  
    float frontDistance = getDistance(forwardDistanceSensorPin);
  Serial.print("front: ");
  Serial.println(frontDistance);
  delay(1000);
  
  float aftDistance = getDistance(aftDistanceSensorPin);
  Serial.print("aft: ");
  Serial.println(aftDistance);
  delay(1000);
  
  turnServoLeft();
  delay(1000);
  float leftDistance = getDistance(forwardDistanceSensorPin);
  Serial.print("left: ");
  Serial.println(leftDistance);
  
  
  initServo();
  
  turnServoRight();
  delay(1000);
  float rightDistance = getDistance(forwardDistanceSensorPin);
  Serial.print("right: ");
  Serial.println(rightDistance);
  initServo();
  delay(1000);
  
  int nextDirection = FRONT;
  
  float furthest = frontDistance;
  
  if (furthest  < aftDistance)
  {
    nextDirection = BACK; 
    furthest = aftDistance;
  }
  
  if (furthest < leftDistance)
  {
    nextDirection = LEFT;
    furthest  = leftDistance;
  }
  
  if (furthest < rightDistance)
  {
    nextDirection = RIGHT;
  }
  
  Serial.print("Direction: ");
  Serial.println(nextDirection);
  
  
}


void turnServoLeft()
{
  for(servoPos = 0; servoPos < servoDegrees; servoPos += 1)  // goes from 0 degrees to 180 degrees 
  {                                  // in steps of 1 degree 
    myservo.write(servoPos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
}

void turnServoRight()
{

  for(servoPos = servoDegrees; servoPos >= 1; servoPos -= 1)  // goes from 0 degrees to 180 degrees 
  {                                  // in steps of 1 degree 
    myservo.write(servoPos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
}

void initServo()
{
  servoPos = 90;
  myservo.write(servoPos);

}

float getDistance(int pin)
{
  int proxSens = analogRead(pin);
  float volts = (float)proxSens * VOLTS_PER_UNIT;
  if (volts < .2)
  {
    return -1;
  }
  float inches = 23.897 * pow(volts, -1.1907);
  return inches;
}


