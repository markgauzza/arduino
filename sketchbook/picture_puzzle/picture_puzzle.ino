
#include <WaveHC.h>
    #include <WaveUtil.h>
    
    SdReader disk; // This object holds the information for the disk
    FatVolume vol; // This holds the information for the partition on the disk
    FatReader root; // This holds the information for the volumes root directory
    FatReader file; // This object represent the WAV file for a pi digit or period
    WaveHC wave; // This is the only wave (audio) object, since we will only play one at a time
    

    
    const int TOTAL_SENSORS = 5;
    uint32_t LIGHT_THRESHOLD = 200;
    
    int litPieces[TOTAL_SENSORS] = {};

    String sounds[TOTAL_SENSORS] = {"GRANDPA.WAV" "GEEGEE.WAV", "GRANNY.WAV", "MARK.WAV", "BRIAN.WAV"};

    
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
      

    }
    
  void loop()
  {
    for (int i = 0; i < TOTAL_SENSORS; i++)
    {

      int pin = i + 1;
      Serial.print("pin: ");
      Serial.println(pin);
      if (!isDark(pin))
      {
        Serial.println("light");
        litPieces[i] = pin;
      }
      else
      {
        Serial.println("Dark");
      }
      
      if (wasLit(pin))
      {
        Serial.println("was lit");
      }

      delay(500);
    }
    delay(1000);
  }
  
    boolean wasLit(int pin)
    {
      for (int i = 0; i < sizeof(litPieces); i++)
      {
        if (litPieces[i] == pin )
        {
          return true;
        }
      }
      return false;
    }
    
    boolean isDark(int pin)
    {
      int lightLevel = analogRead(pin);      
      Serial.println(lightLevel);
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

