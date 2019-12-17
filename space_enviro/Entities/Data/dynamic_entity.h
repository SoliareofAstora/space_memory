//
// Created by SoliareofAstora on 17.12.2019.
//

#ifndef SPACE_ENVIRO_ENTITIES_PRIMITIVES_DYNAMIC_ENTITY_HPP_
#define SPACE_ENVIRO_ENTITIES_PRIMITIVES_DYNAMIC_ENTITY_HPP_

#include <algorithm>
#include <cmath>
#include <utility>
#include "base_entity.h"
#include "../../global_config.h"


namespace entity_data{

struct DynamicEntity:BaseEntity {
  float* v;
  float* a;
  float* mass;

  DynamicEntity(const int entities_count):BaseEntity(entities_count) {
    v = new float[2 * n]();
    a = new float[2 * n]();
    mass = new float[n]();
    ResetValues();
  }

  ~DynamicEntity() {
    delete[] v;
    delete[] a;
    delete[] mass;
  }

  bool Remove(int index) {
    if (BaseEntity::Remove(index)) { return true; }
    v[index] = v[remaining];
    v[n + index] = v[n + remaining];
    a[index] = a[remaining];
    a[n + index] = a[n + remaining];
    mass[index] = mass[remaining];
    return false;
  }

  void ResetValues() {
    std::fill(v, v + 2 * n, 0);
    std::fill(a, a + 2 * n, 0);
    std::fill(mass, mass + n, 100);
  }
  void ResetAllValues() {
    BaseEntity::ResetValues();
    ResetValues();
  }

  void Reset(int index) {
    BaseEntity::Reset(index);
    v[index] = 0;
    v[n + index] = 0;
    a[index] = 0;
    a[n + index] =0;
  }
  void Update() {
//            todo parallel here
    for (int i = 0; i < remaining; ++i) {
      v[i] += a[i] * time_step;
    }
    for (int i = 0; i < remaining; ++i) {
      v[n + i] += a[n + i] * time_step;
    }
    for (int i = 0; i < remaining; ++i) {
      position[i] += v[i] * time_step;
    }
    for (int i = 0; i < remaining; ++i) {
      position[n + i] += v[n + i] * time_step;
    }
  }
  // todo Gravity_influence
};
}// namespace entity_data

#endif //SPACE_ENVIRO_ENTITIES_PRIMITIVES_DYNAMIC_ENTITY_HPP_
