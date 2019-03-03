//
// Created by overlord on 26/01/19.
//

#include "environment_controller.h"

EnvironmentController::EnvironmentController(std::string a) {

    std::cout<<"Initializing Space_Memory Environment\nLoading config from: "<<a<<"\n";
    // Load config file
    teams_.push_back(TeamInfo(0,0,1,sf::Color::Blue));

    std::cout<<"Initializing Boost, Numpy \n";
    Py_Initialize();
    np::initialize();


    std::cout<<"Initializing Entity Manager\n";
    entity_manager_= new entities::EntityManager();

    if(render_to_screen_ || render_to_file_){
        std::cout<<"Initializing RenderEngine\n";
        render_engine_ = new rendering::RenderEngine(teams_);
    }

    std::cout<<"Space Environment initialization DONE\n\n";
}

np::ndarray EnvironmentController::GetObservations() {
//    py::tuple shape = py::make_tuple(2,1024);
//    np::dtype dtype = np::dtype::get_builtin<float>();
//    np::ndarray a = np::zeros(shape, dtype);
//    std::cout<<"test\ns";
//    reinterpret_cast<float*>(a.get_data())[2*1023]=100;
//    std::cout<< reinterpret_cast<float*>(a.get_data())[2*1023]<<std::endl;
//    return a;
    return entity_manager_->ships_wiev;
}

int EnvironmentController::Update(np::ndarray action_vector) {
    //todo Send it to the entity manager

    GetObservations();
    if (render_to_screen_) {
        float delta = toremove.getElapsedTime().asSeconds();
        if (delta > 0.02) {

            toremove.restart();

            if (true){
//                    //render_engine_->window.hasFocus()) {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {

                    render_to_screen_ = false;
                    free(render_engine_);
                    free(entity_manager_);
                    return 1;
                }

                float side_engine=0;
                float main_engine = 0;
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
                    side_engine = -1;
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {

                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
                    side_engine = 1;
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
                    main_engine = 1;

                }

                entity_manager_->update(side_engine, main_engine);

                render_engine_->RenderShip(*entity_manager_);

                render_engine_->RenderAsteroids(*entity_manager_);

                render_engine_->RenderRays(*entity_manager_);

                render_engine_->RenderScreen();
            }
        }
    }
    return 0;
}