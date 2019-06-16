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

class NCheckpoints : public ScenarioBase {
public:
    int n;
    entites::ShipArray *shipArray;
    CheckpointArray *checkpointArray;

    float *observations;
    float *reward;
    float *distance;

    NCheckpoints(int n) : n(n) {
        shipArray = new entites::ShipArray(n);
        checkpointArray = new CheckpointArray(n, 100);
        observations = new float[n * 5];
        reward = new float[n];
        distance = new float[n];
        for (int i = 0; i < n; ++i) {
            distance[i] = entites::Distance(shipArray, i, checkpointArray, i);
            reward[i] = 0;
        }
    }

    ~NCheckpoints(){
        delete shipArray;
        delete checkpointArray;

        delete [] observations;
        delete [] reward;
        delete [] distance;
    }

    sf::VertexArray* InitializeVertexArray() override {
        sf::VertexArray* varr = new sf::VertexArray(sf::Triangles, 15*shipArray->n);
        for (int i = 0; i < shipArray->n; ++i) {
            sf::Color ship_color = sf::Color(rand()%(236)+20,rand()%(236)+20,rand()%(236)+20);

            for (int j = 0; j < 6; ++j) {
                varr->operator[](6*i+j).color = ship_color;
            }
            for (int j = 6; j < 9; ++j) {
                varr->operator[](9*i+j+6*shipArray->n).color = ship_color;
            }
        }
        return varr;
    }

    sf::VertexArray* Render(sf::VertexArray* varr) override {
        for (int i = 0; i < shipArray->n; ++i) {
            render_square(varr, i*6, checkpointArray, i);
        }
        for (int i = 0; i < shipArray->n; ++i) {
            render_ship(varr, shipArray->n*6 + i*9,shipArray, i);
        }
        return varr;
    }

    boost::python::tuple Step(float* actions) override {
        shipArray->Update(actions);

        std::memcpy(reward, distance, n * sizeof(float));

        for (int i = 0; i < n; ++i) {
            distance[i] = entites::Distance(shipArray, i, checkpointArray, i);
            if (distance[i] < 50) {
                reward[i] = 100;
                checkpointArray->ResetCheckpoint(i);
                distance[i] = entites::Distance(shipArray, i, checkpointArray, i);
            } else {
                reward[i] -= distance[i];
            }
        }

        std::memcpy(observations, distance, n * sizeof(float));
        std::memcpy(&observations[4 * n], shipArray->v_angle, n * sizeof(float));
        for (int i = 0; i < n; ++i) {

            float cx = checkpointArray->position[i] - shipArray->position[i];
            float cy = checkpointArray->position[n + i] - shipArray->position[n + i];

            float s = sinf(-shipArray->angle[i]);
            float c = cosf(-shipArray->angle[i]);

            observations[1 * n + i] = atan2f(s * cx + c * cy, c * cx - s * cy);
            observations[2 * n + i] = sqrtf(powf(shipArray->v[i], 2) + powf(shipArray->v[n + i], 2));
            observations[3 * n + i] = atan2f(s * shipArray->v[i] + c * shipArray->v[n + i],
                                             c * shipArray->v[i] - s * shipArray->v[n + i]);
        }

        // TODO optimize
        boost::python::tuple observation_shape = boost::python::make_tuple(n,5);
        boost::python::tuple reward_shape = boost::python::make_tuple(n);
        boost::python::tuple strideo = boost::python::make_tuple(n*sizeof(float),sizeof(float));
        boost::python::tuple strider = boost::python::make_tuple(sizeof(float));
        boost::python::object own;
        boost::python::object owner;
        boost::python::numpy::dtype dt = boost::python::numpy::dtype::get_builtin<float>();

        auto a = boost::python::numpy::from_data(observations, dt, observation_shape, strideo, own);
        auto b =boost::python::numpy::from_data(reward, dt, reward_shape, strider, own);

        return boost::python::make_tuple(
                a,
                b
                );
    }

    int test(){
        return 100;
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
