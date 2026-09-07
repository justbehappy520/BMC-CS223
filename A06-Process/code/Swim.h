#ifndef Swim_H_
#define Swim_H_
#include "Locomotion.h"

class Swim : public Locomotion
{
public:
  Swim() : Locomotion("Swim") {}
  void move() override { std::cout << "Taking a swim\n"; }
};

#endif