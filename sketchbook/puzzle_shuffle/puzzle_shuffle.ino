 
#include <WaveHC.h>
    #include <WaveUtil.h>
    
    SdReader disk; // This object holds the information for the disk
    FatVolume vol; // This holds the information for the partition on the disk
    FatReader root; // This holds the information for the volumes root directory
    FatReader file; // This object represent the WAV file for a pi digit or period
    WaveHC wave; // This is the only wave (audio) object, since we will only play one at a time
    

    #define TOTAL_SENSORS 5
    #define MODE_GAME 1
    #define MODE_PUZZLE 0
    #define DEBUG 1
    #define FANFARE_INDEX 0
    #define PROMPT_COUNT 75
    #define DIDIT_INDEX  1
    #define BUZZER_INDEX 3
    
    int currentMode = MODE_GAME;
    
    boolean shuffled = false;
    
    int correct = 0;
    
    const prog_char string_0[] PROGMEM = "FF.WAV";   // "String 0" etc are strings to store - change to suit.
    const prog_char string_1[] PROGMEM = "EM_FF.WAV";
    const prog_char string_2[] PROGMEM = "REMOVE.WAV";
    const prog_char string_3[] PROGMEM = "BUZZ.WAV";
    const prog_char string_4[] PROGMEM = "BACK.WAV";
    
   
    
    PGM_P const string_table[] PROGMEM = 	   // change "string_table" name to suit
    {   
      string_0,
      string_1,
      string_2,
      string_3,
      string_4
    };
    
    char buffer[15]; 
    
    int removePromptCount = 0;

    uint32_t LIGHT_THRESHOLD = 70;
    
    int litPieces[TOTAL_SENSORS] = {0,0, 0, 0, 0};
    
    int randomSequence[TOTAL_SENSORS] = {3, 1, 2, 0, 4};

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
      
      if (debug())
      {
        PgmPrintln("Files found:");     
        root.ls(); 
        PgmPrintln("Initial light readings");
        for (int l = 0; l < TOTAL_SENSORS; l++)
        {
          Serial.print(l);
          PgmPrint(": ");
          Serial.println(analogRead(l + 1));
        }
      }
      
      playIndex(FANFARE_INDEX);
    }
    
    boolean debug()
    {
      return DEBUG == 1;
    }
    
  void loop()
  {
    if (currentMode == MODE_PUZZLE)
    {
      doPuzzle();
    }
    else
    {
      if (!shuffled && !allPiecesOff())
      {
        if (removePromptCount % PROMPT_COUNT == 0)
        {
          playIndex(2);
        }
        removePromptCount ++;
      } 
      else
      {          
          if (!shuffled)
          {
            PgmPrintln("Ready for sequence");
            shuffleSequence();
            playIndex(4);
            delay(500);
            for (int s = 0; s< TOTAL_SENSORS; s++)
            {
              int seq = randomSequence[s];
              if (debug())
              {
                PgmPrint("playing sequence");  
                Serial.println(seq);
              }
            
              playcomplete(sounds[seq]);
              delay(150);
            }
         }
         
         int firstCorrect = randomSequence[correct];
         if (correct == TOTAL_SENSORS)
         {
           playIndex(DIDIT_INDEX);
           resetGame();
        
         }
         else if (isDark(firstCorrect + 1))
         {
          playIndex(FANFARE_INDEX);         
          correct ++;
         }
         else if (!allPiecesOff())
         {
           if (correct == 0)
           {
             playIndex(BUZZER_INDEX);
             resetGame();
           }
           else
           {
             if (totalPiecesOn() > correct)
             {
               playIndex(BUZZER_INDEX);
               resetGame();
               delay(100);
             }
           }
         }
      }
      
    }
    delay(100);

  }
  
  void resetGame()
  {
    removePromptCount = 0;
    shuffled = 0;
    correct = 0;
  }
  
  boolean sequenceExists(int seed)
  {
    for (int s = 0; s < TOTAL_SENSORS; s++)
    {
      if (randomSequence[s] == seed)
      {
        if (debug())
        {
          Serial.print(seed);
          PgmPrint("exists");
        }
        return true;
      }
    }
    
    if (debug())
    {    
      Serial.print(seed);
      PgmPrintln("doesn't exist");
    }
    delay(100);
    return false;
  }
  
  void shuffleSequence()
  {
    int r = 0;
    for (int a=0; a<TOTAL_SENSORS; a++)
    {
       r = random(a,TOTAL_SENSORS - 1);// dont remember syntax just now, random from a to 8 included.
       int temp = randomSequence[a];
       randomSequence[a] = randomSequence[r];
       randomSequence[r] = temp;
    }
    
    shuffled = true;
  }
 
  int totalPiecesOn()
  {
    int result = 0;
    for (int z = 0; z< TOTAL_SENSORS; z++)
    {
      int pin = z + 1;
      if (isDark(pin))
      {
        result ++;
      }
    }
    return result;
  }
   
  
  boolean allPiecesOff()
  {
    return totalPiecesOn() == 0;
  }
  
  void doPuzzle()
  {
        boolean pieceOn = false;
    for (int p = 0; p < TOTAL_SENSORS; p++)
    {

      int pin = p + 1;
      //rial.print("pin: ");
//      Serial.println(pin);
      pieceOn = isDark(pin);
      if (!pieceOn)
      {
        litPieces[p] = pin;
  //      Serial.print("Piece ");
//       Serial.print(p);
 //       Serial.println(" is off");        
      }
  /*    else
      {
        Serial.print("Piece ");
        Serial.print(p);
        Serial.println(" is on");        


      }*/
      
      if (wasLit(pin) && pieceOn)
      {

        playIndex(FANFARE_INDEX);
        playIndex(DIDIT_INDEX);
        delay(100);
        
      }
      
      
    }
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
        
    void playIndex(int index)
    {
      copyToBuffer(index);
      delay(10);
      playcomplete(buffer);
      delay(50);
    }

