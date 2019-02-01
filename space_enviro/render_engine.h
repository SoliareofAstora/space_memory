//
// Created by overlord on 25/01/19.
//

#ifndef SPACE_ENVIRO_RENDERENGINE_H
#define SPACE_ENVIRO_RENDERENGINE_H

#include <SFML/Graphics.hpp>
#include <math.h>
#include <boost/math_fwd.hpp>
#include <iostream>//todo delete this
#include "team_info.h"

#define PI 3.14159265

//struct TeamInfo{
//    int team;
//    int begin;
//    int n_ships;
//    sf::Color color;
//};
namespace rendering {

    class RenderEngine {
    public:

        sf::RenderWindow window = sf::RenderWindow(sf::VideoMode(1024, 1024), "SFML works!");
        sf::VertexArray ship_vertices_;
        sf::Clock frame_clock_;



        RenderEngine(std::vector<TeamInfo>a) {
            int n_ships = 0;
            for(TeamInfo team:a){
                n_ships+=team.n_ships;
            }
            ship_vertices_ = sf::VertexArray(sf::Triangles,9*n_ships);

            for(TeamInfo team:a){
                std::cout<<team.begin;
                for (int i = team.begin; i < team.begin+team.n_ships; ++i) {
                    for (int j = 6; j < 9; ++j) {
                        ship_vertices_[9*i+j].color = team.color;
                    }
                }
            }
        }

        void RenderShip(int i, float arr[]);

        void RenderScreen() {

            window.clear();
            window.draw(ship_vertices_);
            window.display();

        }
    };

} // namespace rendering
#endif //SPACE_ENVIRO_RENDERENGINE_H
