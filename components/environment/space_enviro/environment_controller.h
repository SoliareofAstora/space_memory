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
#include <boost/mem_fn.hpp>
#include <boost/python.hpp>
#include <boost/shared_ptr.hpp>



class EnvironmentController {

 private:

  rendering::RenderEngine* render_engine;
  scenario::ScenarioBase* scenario;
  bool active = true;
  bool rndr = false;
  bool interactive = false;
  Waiter w = Waiter(static_cast<int>(time_step * 1000));
  SwitchBools controller = SwitchBools(std::vector<std::string>({"record", "render", "real_time", "debug"}));


 public:
  explicit EnvironmentController(const boost::python::dict &parameters);
  void destroy(){
    std::cout<<"Destroy called";
    delete this;
  }

  boost::python::tuple Step(const boost::python::numpy::ndarray &action_vector);
  boost::python::numpy::ndarray Reset();
  bool Active(){return active;}
 private:
  ~EnvironmentController() {
    std::cout<<"1";
    delete scenario;
    std::cout<<"2";
    if(rndr) delete render_engine;
    std::cout<<"3";

  }
};


boost::shared_ptr<EnvironmentController> create_env(boost::python::dict dict){
  return boost::shared_ptr<EnvironmentController>(
      new EnvironmentController(dict),
      boost::mem_fn(&EnvironmentController::destroy));
}

BOOST_PYTHON_MODULE (spaceLib) {
  boost::python::class_<EnvironmentController, boost::shared_ptr<EnvironmentController>,
                        boost::noncopyable>("space_env", boost::python::no_init)
      .def("__init__", boost::python::make_constructor(&create_env))
      .def("step", &EnvironmentController::Step)
      .def("reset", &EnvironmentController::Reset)
      .def("active", &EnvironmentController::Active)
      ;
}




#endif