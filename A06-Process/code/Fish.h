#ifndef Fish_H_
#define Fish_H_
#include "Animal.h"
#include "Swim.h"

class Fish : public Animal
{
public:
  Fish() : Animal() 
  {
    m_name = "Fish";
    m_locomotors.push_back(new Swim());
  }
};

#endif