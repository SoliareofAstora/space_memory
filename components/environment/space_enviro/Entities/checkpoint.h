//
// Created by SoliareofAstora on 28/05/19.
//

#ifndef SPACE_ENVIRO_CHECKPOINT_H
#define SPACE_ENVIRO_CHECKPOINT_H

#include "Data/base_entity.h"
#include "../Utils/randoms.hpp"

struct CheckpointArray:entity_data::BaseEntity{

    int max_distance;
    int randNum()
    {
       return RNG::RandomInt(max_distance);
    }

    CheckpointArray(const int checkpoints_count, int maxdistance):entity_data::BaseEntity(checkpoints_count){
        max_distance = maxdistance;
        for (int i = 0; i < n; ++i) {
          entity_data::BaseEntity::ChangePosition(i, randNum(), randNum());
        }
    }

    void ResetCheckpoint(int i){
      entity_data::BaseEntity::ChangePosition(i, randNum(), randNum());
    }

    void Reset(){
        for (int j = 0; j < entity_data::BaseEntity::n; ++j) {
            ResetCheckpoint(j);
        }
    }
};

#endif //SPACE_ENVIRO_CHECKPOINT_H
