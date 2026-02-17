#ifndef JV_STAIRS
#define JV_STAIRS

#include "bn_rect.h"
#include "bn_regular_bg_tiles_ptr.h"

namespace jv{
class Player;

struct stairs{
    stairs(const bn::regular_bg_tiles_ptr& t_ptr): isOpen(false), _tiles_ptr(t_ptr), _rect(bn::rect(0, 0, 16, 16)) {}

    // Setters
    void set_position(bn::point p);
    void set_open(bool open);
    bool climb();
    
    bool isOpen;
private:
    bn::regular_bg_tiles_ptr _tiles_ptr;
    bn::rect _rect;
};
}

#endif