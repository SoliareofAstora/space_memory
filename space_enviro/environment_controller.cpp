//
// Created by SoliareofAstora on 26/01/19.
//

#include "environment_controller.h"

EnvironmentController::EnvironmentController(const boost::python::dict& parameters) {

  std::cout << "Initializing Space_Memory Environment\n";

  std::cout << "Initializing Boost, Numpy \n";
  Py_Initialize();
  boost::python::numpy::initialize();

  std::cout << "Initializing Scenario\n";
  scenario = load_scenario(parameters);

  if (render) {
    std::cout << "Initializing RenderEngine\n";
    render_engine = new rendering::RenderEngine(scenario);
  }

  std::cout << "Space Environment initialization DONE\n\n";
}

boost::python::tuple EnvironmentController::Step(const boost::python::numpy::ndarray &action_vector) {
  boost::python::tuple state = scenario->Step(reinterpret_cast<float*>(action_vector.get_data()));

  sf::Event e;
  render_engine->window.pollEvent(e);
  if (render_engine->window.hasFocus()) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
      Close();
    }

    //todo refactor
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)
    || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) {
        w.wait();
      }

      render_engine->RenderState(scenario, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) ? true : false);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R)){
      Reset();
    }
  }

  return state;
}

boost::python::numpy::ndarray EnvironmentController::Reset() {return scenario->Reset();}

bool EnvironmentController::Active() { return active;}

EnvironmentController::~EnvironmentController() {
  delete scenario;
  delete render_engine;
}

void EnvironmentController::Close() {
  active = false;
  delete render_engine;
  delete scenario;
}
