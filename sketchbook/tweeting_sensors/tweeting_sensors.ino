#include <Boards.h>
#include <Firmata.h>
#include <SPI.h> // needed in Arduino 0019 or later
#include <Ethernet.h>
#include <Twitter.h>
#include <dht.h>
#include <stdio.h>


  #define ANALOG_LIGHT_PIN 0
#define LIGHT_TOLERANCE .01
#define DHT22_PIN 6
#define READ_TYPE_TEMPERATURE 1
#define READ_TYPE_LIGHT 0
#define READ_TYPE_HUMIDITY 2


int currentRead;
int totalRuns;
dht DHT;
double lastLightRead;
double lastTemperatureRead;
double lastHumidityRead;
// Ethernet Shield Settings
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// If you don't specify the IP address, DHCP is used(only in Arduino 1.0 or later).
byte ip[] = { 192, 168, 1, 177 };

// Your Token to Tweet (get it from http://arduino-tweet.appspot.com/)
Twitter twitter("2355186536-xXWPEzUPbXpNQQd9SgxosgK7SDpsQeE4zSlWc4h");


void setup()
{
  Serial.begin(9600);
  delay(2500);
  Ethernet.begin(mac, ip);
}

void loop()
{
  double sensorReading = 0;
  String message  = "";
  double delta = 0;
  String readDesc = "";
  String unit = "";
        Serial.println(lastLightRead);
        Serial.println(currentRead);
  if (totalRuns >= 25 && totalRuns % 25)
  {
    String run = "Cycled through ";
    run = run + String(totalRuns);
    run = run + " light, humidity and temperature reads.";
    sendTweet(run);
    delay(40000);
  }
          
  if (currentRead == READ_TYPE_LIGHT)
  {  
    sensorReading = (double)analogRead(ANALOG_LIGHT_PIN);  

    if (totalRuns == 0)
    {
      lastTemperatureRead = getTemperatureReading();
      lastHumidityRead = getHumidityReading();
      lastLightRead = max(sensorReading, .001);
      Serial.println(lastLightRead);

  char tempTemperature[10];
  dtostrf(lastTemperatureRead, 2, 2, tempTemperature);
      message = "Just came online, it is " + getLightLevelDescription(lastLightRead) ;
      message = message + " and the temperature is " +  tempTemperature+ ".";

      sendTweet(message);
      delay(20000);      
      totalRuns = totalRuns + 1;
      return;
      
    }
    else
    {
    
      readDesc = "light level";
      delta = getDelta(lastLightRead, sensorReading);    
      lastLightRead = sensorReading;
      currentRead = READ_TYPE_TEMPERATURE;
    }
  }
  else if (currentRead == READ_TYPE_TEMPERATURE)
  {
    readDesc = "temperature";
    sensorReading = getTemperatureReading();
    delta = getDelta(lastTemperatureRead, sensorReading);
    lastTemperatureRead = sensorReading;
    currentRead = READ_TYPE_HUMIDITY;
    unit = " degrees";
  }
  else if (currentRead == READ_TYPE_HUMIDITY)
  {
    readDesc = "humidity level";
    sensorReading = getHumidityReading();
    delta = getDelta(lastHumidityRead, sensorReading);
    lastHumidityRead = sensorReading;
    currentRead = READ_TYPE_LIGHT;
    unit = "%";
  }
  
  char tempDelta[10];
  dtostrf(abs(delta), 2 , 2, tempDelta);
 // Serial.println(tempDelta);
  char tempSensorReading[10];
  dtostrf(sensorReading, 2, 2, tempSensorReading);
 // if (delta > LIGHT_TOLERANCE || delta <  LIGHT_TOLERANCE*-1)
  {
    String verb = "increased";
    if (delta < 0)
    {
      verb = "decreased";
    }
    message = "The "+readDesc;
    if (delta == 0.00)
    {
      message += " is about the same.";
    }
    else
    {
      message +=  " has "+verb+" by ";
      message = message + tempDelta;
      message = message + ("%.  ");
          
    }

    if (currentRead == READ_TYPE_LIGHT + 1)
    {      
      message = message + ("It is currently " + getLightLevelDescription(sensorReading) + ".");
    }
    else 
    {
      message = message + "It is currently " ;
      message = message + tempSensorReading;
      message = message + unit + ".";
    }
    
    sendTweet(message);
    Serial.println(message);

  }
  totalRuns = totalRuns + 1;
  
  delay(240000* 7);
  //delay(10000);
}

double getDelta(double lastReading, double currentReading)
{
  if (lastReading == 0)
  {
    return 100;
  }
  return ((currentReading - lastReading) / lastReading) * 100;
}

boolean sendTweet(String message)
{

  int length = max(message.length(), 140);
  if (length == 0)
  {
    return false;
  }
  length = length + 1;
  char tweet[length];
  message.toCharArray(tweet, length);

  Serial.println(tweet);
  Serial.println("connecting ...");
  if (twitter.post(tweet)) 
  {
    // Specify &Serial to output received response to Serial.
    // If no output is required, you can just omit the argument, e.g.
    // int status = twitter.wait();
    int status = twitter.wait(&Serial);
    if (status == 200) {
    Serial.println("OK.");
    return true;
   } else {
    Serial.print("failed : code ");
     Serial.println(status);
     return false;
  }
 } 
 else 
 {
   Serial.println("connection failed.");
 }
    
  return false;
}

double getLightReading()
{
  return (double)analogRead(ANALOG_LIGHT_PIN);
}

double getTemperatureReading()
{
  initDHT();
  delay(1000);
  double temperature = (double)DHT.temperature;
  return (temperature * 1.8) + 32;
}

double getHumidityReading()
{
  initDHT();
  delay(1000);
  return (double)DHT.humidity;
}

void initDHT()
{
  int chk = DHT.read22(DHT22_PIN);

  switch (chk)
  {
    case DHTLIB_OK:
                Serial.print("OK,\t");
                break;
    case DHTLIB_ERROR_CHECKSUM:
                Serial.print("Checksum error,\t");
                break;
    case DHTLIB_ERROR_TIMEOUT:
                Serial.print("Time out error,\t");
                break;
    default:
                Serial.print("Unknown error,\t");
                break;
  }
}


String getLightLevelDescription(double lightLevel)
{  
  if (lightLevel < 10)
  {
    return "dark";
  }
  if (lightLevel < 100)
  {
    return "dim";
  }
  if (lightLevel < 200)
  {
    return "pretty bright";
  }
  if (lightLevel < 500)
  {
    return "bright";
  }
  return "very bright";
}
