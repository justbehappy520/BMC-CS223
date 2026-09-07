#ifndef Whale_H_
#define Whale_H_
#include "Fish.h"

class Whale : public Fish
{
public:
  Whale() : Fish() { m_name = "Whale";}
};

#endif