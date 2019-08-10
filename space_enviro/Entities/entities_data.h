//
// Created by SoliareofAstora on 13/02/19.
//

#ifndef SPACE_ENVIRO_ENTITIES_DATA_H
#define SPACE_ENVIRO_ENTITIES_DATA_H

#include <algorithm>
#include <cmath>
#include <utility>
#include "../global_config.h"

namespace entites{
//todo: enum klasy do renderowania
//    enum type;
//std::transform(result.begin(), result.end(), result_temp.begin(),
//               result.begin(), std::plus<double>());

int randNum(int max_distance) {
  return random() % (2 * max_distance) - max_distance;
}

struct EntityArray {
  const int n;
  int remaining;
  float* position;
  float* size;

  EntityArray(const int entities_count):n(entities_count) {
    position = new float[2 * n]();
    size = new float[n]();
    remaining = n;
    ResetValues();
  }

  ~EntityArray() {
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
//    position[index] = randNum(200);
//    position[index+n] = randNum(200);
    position[index] = 0;
    position[index+n] = 0;
  }

  void ResetValues() {
    std::fill(position, position + 2 * n, 0);
    std::fill(size, size + n, 25);
  }

  //Returns true if element you want to delete is the current last one
  bool Remove(int index) {
    remaining--;
    if (remaining == index) { return true; }
    position[index] = position[remaining];
    position[n + index] = position[n + remaining];
    size[index] = size[remaining];
    return false;
  }
};

float Distance(EntityArray* a, int i, EntityArray* b, int j) {
  return sqrtf(
      powf(a->position[i] - b->position[j], 2)
          + powf(a->position[a->n + i] - b->position[b->n + j], 2));
}

struct DynamicArray:EntityArray {
  float* v;
  float* a;
  float* mass;

  DynamicArray(const int entities_count):EntityArray(entities_count) {
    v = new float[2 * n]();
    a = new float[2 * n]();
    mass = new float[n]();
    ResetValues();
  }

  ~DynamicArray() {
    delete[] v;
    delete[] a;
    delete[] mass;
  }

  bool Remove(int index) {
    if (EntityArray::Remove(index)) { return true; }
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
    EntityArray::ResetValues();
    ResetValues();
  }

  void Reset(int index) {
    EntityArray::Reset(index);
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

struct ShipArray:DynamicArray {
  float* hit_points;
  float* main_engine_power;
  float* side_engine_power;

  float* angle;
  float* v_angle;
  float* a_angle;

  float* engines_control;

  ShipArray(const int entities_count):DynamicArray(entities_count) {
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
    DynamicArray::ResetAllValues();
    ResetValues();
  }

  bool Remove(int index) {
    if (DynamicArray::Remove(index)) { return true; }
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
    DynamicArray::Reset(index);
    angle[index] = 0; //randNum(360);
    v_angle[index] = 0;
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
    DynamicArray::Update();
  }
  ~ShipArray() {
    delete[] hit_points;
    delete[] main_engine_power;
    delete[] side_engine_power;

    delete[] angle;
    delete[] v_angle;
    delete[] a_angle;

    delete[] engines_control;
  }
};

}//entities

#endif //SPACE_ENVIRO_ENTITIES_DATA_H

//#include <algorithm>
//#include <vector>
//#include <iostream>
//
//struct A{
//    int aa = 10;
//    void fun(int* output, A* other){
//        if (other == this){
//            *output = 100;
//            return;
//        }
//        *output=aa-other->aa;
//    }
//};
//
//struct B:A{
//    int bb = 1;
//};
//
//int main()
//{
//    A a = A();
//    B b = B();
//    b.aa=5;
//    int out = 0;
//    a.fun(&out,&a);
//
//    std::cout << out;