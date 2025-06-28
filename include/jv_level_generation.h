#ifndef JV_LEVEL_GENERATION_H
#define JV_LEVEL_GENERATION_H

#include "bn_rect.h"
#include "bn_vector.h"
#include "bn_random.h"
#include "bn_assert.h"

#include "jv_fog.h"
#include "jv_common.h"
#include "jv_interface.h"
#include "jv_map_classes.h"

namespace jv{
    void BlockFactory(const bn::point top_left, const uint8_t option, const bool blockFlip);
    bn::point InsertRoom(const bn::point top_left, const uint8_t option, Fog* fog_ptr = NULL);
    void GenerateLevel(Zone& zone, Fog* fog_ptr = NULL);
}
#endif