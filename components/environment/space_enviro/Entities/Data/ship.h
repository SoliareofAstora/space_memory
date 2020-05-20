//
// Created by SoliareofAstora on 17.12.2019.
//

#ifndef SPACE_ENVIRO_ENTITIES_DATA_SHIP_H_
#define SPACE_ENVIRO_ENTITIES_DATA_SHIP_H_

#include <algorithm>
#include <cmath>
#include <utility>
#include "dynamic_entity.h"
#include "../../global_config.h"

namespace entity_data{

struct Ship:DynamicEntity {
  float* hit_points;
  float* main_engine_power;
  float* rotation_engine_power;

  float* angle;
  float* v_angle;
  float* a_angle;

  //this will point to numpy array from python
  float* engines_control;

  float default_hitpoints = 100;
  float default_main_engine_power = 100;
  float default_rotation_engine_power = 100;

  Ship(const int entities_count):DynamicEntity(entities_count) {
    hit_points = new float[n]();
    main_engine_power = new float[n]();
    rotation_engine_power = new float[n]();
    angle = new float[n]();
    a_angle = new float[n]();
    v_angle = new float[n]();
    ResetValues();
  }

  Ship(const int entities_count, float new_hitpoints, float new_main_engine_power,
       float new_rotation_engine_power, float new_mass, float new_size
  ):DynamicEntity(entities_count, new_mass, new_size) {
    hit_points = new float[n]();
    main_engine_power = new float[n]();
    rotation_engine_power = new float[n]();
    angle = new float[n]();
    a_angle = new float[n]();
    v_angle = new float[n]();
    default_hitpoints = new_hitpoints;
    default_main_engine_power = new_main_engine_power;
    default_rotation_engine_power = new_rotation_engine_power;
    ResetValues();
  }

  void ResetValues() {
    std::fill(hit_points, hit_points + n, default_hitpoints);
    std::fill(main_engine_power, main_engine_power + n, default_main_engine_power);
    std::fill(rotation_engine_power, rotation_engine_power + n, default_rotation_engine_power);
    std::fill(angle, angle + n, 0);
    std::fill(a_angle, a_angle + n, 0);
    std::fill(v_angle, v_angle + n, 0);
  }

  void ResetAllValues() {
    DynamicEntity::ResetAllValues();
    ResetValues();
  }

  bool Remove(int index) {
    if (DynamicEntity::Remove(index)) { return true; }
    hit_points[index] = hit_points[remaining];
    main_engine_power[index] = main_engine_power[remaining];
    rotation_engine_power[index] = rotation_engine_power[remaining];

    angle[index] = angle[remaining];
    v_angle[index] = v_angle[remaining];
    a_angle[index] = a_angle[remaining];

    engines_control[index] = engines_control[remaining];
    engines_control[n + index] = engines_control[n + remaining];

    return false;
  }

  void Reset(int index) {
    DynamicEntity::Reset(index);
    angle[index] = 0; //randNum(360);
    v_angle[index] = 0;
    a_angle[index] = 0;
  }

  void ResetWithRandomVelocity(int index, int minV, int maxV, int maxAngleV) {
    DynamicEntity::ResetWithRandomVelocity(index, minV, maxV);
    angle[index] = RNG::RandomFloat(360);
    v_angle[index] = RNG::RandomFloatMirror(maxAngleV);
    a_angle[index] = 0;
  }

  void Update(float* actions) {
    engines_control = actions;
    for (int i = 0; i < remaining; ++i) {
      a[i] = engines_control[i] * sinf(angle[i]) * main_engine_power[i] / mass[i];
      a[i + n] = engines_control[i] * cosf(angle[i]) * main_engine_power[i] / mass[i];
    }

    for (int i = 0; i < remaining; ++i) {
      a_angle[i] = (rotation_engine_power[i] * engines_control[i + remaining]) / (0.5f * mass[i] * size[i]);
      v_angle[i] += a_angle[i] * time_step;
      angle[i] += v_angle[i] * time_step;
    }
    DynamicEntity::Update();
  }

  ~Ship() {
    delete[] hit_points;
    delete[] main_engine_power;
    delete[] rotation_engine_power;

    delete[] angle;
    delete[] v_angle;
    delete[] a_angle;
  }
};

Ship* InitializeShipArrayFromDict(const boost::python::dict &parameters) {
  return new Ship(boost::python::extract<int>(parameters["n"]),
                  boost::python::extract<float>(parameters["default_hitpoints"]),
                  boost::python::extract<float>(parameters["default_main_engine_power"]),
                  boost::python::extract<float>(parameters["default_rotation_engine_power"]),
                  boost::python::extract<float>(parameters["default_mass"]),
                  boost::python::extract<float>(parameters["default_size"]));
}
}// namespace entity_data

#endif //SPACE_ENVIRO_ENTITIES_DATA_SHIP_H_
