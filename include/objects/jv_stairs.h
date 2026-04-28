#ifndef JV_STAIRS
#define JV_STAIRS

#include "bn_rect.h"
#include "bn_tile.h"
#include "bn_regular_bg_tiles_ptr.h"

namespace jv{
class Player;

struct Stairs{
    static constexpr int stairs_tiles_begin = 57;
    Stairs(): isOpen(false), _rect(bn::rect(0, 0, 16, 16)) {}

    // Setters
    void set_position(bn::point p);
    void set_open(bool open);
    bool climb();
    
    bool isOpen;
private:
    bn::rect _rect;
};
}

#endif