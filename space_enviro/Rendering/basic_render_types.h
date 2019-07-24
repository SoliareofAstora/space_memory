//
// Created by overlord on 13/06/19.
//

#ifndef SPACE_ENVIRO_BASIC_RENDER_TYPES_H
#define SPACE_ENVIRO_BASIC_RENDER_TYPES_H

#include <cmath>
#include <SFML/Graphics/VertexArray.hpp>
#include "../Entities/entities_data.h"

void RenderShip(sf::VertexArray* varr, int offset, entites::ShipArray* ships, int i){

    float x = ships->position[i];
    float y = ships->position[ships->n+i];
    float angle = ships->angle[i];
    float size = ships->size[i];
    float mainpower = ships->engines_control[i];
    float sidepower = ships->engines_control[i+ships->n];
    sf::Vector2f ap = sf::Vector2f(
            (-1.f/3)*size*sinf(angle)+x,
            (-1.f/3)*size*cosf(angle)+y);

    //Side Engine
    varr->operator[](offset+0).position=sf::Vector2f(
            (2.f/3)*size*sinf(angle)+x,
            (2.f/3)*size*cosf(angle)+y);
    varr->operator[](offset+1).position=sf::Vector2f(
            (1.f/3)*size*sinf(angle)+x -
            ((1.f/2)*sidepower*size*sinf(angle + M_PI_2f32) + (1.f / 4)), //FIXME magic number
            (1.f/3)*size*cosf(angle)+y -
            ((1.f/2)*sidepower*size*cosf(angle + M_PI_2f32) + (1.f / 4)));
    varr->operator[](offset+2).position=sf::Vector2f(
            (1.f/6)*size*sinf(angle)+x,
            (1.f/6)*size*cosf(angle)+y);

    //Main Engine
    varr->operator[](offset+3).position=ap - sf::Vector2f(
            (1.f/2)*size*sinf(angle)*mainpower,
            (1.f/2)*size*cosf(angle)*mainpower);
    varr->operator[](offset+4).position=ap + sf::Vector2f(
            (1.f/4)*size*sinf(angle+M_PI_2f32),
            (1.f/4)*size*cosf(angle+M_PI_2f32));
    varr->operator[](offset+5).position=ap + sf::Vector2f(
            (1.f/4)*size*sinf(angle-M_PI_2f32),
            (1.f/4)*size*cosf(angle-M_PI_2f32));

    //Ship Body
    varr->operator[](offset+6).position=sf::Vector2f(
            (2.f/3)*size*sinf(angle)+x,
            (2.f/3)*size*cosf(angle)+y);
    varr->operator[](offset+7).position=ap + sf::Vector2f(
            (1.f/2)*size*sinf(angle+M_PI_2f32),
            (1.f/2)*size*cosf(angle+M_PI_2f32));
    varr->operator[](offset+8).position=ap + sf::Vector2f(
            (1.f/2)*size*sinf(angle-M_PI_2f32),
            (1.f/2)*size*cosf(angle-M_PI_2f32));

}

void RenderSquare(sf::VertexArray* varr, int offset, entites::EntityArray* checkpoints, int i){

    float x = checkpoints->position[i];
    float y = checkpoints->position[checkpoints->n+i];
    float size = checkpoints->size[i];

    varr->operator[](offset+0).position=sf::Vector2f(
            (1.f/2)*size+x,
            (1.f/2)*size+y);
    varr->operator[](offset+1).position=sf::Vector2f(
            -(1.f/2)*size+x,
            (1.f/2)*size+y);
    varr->operator[](offset+2).position=sf::Vector2f(
            (1.f/2)*size+x,
            -(1.f/2)*size+y);

    varr->operator[](offset+3).position=sf::Vector2f(
            -(1.f/2)*size+x,
            -(1.f/2)*size+y);
    varr->operator[](offset+4).position=sf::Vector2f(
            -(1.f/2)*size+x,
            (1.f/2)*size+y);
    varr->operator[](offset+5).position=sf::Vector2f(
            (1.f/2)*size+x,
            -(1.f/2)*size+y);
}

#endif //SPACE_ENVIRO_BASIC_RENDER_TYPES_H
