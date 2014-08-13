#include <WaveHC.h>
#include <WaveUtil.h>

WaveHC wave;
FatReader file;
FatReader root;
SdReader card;

void setup()
{
  Serial.begin(9600);
  PgmPrintln("Pi speaker");
  
  if (!card.init())
  {
    Serial.println("Card init. failed!");
  }
}

void loop()
{

}


void playfile(char *name)
{
  if (wave.isplaying)
  {
    wave.stop();
  }
  if (!file.open(root, name))
  {
    PgmPrint("Couldn't open file");
    Serial.print(name);
    return;
  }
  if (!wave.create(file))
  {
    PgmPrintln("Not a valid WAV");
    return;
  }
  wave.play();
}
