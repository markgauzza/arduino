#include <Boards.h>
#include <Firmata.h>
#include <SPI.h> // needed in Arduino 0019 or later
#include <Ethernet.h>
#include <Twitter.h>
#include <dht.h>


#define ANALOG_LIGHT_PIN 0
#define LIGHT_TOLERANCE .01
#define DHT22_PIN 6
#define READ_TYPE_TEMPERATURE 1
#define READ_TYPE_LIGHT 0
#define READ_TYPE_HUMIDITY 2


dht DHT;
double lastLightRead;
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
  double sensorReading = (double)analogRead(ANALOG_LIGHT_PIN);
  if (lastLightRead == 0)
  {
    lastLightRead = sensorReading;
    Serial.println("Just came online, it is currently " + getLightLevelDescription(sensorReading) + " in here.");
    return;
  }
  double lightDelta = (sensorReading-lastLightRead) / lastLightRead;
  if (lightDelta > LIGHT_TOLERANCE || lightDelta <  LIGHT_TOLERANCE*-1)
  {
    String verb = "increased";
    if (lightDelta < 0)
    {
      verb = "decreased";
    }
    Serial.print("The light level has "+verb+" by ");
    Serial.print(lightDelta, 5);
    Serial.println("%");
    Serial.println("It is now " + getLightLevelDescription(sensorReading) + ".");
    lastLightRead = sensorReading;
  }
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
