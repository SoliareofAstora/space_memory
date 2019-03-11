//
// Created by overlord on 25/01/19.
//

#ifndef SPACE_ENVIRO_RENDERENGINE_H
#define SPACE_ENVIRO_RENDERENGINE_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include <boost/math_fwd.hpp>
#include <iostream>//todo delete this
#include "team_info.h"
#include "entity_manager.h"


//struct TeamInfo{
//    int team;
//    int begin;
//    int n_ships;
//    sf::Color color;
//};
namespace rendering {

    class RenderEngine {
    public:


        sf::RenderWindow window = sf::RenderWindow(sf::VideoMode(1024, 1024), "Space memory");
        sf::VertexArray ship_vertices_;
        sf::VertexArray asteroid_vertices_;
        sf::VertexArray ray_vertices_;
        sf::Text info_text_;
        sf::Font font;


        RenderEngine(std::vector<TeamInfo>a) {
            int n_ships = 0;
            for(TeamInfo team:a){
                n_ships+=team.n_ships;
            }
            ship_vertices_ = sf::VertexArray(sf::Triangles,9*n_ships);
            asteroid_vertices_ = sf::VertexArray(sf::Triangles,100*3);
            ray_vertices_ = sf::VertexArray(sf::Lines,256*2);

            for (int k = 0; k < 256; ++k) {
                ray_vertices_[k*2].position=sf::Vector2f(0,0);
                ray_vertices_[k*2].color=sf::Color::Black;
                ray_vertices_[k*2+1].position=sf::Vector2f(0,0);
            }

            for(TeamInfo team:a){
                for (int i = team.begin; i < team.begin+team.n_ships; ++i) {
                    for (int j = 6; j < 9; ++j) {
                        ship_vertices_[9*i+j].color = team.color;
                    }
                }
            }

            sf::View v(sf::FloatRect(-750,-750, 1500, 1500));
            window.setView(v);

            if (!font.loadFromFile("space_enviro/PIXEARG_.TTF"))
            {
                std::cout << "Couldn't load font!" << std::endl;
            } else {
                std::cout << "Font has been loaded!" << std::endl;
            }

            info_text_.setFont(font);
            info_text_.setCharacterSize(16);
            info_text_.setFillColor(sf::Color::White);
            info_text_.setPosition(sf::Vector2f(-750,-750));

        }

        void RenderShip(entities::EntityManager a);
        void RenderAsteroids(entities::EntityManager a);
        void RenderRays(entities::EntityManager a);
        void RenderInfo(entities::EntityManager a);

        void RenderScreen() {

            window.clear();
            window.draw(ray_vertices_);
            window.draw(ship_vertices_);
            window.draw(asteroid_vertices_);
            window.draw(info_text_);
            window.display();

        }
    };

} // namespace rendering
#endif //SPACE_ENVIRO_RENDERENGINE_H
