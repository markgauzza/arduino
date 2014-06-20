#define ANALOG_LIGHT_PIN 0
#define LIGHT_THRESHOLD .01

double lastLightRead;


void setup()
{
  Serial.begin(9600);
  delay(2500);
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
  if (lightDelta > LIGHT_THRESHOLD || lightDelta <  LIGHT_THRESHOLD*-1)
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
