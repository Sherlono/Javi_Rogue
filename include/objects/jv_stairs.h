#ifndef JV_STAIRS
#define JV_STAIRS

#include "bn_rect.h"
#include "bn_tile.h"
#include "bn_regular_bg_tiles_ptr.h"

namespace jv{
class Player;

struct stairs{
    static constexpr int stairs_tiles_begin = 41;
    stairs(): isOpen(false), _rect(bn::rect(0, 0, 16, 16)) {}

    // Setters
    void set_position(bn::point p);
    void set_open(const bn::regular_bg_tiles_ptr& t_ptr, bool open);
    bool climb();
    
    bool isOpen;
private:
    bn::rect _rect;
};
}

#endif