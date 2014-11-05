/*
  Kara.h
*/
#ifndef Kara_h
#define Kara_h

#include "Arduino.h"
#include "LinkedList.h"


class Card
{  
	public :
		Card();		
		
		uint32_t signature;
		char *prefix;
		char* getIntroFile();
		char* getInstructionFile();
		char* getPromptFile();
};





#endif