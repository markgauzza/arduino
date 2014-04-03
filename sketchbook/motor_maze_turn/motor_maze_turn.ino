#define VOLTS_PER_UNIT .0049F

//declaring the pins for the IN pins on the L298N
const int rightForwardPin = 4;
const int rightBackwardPin = 2;
const int leftBackwardPin = 7;
const int leftForwardPin = 5;
const int forwardDistanceSensorPin = 0;
const int aftDistanceSensorPin = 1;
const int attemptLimit = 3;

const int directionForward = 0;
const int directionBackward = 1;
const int directionLeft = 2;
const int directionRight = 3;

float acceptableProximity = 8.0; // inches

int currentDirection;

int runTime = 2000;

void setup() 
{
  Serial.begin(9600);
  
  
  //Stating that the pins are OUTPUT
  pinMode(rightForwardPin, OUTPUT);
  pinMode(rightBackwardPin, OUTPUT);
  pinMode(leftForwardPin, OUTPUT);
  pinMode(leftBackwardPin, OUTPUT);
}

//Looping to test the wheels of the car
void loop() 
{

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

//Setting the wheels to go forward by setting the forward pins to HIGH
void forward(boolean ignoreProximity)
{
  float frontDistance = getDistance(forwardDistanceSensorPin);
  Serial.print("front: ");
  Serial.println(frontDistance);
  if (frontDistance < acceptableProximity && !ignoreProximity )
  {
    Serial.println("should go backwards");
    stopCar();
    delay(500);
    
    currentDirection = directionBackward;
    float aftDistance = getDistance(aftDistanceSensorPin);
    if (aftDistance < acceptableProximity *2)
    {    
      backward(true);
      delay(100);

      currentDirection = directionLeft;
    }
    stopCar();
    delay(runTime);
    

    return;
  }
  
  digitalWrite(rightForwardPin, HIGH);
  digitalWrite(rightBackwardPin, LOW);
  digitalWrite(leftForwardPin, HIGH);
  digitalWrite(leftBackwardPin, LOW);

  delay(200);
}

//Setting the wheels to go backward by setting the backward pins to HIGH
void backward(boolean ignoreProximity)
{
  float aftDistance = getDistance(aftDistanceSensorPin);
  Serial.print("aft: ");
  Serial.println(aftDistance);
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
  digitalWrite(rightForwardPin, HIGH);
  digitalWrite(rightBackwardPin, LOW);
  digitalWrite(leftForwardPin, LOW);
  digitalWrite(leftBackwardPin, HIGH);
  
  Serial.println("going left");
  
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

float getDistance(int pin)
{
  int proxSens = analogRead(pin);
  float volts = (float)proxSens * VOLTS_PER_UNIT;
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


