#include "jv_actors.h"

namespace jv{
// ************ Player ************
// Constructor
Player::Player(int x, int y, bn::random* random_ref, game_map* m_r, bn::camera_ptr cam):
    Actor(bn::sprite_items::character.create_sprite(0 , 0 - 8), x, y, bn::rect(x, y, 20, 20)),
    _stats(basic_stats(10, 10, 1, 1, bn::fixed(1.0))),
    _animation(bn::create_sprite_animate_action_forever(_sprite, 4, bn::sprite_items::character.tiles_item(), 0, 1, 0, 2)),
    _prev_dir(2),
    _dir(2),
    _map_ref(m_r),
    _randomizer(random_ref)
{
    _sprite.set_bg_priority(1);
    cam.set_position(x, y);
}
// Setters
void Player::set_position(bn::fixed x, bn::fixed y, bool sprite_follow){
    if(sprite_follow){ _sprite.set_position(x, y - 8);}
    _x = x;
    _y = y;
    _rect.set_position(x.integer(), y.integer());
}
void Player::set_position(bn::point point, bool sprite_follow){
    if(sprite_follow){ _sprite.set_position(point.x(), point.y() - 8);}
    _x = point.x();
    _y = point.y();
    _rect.set_position(point.x(), point.y());
}

void Player::update(bn::camera_ptr cam, bool* isSolid){
    move(cam, *isSolid);
}
// ************** NPC **************
// Constructor
NPC::NPC(int x, int y, bn::camera_ptr cam):
    Actor(bn::sprite_items::cow.create_sprite(x, y - 8), x, y, bn::rect(x, y + 8, 20, 20), cam)
{
    _sprite.set_bg_priority(1);
}
// Setters
void NPC::set_position(bn::fixed x, bn::fixed y){
    _sprite.set_position(x, y - 8);
    _x = x;
    _y = y;
    _rect.set_position(x.integer(), y.integer());
}
void NPC::set_position(bn::point point){
    _sprite.set_position(point.x(), point.y() - 8);
    _x = point.x();
    _y = point.y();
    _rect.set_position(point.x(), point.y());
}

void NPC::update(jv::Player* player){
    priority_update(player->y());
    
    speak(player, "Bitch I'm a cow. Bitch I'm a cow.", "I'm not a cat. I don't go meow.", "...Unlike you.");
}
// ************* Enemy *************
// Constructor
Enemy::Enemy(int x, int y, bn::random* random_ref, game_map* m_r, bn::camera_ptr cam):
    Actor(bn::sprite_items::enemy.create_sprite(x, y - 8), x, y, bn::rect(x, y, 20, 20), cam),
    _stats(basic_stats(10, 10, 1, 1, bn::fixed(0.4))),
    _animation(bn::create_sprite_animate_action_forever(_sprite, 4, bn::sprite_items::enemy.tiles_item(), 0, 1, 0, 2)),
    _prev_dir(2),
    _dir(2),
    _idle_time(0),
    _map_ref(m_r),
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
void Enemy::set_position(bn::point point){
    _sprite.set_position(point.x(), point.y() - 8);
    _x = point.x();
    _y = point.y();
    _rect.set_position(point.x(), point.y());
}

void Enemy::update(jv::Player* player){
    // Change sprite priority to draw behind or above player
    priority_update(player->y());
    
    move();
    
    speak(player, "I am the evil cat. I will attack", "you in a future version of", "the game.");
}
}