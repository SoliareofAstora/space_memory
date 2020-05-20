//
// Created by SoliareofAstora on 10.02.2020.
//

#ifndef SPACE_ENVIRO_SCENARIO_BASE_H_LOAD_H_
#define SPACE_ENVIRO_SCENARIO_BASE_H_LOAD_H_

#include <SFML/Graphics/VertexArray.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/numpy.hpp>
#include <boost/python.hpp>
#include "base.h"
#include "checkpoint_single.h"
#include "checkpoint_double.h"
#include "stopping.hpp"

namespace scenario{

ScenarioBase* load_scenario(const boost::python::dict &parameters) {

  std::string sn = boost::python::extract<std::string>(parameters["scenario_name"]);
  if (sn == "checkpoint_single")
    return new CheckpointSingle(parameters);
//  if (sn == "checkpoints")
//    return new CheckpointDouble(parameters);
  if (sn == "stopping")
    return new Stopping(parameters);

  std::cout << "Scenario " + sn + " Not found";
  exit(1);
}

} //namespace scenario

#endif //SPACE_ENVIRO_SCENARIO_BASE_H_LOAD_H_
