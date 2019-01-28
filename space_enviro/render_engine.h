//
// Created by overlord on 25/01/19.
//

#ifndef SPACE_ENVIRO_RENDERENGINE_H
#define SPACE_ENVIRO_RENDERENGINE_H

#include <SFML/Graphics.hpp>

namespace rendering {

    class RenderEngine {
    public:

        sf::RenderWindow window = sf::RenderWindow(sf::VideoMode(1000, 1000), "SFML works!");
        sf::VertexArray buffer = sf::VertexArray(sf::Triangles,9);

        RenderEngine() {
//            for (int i = 0; i < 30; ++i) {
            buffer[0] = sf::Vector2f(0,0);
            buffer[1] = sf::Vector2f(0,100);
            buffer[2] = sf::Vector2f(100,0);
            buffer[3] = sf::Vector2f(100,100);
            buffer[4] = sf::Vector2f(100,0);
            buffer[5] = sf::Vector2f(0,100);
            buffer[6] = sf::Vector2f(0,0);
            buffer[7] = sf::Vector2f(100,100);
            buffer[8] = sf::Vector2f(0,100);
            buffer[0].color = sf::Color::Red;
            buffer[3].color = sf::Color::Blue;
            buffer[6].color = sf::Color::Green;
//            }
        }

        void display() {

//            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
//                shape.setFillColor(sf::Color::Red);
//            }
//            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
//                shape.setFillColor(sf::Color::Blue);
//            }
//            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
//                shape.setFillColor(sf::Color::Green);
//            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape) && window.hasFocus()) {
                window.close();
            }
            window.clear();
            window.draw(buffer);
            window.display();
        }

    };

} // namespace rendering
#endif //SPACE_ENVIRO_RENDERENGINE_H
