//
// Created by SoliareofAstora on 26/01/19.
//

#ifndef SPACE_ENVIRO_ENVIRONMENT_CONTROLLER_H
#define SPACE_ENVIRO_ENVIRONMENT_CONTROLLER_H

#include <iostream>
#include <SFML/Window/Event.hpp>
#include <boost/python.hpp>
#include <boost/python/numpy.hpp>

#include "Rendering/render_engine.h"
#include "Scenario/scenario_base.h"
#include "Scenario/checkpoint_scenario.h"
#include "global_config.h"
#include "Utils/waiter.h"

class EnvironmentController {

 private:

  rendering::RenderEngine* render_engine;
  ScenarioBase* scenario;

  //todo move frame rate limiter to render_engine
  Waiter w = Waiter(static_cast<int>(time_step * 1000));

  bool render = true;


 public:
  bool active = true;
  EnvironmentController(int);

  ~EnvironmentController();

  boost::python::tuple Step(const boost::python::numpy::ndarray &action_vector);
  boost::python::numpy::ndarray Reset();

  bool Active();
};

BOOST_PYTHON_MODULE (spaceLib) {
  boost::python::class_<EnvironmentController>(
      "initialize", boost::python::init<int>())
      .def("active", &EnvironmentController::Active)
      .def("step", &EnvironmentController::Step)
      .def("reset", &EnvironmentController::Reset);
}

#endif