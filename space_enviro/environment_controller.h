//
// Created by overlord on 26/01/19.
//

#ifndef SPACE_ENVIRO_ENVIROMENT_CONTROLLER_H
#define SPACE_ENVIRO_ENVIROMENT_CONTROLLER_H

#include <iostream>
//#include <stdlib.h>
//#include <vector>
#include <SFML/Window/Event.hpp>
#include <boost/python.hpp>
#include <boost/python/numpy.hpp>

#include "render_engine.h"
#include "Scenarios/scenario_base.h"
#include "Scenarios/checkpoint_scenario.h"
#include "global_config.h"
#include "waiter.h"


class EnvironmentController {

private:

    rendering::RenderEngine* render_engine_;
    ScenarioBase* scenario_;
    Waiter w = Waiter(static_cast<int>(time_step*100));

    bool render = true;

public:

    EnvironmentController(std::string path_to_config);

    ~EnvironmentController(){
        delete scenario_;
    };
    // todo change it to real env.done
    bool simulation(){ return !render;}

    boost::python::tuple Step(boost::python::numpy::ndarray action_vector);
    boost::python::numpy::ndarray Reset(){
        return scenario_->Reset();
    };
};


BOOST_PYTHON_MODULE(spaceLib)
{
    boost::python::class_< EnvironmentController >(
            "initialize", boost::python::init<std::string>())
        .def("done", &EnvironmentController::simulation)
        .def("step", &EnvironmentController::Step)
        .def("reset", &EnvironmentController::Reset);
}

#endif //SPACE_ENVIRO_ENVIROMENT_CONTROLLER_H
//BOOST_PYTHON_MODULE(spaceLib)
//{
//    class_< hello >("hello", init<std::string>())
//            .def("greet", &hello::greet)
//            .def("display",&hello::display)
//            .def("stillPlaying", &hello::stillplayin)
//            .add_property("msg", &hello::get_msg, &hello::set_msg);
//}

//
//#include <iostream>
//#include <string>
//#include <SFML/Graphics.hpp>
//#include <boost/python/numpy.hpp>
//#include <boost/python.hpp>
//
//using namespace boost::python;
//namespace np = boost::python::numpy;
//
//class hello {
//
//public:
//    // Private attribute
//    std::string m_msg;
//    sf::RenderWindow* window = new sf::RenderWindow(sf::VideoMode(1000, 1000), "SFML works!");
//    sf::CircleShape shape;
//
//    // Constructor
//    hello(std::string msg) :m_msg(msg){
//        //   Py_Initialize();
//        //  np::initialize();
//        shape = sf::CircleShape(100.f);
//        shape.setFillColor(sf::Color::Green);
//    }
//
//    // Methods
//    void greet() { std::cout << m_msg << std::endl; }
//
//    // Getter/Setter functions for the attribute
//    void set_msg(std::string msg) { this->m_msg = msg; }
//
//    std::string get_msg() const { return m_msg; }
//
////    np::ndarray func(){
////        tuple shape = make_tuple(3, 3.f);
////        np::dtype dtype = np::dtype::get_builtin<double>();
////        np::ndarray a = np::zeros(shape, dtype);
////        return a;
////    }
//
//    bool stillplayin() const { return window->isOpen(); }
//
//    void display() {
//        sf::Event event;
//        while (window->pollEvent(event)) {
//            if (event.type == sf::Event::Closed)
//                window->close();
//        }
//        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
//            shape.setFillColor(sf::Color::Red);
//        }
//        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
//            shape.setFillColor(sf::Color::Blue);
//        }
//        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
//            shape.setFillColor(sf::Color::Green);
//        }
//        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)){
//            window->close();
//        }
//        window->clear();
//        window->draw(shape);
//        window->display();
//    }
//};
//
////            .def("getArr",&hello::func)
//
