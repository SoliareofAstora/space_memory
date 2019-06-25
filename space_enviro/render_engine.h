//
// Created by overlord on 25/01/19.
//

#ifndef SPACE_ENVIRO_RENDERENGINE_H
#define SPACE_ENVIRO_RENDERENGINE_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <boost/math_fwd.hpp>
#include "Scenarios/scenario_base.h"

namespace rendering {

    class RenderEngine {
    public:


        sf::RenderWindow window = sf::RenderWindow(sf::VideoMode(1024, 1024), "Space memory");
        sf::VertexArray* vertex_array;

//        sf::Text info_text_;
        sf::Font font;

        RenderEngine(ScenarioBase* base) {

            sf::View v(sf::FloatRect(-750,-750, 1500, 1500));
            window.setView(v);

            if (!font.loadFromFile("space_enviro/PIXEARG_.TTF"))
            {
                std::cout << "Couldn't load font!" << std::endl;
            } else {
                std::cout << "Font has been loaded!" << std::endl;
            }
            vertex_array = base->InitializeVertexArray();
//            info_text_.setFont(font);
//            info_text_.setCharacterSize(16);
//            info_text_.setFillColor(sf::Color::White);
//            info_text_.setPosition(sf::Vector2f(-750,-750));
        }

        void RenderState(ScenarioBase* base) {

            window.clear();
            vertex_array = base->Render(vertex_array);
            window.draw(*vertex_array);
//            window.draw(info_text_);
            window.display();

        }
    };

} // namespace rendering
#endif //SPACE_ENVIRO_RENDERENGINE_H
