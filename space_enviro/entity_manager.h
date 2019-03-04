//
// Created by overlord on 27/01/19.
//

#ifndef SPACE_ENVIRO_ENTITY_MANAGER_H
#define SPACE_ENVIRO_ENTITY_MANAGER_H

#include <cmath>
#include <iostream>
#include <boost/python/numpy.hpp>

#include "entities_data.h"
#include "constants.h"

namespace np = boost::python::numpy;

namespace entities{

    float RandomFloat(float min, float max);

    struct asteroid{
        float x; // position on X axis
        float y; // position on Y axis
        float vx=0;
        float vy = 0;
        float size = 0;
        int range = 750;
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

        int n_asteroids = 100;
        asteroid asteroids[100];

        float wiev_range = 500;
        float wiev_angle = 2.5f;
        const int n_rays = 512;
        np::ndarray ships_wiev = np::zeros(boost::python::make_tuple(1,2,n_rays), np::dtype::get_builtin<float>());
        float step = wiev_angle/n_rays;
        float offset = step/2;
        float begin = wiev_angle/2-offset;


        float reward =0;
        float total_reward = 0;

        EntityManager(){
            x=0;
            y=0;
            angle = M_PIf32;
            angle_v = 0;
            angle_a=0;
            ship_mass = 10000;
            size = 50;
            main_engine_power = 2000000;
            side_booster_power = 1000000;
            vx = 0;
            vy = 0;

            for (int i = 0; i < n_asteroids; ++i) {
                asteroids[i] = asteroid();
            }


        }

        void reset(){
            for (int i = 0; i < n_asteroids; ++i) {
                asteroids[i].reset_asteroid();
            }
            angle = M_PIf32;
            angle_v = 0;
            vx = 0;
            vy = 0;
            std::memset(reinterpret_cast<float*>(ships_wiev.get_data()),0,1*2*n_rays* sizeof(float));

            float reward = -100;
            float total_reward = 0;
        }

        void update(float side_engine, float main_engine){

            float dtime = 0.02;


            std::memset(reinterpret_cast<float*>(ships_wiev.get_data()),0,1*2*n_rays* sizeof(float));

            maininput = main_engine;
            sideinput = side_engine;

            ax = maininput*sinf(angle)*main_engine_power/ship_mass;
            ay = maininput*cosf(angle)*main_engine_power/ship_mass;
            vx+=ax*dtime;
            vy+=ay*dtime;

            angle_a = (side_booster_power * sideinput) / (0.5f * ship_mass * size);
            angle_v += angle_a*dtime;
            angle += angle_v*dtime;

            if(angle>2*M_PIf32) angle-=2*M_PIf32;
            if(angle<0) angle +=2*M_PIf32;

            for (int j = 0; j < n_asteroids; ++j) {
                asteroids[j].update_position(vx,vy);
                auto a = asteroids[j];
                float real_distance = sqrtf(powf(a.x,2)+powf(a.y,2));
                if(real_distance>800){
                    asteroids[j].reset_asteroid();
                }
                float distance_between = real_distance - a.size;

                //Obiekt znajduje się w zasięgu widzenia
                if(distance_between<wiev_range){
                    //RESET SYMULACJI PO KOLIZJI
//                    if(distance_between-size < 0){
//                        reset();
//                        break;
//                    }

                    float alfa = asinf(a.size/real_distance);
                    float asteroid_angle =atan2f(a.y,a.x);
                    float beta = asteroid_angle - angle;
                    //znajduje się w polu widzenia
                    if(beta-alfa < wiev_angle/2 or beta+alfa > -wiev_angle/2){
                        std::cout<<asteroid_angle/M_PI<<" "<<beta/M_PI<<"\n";

                        int start = std::max(0,static_cast<int>(ceilf((beta - alfa + begin) / step)));
                        int end = std::min(n_rays, static_cast<int>(floorf((beta + alfa + begin) / step))+1);
                        for (int i = start; i < end; ++i) {
                            float ray_angle = -begin+i*step;
                            float gamma = beta - ray_angle;
                            float dist = real_distance-a.size*sinf(acosf((sinf(gamma)*real_distance)/a.size));
                            if(dist<wiev_range){
                                dist = 1 - dist/wiev_range;
                                if(dist > reinterpret_cast<float*>(ships_wiev.get_data())[i]) {
                                    reinterpret_cast<float*>(ships_wiev.get_data())[512+i]=1;
                                    reinterpret_cast<float*>(ships_wiev.get_data())[i]=dist;
                                }
                            }
                        }


                    }
                }
            }
//            ship_x+=vx*dtime;
//            ship_y+=vy*dtime;

           // std::cout<<"Angle:"<<ship_angle<<" V:"<<vx<<" "<<vy<<"\n";

        }

        void update_view(){

        }
    };

}


#endif //SPACE_ENVIRO_ENTITY_MANAGER_H
