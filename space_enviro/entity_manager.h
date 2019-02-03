//
// Created by overlord on 27/01/19.
//

#ifndef SPACE_ENVIRO_ENTITY_MANAGER_H
#define SPACE_ENVIRO_ENTITY_MANAGER_H

#include <math.h>
#include <iostream>

namespace entities{


    class EntityManager {
    public:
        float ship_x; // position on X axis
        float ship_y; // position on Y axis
        float ship_angle; // angle from the south direction measured in radians
        float ship_size; //ship size used to calculate visualization, gravitational impact and collisions
        float ship_mass;
        int ship_hp;
        int shoot_delay_counter;
        float main_engine_power;
        float side_booster_power;
        float direction = 0;
        float ax=0, ay=0, vx=0, vy = 0;
        float angle_v = 0;
        float angle_a = 0;

        float maininput = 0;
        float sideinput=0;

        float center = 500;
        float centerMass = 99999999;

        EntityManager(){
            ship_x=1000;
            ship_y=1000;
            ship_angle = 0;
            angle_v = 0;
            angle_a=0;
            ship_mass = 10000;
            ship_size = 50;
            main_engine_power = 1000000;
            side_booster_power = 4000000;
            vx = 0;
            vy = 0;

        }

        void update(float dir, float power, float main_engine){
            maininput = main_engine;
            sideinput = power;
            direction = dir;

            float dtime = 0.02;

            ax = maininput*sin(ship_angle*3.14159265/180)*main_engine_power/ship_mass;
            ay = maininput*cos(ship_angle*3.14159265/180)*main_engine_power/ship_mass;

            float dx = ship_x-center;
            float dy = ship_y-center;
            float distance = pow(dx,2)+pow(dy,2);
            dx = dx/distance;
            dy = dy/distance;

            float gravx = -dx*centerMass/distance;
            float gravy = -dy*centerMass/distance;

            vx+=(ax+gravx)*dtime;
            vy+=(ay+gravy)*dtime;
            ship_x+=vx*dtime;
            ship_y+=vy*dtime;

            float M = dir*side_booster_power*sideinput*ship_size;
            float I = 0.5 *ship_mass*ship_size*ship_size;
            float E = M/I;
            angle_v += E*dtime;
            ship_angle += angle_v*dtime;

            std::cout<<"Angle:"<<ship_angle*3.14159265/180<<" V:"<<vx<<" "<<vy<<"\n";
        }
    };

}


#endif //SPACE_ENVIRO_ENTITY_MANAGER_H
