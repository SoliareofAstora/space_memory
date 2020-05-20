//
// Created by SoliareofAstora on 17.12.2019.
//

#ifndef SPACE_ENVIRO_ENTITIES_ENTITIES_DATA_ENTITY_ARRAY_HPP_
#define SPACE_ENVIRO_ENTITIES_ENTITIES_DATA_ENTITY_ARRAY_HPP_

#include <algorithm>
#include <cmath>
#include <utility>

namespace entity_data{

struct BaseEntity {
  const int n;
  int remaining;
  float* position;
  float* size;

  float default_size = 25;

  BaseEntity(const int entities_count):n(entities_count) {
    position = new float[2 * n]();
    size = new float[n]();
    remaining = n;
    ResetValues();
  }

  BaseEntity(const int entities_count, float new_size):n(entities_count) {
    position = new float[2 * n]();
    size = new float[n]();
    remaining = n;
    default_size = new_size;
    ResetValues();
  }

  ~BaseEntity() {
    delete[] position;
    delete[] size;
  }

  void ChangePosition(int index, float new_x, float new_y) {
    position[index] = new_x;
    position[n + index] = new_y;
  }

  void ChangePosition(float* new_positions) {
    position = new_positions;
  }

  void Reset(int index) {
    position[index] = 0;
    position[n + index] = 0;
  }

  void ResetValues() {
    std::fill(position, position + 2 * n, 0);
    std::fill(size, size + n, default_size);
  }

  //Returns true if element you want to delete is the current last one
  //UNTESTED PIPELINE
  bool Remove(int index) {
    remaining--;
    if (remaining == index) { return true; }
    position[index] = position[remaining];
    position[n + index] = position[n + remaining];
    size[index] = size[remaining];
    return false;
  }

};

float Distance(BaseEntity* a, int i, BaseEntity* b, int j) {
  return sqrtf(
      powf(a->position[i] - b->position[j], 2)
          + powf(a->position[a->n + i] - b->position[b->n + j], 2));
}

void PairwiseDistance(BaseEntity* a, BaseEntity* b, float* output) {
  for (int i = 0; i < a->n; ++i) {
    output[i] = sqrtf(powf(a->position[i] - b->position[i], 2)
                          + powf(a->position[a->n + i] - b->position[a->n + i], 2));
  }
}

}// namespace entity_data
#endif //SPACE_ENVIRO_ENTITIES_ENTITIES_DATA_ENTITY_ARRAY_HPP_
