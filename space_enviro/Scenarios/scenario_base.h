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

    virtual sf::VertexArray* InitializeVertexArray(){}
    virtual sf::VertexArray* Render(sf::VertexArray* varr){}

    virtual sf::VertexArray* InitializeDebugRender(){}
    virtual sf::VertexArray* RenderDebug(sf::VertexArray* varr){}

    virtual boost::python::tuple Step(boost::python::numpy::ndarray* actions){}

    virtual ~ScenarioBase(){}
};

#endif //SPACE_ENVIRO_SCENARIO_BASE_H
