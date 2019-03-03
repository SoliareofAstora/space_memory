//
// Created by overlord on 27/01/19.
//

#include "entity_manager.h"

namespace entities{

    float RandomFloat(float min, float max) {
        float random = ((float) rand()) / (float) RAND_MAX;
        float range = max - min;
        return (random*range) + min;
    }

}
