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
		Card(uint32_t  signature, char* prefix) ;
		
		char* getIntroFile();
		char* getInstructionFile();
	private :
		uint32_t  _signature;
		char* _prefix;
};


typedef struct CardMap
{
  uint32_t signature;
  Card card;
};

class CardList
{
  public :
    CardList();
    void addCard(Card card);
  private :
    Node<Card>* _cards;
    int _totalCards;
  
};




#endif