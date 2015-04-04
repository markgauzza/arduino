#include <WaveHC.h>
    #include <WaveUtil.h>
    #include <Wire.h>
    #include <Adafruit_NFCShield_I2C.h>   

         
    #define IRQ 6 // this trace must be cut and rewired!
    #define RESET 8
    #define MODE_COLORS  2
    #define MODE_NUMBERS  1
    #define MODE_NAMES  0
    #define BUTTON_PIN 8
    #define GAME_PIN 7
    

    int currentMode = MODE_NAMES;
    byte isGameMode = 0;
   
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
    
    uint32_t cards[] = {3496599971, 3584577213, 3492105171, 372801280, 17936672, 17931312,383666656,383756576, 384227840,372429680 };
   
    uint32_t currentCard;
    
    int currentIndex = 0;
    int totalCards = 0;
    int readAttempts = 0;
    int buttonPushes = 0;
    
    const prog_char string_0[] PROGMEM = "NAMES.WAV";   // "String 0" etc are strings to store - change to suit.
    const prog_char string_1[] PROGMEM = "NUMBERS.WAV";
    const prog_char string_2[] PROGMEM = "COLORS.WAV";
    const prog_char string_3[] PROGMEM = "4075_P.WAV";
    const prog_char string_4[] PROGMEM = "AJ_A.WAV";
    const prog_char string_5[] PROGMEM = "AJ_W.WAV";
    const prog_char string_6[] PROGMEM = "_N.WAV";
    const prog_char string_7[] PROGMEM = "_C.WAV";
    const prog_char string_8[] PROGMEM = "_D.WAV";
    const prog_char string_9[] PROGMEM = "_S.WAV";
    const prog_char string_10[] PROGMEM ="AJ_P.WAV";
    const prog_char string_11[] PROGMEM ="EM_CI.WAV";
    const prog_char string_12[] PROGMEM = "EM_GI.WAV";


// Then set up a table to refer to your strings.

PGM_P const string_table[] PROGMEM = 	   // change "string_table" name to suit
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
  string_9,
  string_10,
  string_11,
  string_12
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

        nfc.setPassiveActivationRetries(0x002);
        // configure board to read RFID tags
        nfc.SAMConfig();
       
       delay(500);
      
       pinMode(BUTTON_PIN, INPUT_PULLUP);
       pinMode(GAME_PIN, INPUT_PULLUP);
   
       playPrompt();
     
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
          readAttempts = 0;
          buttonPushes ++;
          currentMode++;
          if (currentMode > MODE_COLORS)
          {
            currentMode = MODE_NAMES;
          }          
          playIndex(currentMode);


          delay(500);
          return;
      }
      
      if (!digitalRead(GAME_PIN))
      {
        if (isGameMode == 1)
        {         
          isGameMode = 0;
          playPrompt();
        }
        else
        {
          playIndex(12);
          isGameMode = 1;
        }
        readAttempts = 0;
        buttonPushes++;
        
      }
      
      if (readAttempts == 0)
      {
        if (isGameMode == 1)
        {
          playPrompt();
        }
      }           
      else if (readAttempts % 1024 == 0 || buttonPushes > 2)
      {
        buttonPushes = 0;
        playRandomSong();
        delay(100);
        playPrompt();
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
        readAttempts = 0;
        buttonPushes = 0;
        
        // Found a card!     
        PgmPrint("card detected # ");

        // turn the four byte UID of a mifare classic into a single variable #
        cardidentifier = uid[3];
        cardidentifier <<= 8; cardidentifier |= uid[2];
        cardidentifier <<= 8; cardidentifier |= uid[1];
        cardidentifier <<= 8; cardidentifier |= uid[0];      
        Serial.println(cardidentifier);
     
        if (isGameMode == 0)
        {
          playCardMode(cardidentifier);
          if (currentMode == MODE_NAMES)
          {
            playCardFile(cardidentifier, 9);
          }
        }
        else if (cardidentifier == currentCard) 
        { 
          // Game mode
          PgmPrintln("correct");
          playIndex(4);         

         
          if (currentIndex == (totalCards - 1))
          {
            PgmPrintln("Reached end of list");
            currentIndex = 0;            
          }
          else
          {
            if (currentMode == MODE_NUMBERS)
            {
              currentIndex++;
            }
            else
            {
              int randomNumber = random(0, totalCards);
              if (randomNumber == currentIndex)
              {
                randomNumber ++;
              }
              currentIndex = randomNumber;
            }
          }
          
          if (currentMode == MODE_NAMES)
          {
            playCardFile(cardidentifier, 9);
          }

        }
        // Incorrect card
        else
        {
          playIndex(5);
          playCardMode(currentCard);

          
        }
      }
      else      
      {
        readAttempts ++;
      }
      
      delay(10);
      
    } // End method loop
    
    void playRandomSong()
    {
      int randomNumber = random(0, totalCards);
      playCardFile(cards[randomNumber], 9);
      
    } // End method playRandomSong
    
    void playCardMode(uint32_t cardidentifier)
    {
          switch(currentMode)
          {
            case MODE_NAMES:
              playCardFile(cardidentifier, 6);
              break;
            case MODE_COLORS:
              playCardFile(cardidentifier, 7);
              break;              
            case MODE_NUMBERS:
              playCardFile(cardidentifier, 8);
              break;              
           
          }   
    }
    
    void playPrompt()
    {
      if (isGameMode == 0)
      {
        playIndex(11);
        return;
      }
      // Play prompt
      if (currentIndex > 0 && currentIndex % 2 == 0)
      {
        playIndex(10);
      }
      else
      {
        playIndex(3);
      }      
 
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
      delay(100);
      
    } // End method playPrompt
    
     
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
    PgmPrintln("Couldn't open file ");
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
