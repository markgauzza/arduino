/*
  Kara.cpp 
*/

#include "Arduino.h"
#include "Kara.h"
#include "LinkedList.h"

Card::Card()
{
}

char* Card :: getIntroFile()
{
  char str[80];
  strcpy(str, prefix);
  strcat(str, "_H.WAV");
  
  return str;
}

char* Card :: getInstructionFile()
{  
  char str[80];
  strcpy(str, prefix);
  strcat(str, "_I.WAV");
  
  return str;
}

char* Card :: getPromptFile()
{
  char str[80];
  strcpy(str, prefix);
  strcat(str, "_P.WAV");
  return str;
}


