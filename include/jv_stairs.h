#ifndef JV_STAIRS
#define JV_STAIRS

#include "bn_rect.h"
#include "bn_keypad.h"
#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_camera_ptr.h"

#include "jv_actors.h"

#include "bn_sprite_items_stairs.h"


namespace jv{
struct stairs{
    stairs(int x, int y, bn::camera_ptr cam):_sprite(bn::sprite_items::stairs.create_sprite(x, y)), _rect(bn::rect(x, y, 16, 16)), isOpen(false)
    {
        _sprite.set_camera(cam);
        _sprite.set_bg_priority(2);
    }
    // Setters
    void set_position(int x, int y){
        _sprite.set_position(x, y);
        _rect.set_position(x, y);
    }
    void set_position(bn::point p){
        _sprite.set_position(p);
        _rect.set_position(p);
    }
    void set_visible(bool visible){
        _sprite.set_visible(visible);
    }
    void set_open(bool open){
        isOpen = open;
        if(open){
            _sprite.set_tiles(bn::sprite_items::stairs.tiles_item().create_tiles(1));}
        else{
            _sprite.set_tiles(bn::sprite_items::stairs.tiles_item().create_tiles(0));
        }
    }

    bool climb();

    bn::sprite_ptr _sprite;
    bn::rect _rect;
    bool isOpen;
};
}

#endif