#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>
#include <boost/python/numpy.hpp>
#include <boost/python.hpp>

using namespace boost::python;
namespace np = boost::python::numpy;


class hello {

public:
    // Private attribute
    std::string m_msg;
    sf::RenderWindow* window = new sf::RenderWindow(sf::VideoMode(1000, 1000), "SFML works!");
    sf::CircleShape shape;

    // Constructor
    hello(std::string msg) :m_msg(msg){
        Py_Initialize();
        np::initialize();
        shape = sf::CircleShape(100.f);
        shape.setFillColor(sf::Color::Green);
    }

    // Methods
    void greet() { std::cout << m_msg << std::endl; }

    // Getter/Setter functions for the attribute
    void set_msg(std::string msg) { this->m_msg = msg; }

    std::string get_msg() const { return m_msg; }

    np::ndarray func(){
        tuple shape = make_tuple(3, 3);
        np::dtype dtype = np::dtype::get_builtin<double>();
        np::ndarray a = np::zeros(shape, dtype);
        return a;
    }

    bool stillplayin() const { return window->isOpen(); }

    void display() {
        sf::Event event;
        while (window->pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window->close();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
            shape.setFillColor(sf::Color::Red);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
            shape.setFillColor(sf::Color::Blue);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
            shape.setFillColor(sf::Color::Green);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)){
            window->close();
        }
        window->clear();
        window->draw(shape);
        window->display();
    }
};


BOOST_PYTHON_MODULE(spaceLib)
{
    class_< hello >("hello", init<std::string>())
            .def("greet", &hello::greet)
            .def("getArr",&hello::func)
            .def("display",&hello::display)
            .def("stillPlaying", &hello::stillplayin)
            .add_property("msg", &hello::get_msg, &hello::set_msg);
}