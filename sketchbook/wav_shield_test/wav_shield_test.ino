#include <LinkedList.h>



#include <WaveHC.h>
#include <WaveUtil.h>
#include <Wire.h>
#include <Adafruit_NFCShield_I2C.h>


class Card
{
  public :
    char *prefix;
    uint32_t signature;
    
};

const int buttonPin = 6; 

#define LEDPIN     7
#define IRQ 6 // this trace must be cut and rewired!
#define RESET 8


Adafruit_NFCShield_I2C nfc(IRQ, RESET);


SdReader card; // This object holds the information for the card
FatVolume vol; // This holds the formatformatinformation for the partition on the card
FatReader root; // This holds the information for the volumes root directory
FatReader file; // This object represent the WAV file for a pi digit or period
WaveHC wave; // This is the only wave (audio) object, since we will only play one at a time
/*
* Define macro to put error messPages in flash memory
*/
#define error(msg) error_P(PSTR(msg))


LinkedList<Card*> cardList = LinkedList<Card*>();


String stringOne;


//////////////////////////////////// SETUP

void setup() {
  // set up Serial library at 9600 bps
  Serial.begin(9600);
  
  initSDCard();  
  
  initNFC();

  
  Card *grandmom = new Card();
  grandmom->signature = (uint32_t)3973324075;
  char gm[] = "GM";
  grandmom->prefix = gm;


  
  Card *auntJen = new Card();
  auntJen->signature = (uint32_t)3973446155;
  char aj[] = "AJ";
  auntJen->prefix = aj;
  

  cardList.add(auntJen);
  cardList.add(grandmom);  
  
  Card *prompt;

  prompt = cardList.get(0);
  char* prefix = prompt->prefix;
  char* suffix = "_N.WAV";
  playcomplete(getFile(prefix, suffix));
  


}


char* getFile(char* prefix, char* suffix)
{
    char fileName[10];    

    strcpy(fileName, prefix);
    strcat(fileName, suffix);
    Serial.println(fileName);
    return fileName;
  
}


/////////////////////////////////// LOOP

unsigned digit = 0;

void initNFC()
{
  
  // find Adafruit RFID/NFC shield
  nfc.begin();
  Serial.println("begin");
  //nfc.setPassiveActivationRetries(0xFF);

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  // configure board to read RFID tags
  nfc.SAMConfig();
  
}

void initSDCard()
{
    PgmPrintln("Pi speaker");
  
  if (!card.init()) {
    error("Card init. failed!");
  }
  if (!vol.init(card)) {
    error("No partition!");
  }
  if (!root.openRoot(vol)) {
    error("Couldn't open dir");
  }

  PgmPrintln("Files found:");
  root.ls();
    

  
}

void loop() 
{
  
  //Card *prompt = findPrompt();
//  Serial.print("prompt");
//  Serial.println(prompt -> getPromptFile());
//  playcomplete(prompt-> getPromptFile());

  
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 }; // Buffer to store the returned UID
  uint8_t uidLength; // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

   //   rainbow(20);

  // wait for RFID card to show up!
  Serial.println("Waiting for an ISO14443A Card ...");

    
  // Wait for an ISO14443A type cards (Mifare, etc.). When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  uint32_t cardidentifier = 0;
  
  if (success) 
  {
    // Found a card!

    Serial.print("Card detected #");
    // turn the four byte UID of a mifare classic into a single variable #
    cardidentifier = uid[3];
    cardidentifier <<= 8; cardidentifier |= uid[2];
    cardidentifier <<= 8; cardidentifier |= uid[1];
    cardidentifier <<= 8; cardidentifier |= uid[0];
    Serial.println(cardidentifier);

  }
  
  if (! digitalRead(buttonPin))
  {
    Serial.print("button");
    playcomplete("AFFIRM~1.WAV");

  }  
  
//  Card *promptCard = findPrompt();

  
  delay(1000);

 // playcomplete(promptCard->getPromptFile());
  
  

}



/////////////////////////////////// HELPERS

/*
* print error message and halt
*/
void error_P(const char *str) {
  PgmPrint("Error: ");
  SerialPrint_P(str);
  sdErrorCheck();
  while(1);
}
/*
* print error message and halt if SD I/O error
*/
void sdErrorCheck(void) {
  if (!card.errorCode()) return;
  PgmPrint("\r\nSD I/O error: ");
  Serial.print(card.errorCode(), HEX);
  PgmPrint(", ");
  Serial.println(card.errorData(), HEX);
  while(1);
}
/*
* Play a file and wait for it to complete
*/
void playcomplete(char *name) 
{
  int a = 0;
  char arr[80];
  while (name[a] != '\0')
  {
    arr[a] = name[a];
     
    a++;
  }
  arr[a] = '\0';
  Serial.println(arr);
  playfile(arr);
  while (wave.isplaying);
  
  // see if an error occurred while playing
  sdErrorCheck();
}
/*
* Open and start playing a WAV file
*/
void playfile(char *name) {
  if (wave.isplaying) {// already playing something, so stop it!
    wave.stop(); // stop it
  }
  if (!file.open(root, name)) {
    PgmPrint("Couldn't open file ");
    Serial.print(name);
    return;
  }
  if (!wave.create(file)) {
    PgmPrintln("Not a valid WAV");
    return;
  }
  // ok time to play!
  wave.play();
}



