//
// Created by SoliareofAstora on 29/05/19.
//

#ifndef SPACE_ENVIRO_CHECKPOINT_SINGLE_SCENARIO_H
#define SPACE_ENVIRO_CHECKPOINT_SINGLE_SCENARIO_H

#include <cmath>
#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include "base.h"
#include "../Entities/Data/ship.h"
#include "../Entities/checkpoint.h"
#include "../Rendering/Primitives/render_ship.h"
#include "../Rendering/Primitives/render_square.hpp"

namespace scenario{

class CheckpointSingleV1:public ScenarioBase {
  /*
 * 0 - distance between ship and checkpoint
 * 1 - angle between ship direction and checkpoint
 * 2 - velocity
 * 3 - velocity vector angle wr to ship direction
 * 4 - angular velocity
 */
 public:
  int n;
  float passThreshold;
  float resetThreshold;
  float resetAngleThreshold;
  int minV;
  int maxV;
  int maxAngleV;

  entity_data::Ship* ship_array;
  CheckpointArray* checkpoint_array;
  float* observations;
  float* reward;
  float* distance;
  bool* done;

  int pasedCount = 0;
  int failedCount = 0;

  CheckpointSingleV1(const boost::python::dict &parameters)
  :n(boost::python::extract<int>(parameters["n"])) {
    passThreshold = boost::python::extract<float>(parameters["passThreshold"]);
    resetThreshold = boost::python::extract<float>(parameters["resetThreshold"]);
    resetAngleThreshold = boost::python::extract<float>(parameters["resetAngleThreshold"]);
    minV = boost::python::extract<int>(parameters["minV"]);
    maxV = boost::python::extract<int>(parameters["maxV"]);
    maxAngleV = boost::python::extract<int>(parameters["maxAngleV"]);

    ship_array = new entity_data::Ship(n);
    checkpoint_array = new CheckpointArray(n, boost::python::extract<int>(parameters["spawn_square"]));
    observations = new float[n * 5];
    reward = new float[n];
    distance = new float[n];
    done = new bool[n];
    entity_data::PairwiseDistance(ship_array, checkpoint_array, distance);
  }

  ~CheckpointSingleV1() {
    delete ship_array;
    delete checkpoint_array;

    delete[] observations;
    delete[] reward;
    delete[] distance;
    delete[] done;
  }

  boost::python::tuple Step(float* actions) override {
    ship_array->Update(actions);

    // TODO: change it to swapping arrays. Do not copy values over and over
    std::memcpy(reward, distance, n * sizeof(float));
    std::fill(done, done + n, false);

    for (int i = 0; i < n; ++i) {
      distance[i] = entity_data::Distance(ship_array, i, checkpoint_array, i);
      reward[i] -= distance[i];
      if (distance[i] > resetThreshold ||
          (ship_array->v_angle[i] > resetAngleThreshold || ship_array->v_angle[i] < -resetAngleThreshold)) {
        reward[i] = -1;
        done[i] = true;
        failedCount += 1;
        ship_array->ResetWithRandomVelocity(i, minV, maxV, maxAngleV);
        distance[i] = entity_data::Distance(ship_array, i, checkpoint_array, i);
      }
      if (distance[i] < passThreshold) {
        reward[i] = 1;
        done[i] = true;
        pasedCount += 1;
        ship_array->ResetWithRandomVelocity(i, minV, maxV, maxAngleV);
        checkpoint_array->ResetCheckpoint(i);
        distance[i] = entity_data::Distance(ship_array, i, checkpoint_array, i);
      }
    }

    boost::python::tuple reward_shape = boost::python::make_tuple(n);
    boost::python::tuple strider = boost::python::make_tuple(sizeof(float));
    boost::python::object own;
    boost::python::numpy::dtype dt = boost::python::numpy::dtype::get_builtin<float>();

    return boost::python::make_tuple(
        CalculateObservations(),
        boost::python::numpy::from_data(reward, dt, reward_shape, strider, own),
        boost::python::numpy::from_data(done,
                                        boost::python::numpy::dtype::get_builtin<bool>(),
                                        reward_shape,
                                        boost::python::make_tuple(sizeof(bool)),
                                        own)
    );
  }

  /*
* 0 - distance between ship and checkpoint
* 1 - angle between ship direction and checkpoint
* 2 - velocity
* 3 - velocity vector angle wr to ship direction
* 4 - angular velocity
*/
  boost::python::numpy::ndarray CalculateObservations() {
    std::memcpy(observations, distance, n * sizeof(float));
    std::memcpy(&observations[4 * n], ship_array->v_angle, n * sizeof(float));
    for (int i = 0; i < n; ++i) {
      float dx = checkpoint_array->position[i] - ship_array->position[i];
      float dy = checkpoint_array->position[n + i] - ship_array->position[n + i];
      float s = sinf(ship_array->angle[i] - M_PI_2f32);
      float c = cosf(ship_array->angle[i] - M_PI_2f32);
      observations[1 * n + i] = atan2f(-(s * dx + c * dy), c * dx - s * dy);
      observations[2 * n + i] = sqrtf(powf(ship_array->v[i], 2) + powf(ship_array->v[n + i], 2));
      observations[3 * n + i] = atan2f(-(s * ship_array->v[i] + c * ship_array->v[n + i]),
                                       c * ship_array->v[i] - s * ship_array->v[n + i]);
    }
    boost::python::tuple observation_shape = boost::python::make_tuple(n, 5);
    boost::python::tuple strideo = boost::python::make_tuple(sizeof(float), n * sizeof(float));
    boost::python::object own;
    boost::python::numpy::dtype dt = boost::python::numpy::dtype::get_builtin<float>();
    return boost::python::numpy::from_data(observations, dt, observation_shape, strideo, own);
  }

