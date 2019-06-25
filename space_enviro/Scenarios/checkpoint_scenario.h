//
// Created by overlord on 29/05/19.
//

#ifndef SPACE_ENVIRO_CHECKPOINT_SCENARIO_H
#define SPACE_ENVIRO_CHECKPOINT_SCENARIO_H

#include <cmath>
#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include "scenario_base.h"
#include "../Entities/entities_data.h"
#include "../Entities/checkpoint.h"
#include "../Entities/basic_render_types.h"

class Checkpoints : public ScenarioBase {
public:
    int n;
    entites::ShipArray* ship_array;
    CheckpointArray* checkpoint_array;

    float* observations;
    float* reward;
    float* distance;

    Checkpoints(int n) : n(n) {
        ship_array = new entites::ShipArray(n);
        checkpoint_array = new CheckpointArray(n, 700);
        observations = new float[n * 5];
        reward = new float[n];
        distance = new float[n];
        for (int i = 0; i < n; ++i) {
            distance[i] = entites::Distance(ship_array, i, checkpoint_array, i);
            reward[i] = 0;
        }
    }

  ~Checkpoints(){
        delete ship_array;
        delete checkpoint_array;

        delete [] observations;
        delete [] reward;
        delete [] distance;
    }

    sf::VertexArray* InitializeVertexArray() override {
        sf::VertexArray* vertex_array = new sf::VertexArray(sf::Triangles, 15*ship_array->n);
        for (int i = 0; i < ship_array->n; ++i) {
            sf::Color ship_color = sf::Color(random()%(236)+20,random()%(236)+20,random()%(236)+20);

            for (int j = 0; j < 6; ++j) {
                vertex_array->operator[](6*i+j).color = ship_color;
            }
            for (int j = 6; j < 9; ++j) {
                vertex_array->operator[](9*i+j+6*ship_array->n).color = ship_color;
            }
        }
        return vertex_array;
    }

    sf::VertexArray* Render(sf::VertexArray*vertex_array) override {
        for (int i = 0; i < ship_array->n; ++i) {
          RenderSquare(vertex_array, i * 6, checkpoint_array, i);
        }
        for (int i = 0; i < ship_array->n; ++i) {
          RenderShip(vertex_array, ship_array->n * 6 + i * 9, ship_array, i);
        }
        return vertex_array;
    }

    boost::python::tuple Step(float* actions) override {
        ship_array->Update(actions);

        std::memcpy(reward, distance, n * sizeof(float));

        for (int i = 0; i < n; ++i) {
            distance[i] = entites::Distance(ship_array, i, checkpoint_array, i);
            if (distance[i] < 50) {
                reward[i] = 100;
                checkpoint_array->ResetCheckpoint(i);
                distance[i] = entites::Distance(ship_array, i, checkpoint_array, i);
            } else {
                reward[i] -= distance[i];
            }
        }

        std::memcpy(observations, distance, n * sizeof(float));
        std::memcpy(&observations[4 * n], ship_array->v_angle, n * sizeof(float));
        for (int i = 0; i < n; ++i) {

            float cx = checkpoint_array->position[i] - ship_array->position[i];
            float cy = checkpoint_array->position[n + i] - ship_array->position[n + i];

            float s = sinf(-ship_array->angle[i]);
            float c = cosf(-ship_array->angle[i]);

            observations[1 * n + i] = atan2f(s * cx + c * cy, c * cx - s * cy);
            observations[2 * n + i] = sqrtf(powf(ship_array->v[i], 2) + powf(ship_array->v[n + i], 2));
            observations[3 * n + i] = atan2f(s * ship_array->v[i] + c * ship_array->v[n + i],
                                             c * ship_array->v[i] - s * ship_array->v[n + i]);
        }

        // TODO optimize
        boost::python::tuple observation_shape = boost::python::make_tuple(n,5);
        boost::python::tuple reward_shape = boost::python::make_tuple(n);
        boost::python::tuple strideo = boost::python::make_tuple(sizeof(float),n*sizeof(float));
        boost::python::tuple strider = boost::python::make_tuple(sizeof(float));
        boost::python::object own;
        boost::python::numpy::dtype dt = boost::python::numpy::dtype::get_builtin<float>();

        auto a = boost::python::numpy::from_data(observations, dt, observation_shape, strideo, own);
        auto b =boost::python::numpy::from_data(reward, dt, reward_shape, strider, own);

        return boost::python::make_tuple(
                a,
                b
                );
    }

    boost::python::numpy::ndarray Reset() override {
//        ship_array->ResetAllValues();
//        checkpoint_array->Reset();
        for (int i = 0; i < n; ++i) {
            distance[i] = entites::Distance(ship_array, i, checkpoint_array, i);
            if (distance[i] < 50) {
                reward[i] = 100;
                checkpoint_array->ResetCheckpoint(i);
                distance[i] = entites::Distance(ship_array, i, checkpoint_array, i);
            } else {
                reward[i] -= distance[i];
            }
        }

        std::memcpy(observations, distance, n * sizeof(float));
        std::memcpy(&observations[4 * n], ship_array->v_angle, n * sizeof(float));
        for (int i = 0; i < n; ++i) {

            float cx = checkpoint_array->position[i] - ship_array->position[i];
            float cy = checkpoint_array->position[n + i] - ship_array->position[n + i];

            float s = sinf(-ship_array->angle[i]);
            float c = cosf(-ship_array->angle[i]);

            observations[1 * n + i] = atan2f(s * cx + c * cy, c * cx - s * cy);
            observations[2 * n + i] = sqrtf(powf(ship_array->v[i], 2) + powf(ship_array->v[n + i], 2));
            observations[3 * n + i] = atan2f(s * ship_array->v[i] + c * ship_array->v[n + i],
                                             c * ship_array->v[i] - s * ship_array->v[n + i]);
        }

        boost::python::tuple observation_shape = boost::python::make_tuple(n,5);
        boost::python::tuple strideo = boost::python::make_tuple(sizeof(float),n*sizeof(float));
        boost::python::object own;
        boost::python::numpy::dtype dt = boost::python::numpy::dtype::get_builtin<float>();

        return boost::python::numpy::from_data(observations, dt, observation_shape, strideo, own);
    }
};

/*
 * Obserwacje
 * 0. odległość od celu
 * 1. Kąt do celu
 * 2. długość prędkości
 * 3. Kierunek prędkości
 * 4. Prędkość kątowa
 *
 */


//#include <iostream>
//#include <cstdint>
//#include <cstring>
//#include <algorithm>
//#include <cmath>
//#include <utility>
//
//int main()
//{
//    float pi = 3.14159265359;
//    float sx = 3;
//    float sy = 2;
//
//    float cx = 5;
//    float cy = 4;
//
//    cx-= sx;
//    cy-=sy;
//
//    float sangle = -1*pi/4;
//
//    float c = cosf(-sangle);
//    float s = sinf(-sangle);
//
//    float rx = c*cx-s*cy;
//    float ry = s*cx+c*cy;
//
//    std::cout<<sqrt(pow(cx,2)+pow(cy,2))<<"  "<<sqrt(pow(rx,2)+pow(ry,2))<<"  "<<180*atan2(ry,rx)/pi;
//
//    return 0;
//}

#endif //SPACE_ENVIRO_CHECKPOINT_SCENARIO_H
