//
// Created by SoliareofAstora on 26/01/19.
//

#ifndef SPACE_ENVIRO_ENVIRONMENT_CONTROLLER_H
#define SPACE_ENVIRO_ENVIRONMENT_CONTROLLER_H

#include <iostream>

#include <SFML/Window/Event.hpp>
#include <boost/python.hpp>
#include <boost/python/numpy.hpp>

#include "Scenario/base.h"
#include "global_config.h"
#include "Utils/waiter.h"
#include "Utils/switch_bools.hpp"
#include "Rendering/render_engine.h"

class EnvironmentController {

 private:

  rendering::RenderEngine* render_engine;
  scenario::ScenarioBase* scenario;
  bool active = true;
  Waiter w = Waiter(static_cast<int>(time_step * 1000));
  SwitchBools controller = SwitchBools(std::vector<std::string>({"reset", "render", "real_time", "debug"}));


 public:
  explicit EnvironmentController(const boost::python::dict &parameters);

  ~EnvironmentController();

  boost::python::tuple Step(const boost::python::numpy::ndarray &action_vector);
  boost::python::tuple RenderStep(const boost::python::numpy::ndarray &action_vector);
//  boost::python::tuple ManualStep();
  boost::python::numpy::ndarray Reset();
  bool Active(){return active;}
  void Close();
};

BOOST_PYTHON_MODULE (spaceLib) {
  boost::python::class_<EnvironmentController>
      ("initialize", boost::python::init<boost::python::dict>())
      .def("step", &EnvironmentController::Step)
      .def("render_step", &EnvironmentController::RenderStep)
      .def("reset", &EnvironmentController::Reset)
      .def("close",&EnvironmentController::Close)
      .def("active", &EnvironmentController::Active)
//      .def("manual_step", &EnvironmentController::ManualStep)
      ;
}

#endif