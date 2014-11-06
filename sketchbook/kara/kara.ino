    #include <WaveHC.h>
    #include <WaveUtil.h>
    #include <Wire.h>
    #include <Adafruit_NFCShield_I2C.h>     
     
    #define IRQ 6 // this trace must be cut and rewired!
    #define RESET 8
     
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
    
    class Card
    {
      public :
        char *name;
        uint32_t signature;    
    };
    
    Card *currentCard;
    
    Card *auntLauren;

    
     
    //////////////////////////////////// SETUP
     
    void setup() 
    {
      // set up Serial library at 9600 bps
      Serial.begin(9600);
      PgmPrintln("Pi speaker");
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
      root.ls();
      // find Adafruit RFID/NFC shield
      nfc.begin();
     
      uint32_t versiondata = nfc.getFirmwareVersion();
      if (! versiondata) 
      {
        Serial.print("Didn't find PN53x board");
        while (1); // halt
      }
        // Got ok data, print it out!
        Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX);
        Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC);
        Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
        // configure board to read RFID tags
        nfc.SAMConfig();
      
       Card *grandmom = new Card();
       grandmom->signature = (uint32_t)3973483339;
       grandmom->name="GM_N.WAV";
  
  
       auntLauren = new Card();
       auntLauren->signature=(uint32_t)3973443899;
       auntLauren->name="AL_N.WAV";
       
       
       
       currentCard = grandmom;
     
    }
     
    /////////////////////////////////// LOOP
     
    unsigned digit = 0;
     
    void loop() 
    {
      
      playcomplete("AJ_P.WAV");
      playcomplete(currentCard->name);
      uint8_t success;
      uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 }; // Buffer to store the returned UID
      uint8_t uidLength; // Length of the UID (4 or 7 bytes depending on ISO14443A disk type)
     
      // wait for RFID disk to show up!
      Serial.println("Waiting for an ISO14443A disk ...");
     
      // Wait for an ISO14443A type disks (Mifare, etc.). When one is found
      // 'uid' will be populated with the UID, and uidLength will indicate
      // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
      success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
     
      uint32_t diskidentifier = 0;
      if (success) 
      {
        // Found a disk!     
        Serial.print("disk detected #");
        // turn the four byte UID of a mifare classic into a single variable #
        diskidentifier = uid[3];
        diskidentifier <<= 8; diskidentifier |= uid[2];
        diskidentifier <<= 8; diskidentifier |= uid[1];
        diskidentifier <<= 8; diskidentifier |= uid[0];
        Serial.println(diskidentifier);
     
        // repeat this for loop as many times as you have RFID disks
        if (diskidentifier == currentCard->signature) 
        { 
          playcomplete("AJ_A.WAV");
          currentCard = auntLauren;

        }
        else
        {
          playcomplete("AJ_W.WAV");
          playcomplete(currentCard->name);
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
