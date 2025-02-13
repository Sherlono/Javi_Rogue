#include "jv_actors.h"

namespace jv{
// Constructor
Player::Player(int x, int y, bn::random* ptr):
_sprite(bn::sprite_items::character.create_sprite(0 , 0 - 8)),
_animation(bn::create_sprite_animate_action_forever(this->_sprite, 4, bn::sprite_items::character.tiles_item(), 0, 1, 0, 2)),
_speed(bn::fixed(1.0)),
_prev_dir(2),
_dir(2),
_hp(20),
_random_ptr(ptr)
{
    _sprite.set_bg_priority(1);
    _x = x;
    _y = y;
}
// Setters
void Player::set_x(bn::fixed x, bool sprite_follow){
    if(sprite_follow){
        _sprite.set_position(x, _y - 8);
    }
    _x = x;
}
void Player::set_y(bn::fixed y, bool sprite_follow){
    if(sprite_follow){
        _sprite.set_position(_x, y - 8);
    }
    _y = y;
}
void Player::set_position(bn::fixed x, bn::fixed y, bool sprite_follow){
    if(sprite_follow){
        _sprite.set_position(x, y - 8);
    }
    _x = x;
    _y = y;
}
void Player::set_visible(bool visible){
    _sprite.set_visible(visible);
}

void Player::update(bn::camera_ptr& cam){
    this->move(cam);
}

// Constructor
NPC::NPC(int x, int y, bn::camera_ptr& cam):
_sprite(bn::sprite_items::cow.create_sprite(x , y - 8))
{
    _sprite.set_bg_priority(1);
    _sprite.set_camera(cam);
    _x = x;
    _y = y;
}
// Setters
void NPC::set_x(bn::fixed x, bool sprite_follow){
    if(sprite_follow){
        _sprite.set_position(x, _y - 16);
    }
    _x = x;
}
void NPC::set_y(bn::fixed y, bool sprite_follow){
    if(sprite_follow){
        _sprite.set_position(_x, y - 8);
    }
    _y = y;
}
void NPC::set_position(bn::fixed x, bn::fixed y, bool sprite_follow){
    if(sprite_follow){
        _sprite.set_position(x, y - 8);
    }
    _x = x;
    _y = y;
}
void NPC::set_visible(bool visible){
    _sprite.set_visible(visible);
}

void NPC::update(jv::Player& player){
    priority_update(player.y());
    
    // Speak
    /*if(bn::keypad::a_pressed()){
        jv::Dialog::init("Bitch I'm a cow. Bitch I'm a cow.", "I'm not a cat. I don't go meow.", "...Unlike you.");
    }*/
}

// Constructor
Enemy::Enemy(int x, int y, bn::camera_ptr& cam, bn::random* ptr):
_sprite(bn::sprite_items::enemy.create_sprite(x , y - 8)),
_animation(bn::create_sprite_animate_action_forever(this->_sprite, 4, bn::sprite_items::enemy.tiles_item(), 0, 1, 0, 2)),
_speed(bn::fixed(0.4)),
_prev_dir(2),
_dir(2),
_hp(20),
_idle_time(0),
_random_ptr(ptr)
{
    _sprite.set_bg_priority(1);
    _sprite.set_camera(cam);
    _x = x;
    _y = y;
}
// Setters
void Enemy::set_x(bn::fixed x, bool sprite_follow){
    if(sprite_follow){
        _sprite.set_position(x, _y - 8);
    }
    _x = x;
}
void Enemy::set_y(bn::fixed y, bool sprite_follow){
    if(sprite_follow){
        _sprite.set_position(_x, y - 8);
    }
    _y = y;
}
void Enemy::set_position(bn::fixed x, bn::fixed y, bool sprite_follow){
    if(sprite_follow){
        _sprite.set_position(x, y - 8);
    }
    _x = x;
    _y = y;
}
void Enemy::set_visible(bool visible){
    _sprite.set_visible(visible);
}

void Enemy::update(jv::Player& player){
    // Change sprite priority to draw behind or above player
    priority_update(player.y());
    
    this->move();
}
}