#ifndef JV_ACTORS_H
#define JV_ACTORS_H

#include "bn_core.h"
#include "bn_sprite_animate_actions.h"
#include "bn_keypad.h"
#include "bn_camera_actions.h"
#include "bn_math.h"

#include "bn_sprite_items_character.h"
#include "bn_sprite_items_cow.h"
#include "jv_constants.h"
#include "jv_dialog.h"
#include "jv_math.h"

namespace jv{
class Actor{
public:
    virtual ~Actor(){};
    [[nodiscard]] bn::fixed x() const{
        return _x;
    }
    [[nodiscard]] bn::fixed y() const{
        return _y;
    }
    [[nodiscard]] bn::fixed_point xy() const{
        return bn::fixed_point(_x, _y);
    }
    bn::fixed _x, _y;
};


class Player: public Actor{
public:
    ~Player(){};
    Player(unsigned int x, unsigned int y);
    void set_x(bn::fixed x, bool sprite_follow);
    void set_y(bn::fixed y, bool sprite_follow);
    void set_position(bn::fixed x, bn::fixed y, bool sprite_follow);

    jv::para get_para(){
        return _para;
    }
    void set_visible(bool visible);
    void walk_update();
    void wait(); 
    void move_player(bn::camera_ptr& cam, bn::vector<jv::para, MAX_BLOCKS>& para_v);
private:
    bn::sprite_ptr _sprite;
    jv::para _para;
    bn::sprite_animate_action<4> _walk;
    bn::fixed _speed;
    unsigned char _prev_dir, _dir;
};

class npc: public Actor{
public:
    ~npc(){}
    npc(int x, int y, bn::camera_ptr cam);
    void set_x(bn::fixed x, bool sprite_follow);
    void set_y(bn::fixed y, bool sprite_follow);
    void set_position(bn::fixed x, bn::fixed y, bool sprite_follow);

    void set_visible(bool visible);
    void update(jv::Player& player);
private:
    bn::sprite_ptr _sprite;
    jv::para _para;
};
}
#endif