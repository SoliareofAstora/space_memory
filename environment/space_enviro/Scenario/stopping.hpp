//
// Created by SoliareofAstora on 09.02.2020.
//

#ifndef SPACE_ENVIRO_SCENARIO_STOPPING_HPP_
#define SPACE_ENVIRO_SCENARIO_STOPPING_HPP_

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

class Stopping:public ScenarioBase {
 public:
  int n;
  float passThreshold;
  float resetThreshold;
  float resetAngleThreshold;
  int minV;
  int maxV;
  int maxAngleV;
/*
* 0 - velocity length
* 1 - velocity vector angle wr to ship direction
* 2 - angular velocity
*/
  entity_data::Ship* ship_array;

  float* observations;
  float* reward;
  float* previousVelocity;
  float* currentVelocity;
  bool* done;

  Stopping(const boost::python::dict &parameters)
      :n(boost::python::extract<int>(parameters["n"])) {
    passThreshold = boost::python::extract<float>(parameters["passThreshold"]);
    resetThreshold = boost::python::extract<float>(parameters["resetThreshold"]);
    resetAngleThreshold = boost::python::extract<float>(parameters["resetAngleThreshold"]);

    minV = boost::python::extract<int>(parameters["minV"]);
    maxV = boost::python::extract<int>(parameters["maxV"]);
    maxAngleV = boost::python::extract<int>(parameters["maxAngleV"]);

    ship_array = entity_data::InitializeShipArrayFromDict(parameters);
    observations = new float[n * 3];
    reward = new float[n];
    previousVelocity = new float[n];
    currentVelocity = new float[n];
    ship_array->SaveVelocityNorms(previousVelocity);

    done = new bool[n];
    for (int i = 0; i < n; ++i) {
      reward[i] = 0;
    }
  }

  ~Stopping() {
    delete ship_array;

    delete[] observations;
    delete[] reward;
    delete[] previousVelocity;
    delete[] currentVelocity;
    delete[] done;
  }

  boost::python::tuple ManualStep() override {

  }

  boost::python::tuple Step(float* actions) override {
    ship_array->Update(actions);
    ship_array->SaveVelocityNorms(currentVelocity);
    std::fill(done, done + n, false);

    for (int i = 0; i < n; ++i) {

      reward[i] = previousVelocity[i] - currentVelocity[i];
      if (currentVelocity[i] < passThreshold) {
        reward[i] = 1;
        done[i] = true;
        ship_array->ResetWithRandomVelocity(i, minV, maxV, maxAngleV);
        currentVelocity[i] = ship_array->GetVelovityNorm(i);
      }
      if (currentVelocity[i] > resetThreshold ||
      (ship_array->v_angle[i]> resetAngleThreshold|| ship_array->v_angle[i]<-resetAngleThreshold)) {
        reward[i] = -1;
        done[i] = true;
        ship_array->ResetWithRandomVelocity(i, minV, maxV, maxAngleV);
        currentVelocity[i] = ship_array->GetVelovityNorm(i);
      }
      previousVelocity[i] = currentVelocity[i];
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

  boost::python::numpy::ndarray CalculateObservations() {
    for (int i = 0; i < n; ++i) {
      float s = sinf(ship_array->angle[i] - M_PI_2f32);
      float c = cosf(ship_array->angle[i] - M_PI_2f32);
      observations[0 * n + i] = ship_array->GetVelovityNorm(i);
      observations[1 * n + i] = atan2f(-(s * ship_array->v[i] + c * ship_array->v[n + i]),
                                       c * ship_array->v[i] - s * ship_array->v[n + i]);
    }
    std::memcpy(&observations[2 * n], ship_array->v_angle, n * sizeof(float));

    boost::python::tuple observation_shape = boost::python::make_tuple(n, 3);
    boost::python::tuple strideo = boost::python::make_tuple(sizeof(float), n * sizeof(float));
    boost::python::object own;
    boost::python::numpy::dtype dt = boost::python::numpy::dtype::get_builtin<float>();
    return boost::python::numpy::from_data(observations, dt, observation_shape, strideo, own);
  }

  boost::python::numpy::ndarray Reset() override {
    for (int i = 0; i < n; ++i) {
      ship_array->ResetWithRandomVelocity(i, minV, maxV, maxAngleV);
      currentVelocity[i] = ship_array->GetVelovityNorm(i);
    }
    return CalculateObservations();
  }

  sf::VertexArray* InitializeVertexArray() override {
    sf::VertexArray* vertex_array = new sf::VertexArray(sf::Triangles, 9 * n);
    for (int i = 0; i < n; ++i) {
      sf::Color ship_color = sf::Color(random() % (236) + 20, random() % (236) + 20, random() % (236) + 20);

      for (int j = 6; j < 9; ++j) {
        vertex_array->operator[](9 * i + j).color = ship_color;
      }
    }
    return vertex_array;
  }

  void Render(sf::VertexArray* vertex_array) override {
    for (int i = 0; i < n; ++i) {
      rendering::RenderShip(vertex_array, i * 9, ship_array, i);
    }
  }

  virtual sf::VertexArray* InitializeDebugRender() {
    return new sf::VertexArray(sf::Lines, 4 * n);
  }
/*
  * 0 - velocity
  * 1 - velocity vector angle wr to ship direction
  * 2 - angular velocity
  * */
  virtual void RenderDebug(sf::VertexArray* vertex_array) {
    for (int i = 0; i < n; ++i) {
      sf::Vector2f ship_position = sf::Vector2f(
          ship_array->position[i],
          ship_array->position[n + i]);

      //Velocity vector
      vertex_array->operator[](4 * i + 0).position = ship_position;
      vertex_array->operator[](4 * i + 1).position =
          ship_position + sf::Vector2f(
              10 * observations[0 * n + i] * sinf(ship_array->angle[i] + observations[1 * n + i]),
              10 * observations[0 * n + i] * cosf(ship_array->angle[i] + observations[1 * n + i]));

      //Angular velocity
      vertex_array->operator[](4 * i + 2).position = vertex_array->operator[](4 * i + 1).position;
      vertex_array->operator[](4 * i + 3).position = vertex_array->operator[](4 * i + 1).position + sf::Vector2f(
          20 * observations[2 * n + i] * sinf(ship_array->angle[i] + observations[1 * n + i] + M_PI_2f32),
          20 * observations[2 * n + i] * cosf(ship_array->angle[i] + observations[1 * n + i] + M_PI_2f32));
    }
  }
};

} // namespace scenario

#endif //SPACE_ENVIRO_SCENARIO_STOPPING_HPP_
