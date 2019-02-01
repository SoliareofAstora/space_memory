//
// Created by overlord on 25/01/19.
//

#include "render_engine.h"

namespace rendering{

    void RenderEngine::RenderShip(int i, float arr[]){

        float angle = arr[2]*PI/180;
        float size = arr[3];
        float mainpower = arr[6];
        float sidepower = arr[5];
        sf::Vector2f ap = sf::Vector2f((-1./3.)*size*sin(angle)+arr[0],(-1./3.)*size*cos(angle)+arr[1]);
            sf::Vector2f hp = sf::Vector2f(
                    (1./3.)*size*sin(angle)+arr[0],
                    (1./3.)*size*cos(angle)+arr[1]);

        //Side Engine
        ship_vertices_[i+0].position=sf::Vector2f(
                (2./3.)*size*sin(angle)+arr[0],
                (2./3.)*size*cos(angle)+arr[1]);
        ship_vertices_[i+1].position= hp + sf::Vector2f(
                ((1./2.)*sidepower*size*sin(angle+90*PI/180)+(1./4.))*arr[4],
                ((1./2.)*sidepower*size*cos(angle+90*PI/180)+(1./4.))*arr[4]);
        ship_vertices_[i+2].position=sf::Vector2f(
                (1./6.)*size*sin(arr[2])+arr[0],
                (1./6.)*size*cos(arr[2])+arr[1]);

        //Main Engine
        ship_vertices_[i+3].position=ap - sf::Vector2f(
                (1./2.)*size*mainpower*sin(angle),
                (1./2.)*size*mainpower*cos(angle));
        ship_vertices_[i+4].position=ap + sf::Vector2f(
                (1./4.)*size*sin(angle+90*PI/180),
                (1./4.)*size*cos(angle+90*PI/180));
        ship_vertices_[i+5].position=ap + sf::Vector2f(
                (1./4.)*size*sin(angle-90*PI/180),
                (1./4.)*size*cos(angle-90*PI/180));

        //Ship Body
        ship_vertices_[i+6].position=sf::Vector2f(
                (2./3.)*size*sin(angle)+arr[0],
                (2./3.)*size*cos(angle)+arr[1]);
        ship_vertices_[i+7].position=ap + sf::Vector2f(
                (1./2.)*size*sin(angle+90*PI/180),
                (1./2.)*size*cos(angle+90*PI/180));
        ship_vertices_[i+8].position=ap + sf::Vector2f(
                (1./2.)*size*sin(angle-90*PI/180),
                (1./2.)*size*cos(angle-90*PI/180));

    }
}