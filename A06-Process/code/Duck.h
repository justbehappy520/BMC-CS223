#ifndef Duck_H_
#define Duck_H_
#include "Bird.h"
#include "Swim.h"

class Duck : public Bird
{
public:
  Duck() : Bird() 
  {
    m_name = "Duck";
    m_locomotors.push_back(new Swim());
  }
};

#endif