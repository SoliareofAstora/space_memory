//
// Created by overlord on 26/01/19.
//

#include "environment_controller.h"

enviroment_controller::enviroment_controller(std::string a) {

    std::cout<<"Initializing Environment\nLoading config from: "<<a;
    // Load config file
    std::cout<<"Initializing...";
    Py_Initialize();
    np::initialize();

    render_engine_ = new rendering::RenderEngine();
}

int enviroment_controller::Update(np::ndarray action_vector) {
    render_engine_->display();
    return 0;
}

np::ndarray enviroment_controller::GetObservations() {
    py::tuple shape = py::make_tuple(3, 3);
    np::dtype dtype = np::dtype::get_builtin<double>();
    np::ndarray a = np::zeros(shape, dtype);
    return a;
}

