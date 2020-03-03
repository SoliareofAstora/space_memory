//
// Created by SoliareofAstora on 17.12.2019.
//

#ifndef SPACE_ENVIRO_RENDERING_PRIMITIVES_RENDER_SQUARE_HPP_
#define SPACE_ENVIRO_RENDERING_PRIMITIVES_RENDER_SQUARE_HPP_

#include <SFML/Graphics/VertexArray.hpp>
#include "../../Entities/Data/base_entity.h"
namespace rendering{

void RenderSquare(sf::VertexArray* varr, int offset, entity_data::BaseEntity* entities, int i) {

  float x = entities->position[i];
  float y = entities->position[entities->n + i];
  float size = entities->size[i];

  varr->operator[](offset + 0).position = sf::Vector2f(
      (1.f / 2) * size + x,
      (1.f / 2) * size + y);
  varr->operator[](offset + 1).position = sf::Vector2f(
      -(1.f / 2) * size + x,
      (1.f / 2) * size + y);
  varr->operator[](offset + 2).position = sf::Vector2f(
      (1.f / 2) * size + x,
      -(1.f / 2) * size + y);

  varr->operator[](offset + 3).position = sf::Vector2f(
      -(1.f / 2) * size + x,
      -(1.f / 2) * size + y);
  varr->operator[](offset + 4).position = sf::Vector2f(
      -(1.f / 2) * size + x,
      (1.f / 2) * size + y);
  varr->operator[](offset + 5).position = sf::Vector2f(
      (1.f / 2) * size + x,
      -(1.f / 2) * size + y);
}
} // namespace rendering

#endif //SPACE_ENVIRO_RENDERING_PRIMITIVES_RENDER_SQUARE_HPP_
