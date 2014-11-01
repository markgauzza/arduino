/*
  Kara.cpp 
*/

#include "Arduino.h"
#include "Kara.h"
#include "LinkedList.h"

Card::Card(uint32_t  signature, char* prefix)
{
	_signature = signature;
	_prefix = prefix;
}

char* Card :: getIntroFile()
{
  char str[80];
  strcpy(str, _prefix);
  strcat(str, "_H.WAV");
  
  return str;
}

char* Card :: getInstructionFile()
{  
  char str[80];
  strcpy(str, _prefix);
  strcat(str, "_I.WAV");
  
  return str;
}




CardList :: CardList()
{
  _totalCards = 0;
  
}

void CardList :: addCard(Card card)
{
  Node<Card>* nx = create_node(card);
 
}