/*
  Kara.h
*/
#ifndef Kara_h
#define Kara_h

#include "Arduino.h"

class Card
{
	public :
		Card(uint32_t  signature);
	private :
		uint32_t  _signature;
};

#endif