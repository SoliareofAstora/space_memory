//
// Created by overlord on 26/01/19.
//

#include "environment_controller.h"

EnvironmentController::EnvironmentController(std::string a) {

    std::cout<<"Initializing Space_Memory Environment\nLoading config from: "<<a<<"\n";
    // Load config file

    std::cout<<"Initializing Boost, Numpy \n";
    Py_Initialize();
    boost::python::numpy::initialize();

    std::cout<<"Initializing Scenario\n";
    scenario_= new NCheckpoints(10000);

    if(render_to_screen_ || render_to_file_) {
        std::cout << "Initializing RenderEngine\n";
        render_engine_ = new rendering::RenderEngine(scenario_);
    }

    std::cout<<"Space Environment initialization DONE\n\n";
}


boost::python::tuple EnvironmentController::Step(boost::python::numpy::ndarray action_vector) {
    auto tmp = scenario_->Step(reinterpret_cast<float *>(action_vector.get_data()));

    sf::Event e;
    render_engine_->window.pollEvent(e);

    if (render_engine_->window.hasFocus()) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
            render_to_screen_ = false;
            free(render_engine_);
            free(scenario_);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {

            render_engine_->RenderState(scenario_);
        }
    }
//    w.wait();
//    render_engine_->RenderState(scenario_);
    return tmp;
}
