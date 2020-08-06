//
// Created by SoliareofAstora on 25/01/19.
//

#ifndef SPACE_ENVIRO_RENDERENGINE_H
#define SPACE_ENVIRO_RENDERENGINE_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <boost/math_fwd.hpp>
#include "../Scenario/base.h"

namespace rendering{

class RenderEngine {
 public:

  sf::RenderWindow* window;
  sf::RenderTexture* texture;
  sf::VertexArray* vertex_array;
  sf::VertexArray* debug_vertex_array;

  bool rendering = false;
  bool recording = false;
  bool show_debug = false;
  std::string recording_path;
  int frame_counter;

  RenderEngine(scenario::ScenarioBase* scenario, std::string redpth, bool render, bool record, bool debug) {
    vertex_array = scenario->InitializeVertexArray();
    debug_vertex_array = scenario->InitializeDebugRender();
    recording_path = redpth;
    show_debug = debug;
    frame_counter = 0;
    sf::View v(sf::FloatRect(-750, -750, 1500, 1500));

    if(render) {
      window = new sf::RenderWindow(sf::VideoMode(1024, 1024), "Space memory");
      window->setView(v);
    }

    if(render || record) {
      texture = new sf::RenderTexture();
      texture->create(1024, 1024);
      texture->setView(v);
    }
  }

  void RenderState(scenario::ScenarioBase* scenario) {
    if (rendering || recording) {
      scenario->Render(vertex_array);
      scenario->RenderDebug(debug_vertex_array);

      if (rendering) {
        window->clear();
        window->draw(*vertex_array);
        if (show_debug) {
          window->draw(*debug_vertex_array);
        }
        window->display();
      }
      if (recording) {
        texture->clear();
        texture->draw(*vertex_array);
        if (show_debug) {
          texture->draw(*debug_vertex_array);
        }
        texture->display();

        std::stringstream filename;
        filename << std::setw(10) << std::setfill('0') << frame_counter;
        texture->getTexture().copyToImage().saveToFile(recording_path+"/"+filename.str()+".png");
        frame_counter++;
      }
    }
  }

  ~RenderEngine() {
    delete window;
    delete texture;
    delete vertex_array;
    delete debug_vertex_array;
  }
};

} // namespace rendering
#endif //SPACE_ENVIRO_RENDERENGINE_H
