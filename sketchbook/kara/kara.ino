#include <WaveHC.h>
    #include <WaveUtil.h>
    #include <Wire.h>
    #include <Adafruit_NFCShield_I2C.h>   

         
    #define IRQ 6 // this trace must be cut and rewired!
    #define RESET 8
    #define MODE_COLORS  2
    #define MODE_NUMBERS  1
    #define MODE_NAMES  0
    #define BUTTON_PIN 6
    

    int currentMode = MODE_NAMES;
   
    Adafruit_NFCShield_I2C nfc(IRQ, RESET);
     
    SdReader disk; // This object holds the information for the disk
    FatVolume vol; // This holds the information for the partition on the disk
    FatReader root; // This holds the information for the volumes root directory
    FatReader file; // This object represent the WAV file for a pi digit or period
    WaveHC wave; // This is the only wave (audio) object, since we will only play one at a time
    /*
    * Define macro to put error messages in flash memory
    */
    

    #define error(msg) error_P(PSTR(msg))
    
    uint32_t cards[] = {3973351563, 3973371115, 3973446219, 3973369115, 3973446155,3973483131,3973483339, 3973443899,3973352907 };
   
    uint32_t currentCard;
    
    int currentIndex = 0;
    int totalCards = 0;
    
    prog_char string_0[] PROGMEM = "NAMES.WAV";   // "String 0" etc are strings to store - change to suit.
    prog_char string_1[] PROGMEM = "NUMBERS.WAV";
    prog_char string_2[] PROGMEM = "COLORS.WAV";
    prog_char string_3[] PROGMEM = "AL_P.WAV";
    prog_char string_4[] PROGMEM = "AJ_A.WAV";
    prog_char string_5[] PROGMEM = "AJ_W.WAV";
    prog_char string_6[] PROGMEM = "_N.WAV";
    prog_char string_7[] PROGMEM = "_C.WAV";
    prog_char string_8[] PROGMEM = "_D.WAV";
    prog_char string_9[] PROGMEM = "_S.WAV";


// Then set up a table to refer to your strings.

PROGMEM const char *string_table[] = 	   // change "string_table" name to suit
{   
  string_0,
  string_1,
  string_2,
  string_3,
  string_4,
  string_5,
  string_6,
  string_7,
  string_8,
  string_9
};

char buffer[15];    // make sure this is large enough for the largest string it must hold
    
     
    //////////////////////////////////// SETUP
     
    void setup() 
    {
      // set up Serial library at 9600 bps
      Serial.begin(9600);
      
      totalCards = sizeof(cards) / (sizeof(cards[0]));

//      PgmPrintln("Pi speaker");
      if (!disk.init()) 
      {
        error("disk init. failed!");
      }
      if (!vol.init(disk)) 
      {
        error("No partition!");
      }
      if (!root.openRoot(vol)) 
      {
        error("Couldn't open dir");
      }
     
      PgmPrintln("Files found:");      
      // find Adafruit RFID/NFC shield
      nfc.begin();

      uint32_t versiondata = nfc.getFirmwareVersion();
      if (! versiondata) 
      {
        PgmPrintln("Didn't find PN53x board");
        while (1); // halt
      }
        // Got ok data, print it out!
        PgmPrintln("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX);
        PgmPrintln("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC);
        Serial.println('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
        // configure board to read RFID tags
        nfc.SAMConfig();
       
      
      
       pinMode(BUTTON_PIN, INPUT_PULLUP);

   
     
    }
    
    void copyToBuffer(int index)
    {
      strcpy_P(buffer, (char*)pgm_read_word(&(string_table[index]))); // Necessary casts and dereferencing, just copy. 
    }
    
    void playIndex(int index)
    {
      copyToBuffer(index);
      delay(10);
      playcomplete(buffer);
      delay(50);
    }
    
    void buildCardFileName(uint32_t cardIdentifier, int index)
    {
      char idBuffer[16];


      String strCardId = String(cardIdentifier);
      int length = strCardId.length();
      
      strCardId.substring(length - 4).toCharArray(idBuffer, 10);
      strcpy(buffer, idBuffer);

      char extBuffer[6];
      
      strcpy_P(extBuffer, (char*)pgm_read_word(&(string_table[index]))); // Necessary casts and dereferencing, just copy. 
      strcat(buffer, extBuffer);
      
    }
    
    void playCardFile(uint32_t cardIdentifier, int index)
    {
      buildCardFileName(cardIdentifier, index);
      delay(10);
      playcomplete(buffer);
    }
    
    
     
    /////////////////////////////////// LOOP
     
    unsigned digit = 0;
     
    void loop() 
    {
       currentCard = (uint32_t)cards[currentIndex];

      if (!digitalRead(BUTTON_PIN))
      {
          currentMode++;
          if (currentMode > MODE_COLORS)
          {
            currentMode = MODE_NAMES;
          }          
          playIndex(currentMode);


          delay(500);
          return;
      }
      
      
      // Play prompt

      playIndex(3);
      
      
      

      switch (currentMode)
      {
        case MODE_NAMES:
          playCardFile(currentCard, 6);
          break;
        case MODE_COLORS:  
          playCardFile(currentCard, 7);
          break;
        case MODE_NUMBERS:
          playCardFile(currentCard, 8);
          break;
      }
      





     uint8_t success;
     uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 }; // Buffer to store the returned UID
     uint8_t uidLength; // Length of the UID (4 or 7 bytes depending on ISO14443A disk type)
      
      // wait for RFID disk to show up!
      //Serial.println("Waiting for an ISO14443A disk ...");
     
      // Wait for an ISO14443A type disks (Mifare, etc.). When one is found
      // 'uid' will be populated with the UID, and uidLength will indicate
      // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
      success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);         
    

     
      uint32_t cardidentifier = 0;
      if (success) 
      {
        // Found a card!     
        PgmPrint("card detected # ");
        // turn the four byte UID of a mifare classic into a single variable #
        cardidentifier = uid[3];
        cardidentifier <<= 8; cardidentifier |= uid[2];
        cardidentifier <<= 8; cardidentifier |= uid[1];
        cardidentifier <<= 8; cardidentifier |= uid[0];      

     
        // Correct card
        if (cardidentifier == currentCard) 
        { 
          PgmPrintln("correct");
          playIndex(4);
          

          Serial.println(currentIndex <= totalCards - 1);
          if (currentIndex == (totalCards - 1))
          {
            PgmPrintln("Reached end of list");
            currentIndex = 0;            
          }
          else
          {
            currentIndex++;
          }
                    Serial.println(currentIndex);
          
          playCardFile(cardidentifier, 9);

        }
        // Incorrect card
        else
        {
          playIndex(5);
          
          switch(currentMode)
          {
            case MODE_NAMES:
              playCardFile(currentCard, 6);
              break;
            case MODE_COLORS:
              playCardFile(currentCard, 7);
              break;              
            case MODE_NUMBERS:
              playCardFile(currentCard, 8);
              break;              
           
          }   
          
        }
      }
      
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
    if (!disk.errorCode()) return;
    PgmPrint("\r\nSD I/O error: ");
    Serial.print(disk.errorCode(), HEX);
    PgmPrint(", ");
    Serial.println(disk.errorData(), HEX);
    while(1);
    }
    /*
    * Play a file and wait for it to complete
    */
    void playcomplete(char *name) {
    playfile(name);
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
