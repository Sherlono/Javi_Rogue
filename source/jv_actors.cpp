#include "jv_actors.h"

namespace jv{
// Constructor
Player::Player(int x, int y, bn::random& random_ref, bn::unique_ptr<bg_map>& m_r):
    Actor(bn::sprite_items::character.create_sprite(0 , 0 - 8), x, y, bn::rect(x, y, 20, 20)),
    _animation(bn::create_sprite_animate_action_forever(_sprite, 4, bn::sprite_items::character.tiles_item(), 0, 1, 0, 2)),
    _speed(bn::fixed(1.0)),
    _prev_dir(2),
    _dir(2),
    _hp(20),
    _map_ref(m_r),
    _randomizer(random_ref)
{
    _sprite.set_bg_priority(1);
}
// Setters
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

void Player::update(bn::camera_ptr& cam, bool& isSolid){
    move(cam, isSolid);
}

// Constructor
NPC::NPC(int x, int y):
    Actor(bn::sprite_items::cow.create_sprite(x , y - 8), x, y, bn::rect(x, y + 8, 20, 20))
{
    _sprite.set_bg_priority(1);
}
// Setters
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
    if(bn::keypad::a_pressed() && player.rect().intersects(rect())){
        jv::Dialog::init("Bitch I'm a cow. Bitch I'm a cow.", "I'm not a cat. I don't go meow.", "...Unlike you.");
    }
}

// Constructor
Enemy::Enemy(int x, int y, bn::random& random_ref):
    Actor(bn::sprite_items::enemy.create_sprite(x , y - 8), x, y, bn::rect(x, y, 20, 20)),
    _animation(bn::create_sprite_animate_action_forever(_sprite, 4, bn::sprite_items::enemy.tiles_item(), 0, 1, 0, 2)),
    _speed(bn::fixed(0.4)),
    _prev_dir(2),
    _dir(2),
    _hp(20),
    _idle_time(0),
    _randomizer(random_ref)
{
    _sprite.set_bg_priority(1);
}
// Setters
void Enemy::set_position(bn::fixed x, bn::fixed y){
    _sprite.set_position(x, y - 8);
    _x = x;
    _y = y;
    _rect.set_position(x.integer(), y.integer());
}
void Enemy::set_visible(bool visible){
    _sprite.set_visible(visible);
}

void Enemy::update(jv::Player& player){
    // Change sprite priority to draw behind or above player
    priority_update(player.y());
    
    move();
    
    // Speak
    if(bn::keypad::a_pressed() && player.rect().intersects(rect())){
        jv::Dialog::init("I am the evil cat. I will attack", "you in a future version of", "the game.");
    }
}
}