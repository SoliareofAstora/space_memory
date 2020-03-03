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
  float* side_engine_power;

  float* angle;
  float* v_angle;
  float* a_angle;

  float* engines_control;

  Ship(const int entities_count):DynamicEntity(entities_count) {
    hit_points = new float[n]();
    main_engine_power = new float[n]();
    side_engine_power = new float[n]();
    angle = new float[n]();
    a_angle = new float[n]();
    v_angle = new float[n]();
    engines_control = new float[2 * n]();
    ResetValues();
  }

  void ResetValues() {
    std::fill(hit_points, hit_points + n, 100);
    std::fill(main_engine_power, main_engine_power + n, 100);
    std::fill(side_engine_power, side_engine_power + n, 100);
    std::fill(angle, angle + n, 0);
    std::fill(a_angle, a_angle + n, 0);
    std::fill(v_angle, v_angle + n, 0);
    std::fill(engines_control, engines_control + 2 * n, 0);
  }

  void ResetAllValues() {
    DynamicEntity::ResetAllValues();
    ResetValues();
  }

  bool Remove(int index) {
    if (DynamicEntity::Remove(index)) { return true; }
    hit_points[index] = hit_points[remaining];
    main_engine_power[index] = main_engine_power[remaining];
    side_engine_power[index] = side_engine_power[remaining];

    angle[index] = angle[remaining];
    v_angle[index] = v_angle[remaining];
    a_angle[index] = a_angle[remaining];

    engines_control[index] = engines_control[remaining];
    engines_control[index + n] = engines_control[remaining + n];

    return false;
  }

  void Reset(int index) {
    DynamicEntity::Reset(index);
    angle[index] = 0; //randNum(360);
    v_angle[index] = 0;
    a_angle[index] = 0;

    engines_control[index] = 0;
    engines_control[index + n] = 0;
  }

  void ResetWithRandomVelocity(int index, int minV, int maxV, int maxAngleV) {
    DynamicEntity::ResetWithRandomVelocity(index, minV, maxV);
    angle[index] = RNG::RandomFloat(360);
    v_angle[index] = RNG::RandomFloatMirror(maxAngleV);
    a_angle[index] = 0;

    engines_control[index] = 0;
    engines_control[index + n] = 0;
  }

  void Update(float* actions) {
    engines_control = actions;
    for (int i = 0; i < remaining; ++i) {
      a[i] = engines_control[i] * sinf(angle[i]) * main_engine_power[i] / mass[i];
      a[i + n] = engines_control[i] * cosf(angle[i]) * main_engine_power[i] / mass[i];
    }

    for (int i = 0; i < remaining; ++i) {
      a_angle[i] = (side_engine_power[i] * engines_control[i + remaining]) / (0.5f * mass[i] * size[i]);
      v_angle[i] += a_angle[i] * time_step;
      angle[i] += v_angle[i] * time_step;
    }
    DynamicEntity::Update();
  }

  ~Ship() {
    delete[] hit_points;
    delete[] main_engine_power;
    delete[] side_engine_power;

    delete[] angle;
    delete[] v_angle;
    delete[] a_angle;

    delete[] engines_control;
  }
};
}// namespace entity_data

#endif //SPACE_ENVIRO_ENTITIES_DATA_SHIP_H_
