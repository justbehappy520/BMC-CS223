#ifndef Walk_H_
#define Walk_H_
#include "Locomotion.h"

class Walk : public Locomotion
{
public:
  Walk() : Locomotion("Walk") {}
  void move() override { std::cout << "Taking a walk\n"; }
};

#endif