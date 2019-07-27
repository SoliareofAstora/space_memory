//
// Created by overlord on 26/01/19.
//

#include "environment_controller.h"

EnvironmentController::EnvironmentController(std::string a) {

  std::cout << "Initializing Space_Memory Environment\nLoading config from: " << a << "\n";
  // Load config file

  std::cout << "Initializing Boost, Numpy \n";
  Py_Initialize();
  boost::python::numpy::initialize();

  std::cout << "Initializing Scenario\n";
  scenario = new Checkpoints(1000);

  if (render) {
    std::cout << "Initializing RenderEngine\n";
    render_engine = new rendering::RenderEngine(scenario);
  }

  std::cout << "Space Environment initialization DONE\n\n";
}

boost::python::tuple EnvironmentController::Step(const boost::python::numpy::ndarray &action_vector) {
  boost::python::tuple tmp = scenario->Step(reinterpret_cast<float*>(action_vector.get_data()));

  sf::Event e;
  render_engine->window.pollEvent(e);

  if (render_engine->window.hasFocus()) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
      render = false;
      delete render_engine;
      delete scenario;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
      if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) {
        w.wait();
      }
      render_engine->RenderState(scenario);
    }
  }

  return tmp;
}
