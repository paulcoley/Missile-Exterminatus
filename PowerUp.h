#pragma once
#include "Grid.h"
#include <deque>

class PowerUp {
private:
    
public:
	PowerUp();
	~PowerUp();
    int timeActive;
    int speedModifier;
    int fire_speed_modifier;
    bool death;
    bool deliver_the_fish;
};
