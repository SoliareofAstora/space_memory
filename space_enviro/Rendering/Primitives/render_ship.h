//
// Created by SoliareofAstora on 17.12.2019.
//

#ifndef SPACE_ENVIRO_RENDERING_PRIMITIVES_RENDER_SHIP_H_
#define SPACE_ENVIRO_RENDERING_PRIMITIVES_RENDER_SHIP_H_

#include <cmath>
#include <SFML/Graphics/VertexArray.hpp>
#include "../../Entities/Data/ship.h"

namespace rendering{

void RenderShip(sf::VertexArray* varr, int offset, entity_data::Ship* ships, int i) {

  float x = ships->position[i];
  float y = ships->position[ships->n + i];
  float angle = ships->angle[i];
  float size = ships->size[i];
  float mainpower = ships->engines_control[i];
  float sidepower = ships->engines_control[i + ships->n];
  sf::Vector2f ap = sf::Vector2f(
      (-1.f / 3) * size * sinf(angle) + x,
      (-1.f / 3) * size * cosf(angle) + y);

  //Side Engine
  varr->operator[](offset + 0).position = sf::Vector2f(
      (2.f / 3) * size * sinf(angle) + x,
      (2.f / 3) * size * cosf(angle) + y);
  varr->operator[](offset + 1).position = sf::Vector2f(
      (1.f / 3) * size * sinf(angle) + x
          - ((1.f / 2) * sidepower * size * sinf(angle + M_PI_2f32) + (1.f / 4)), //FIXME magic number
      (1.f / 3) * size * cosf(angle) + y
          - ((1.f / 2) * sidepower * size * cosf(angle + M_PI_2f32) + (1.f / 4))); //FIXME magic number
  varr->operator[](offset + 2).position = sf::Vector2f(
      (1.f / 6) * size * sinf(angle) + x,
      (1.f / 6) * size * cosf(angle) + y);

  //Main Engine
  varr->operator[](offset + 3).position = ap - sf::Vector2f(
      (1.f / 2) * size * sinf(angle) * mainpower,
      (1.f / 2) * size * cosf(angle) * mainpower);
  varr->operator[](offset + 4).position = ap + sf::Vector2f(
      (1.f / 4) * size * sinf(angle + M_PI_2f32),
      (1.f / 4) * size * cosf(angle + M_PI_2f32));
  varr->operator[](offset + 5).position = ap + sf::Vector2f(
      (1.f / 4) * size * sinf(angle - M_PI_2f32),
      (1.f / 4) * size * cosf(angle - M_PI_2f32));

  //Ship Body
  varr->operator[](offset + 6).position = sf::Vector2f(
      (2.f / 3) * size * sinf(angle) + x,
      (2.f / 3) * size * cosf(angle) + y);
  varr->operator[](offset + 7).position = ap + sf::Vector2f(
      (1.f / 2) * size * sinf(angle + M_PI_2f32),
      (1.f / 2) * size * cosf(angle + M_PI_2f32));
  varr->operator[](offset + 8).position = ap + sf::Vector2f(
      (1.f / 2) * size * sinf(angle - M_PI_2f32),
      (1.f / 2) * size * cosf(angle - M_PI_2f32));
}
} // namespace rendering

#endif //SPACE_ENVIRO_RENDERING_PRIMITIVES_RENDER_SHIP_H_
