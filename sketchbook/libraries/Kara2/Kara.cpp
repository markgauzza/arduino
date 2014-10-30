/*
  Kara.cpp 
*/

#include "Arduino.h"
#include "Kara.h"
#include "LinkedList.h"

Card::Card(uint32_t  signature)
{
	_signature = signature;
}

CardList :: CardList()
{
  _totalCards = 0;
  
}

void CardList :: addCard(Card card)
{
  Node<Card>* nx = create_node(card);
 
}