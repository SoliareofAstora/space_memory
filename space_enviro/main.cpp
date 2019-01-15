#include <iostream>
#include <string>

class hello
{
    // Private attribute
    std::string m_msg;
public:
    // Constructor
    hello(std::string msg):m_msg(msg) { }

    // Methods
    void greet() { std::cout << m_msg << std::endl; }

    // Getter/Setter functions for the attribute
    void set_msg(std::string msg) { this->m_msg = msg; }
    std::string get_msg() const { return m_msg; }
};

#include <boost/python.hpp>

using namespace boost::python;

BOOST_PYTHON_MODULE(spaceLib)
{
    class_< hello >("hello", init<std::string>())
            .def("greet", &hello::greet)
            .add_property("msg", &hello::get_msg, &hello::set_msg);
}