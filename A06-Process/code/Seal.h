#ifndef Seal_H_
#define Seal_H_
#include "Animal.h"
#include "Swim.h"
#include "Walk.h"

class Seal : public Animal
{
public:
  Seal() : Animal() 
  {
    m_name = "Seal";
    m_locomotors.push_back(new Swim());
    m_locomotors.push_back(new Walk());
  }
};

#endif