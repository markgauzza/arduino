#include <Wire.h>

#include <Adafruit_NFCShield_I2C.h>

#define IRQ 2 // this trace must be cut and rewired!
#define RESET 8
#define BLUE_PIN 8
#define YELLOW_PIN 9
#define BLUE_CARDID 3973483339
#define YELLOW_CARDID 3973446155
 
Adafruit_NFCShield_I2C nfc(IRQ, RESET);


void setup() 
{
  // set up Serial library at 9600 bps
  Serial.begin(9600);
  Serial.println("started");
  
  nfc.begin();
 
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
  
  pinMode(BLUE_PIN, OUTPUT);
  digitalWrite(BLUE_PIN, LOW);
  
  pinMode(YELLOW_PIN, OUTPUT);
  digitalWrite(YELLOW_PIN, LOW);
}

void loop() 
{
  
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 }; // Buffer to store the returned UID
  uint8_t uidLength; // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  // wait for RFID card to show up!
  Serial.println("Waiting for an ISO14443A Card ...");
  digitalWrite(BLUE_PIN, LOW);
  digitalWrite(YELLOW_PIN, LOW);
 
      
  // Wait for an ISO14443A type cards (Mifare, etc.). When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
 
  uint32_t cardidentifier = 0;
  
  if (success) 
  {
    // Found a card!
 
    Serial.print("Card detected #");
     cardidentifier = uid[3];
    cardidentifier <<= 8; cardidentifier |= uid[2];
    cardidentifier <<= 8; cardidentifier |= uid[1];
    cardidentifier <<= 8; cardidentifier |= uid[0];
    Serial.println(cardidentifier);
    if(cardidentifier==BLUE_CARDID)
    {
      digitalWrite(BLUE_PIN, HIGH);
      Serial.println("blue");
    }
    else if (cardidentifier == YELLOW_CARDID)
    {
      digitalWrite(YELLOW_PIN, HIGH);
    }
    
    
  }
  else
  {
    digitalWrite(BLUE_PIN, LOW);
  }
  
  delay(1000);
  
}
 