  boost::python::numpy::ndarray Reset() override {
    checkpoint_array->Reset();
    ship_array->ResetAllValues();
    entity_data::PairwiseDistance(ship_array, checkpoint_array, distance);
    return CalculateObservations();
  }

  sf::VertexArray* InitializeVertexArray() override {
    sf::VertexArray* vertex_array = new sf::VertexArray(sf::Triangles, (9 + 6) * n);
    for (int i = 0; i < n; ++i) {
      sf::Color ship_color = sf::Color(random() % (236) + 20, random() % (236) + 20, random() % (236) + 20);

      for (int j = 0; j < 6; ++j) {
        vertex_array->operator[](6 * i + j).color = ship_color;
      }
      for (int j = 6; j < 9; ++j) {
        vertex_array->operator[](9 * i + j + 6 * n).color = ship_color;
      }
    }
    return vertex_array;
  }

  void Render(sf::VertexArray* vertex_array) override {
    for (int i = 0; i < n; ++i) {
      rendering::RenderSquare(vertex_array, i * 6, checkpoint_array, i);
    }
    for (int i = 0; i < n; ++i) {
      rendering::RenderShip(vertex_array, n * 6 + i * 9, ship_array, i);
    }
  }

  virtual sf::VertexArray* InitializeDebugRender() {
    return new sf::VertexArray(sf::Lines, 6 * n);
  }

  virtual void RenderDebug(sf::VertexArray* vertex_array) {
    for (int i = 0; i < n; ++i) {
      sf::Vector2f ship_position = sf::Vector2f(
          ship_array->position[i],
          ship_array->position[n + i]);

      //Target checkpoint
      vertex_array->operator[](6 * i + 0).position = ship_position;
      vertex_array->operator[](6 * i + 1).position =
          ship_position + sf::Vector2f(
              observations[i] * sinf(ship_array->angle[i] + observations[1 * n + i]),
              observations[i] * cosf(ship_array->angle[i] + observations[1 * n + i]));

      //Velocity vector
      vertex_array->operator[](6 * i + 2).position = ship_position;
      vertex_array->operator[](6 * i + 3).position =
          ship_position + sf::Vector2f(
              3 * observations[2 * n + i] * sinf(ship_array->angle[i] + observations[3 * n + i]),
              3 * observations[2 * n + i] * cosf(ship_array->angle[i] + observations[3 * n + i]));

      //Angular velocity
      vertex_array->operator[](6 * i + 4).position = vertex_array->operator[](6 * i + 3).position;
      vertex_array->operator[](6 * i + 5).position = vertex_array->operator[](6 * i + 3).position + sf::Vector2f(
          20 * observations[4 * n + i] * sinf(ship_array->angle[i] + observations[3 * n + i] + M_PI_2f32),
          20 * observations[4 * n + i] * cosf(ship_array->angle[i] + observations[3 * n + i] + M_PI_2f32));
    }
  }
  virtual void UpdateText(sf::Text* text){
    text->setString("Second scenario: \n success: "+std::to_string(pasedCount)+"\n failure: "+std::to_string(failedCount));

  }
};

} // namespace scenario
/*
* 0 - distance between ship and checkpoint
* 1 - angle between ship direction and checkpoint
* 2 - velocity
* 3 - velocity vector angle wr to ship direction
* 4 - angular velocity
*/


//#include <iostream>
//#include <cstdint>
//#include <cstring>
//#include <algorithm>
//#include <cmath>
//#include <utility>
//
//int main()
//{
//    float pi = 3.14159265359;
//    float sx = 3;
//    float sy = 2;
//
//    float cx = 5;
//    float cy = 4;
//
//    cx-= sx;
//    cy-=sy;
//
//    float sangle = -1*pi/4;
//
//    float c = cosf(-sangle);
//    float s = sinf(-sangle);
//
//    float rx = c*cx-s*cy;
//    float ry = s*cx+c*cy;
//
//    std::cout<<sqrt(pow(cx,2)+pow(cy,2))<<"  "<<sqrt(pow(rx,2)+pow(ry,2))<<"  "<<180*atan2(ry,rx)/pi;
//
//    return 0;
//}

#endif //SPACE_ENVIRO_CHECKPOINT_SINGLE_SCENARIO_H
