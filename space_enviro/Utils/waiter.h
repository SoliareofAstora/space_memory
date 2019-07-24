//
// Created by overlord on 14/06/19.
//

#ifndef SPACE_ENVIRO_WAITER_H
#define SPACE_ENVIRO_WAITER_H

#include <SFML/System/Clock.hpp>

struct Waiter{
    int dtime;
    sf::Clock clock;

    Waiter(int miliseconds){
        dtime = miliseconds;
    }

    void wait(){
        while(clock.getElapsedTime().asMilliseconds()<dtime){}
        clock.restart();
    }
};

#endif //SPACE_ENVIRO_WAITER_H
