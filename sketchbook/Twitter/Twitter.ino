#include <Boards.h>
#include <Firmata.h>

#include <SPI.h> // needed in Arduino 0019 or later
#include <Ethernet.h>
#include <Twitter.h>
#include <dht.h>

dht DHT;

#define DHT22_PIN 6

// The includion of EthernetDNS is not needed in Arduino IDE 1.0 or later.
// Please uncomment below in Arduino IDE 0022 or earlier.
//#include <EthernetDNS.h>


// Ethernet Shield Settings
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// If you don't specify the IP address, DHCP is used(only in Arduino 1.0 or later).
byte ip[] = { 192, 168, 1, 177 };

// Your Token to Tweet (get it from http://arduino-tweet.appspot.com/)
Twitter twitter("2355186536-xXWPEzUPbXpNQQd9SgxosgK7SDpsQeE4zSlWc4h");

// Message to postS

double lastHumidity;
double lastTemperature;
double lastLight;

void setup()
{
  delay(1000);
  Ethernet.begin(mac, ip);
  // or you can use DHCP for autoomatic IP address configuration.
  // Ethernet.begin(mac);
  Serial.begin(9600);
  
 
  
}

void loop()
{
  String message = getHumidityTweet();  
  
  delay(1000);
  if (message.length() == 0)
  {
    message = getLightTweet();
  }
  
  int len = message.length();
  
  
  if (len == 0)
  {
    delay(10000);
    return;
  }
  
  char tweet[len + 1];
  for (int a = 0; a < len; a++)
  {
    tweet[a] = message[a];
  }
  Serial.println("Tweet: ");
  Serial.print(tweet);
  
  Serial.println("connecting ...");
  if (twitter.post(tweet)) 
  {
    // Specify &Serial to output received response to Serial.
    // If no output is required, you can just omit the argument, e.g.
    // int status = twitter.wait();
    int status = twitter.wait(&Serial);
    if (status == 200) {
    Serial.println("OK.");
   } else {
    Serial.print("failed : code ");
     Serial.println(status);
  }
 } else {
   Serial.println("connection failed.");
 }
  
    delay(10000);
  
}

String getLightTweet()
{
   int sensorReading = analogRead(0);
   
   if (lastLight == 0)
   {
     lastLight = sensorReading;
     return "";
   }
   
   
   
   double lightDelta = (double)sensorReading / (double)lastLight;
   Serial.print("light value: ");
   Serial.println(sensorReading);
        Serial.println(lightDelta, 5);
        
   String lightMessage= formatRiseFallMessage((double)sensorReading, (double)lastLight, "light level", 0.5);
   if (lightMessage.length() > 0)        
   {
     lastLight = sensorReading;
   }
   else
   {
     return "";
   }
   
   
   String msg = "The light level has changed.  It is currently ";  
   if (sensorReading < 10) 
   {
      msg = msg + "dark.";
   }
   else if (sensorReading < 100)
   {
      msg = msg + "dim.";
   }
   else if (sensorReading < 200)
   {
     msg = msg + "pretty bright.";
   }
   else if (sensorReading < 500) 
   {
      msg = "bright.";
   }
   else 
   {
      msg = "very bright";
   }
   return msg;
}

String getHumidityTweet()
{
  
  Serial.print("DHT22, \t");
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
  
  double currentHumidity = (double)DHT.humidity;
  double currentTemperature = (double)DHT.temperature;
  
  if (lastHumidity == 0)
  {
    lastHumidity = currentHumidity;
  }
  if (lastTemperature == 0)
  {
    lastTemperature = currentTemperature;
  }
  
  String envMessage = "";

  envMessage = formatRiseFallMessage(currentHumidity, lastHumidity, "humidity", 0.08);
  if (envMessage.length() == 0)
  {
    envMessage = formatRiseFallMessage(currentTemperature, lastTemperature, "temperature", 0.08);
    if (envMessage.length() > 0)
    {
      lastTemperature = currentTemperature;
    }
  }
  else
  {
    lastHumidity = currentHumidity;
  }
  


   // DISPLAY DATA
  Serial.print(DHT.humidity, 1);
  Serial.print(",\t");
  Serial.println(DHT.temperature, 1);
  //Serial.println(message);
  return envMessage;

}

String formatRiseFallMessage(double currentValue, double lastValue, String term, double tolerance)
{
  if (lastValue == 0)
  {
    return "";
  }
  Serial.print("change in " + term + ": ");
  float delta = (currentValue / lastValue) - 1.0;
  Serial.println(delta, 10);
  String verb = "";
  String label = "The " + term;
  char tmp[10];
  
  if (delta > tolerance)
  {
    verb = " risen ";    
  }
  else if (delta < (tolerance * -1.0))
  {
    verb = " fallen ";
  }
  
  if (verb.length() == 0)
  {
    return "";
  }
  
  dtostrf(currentValue, 1,2,tmp);    
  
  label = label + " has " + verb + " to " +tmp;
  return label;
}


