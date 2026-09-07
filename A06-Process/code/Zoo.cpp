#include "Animal.h"
#include "Bird.h"
#include "Duck.h"
#include "Fish.h"
#include "Seal.h"
#include "Whale.h"

#include "Locomotion.h"
#include "Walk.h"
#include "Fly.h"
#include "Swim.h"

int main()
{
    std::vector<Animal*> zoo;
    zoo.push_back(new Whale()); 
    zoo.push_back(new Duck()); 
    zoo.push_back(new Fish()); 
    zoo.push_back(new Bird()); 
    zoo.push_back(new Seal()); 

    for (Animal* a : zoo)
    {
        a->move("Fly");
    }

    for (Animal* a : zoo)
    {
        delete a;
    } 
    zoo.clear();
    return 0;
}
