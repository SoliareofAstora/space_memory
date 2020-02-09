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
#include "Scenario/load_scenario.hpp"
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
  explicit EnvironmentController(const boost::python::dict&);

  ~EnvironmentController();

  boost::python::tuple Step(const boost::python::numpy::ndarray &action_vector);
  boost::python::numpy::ndarray Reset();

  void Close();
  bool Active();
};

BOOST_PYTHON_MODULE (spaceLib) {
  boost::python::class_<EnvironmentController>("initialize", boost::python::init<boost::python::dict>())
      .def("active", &EnvironmentController::Active)
      .def("step", &EnvironmentController::Step)
      .def("reset", &EnvironmentController::Reset)
      .def("close",&EnvironmentController::Close)
      ;
}

#endif