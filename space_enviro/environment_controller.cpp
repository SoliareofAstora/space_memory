//
// Created by SoliareofAstora on 26/01/19.
//

#include "environment_controller.h"
#include "Scenario/load.h"

EnvironmentController::EnvironmentController(const boost::python::dict& parameters) {

  std::cout << "Initializing Space_Memory Environment\n";

  std::cout << "Initializing Boost and Numpy in C++ \n";
  Py_Initialize();
  boost::python::numpy::initialize();

  std::cout << "Loading Scenario ";
  std::cout << std::string(boost::python::extract<std::string>(parameters["scenario_name"])) + "\n";
  scenario = scenario::load_scenario(parameters);

  if (boost::python::extract<bool>(parameters["render"])) {
    std::cout << "Initializing RenderEngine\n";
    render_engine = new rendering::RenderEngine(scenario);
    controller.Press("render");
  }

  std::cout << "Space Environment initialization DONE\n\n";

}

boost::python::tuple EnvironmentController::Step(const boost::python::numpy::ndarray &action_vector) {
  return scenario->Step(reinterpret_cast<float*>(action_vector.get_data()));
}

boost::python::tuple EnvironmentController::RenderStep(const boost::python::numpy::ndarray &action_vector) {

  if (controller["render"]) {
    if (controller["real_time"])
      w.wait();
    render_engine->RenderState(scenario, controller["debug"]);
  }

  sf::Event e;
  render_engine->window.pollEvent(e);
  controller.Update();
  if (render_engine->window.hasFocus()) {

    //todo refactor
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
      controller.Press("render");
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
      controller.Press("debug");
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift))
      controller.Press("real_time");
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R))
      Reset();
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
      Close();
  }

  return Step(action_vector);
}


//boost::python::tuple EnvironmentController::ManualStep() {
//
//  float* actions = new float[2];
//
//  if (render_engine->window.hasFocus()) {
//    w.wait();
//    render_engine->RenderState(scenario, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D));
//    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R)){
//      Reset();
//    }
//    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
//      Close();
//    }
//  }
//
//
//  boost::python::tuple state = scenario->Step(reinterpret_cast<float*>(action_vector.get_data()));
//
//  sf::Event e;
//  render_engine->window.pollEvent(e);
//
//  return state;
//}


boost::python::numpy::ndarray EnvironmentController::Reset() {
  return scenario->Reset();
}

EnvironmentController::~EnvironmentController() {
  delete scenario;
  delete render_engine;
}

void EnvironmentController::Close() {
  delete render_engine;
  delete scenario;
}
