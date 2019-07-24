//
// Created by overlord on 29/05/19.
//

#ifndef SPACE_ENVIRO_SCENARIO_BASE_H
#define SPACE_ENVIRO_SCENARIO_BASE_H

#include <SFML/Graphics/VertexArray.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/numpy.hpp>

struct ScenarioBase{
    ScenarioBase() = default;
    virtual ~ScenarioBase()= default;

    virtual sf::VertexArray* InitializeVertexArray(){}
    virtual sf::VertexArray* Render(sf::VertexArray* vertex_array){}

    virtual sf::VertexArray* InitializeDebugRender(){}
    virtual sf::VertexArray* RenderDebug(sf::VertexArray* vertex_array){}

    virtual boost::python::tuple Step(float* actions){}
    virtual boost::python::numpy::ndarray Reset(){}
    virtual int test(){}
};

#endif //SPACE_ENVIRO_SCENARIO_BASE_H