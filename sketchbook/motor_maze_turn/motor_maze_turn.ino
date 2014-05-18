#include <Servo.h>

#define VOLTS_PER_UNIT .0049F

Servo myservo;  // create servo object to control a servo 
                // a maximum of eight servo objects can be created 
 
int servoPos = 0;    // variable to store the servo position 
 

//declaring the pins for the IN pins on the L298N
const int rightForwardPin = 4;
const int rightBackwardPin = 2;
const int leftBackwardPin = 7;
const int leftForwardPin = 5;
const int forwardDistanceSensorPin = 0;
const int aftDistanceSensorPin = 1;
const int attemptLimit = 3;
const int servoPin = 9;
const int servoDegrees = 180;

const int directionForward = 0;
const int directionBackward = 1;
const int directionLeft = 2;
const int directionRight = 3;

const int servoReadDelay = 400;

float acceptableProximity = 12.0; // inches

int currentDirection;

int runTime = 2000;

float lastFrontDistance = 0.0;
float lastAftDistance = 0.0;
int stuckAttempts = 0;

void setup() 
{
  Serial.begin(9600);
  
  
  //Stating that the pins are OUTPUT
  pinMode(rightForwardPin, OUTPUT);
  pinMode(rightBackwardPin, OUTPUT);
  pinMode(leftForwardPin, OUTPUT);
  pinMode(leftBackwardPin, OUTPUT);

  myservo.attach(servoPin);  // attaches the servo on pin 9 to the servo object 
  
  lastFrontDistance = getDistance(forwardDistanceSensorPin);
  lastAftDistance = getDistance(aftDistanceSensorPin);


}

//Looping to test the wheels of the car
void loop() 
{

  boolean stuck = isStuck(getDistance(forwardDistanceSensorPin), getDistance(aftDistanceSensorPin))  ;
  if (stuck && stuckAttempts > 5)
  {
    Serial.println("stuck");
    stuckAttempts = 0;
    stopCar();
    backUp();
    currentDirection = getNextDirection(); 
    delay(runTime );
   
    return;
    
  }
  else if (stuck)
  {
    stuckAttempts += 1;
  }
  
  if (currentDirection == directionBackward)
  {
    backward(false);
  }
  else if (currentDirection == directionLeft) 
  {
    left();
  }
  else if (currentDirection == directionRight)
  {
    right();
  }
  else
  {
    forward(false);
  }
  delay(200);
  //stopCar();
 
 // backward();
 
//  stopCar();
 
//  left();
 
//  stopCar();
 
//  right();
 
//  stopCar();
}

void initServo()
{
  servoPos = 90;
  myservo.write(servoPos);

}

boolean isStuck(float frontRead, float aftRead)
{
  float frontVariance = (frontRead / lastFrontDistance);
  if (frontVariance > 1.08 || frontVariance < .92)
  {
    return false;
  }
  return true;
  float aftVariance = (aftRead / lastAftDistance);
  return (aftVariance < 1.08 && aftVariance > .92);

}

//Setting the wheels to go forward by setting the forward pins to HIGH
void forward(boolean ignoreProximity)
{
  float frontDistance = getDistance(forwardDistanceSensorPin);

  
  Serial.print("front: ");
  Serial.println(frontDistance / lastFrontDistance);
  lastFrontDistance = frontDistance;
  if (frontDistance < acceptableProximity && !ignoreProximity )
  {
    Serial.println("should go backwards");
    stopCar();
    delay(500);    

    backUp();
    
    currentDirection = getNextDirection(); 

    delay(runTime);
    

    return;
  }
  
  digitalWrite(rightForwardPin, HIGH);
  digitalWrite(rightBackwardPin, LOW);
  digitalWrite(leftForwardPin, HIGH);
  digitalWrite(leftBackwardPin, LOW);

  delay(200);
}

void backUp()
{
    backward(true);
    delay(300);
    stopCar();
}

