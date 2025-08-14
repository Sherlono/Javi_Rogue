#ifndef JV_LEVEL_GENERATION_H
#define JV_LEVEL_GENERATION_H

#include "bn_rect.h"
#include "bn_vector.h"
#include "bn_random.h"
#include "bn_assert.h"

namespace jv{
class iFog;
class stairs;

namespace Level{
    [[nodiscard]] bn::point pop_point(bn::point* points, int& size, const int index);
    void random_coords(bn::ivector<bn::point>& points_out);

    void BlockFactory(const bn::point top_left, const uint8_t option, const bool blockFlip);
    bn::point InsertRoom(const bn::point top_left, const uint8_t option, iFog* fog_ptr = nullptr);
    void Generate(int const zone_x, int const zone_y, iFog* fog_ptr = nullptr);
    void Populate(jv::stairs& stairs);
}
}
#endif