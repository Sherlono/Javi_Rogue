#ifndef JV_STAIRS
#define JV_STAIRS

#include "bn_rect.h"
#include "bn_keypad.h"
#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_tiles_ptr.h"
#include "bn_camera_ptr.h"


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