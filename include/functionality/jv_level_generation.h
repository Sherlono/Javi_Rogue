#ifndef JV_LEVEL_GENERATION_H
#define JV_LEVEL_GENERATION_H

#include "bn_rect.h"
#include "bn_vector.h"
#include "bn_random.h"
#include "bn_assert.h"

#include "jv_fog.h"
#include "jv_stairs.h"

namespace jv::Level{
    void BlockFactory(const bn::point top_left, const uint8_t option, const bool blockFlip);
    bn::point InsertRoom(const bn::point top_left, const uint8_t option, iFog* fog_ptr = nullptr);
    void Generate(int const z_x, int const z_y, iFog* fog_ptr = nullptr);
    void Populate(jv::stairs& stairs);
}
#endif