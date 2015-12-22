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
#define INTERVAL 5


int currentRead;
int totalRuns;
dht DHT;
double lastLightRead;
double lastTemperatureRead;
double lastHumidityRead;
boolean debug = false;
char buffer [40];
char tweetBuffer[120];

// Ethernet Shield Settings
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// If you don't specify the IP address, DHCP is used(only in Arduino 1.0 or later).
byte ip[] = { 192, 168, 0, 177 };

// Your Token to Tweet (get it from http://arduino-tweet.appspot.com/)
Twitter twitter("2355186536-xXWPEzUPbXpNQQd9SgxosgK7SDpsQeE4zSlWc4h");

const prog_char string_0[] PROGMEM = " #arduino bit.ly/1APgD0F";  
const prog_char string_1[] PROGMEM = "Cycled through ";  
const prog_char string_2[] PROGMEM = " light, temperature and humidity reads.";  

 PGM_P const string_table[] PROGMEM= 	   // change "string_table" name to suit
{   
  string_0,
  string_1,
  string_2
};


void setup()
{
  Serial.begin(9600);
  delay(2500);
  Ethernet.begin(mac, ip);
  Serial.println("startup");
}

void loop()
{
  Serial.print("RAM: ");
  Serial.println(freeRam());
  double sensorReading = 0;
  String message  = "";
  double delta = 0;
  String readDesc = "";
  String unit = "";
  char msgBuffer [100];
  
  if (totalRuns >= INTERVAL && totalRuns % INTERVAL == 0)
  {
    Serial.println("Reached interval");
    
    strcpy_P(msgBuffer, (char*)pgm_read_word(&(string_table[1]))); 


    char runBuf[4];
    dtostrf(totalRuns, 0, 0, runBuf);

    strcat(msgBuffer, runBuf);
    
    copyToBuffer(2);
    
    strcpy(tweetBuffer, msgBuffer);
    strcat(tweetBuffer, buffer);    

    //strcpy(tweetBuffer, buffer);
    //puts(tweetBuffer);
    Serial.println(tweetBuffer);
    sendTweet(tweetBuffer);
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

  }
  totalRuns = totalRuns + 1;
  
  if (!debug)
  {
    delay(1800000);
  }
  else
  {
    delay(5000);
  }
  memset(tweetBuffer, 0, 140);
  memset(buffer, 0, 40);
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
  message.toCharArray(tweet, length);
  copyToBuffer(0);
  strcat(tweet, buffer);  

  Serial.println(tweet);
  if (debug)
  {
    return true;
  }
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


int freeRam ()
{
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void copyToBuffer(int index)
{
  strcpy_P(buffer, (char*)pgm_read_word(&(string_table[index]))); // Necessary casts and dereferencing, just copy. 
}
