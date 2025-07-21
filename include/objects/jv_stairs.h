#ifndef JV_STAIRS
#define JV_STAIRS

#include "bn_rect.h"

namespace jv{
class Player;

struct stairs{
    stairs(): _rect(bn::rect(0, 0, 16, 16)), isOpen(false) {}

    // Setters
    void set_position(bn::point p);
    void set_open(bool open);
    bool climb();
    
    bn::rect _rect;
    bool isOpen;
};
}

#endif