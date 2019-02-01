//
// Created by overlord on 26/01/19.
//

#include "environment_controller.h"

environment_controller::environment_controller(std::string a) {

    std::cout<<"Initializing Environment\nLoading config from: "<<a<<"\n";
    // Load config file
    std::cout<<"Initializing...\n";
    Py_Initialize();
    np::initialize();
    if(render_)render_engine_ = new rendering::RenderEngine();

    std::cout<<"Initialization Finished correctly\n\n";
}

np::ndarray environment_controller::GetObservations() {
    py::tuple shape = py::make_tuple(3, 3);
    np::dtype dtype = np::dtype::get_builtin<double>();
    np::ndarray a = np::zeros(shape, dtype);
    return a;
}

int environment_controller::Update(np::ndarray action_vector) {
    if(render_engine_->window.hasFocus())
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
            render_ = false;
            free(render_engine_);
            free(entity_manager_);

        }
    }
//            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
//                shape.setFillColor(sf::Color::Red);
//            }
//            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
//                shape.setFillColor(sf::Color::Blue);
//            }
//            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
//                shape.setFillColor(sf::Color::Green);
//            }


    if(render_)render_engine_->display();
    return 0;
}


