#ifndef Bird_H_
#define Bird_H_
#include "Animal.h"
#include "Fly.h"

class Bird : public Animal
{
public:
  Bird() : Animal() 
  {
    m_name = "Bird";
    m_locomotors.push_back(new Fly());
  }
};

#endif