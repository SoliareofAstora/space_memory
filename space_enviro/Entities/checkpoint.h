//
// Created by overlord on 28/05/19.
//

#ifndef SPACE_ENVIRO_CHECKPOINT_H
#define SPACE_ENVIRO_CHECKPOINT_H

#include "entities_data.h"
#include <iostream>
#include <cstdlib>
#include <ctime>


struct CheckpointArray:entites::EntityArray{

    int max_distance;
    int randNum()
    {
       return rand() % (2*max_distance) -max_distance;
    }

    CheckpointArray(const int checkpoints_count, int maxdistance):entites::EntityArray(checkpoints_count){
        srand(time(0));
        max_distance = maxdistance;
        for (int i = 0; i < n; ++i) {
            entites::EntityArray::ChangePosition(i,randNum(), randNum());
        }
    }

    void ResetCheckpoint(int i){
        entites::EntityArray::ChangePosition(i,randNum(),randNum());
    }

    void Reset(){
        for (int j = 0; j < entites::EntityArray::n; ++j) {
            ResetCheckpoint(j);
        }
    }
};

#endif //SPACE_ENVIRO_CHECKPOINT_H
