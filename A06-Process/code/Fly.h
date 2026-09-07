#ifndef Fly_H_
#define Fly_H_
#include "Locomotion.h"

class Fly : public Locomotion
{
public:
  Fly() : Locomotion("Fly") {}
  void move() override { std::cout << "Flying around\n"; }
};

#endif