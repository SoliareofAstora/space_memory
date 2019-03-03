//
// Created by overlord on 13/02/19.
//

#ifndef SPACE_ENVIRO_ENTITIES_DATA_H
#define SPACE_ENVIRO_ENTITIES_DATA_H


namespace entities{
    struct Entity{
        const int n;
        float* position_x;
        float* position_y;
        float* size;
        float* mass;

        Entity(int entities_count):n(entities_count){
            position_x = new float [n]();
            position_y = new float [n]();
            size = new float [n]();
            mass = new float [n]();
        }
    };

    struct Movable:Entity{
        float* ax;
        float* ay;
        float* vx;
        float* vy;

        Movable(int entities_count):Entity(entities_count){
            ax = new float [n]();
            ay = new float [n]();
            vx = new float [n]();
            vy = new float [n]();
        }
    };

    struct Ship:Movable{
        int* hit_points;
        float* engine_power;
        float* booster_power;
        float* angle;
        float* aangle;
        float* vangle;
        float* engine_input;
        float* booster_input;

        Ship(int entities_count):Movable(entities_count){
            hit_points = new int [n]();
            engine_power = new float [n]();
            booster_power = new float [n]();
            angle = new float [n]();
            aangle = new float [n]();
            vangle = new float [n]();
            engine_input = new float [n]();
            booster_input = new float [n]();
        }
    };


}//entities

#endif //SPACE_ENVIRO_ENTITIES_DATA_H
