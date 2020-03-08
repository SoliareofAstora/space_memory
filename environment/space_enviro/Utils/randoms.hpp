//
// Created by SoliareofAstora on 14.02.2020.
//

#ifndef SPACE_ENVIRO_UTILS_RANDOMS_HPP_
#define SPACE_ENVIRO_UTILS_RANDOMS_HPP_

#include <cstdlib>
namespace RNG{

int RandomInt() {
  return rand();
}
int RandomInt(int max) {
  return rand() % max;
}
int RandomInt(int lo, int hi) {
  return rand() % (hi - lo) + lo;
}
int RandomIntMirror(int limit) {
  return RandomInt(-limit, limit);
}

float RandomFloat() {
  return float(rand()) / float(RAND_MAX);
}

float RandomFloat(float max) {
  return RandomFloat() * max;
}

float RandomFloat(int max) {
  return RandomFloat() * float(max);
}

float RandomFloat(float lo, float hi) {
  return RandomFloat(hi - lo) + lo;
}
float RandomFloat(int lo, int hi) {
  return RandomFloat(float(lo),float(hi));
}
float RandomFloatMirror(float limit) {
  return RandomFloat(-limit, limit);
}
float RandomFloatMirror(int limit){
  return RandomFloatMirror(float(limit));
}
}//namespace RNG

#endif //SPACE_ENVIRO_UTILS_RANDOMS_HPP_
