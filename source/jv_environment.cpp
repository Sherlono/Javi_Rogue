#include "jv_environment.h"

namespace jv{

Block::Block(int x, int y, bn::camera_ptr& cam, unsigned char option, int z_order):
_sprite(this->sprite_factory(x, y, option)),
_para(this->para_factory(x, y, option))
{
    _sprite.set_camera(cam);
    _sprite.set_bg_priority(3);
    _sprite.set_z_order(z_order);
    this->set_xy(x, y);
}
void Block::set_camera(bn::camera_ptr& new_cam){
    _sprite.set_camera(new_cam);
}
void Block::set_block(int x, int y, bn::camera_ptr& cam, unsigned char option, int z_order){
    _sprite = sprite_factory(x, y, option);
    _sprite.set_camera(cam);
    _sprite.set_bg_priority(3);
    _sprite.set_z_order(z_order);
    _para = para_factory(x, y, option);
    this->set_xy(x, y);
}

}