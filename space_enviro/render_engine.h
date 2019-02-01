//
// Created by overlord on 25/01/19.
//

#ifndef SPACE_ENVIRO_RENDERENGINE_H
#define SPACE_ENVIRO_RENDERENGINE_H

#include <SFML/Graphics.hpp>
#include <math.h>
#include <boost/math_fwd.hpp>

#include <iostream>

#define PI 3.14159265

namespace rendering {

    class RenderEngine {
    public:

        sf::RenderWindow window = sf::RenderWindow(sf::VideoMode(1000, 1000), "SFML works!");
        sf::VertexArray buffer = sf::VertexArray(sf::Triangles,9);

        RenderEngine() {

            window.setFramerateLimit(60);
            float arr[7]={500,500,190,200,0,0,0};
            arr[2] = arr[2]*PI/180;
            int i = 0;
            sf::Vector2f tmp = sf::Vector2f((-1./3.)*arr[3]*sin(arr[2])+arr[0],(-1./3.)*arr[3]*cos(arr[2])+arr[1]);
            buffer[i+0]=sf::Vector2f((2./3.)*arr[3]*sin(arr[2])+arr[0],(2./3.)*arr[3]*cos(arr[2])+arr[1]);
            buffer[0].color = sf::Color::Red;
            buffer[i+1]=sf::Vector2f((1./2.)*arr[3]*sin(arr[2]+90),(1./2.)*arr[3]*cos(arr[2]+90))+tmp;
            buffer[1].color = sf::Color::Green;
            buffer[i+2]=sf::Vector2f((1./2.)*arr[3]*sin(arr[2]-90),(1./2.)*arr[3]*cos(arr[2]-90))+tmp;
            buffer[2].color = sf::Color::Blue;
//            buffer[i+3]=sf::Vector2f(sin(arr[2]),cos(arr[2]));
//            buffer[i+4]=sf::Vector2f(sin(arr[2]),cos(arr[2]));
//            buffer[i+5]=sf::Vector2f(sin(arr[2]),cos(arr[2]));
//            buffer[i+6]=sf::Vector2f(sin(arr[2]),cos(arr[2]));
//            buffer[i+7]=sf::Vector2f(sin(arr[2]),cos(arr[2]));
//            buffer[i+8]=sf::Vector2f(sin(arr[2]),cos(arr[2]));

        }

        void display() {

            window.clear();
            window.draw(buffer);
            window.display();

        }
    };

} // namespace rendering
#endif //SPACE_ENVIRO_RENDERENGINE_H
