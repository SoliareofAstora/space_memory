//
// Created by SoliareofAstora on 09.02.2020.
//

#ifndef SPACE_ENVIRO_SCENARIO_LOAD_SCENARIO_HPP_
#define SPACE_ENVIRO_SCENARIO_LOAD_SCENARIO_HPP_

#include "scenario_base.h"
#include <boost/python.hpp>
#include "checkpoint_scenario.h"

ScenarioBase* load_scenario(const boost::python::dict& parameters){

  std::string scenario_name = boost::python::extract<std::string>(parameters["scenario_name"]);
  if(scenario_name=="checkpoints")
    return new Checkpoints(100);

  std::cout<<"Scenario "+scenario_name+" Not found";
  std::exit(1);
}

#endif //SPACE_ENVIRO_SCENARIO_LOAD_SCENARIO_HPP_
