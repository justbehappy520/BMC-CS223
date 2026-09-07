#ifndef Animal_H_
#define Animal_H_

#include <string>
#include <vector>
#include "Locomotion.h"

class Animal
{
public:
  Animal() : m_name("Animal") {}

  virtual ~Animal() 
  { 
    for (Locomotion* loco : m_locomotors)
    {
        delete loco;
    }
    m_locomotors.clear();
  }

  void move(const std::string& ability) 
  {
    bool found = false;
    for (Locomotion* loco : m_locomotors)
    {
        if (loco->name() == ability)
        {
            std::cout << name() << " is ";
            loco->move();
            found = true;
            break;
        }
    }

    if (!found)
    {
        std::cout << name() << " cannot " << ability << std::endl;
    }
  }
  
  std::string name() const { return m_name; }

protected:

  std::vector<Locomotion*> m_locomotors;
  std::string m_name;
};

#endif