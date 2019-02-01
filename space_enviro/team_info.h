#ifndef SPACE_ENVIRO_TEAM_INFO_H
#define SPACE_ENVIRO_TEAM_INFO_H


#include <SFML/Graphics/Color.hpp>

struct TeamInfo{
    int team;
    int begin;
    int n_ships;
    sf::Color color;
    TeamInfo(int team,int begin,int n_ships,sf::Color color):
    team(team),begin(begin),n_ships(n_ships),color(color)
    {}
};

#endif //SPACE_ENVIRO_TEAM_INFO