 
#include <WaveHC.h>
    #include <WaveUtil.h>
    
    SdReader disk; // This object holds the information for the disk
    FatVolume vol; // This holds the information for the partition on the disk
    FatReader root; // This holds the information for the volumes root directory
    FatReader file; // This object represent the WAV file for a pi digit or period
    WaveHC wave; // This is the only wave (audio) object, since we will only play one at a time
    
    
    const prog_char string_0[] PROGMEM = "FF.WAV";   // "String 0" etc are strings to store - change to suit.
    const prog_char string_1[] PROGMEM = "EM_FF.WAV";
    
    PGM_P const string_table[] PROGMEM = 	   // change "string_table" name to suit
    {   
      string_0,
      string_1,
    };
    
    char buffer[15]; 
    
    const int TOTAL_SENSORS = 5;
    uint32_t LIGHT_THRESHOLD = 100;
    
    int litPieces[TOTAL_SENSORS] = {0, 0, 0, 0, 0};

    char* sounds[TOTAL_SENSORS] = {"GRANDPA.WAV", "GEEGEE.WAV", "GRANNY.WAV", "MARK.WAV", "BRIAN2.WAV"};

    
    #define error(msg) error_P(PSTR(msg))

    void setup() 
    {
      // set up Serial library at 9600 bps
      Serial.begin(9600);
      
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
      
      playcomplete("FF.WAV");
    }
    
  void loop()
  {
    boolean pieceOn = false;
    for (int p = 0; p < TOTAL_SENSORS; p++)
    {

      int pin = p + 1;
      Serial.print("pin: ");
      Serial.println(pin);
      pieceOn = isDark(pin);
      if (!pieceOn)
      {
        litPieces[p] = pin;
        Serial.print("Piece ");
        Serial.print(p);
        Serial.println(" is off");        
      }
      else
      {
        Serial.print("Piece ");
        Serial.print(p);
        Serial.println(" is on");        


      }
      
      if (wasLit(pin) && pieceOn)
      {

        playcomplete(sounds[p]);
        litPieces[p] = 0;
        copyStringOutOfMemory(0);
        playcomplete(buffer);
        clearBuffer();
        delay(10);
       // copyStringOutOfMemory(1);
        playcomplete("EM_FF.WAV");
//        clearBuffer();
        delay(100);
        
      }
      
      
    }
    delay(100);

  }
  
  void clearBuffer()
  {
    strcpy(buffer, "");
  }
  
    boolean wasLit(int pin)
    {
      for (int j = 0; j < TOTAL_SENSORS; j++)
      {        
        if (litPieces[j] == pin )
        {
          return true;
        }
      }
      return false;
    }
    
    boolean isDark(int pin)
    {
      int lightLevel = analogRead(pin);      
     
      return lightLevel < LIGHT_THRESHOLD;
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
    
        
    void copyToBuffer(int index)
    {
      strcpy_P(buffer, (char*)pgm_read_word(&(string_table[index]))); // Necessary casts and dereferencing, just copy. 
    }
    
    void copyStringOutOfMemory(int index)
    {     

      char extBuffer[8];
      
      strcpy_P(extBuffer, (char*)pgm_read_word(&(string_table[index]))); // Necessary casts and dereferencing, just copy. 
      strcat(buffer, extBuffer);
      
    }

