//
// Created by SoliareofAstora on 26/01/19.
//

#include "environment_controller.h"
#include "Scenario/load.h"

EnvironmentController::EnvironmentController(const boost::python::dict &parameters) {

  Py_Initialize();
  boost::python::numpy::initialize();
  srand(boost::python::extract<int>(parameters["seed"]));

  scenario = scenario::load_scenario(parameters);

  if (boost::python::extract<bool>(parameters["render"]) || boost::python::extract<bool>(parameters["record"])) {
    rndr = true;
    render_engine = new rendering::RenderEngine(scenario,
                                                boost::python::extract<std::string>(parameters["frames_path"]),
                                                boost::python::extract<bool>(parameters["render"]),
                                                boost::python::extract<bool>(parameters["record"]),
                                                boost::python::extract<bool>(parameters["debug"]));
    if (boost::python::extract<bool>(parameters["render"])) {
      controller.Press("render");
      render_engine->rendering = true;
      interactive = true;
    }
    if (boost::python::extract<bool>(parameters["record"])) {
      controller.Press("record");
      render_engine->recording = true;
    }
    if (boost::python::extract<bool>(parameters["debug"])) {
      controller.Press("debug");
      render_engine->show_debug = true;
    }
  }
}

boost::python::tuple EnvironmentController::Step(const boost::python::numpy::ndarray &action_vector) {
  auto output = scenario->Step(reinterpret_cast<float*>(action_vector.get_data()));
  if  (rndr) {
    render_engine->RenderState(scenario);

    if (interactive) {
      if (controller["real_time"])
        w.wait();
      if (render_engine->window->hasFocus()) {
        sf::Event e;
        render_engine->window->pollEvent(e);
        controller.Update();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) {
          if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
            controller.Press("render");
            render_engine->rendering = controller["render"];
          }
          if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
            controller.Press("debug");
            render_engine->show_debug = controller["debug"];
          }
          if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
            controller.Press("real_time");
          if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R)) {
            controller.Press("record");
            render_engine->recording = controller["record"];
          }
          if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
            active = false;
        }
      }
    }
  }
  return output;
}

boost::python::numpy::ndarray EnvironmentController::Reset() {
  return scenario->Reset();
}