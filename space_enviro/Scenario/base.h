//
// Created by SoliareofAstora on 29/05/19.
//

#ifndef SPACE_ENVIRO_SCENARIO_BASE_H
#define SPACE_ENVIRO_SCENARIO_BASE_H

#include <SFML/Graphics/VertexArray.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/numpy.hpp>
#include <boost/python.hpp>

namespace scenario{

struct ScenarioBase {
  ScenarioBase() = default;
  virtual ~ScenarioBase() = default;

  virtual boost::python::tuple Step(float* actions) {}
  virtual boost::python::tuple ManualStep() {}
  virtual boost::python::numpy::ndarray Reset() {}
  virtual int test() {}

  // TODO add not implemented error if rendering is not implemented
  virtual sf::VertexArray* InitializeVertexArray() {}
  virtual void Render(sf::VertexArray* vertex_array) {}

  virtual sf::VertexArray* InitializeDebugRender() {}
  virtual void RenderDebug(sf::VertexArray* vertex_array) {}
};

}// namespace scenario
#endif //SPACE_ENVIRO_SCENARIO_BASE_H