//Setting the wheels to go backward by setting the backward pins to HIGH
void backward(boolean ignoreProximity)
{
  float aftDistance = getDistance(aftDistanceSensorPin);
  
  
  
  Serial.print("aft: ");
  Serial.println(aftDistance/ lastAftDistance);
  
  lastAftDistance = aftDistance;
  
  if ( aftDistance < acceptableProximity && !ignoreProximity)
  {
    Serial.println("should go forwards");
    stopCar();
    delay(500);
    
    currentDirection = directionForward;
    float frontDistance = getDistance(forwardDistanceSensorPin);
    if (frontDistance < acceptableProximity *2)
    {    
      forward(true);
      delay(100);

      currentDirection = directionRight;
    }
    stopCar();
    delay(runTime);
    

    return;
  }
  digitalWrite(rightForwardPin, LOW);
  digitalWrite(rightBackwardPin, HIGH);
  digitalWrite(leftForwardPin, LOW);
  digitalWrite(leftBackwardPin, HIGH);
 

}

//Setting the wheels to go right by setting the rightBackwardPin and leftForwardPin to HIGH
void right()
{
  Serial.println("Going right");
  digitalWrite(rightForwardPin, LOW);
  digitalWrite(rightBackwardPin, HIGH);
  digitalWrite(leftForwardPin, HIGH);
  digitalWrite(leftBackwardPin, LOW);
  delay(800);
  stopCar();
  delay(50);  
  currentDirection = getPostTurnDirection();
  
}

//Setting the wheels to go left by setting the rightForwardPin and leftBackwardPin to HIGH
void left()
{
  Serial.println("Going left");
  digitalWrite(rightForwardPin, HIGH);
  digitalWrite(rightBackwardPin, LOW);
  digitalWrite(leftForwardPin, LOW);
  digitalWrite(leftBackwardPin, HIGH);
  
  delay(800);
  stopCar();
  delay(50);
  currentDirection = getPostTurnDirection();
}

//Setting the wheels to go stop by setting all the pins to LOW
void stopCar()
{
  digitalWrite(rightForwardPin, LOW);
  digitalWrite(rightBackwardPin, LOW);
  digitalWrite(leftForwardPin, LOW);
  digitalWrite(leftBackwardPin, LOW);
  delay(1000);
}

float getLeftDistance()
{  
  for(servoPos = 0; servoPos < servoDegrees; servoPos += 1)  // goes from 0 degrees to 180 degrees 
  {                                  // in steps of 1 degree 
    myservo.write(servoPos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
  delay(servoReadDelay);
  return getDistance(forwardDistanceSensorPin);

}

float getRightDistance()
{  
  for(servoPos = servoDegrees; servoPos >= 1; servoPos -= 1)  // goes from 0 degrees to 180 degrees 
  {                                  // in steps of 1 degree 
    myservo.write(servoPos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
  delay(servoReadDelay);
  return getDistance(forwardDistanceSensorPin);

}

int getNextDirection()
{
  initServo();
  delay(100);
  
  float front = getDistance(forwardDistanceSensorPin);
  delay(15);
  if (front <= 1.0)
  {
    stopCar();
    return directionBackward;
  }

  float left = getLeftDistance();
  float right = getRightDistance();
  float aft = getDistance(aftDistanceSensorPin);  
  
  float result = left;
  if (left < right)
  {
    result = right;
  }
  
  if (aft > result)
  {
    result = aft;
  }
  
  result = max(left, right);
  result = max(result, aft);
  
  initServo();
  if (result == left)
  {
    return directionLeft;
  }
  if (result == right)
  {
    return directionRight;
  }
  

  return directionBackward;
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

int getPostTurnDirection()
{
  float frontDistance = getDistance(forwardDistanceSensorPin);
  float aftDistance = getDistance(aftDistanceSensorPin);
  if (aftDistance < frontDistance)
  {
    return directionForward;
  }
  return directionBackward;
}


