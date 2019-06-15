//
// Created by overlord on 27/01/19.
//

#ifndef SPACE_ENVIRO_ENTITY_MANAGER_H
#define SPACE_ENVIRO_ENTITY_MANAGER_H

#include <cmath>
#include <iostream>
#include <boost/python/numpy.hpp>

namespace np = boost::python::numpy;

namespace entities{

    float RandomFloat(float min, float max);

    struct asteroid{
        float x; // position on X axis
        float y; // position on Y axis
        float vx=0;
        float vy = 0;
        float size = 0;
        int range = 500;
        asteroid(){
            reset_asteroid();
        }

        void reset_asteroid(){
            float angle = RandomFloat(0.f,2.f)*M_PIf32;
            x = sinf(angle)*range;
            y = cosf(angle)*range;

            angle = angle - M_PIf32 + RandomFloat(-1.f,1.f);
            float speed = RandomFloat(0.f,50.f);
            vx = sinf(angle)*speed;
            vy = cosf(angle)*speed;

            size = RandomFloat(10,25);
        }

        void update_position(float shipvx, float shipvy){
            float dtime = 0.02;
            x+=(vx-shipvx)*dtime;
            y+=(vy-shipvy)*dtime;
        }
    };


    class EntityManager {
    public:
        float x; // position on X axis
        float y; // position on Y axis
        float angle; // angle from the south direction measured in radians
        float size; //ship size used to calculate visualization, gravitational impact and collisions
        float ship_mass;
        float main_engine_power;
        float side_booster_power;
        float ax=0, ay=0, vx=0, vy = 0;
        float angle_v = 0;
        float angle_a = 0;

        float sideinput = 0;
        float maininput = 0;

        const static int n_asteroids = 40;
        asteroid asteroids[n_asteroids];

        float wiev_range = 500;
        float wiev_angle = 5.f;
        const int n_rays = 128;
        np::ndarray ships_wiev = np::zeros(boost::python::make_tuple(1,2,n_rays), np::dtype::get_builtin<float>());
        np::ndarray ships_stats = np::zeros(boost::python::make_tuple(1,7), np::dtype::get_builtin<float>());
        float step = wiev_angle/n_rays;
        float offset = step/2;
        float begin = wiev_angle/2-offset;

        int epoch_counter_ = 0;
        float reward =0;
        float total_reward = 0;
        float max_reward= 0;

        EntityManager(){
            x=0;
            y=0;
            angle = M_PIf32;
            angle_v = 0;
            angle_a=0;
            ship_mass = 1000;
            size = 50;
            main_engine_power = 2000000;
            side_booster_power = 1000000;
            vx = 0;
            vy = 0;

            for (auto &i : asteroids) {
                i = asteroid();
            }


        }

        void reset(){
            for (auto &asteroid : asteroids) {
                asteroid.reset_asteroid();
            }
            angle = M_PIf32;
            angle_v = 0;
            vx = 0;
            vy = 0;
            std::memset(reinterpret_cast<float*>(ships_wiev.get_data()),0,1*2*n_rays* sizeof(float));

            if(total_reward>max_reward)max_reward=total_reward;
            reward = -100;
            total_reward = 0;
            epoch_counter_++;
        }
        void save_stats(){
            reinterpret_cast<float *>(ships_stats.get_data())[0] = vx;
            reinterpret_cast<float *>(ships_stats.get_data())[1] = vy;
            reinterpret_cast<float *>(ships_stats.get_data())[2] = angle_v;
            reinterpret_cast<float *>(ships_stats.get_data())[3] = sinf(angle);
            reinterpret_cast<float *>(ships_stats.get_data())[4] = cosf(angle);
            reinterpret_cast<float *>(ships_stats.get_data())[5] = angle;
//            for (int i = 0; i < 5; ++i) {
//                std::cout<<reinterpret_cast<float *>(ships_stats.get_data())[i]<<" ";
//
//            }
            std::cout<<std::endl;
        }

        bool update(float side_engine, float main_engine) {

            float dtime = 0.02;


            std::memset(reinterpret_cast<float *>(ships_wiev.get_data()), 0, 1 * 2 * n_rays * sizeof(float));

            maininput = main_engine;
            sideinput = side_engine;

            ax = maininput * sinf(angle) * main_engine_power / ship_mass;
            ay = maininput * cosf(angle) * main_engine_power / ship_mass;
            vx += ax * dtime;
            vy += ay * dtime;

//            reward = sqrtf(powf(vx, 2) + powf(vy, 2))/1000;
//            total_reward += reward;

            angle_a = (side_booster_power * sideinput) / (0.5f * ship_mass * size);
            angle_v += angle_a * dtime;
            angle += angle_v * dtime;



            if (angle > 2 * M_PIf32) angle -= 2 * M_PIf32;
            if (angle < 0) angle += 2 * M_PIf32;

            save_stats();

            for (auto &asteroid : asteroids) {
                asteroid.update_position(vx, vy);
                auto a = asteroid;
                float real_distance = sqrtf(powf(a.x, 2) + powf(a.y, 2));
                if (real_distance > 800) {
                    asteroid.reset_asteroid();
                }
                float distance_between = real_distance - a.size;

                //Obiekt znajduje się w zasięgu widzenia
                if (distance_between < wiev_range) {
                    //RESET SYMULACJI PO KOLIZJI
                    if (distance_between - size < 0) {
                        reset();
                        return true;
                    }

                    float alfa = asinf(a.size / real_distance);
//                    float beta = acosf((a.x*sinf(angle)+a.y*cosf(angle))/real_distance);

                    float beta = atan2(a.x, a.y) - angle;
                    if (beta > M_PIf32) beta -= 2 * M_PIf32;
                    if (beta < -M_PIf32) beta += 2 * M_PIf32;

                    if (abs(beta) - alfa < wiev_angle / 2) {

                        int start = std::max(0, static_cast<int>(ceilf((beta - alfa + begin) / step)));
                        int end = std::min(n_rays, static_cast<int>(floorf((beta + alfa + begin) / step)) + 1);
                        for (int i = start; i < end; ++i) {
                            float ray_angle = -begin + i * step;
                            float gamma = beta - ray_angle;
                            float dist = real_distance - a.size * sinf(acosf((sinf(gamma) * real_distance) / a.size));
                            if (dist < wiev_range) {
                                dist = 1 - dist / wiev_range;
                                if (dist > reinterpret_cast<float *>(ships_wiev.get_data())[i]) {
                                    reinterpret_cast<float *>(ships_wiev.get_data())[n_rays + i] = 1;
                                    reinterpret_cast<float *>(ships_wiev.get_data())[i] = dist;
                                }
                            }
                        }
                    }
                }
            }
            x+=vx*dtime;
            y+=vy*dtime;
            return false;
        }
    };

}


#endif //SPACE_ENVIRO_ENTITY_MANAGER_H
