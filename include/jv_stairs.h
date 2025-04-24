#ifndef JV_STAIRS
#define JV_STAIRS

#include "bn_rect.h"
#include "bn_sprite_ptr.h"
#include "bn_camera_ptr.h"

#include "bn_sprite_items_stairs.h"


namespace jv{
struct stairs{
    stairs(int x, int y, bn::camera_ptr cam):_sprite(bn::sprite_items::stairs.create_sprite(x, y)), _rect(bn::rect(x, y, 16, 16)), isOpen(false)
    {
        _sprite.set_camera(cam);
        _sprite.set_bg_priority(2);
        _sprite.set_blending_enabled(true);
    }
    // Setters
    void set_position(int x, int y){
        _sprite.set_position(x, y);
        _rect.set_position(x, y);
    }
    void set_open(bool open){
        isOpen = open;
        if(open){
            _sprite.set_tiles(bn::sprite_items::stairs.tiles_item().create_tiles(1));}
        else{
            _sprite.set_tiles(bn::sprite_items::stairs.tiles_item().create_tiles(0));
        }
    }

    inline bool climb(bn::rect playerRect, bool playerState);

    bn::sprite_ptr _sprite;
    bn::rect _rect;
    bool isOpen;
};
}

#endif