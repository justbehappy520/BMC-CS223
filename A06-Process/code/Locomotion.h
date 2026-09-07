#ifndef Locomotion_H_
#define Locomotion_H_
#include <string>
#include <iostream>

class Locomotion
{
public:
  virtual ~Locomotion() {}
  std::string name() { return m_name; }
  virtual void move() = 0;

protected:
  Locomotion(const std::string& name) : m_name(name) {}
  std::string m_name;
};

#endif