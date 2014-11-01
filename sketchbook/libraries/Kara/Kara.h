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
		Card(uint32_t  signature);
	private :
		uint32_t  _signature;
};


typedef struct CardMap
{
  uint32_t signature;
  Card card;
};

class CardList
{
  public :
    void addCard(Card card);
  private :
    CardMap _cards;
  
};




#endif