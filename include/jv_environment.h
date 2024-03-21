#ifndef JV_ENVIRONMENT_H
#define JV_ENVIRONMENT_H

#include "bn_rect.h"
#include "bn_fixed.h"
#include "bn_vector.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_items_tileset.h"

#include "jv_math.h"

namespace jv{
class Block{
public:
    virtual ~Block(){}
//private:

};

class Wall2 : public Block{
public:
    ~Wall2(){}
    Wall2(int x, int y, bn::camera_ptr& cam, bn::vector<jv::para, 40>& para_vector, unsigned char option)
    {
        unsigned char sprt_index[2];
        switch(option){
            case 0:
                sprt_index[0] = 5;
                sprt_index[1] = 4;
                for(unsigned char i = 0; i < 2; i++){
                    bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 + 8, y * 32 + 8 - 16*(i%2), sprt_index[i]);
                    sprite.set_camera(cam);
                    sprite.set_bg_priority(1);
                    _sprites.push_back(sprite);
                }
                _para = jv::para(x * 32 + 12, y * 32 + 4, 16, 34);
                break;
            case 1:
                sprt_index[0] = 5;
                sprt_index[1] = 4;
                for(unsigned char i = 0; i < 2; i++){
                    bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 - 8, y * 32 + 8 - 16*(i%2), sprt_index[i]);
                    sprite.set_horizontal_flip(true);
                    sprite.set_camera(cam);
                    sprite.set_bg_priority(1);
                    _sprites.push_back(sprite);
                }
                _para = jv::para(x * 32 - 12, y * 32 + 4, 16, 34);
                break;
            case 2:
                {
                bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 + 8, y * 32 + 8, 4);
                sprite.set_camera(cam);
                sprite.set_bg_priority(1);
                _sprites.push_back(sprite);
                _para = jv::para(x * 32 + 12, y * 32 + 12, 16, 16);
                break;
                }
            case 3:
                {
                bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 - 8, y * 32 + 8, 4);
                sprite.set_horizontal_flip(true);
                sprite.set_camera(cam);
                sprite.set_bg_priority(1);
                _sprites.push_back(sprite);
                _para = jv::para(x * 32 - 12, y * 32 + 12, 16, 16);
                break;
                }
            case 4:
                for(unsigned char i = 0; i < 2; i++){
                    bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 + 8, y * 32 + 8 - 16*(i%2), 5);
                    sprite.set_camera(cam);
                    sprite.set_bg_priority(1);
                    _sprites.push_back(sprite);
                }
                _para = jv::para(x * 32 + 12, y * 32 + 4, 16, 34);
                break;
            case 5:
                for(unsigned char i = 0; i < 2; i++){
                    bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 - 8, y * 32 + 8 - 16*(i%2), 5);
                    sprite.set_horizontal_flip(true);
                    sprite.set_camera(cam);
                    sprite.set_bg_priority(1);
                    _sprites.push_back(sprite);
                }
                _para = jv::para(x * 32 - 12, y * 32 + 4, 16, 34);
                break;
            case 6:
                for(unsigned char i = 0; i < 2; i++){
                    bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 + 8, y * 32 + 8 - 16, 35);
                    sprite.set_camera(cam);
                    sprite.set_bg_priority(1);
                    _sprites.push_back(sprite);
                }
                _para = jv::para(x * 32 + 12, y * 32 - 4, 16, 16);
                break;
            case 7:
                for(unsigned char i = 0; i < 2; i++){
                    bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 - 8, y * 32 + 8 - 16, 35);
                    sprite.set_horizontal_flip(true);
                    sprite.set_camera(cam);
                    sprite.set_bg_priority(1);
                    _sprites.push_back(sprite);
                }
                _para = jv::para(x * 32 - 12, y * 32 - 4, 16, 16);
                break;
            case 8:
                for(unsigned char i = 0; i < 2; i++){
                    bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 + 8, y * 32 + 8 - 16*(i%2), 35);
                    if(i==1){
                        sprite.set_camera(cam);
                        sprite.set_bg_priority(1);
                        _sprites.push_back(sprite);
                    }
                }
                _para = jv::para(x * 32 + 12, y * 32 - 8, 16, 16);
                break;
            case 9:
                for(unsigned char i = 0; i < 2; i++){
                    bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 - 8, y * 32 + 8 - 16*(i%2), 35);
                    if(i == 1){
                        sprite.set_horizontal_flip(true);
                        sprite.set_camera(cam);
                        sprite.set_bg_priority(1);
                        _sprites.push_back(sprite);
                    }
                }
                _para = jv::para(x * 32 - 12, y * 32 - 8, 16, 16);
                break;
            case 10:
                sprt_index[0] = 6;
                sprt_index[1] = 5;
                for(unsigned char i = 0; i < 2; i++){
                    bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 + 8, y * 32 + 8 - 16*(i%2), sprt_index[i]);
                    sprite.set_camera(cam);
                    sprite.set_bg_priority(1);
                    _sprites.push_back(sprite);
                }
                _para = jv::para(x * 32 + 12, y * 32 + 4, 16, 34);
                break;
            case 11:
                sprt_index[0] = 6;
                sprt_index[1] = 5;
                for(unsigned char i = 0; i < 2; i++){
                    bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 - 8, y * 32 + 8 - 16*(i%2), sprt_index[i]);
                    sprite.set_horizontal_flip(true);
                    sprite.set_camera(cam);
                    sprite.set_bg_priority(1);
                    _sprites.push_back(sprite);
                }
                _para = jv::para(x * 32 - 12, y * 32 + 4, 16, 34);
                break;
            case 12:
                for(unsigned char i = 0; i < 2; i++){
                    bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 + 8, y * 32 + 8 - 16*(i%2), 7);
                    sprite.set_camera(cam);
                    sprite.set_bg_priority(1);
                    _sprites.push_back(sprite);
                }
                _para = jv::para(x * 32 + 12, y * 32 + 4, 16, 34);
                break;
            case 13:
                for(unsigned char i = 0; i < 2; i++){
                    bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 - 8, y * 32 + 8 - 16*(i%2), 7);
                    sprite.set_horizontal_flip(true);
                    sprite.set_camera(cam);
                    sprite.set_bg_priority(1);
                    _sprites.push_back(sprite);
                }
                _para = jv::para(x * 32 - 12, y * 32 + 4, 16, 34);
                break;
            case 14:
                for(unsigned char i = 0; i < 2; i++){
                    bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 - 8 + 16*(i%2), y * 32 - 24, 43);
                    sprite.set_horizontal_flip(true);
                    sprite.set_camera(cam);
                    sprite.set_bg_priority(1);
                    _sprites.push_back(sprite);
                }
                _para = jv::para(x * 32, y * 32 - 24, 34, 16);
                break;
            default:
                break;
        }
        if(option > 1){
            para_vector.push_back(_para);
        }
    }
    
private:
    bn::vector<bn::sprite_ptr, 2> _sprites;
    jv::para _para;
};

class Wall4 : public Block{
public:
    ~Wall4(){}
    Wall4(int x, int y, bn::camera_ptr& cam, bn::vector<jv::para, 40>& para_vector, unsigned char option)
    {
        unsigned char sprt_index[4];
        switch(option){
            case 0:
                sprt_index[0] = 39;
                sprt_index[1] = 40;
                sprt_index[2] = 40;
                sprt_index[3] = 39;
                for(unsigned char i = 0; i < 4; i++){
                    bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 - 8 + 16*(i%2), y * 32 - 24 - 16*(i>=2), sprt_index[i]);
                    if(i>=2){
                        sprite.set_rotation_angle(180);
                        sprite.set_x(sprite.x()-1);
                    }
                    sprite.set_camera(cam);
                    sprite.set_bg_priority(1);
                    _sprites.push_back(sprite);
                }
                _para = jv::para(x * 32, y * 32 - 28, 34, 16, 8);
                break;
            case 1:
                sprt_index[0] = 40;
                sprt_index[1] = 39;
                sprt_index[2] = 39;
                sprt_index[3] = 40;
                for(unsigned char i = 0; i < 4; i++){
                    bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 - 8 + 16*(i%2), y * 32 - 24 - 16*(i>=2), sprt_index[i]);
                    if(i>=2){
                        sprite.set_rotation_angle(180);
                    }
                    sprite.set_horizontal_flip(true);
                    sprite.set_camera(cam);
                    sprite.set_bg_priority(1);
                    _sprites.push_back(sprite);
                }
                _para = jv::para(x * 32, y * 32 - 28, 34, 16, -7);
                break;
            case 2:
                sprt_index[0] = 39;
                sprt_index[1] = 40;
                sprt_index[2] = 40;
                sprt_index[3] = 39;
                for(unsigned char i = 0; i < 4; i++){
                    bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 - 8 + 16*(i%2), y * 32 - 16*(i>=2), sprt_index[i]);
                    if(i>=2){
                        sprite.set_rotation_angle(180);
                        sprite.set_x(sprite.x()-1);
                    }
                    sprite.set_camera(cam);
                    sprite.set_bg_priority(1);
                    _sprites.push_back(sprite);
                }
                _para = jv::para(x * 32, y * 32 - 44, 34, 16, 8);
                break;
            case 3:
                sprt_index[0] = 40;
                sprt_index[1] = 39;
                sprt_index[2] = 39;
                sprt_index[3] = 40;
                for(unsigned char i = 0; i < 4; i++){
                    bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 - 8 + 16*(i%2), y * 32 - 16*(i>=2), sprt_index[i]);
                    if(i>=2){
                        sprite.set_rotation_angle(180);
                    }
                    sprite.set_horizontal_flip(true);
                    sprite.set_camera(cam);
                    sprite.set_bg_priority(1);
                    _sprites.push_back(sprite);
                }
                _para = jv::para(x * 32, y * 32 - 12, 34, 16, -7);
                break;
            default:
                break;
        }
        para_vector.push_back(_para);
    }
    
private:
    bn::vector<bn::sprite_ptr, 4> _sprites;
    jv::para _para;
};

class Wall6 : public Block{
public:
    ~Wall6(){}
    Wall6(int x, int y, bn::camera_ptr& cam, bn::vector<jv::para, 40>& para_vector)
    {
        unsigned char sprt_index[6] = {0, 1, 0, 1, 2, 3};
        for(unsigned char i = 0; i < 6; i++){
            bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 - 8 + 16*(i%2), y * 32 + 8 - 16*(i%3), sprt_index[i]);
            sprite.set_camera(cam);
            sprite.set_bg_priority(1);
            _sprites.push_back(sprite);
        }
        _para = jv::para(x * 32, y * 32 + 12, 34, 16);
        para_vector.push_back(_para);
    }
    
private:
    bn::vector<bn::sprite_ptr, 6> _sprites;
    jv::para _para;
};

class Wall8 : public Block{
public:
    ~Wall8(){}
    Wall8(int x, int y, bn::camera_ptr& cam, bn::vector<jv::para, 40>& para_vector, unsigned char option)
    {
        unsigned char sprt_index[8];
        switch(option){
            case 0:
                sprt_index[0] = 14;
                sprt_index[1] = 15;
                sprt_index[2] = 13;
                sprt_index[3] = 12;
                sprt_index[4] = 11;
                sprt_index[5] = 12;
                sprt_index[6] = 8;
                sprt_index[7] = 9;
                for(unsigned char i = 0; i < 8; i++){
                    bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 - 8 + 16*(i%2), y * 32 + 24 - 16*((i/2)%4), sprt_index[i]);
                    sprite.set_camera(cam);
                    sprite.set_bg_priority(1);
                    _sprites.push_back(sprite);
                }
                _para = jv::para(x * 32, y * 32 + 20, 34, 16, -7);
                break;
            case 1:
                sprt_index[0] = 15;
                sprt_index[1] = 14;
                sprt_index[2] = 12;
                sprt_index[3] = 13;
                sprt_index[4] = 12;
                sprt_index[5] = 11;
                sprt_index[6] = 9;
                sprt_index[7] = 8;
                for(unsigned char i = 0; i < 8; i++){
                    bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 - 8 + 16*(i%2), y * 32 + 24 - 16*((i/2)%4), sprt_index[i]);
                    sprite.set_horizontal_flip(true);
                    sprite.set_camera(cam);
                    sprite.set_bg_priority(1);
                    _sprites.push_back(sprite);
                }
                _para = jv::para(x * 32, y * 32 + 20, 34, 16, 8);
                break;
            case 2:
                sprt_index[0] = 14;
                sprt_index[1] = 15;
                sprt_index[2] = 13;
                sprt_index[3] = 12;
                sprt_index[4] = 11;
                sprt_index[5] = 12;
                sprt_index[6] = 8;
                sprt_index[7] = 10;
                for(unsigned char i = 0; i < 8; i++){
                    bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 - 8 + 16*(i%2), y * 32 + 24 - 16*((i/2)%4), sprt_index[i]);
                    sprite.set_camera(cam);
                    sprite.set_bg_priority(1);
                    _sprites.push_back(sprite);
                }
                _para = jv::para(x * 32, y * 32 + 20, 34, 16, -7);
                break;
            case 3:
                sprt_index[0] = 15;
                sprt_index[1] = 14;
                sprt_index[2] = 12;
                sprt_index[3] = 13;
                sprt_index[4] = 12;
                sprt_index[5] = 11;
                sprt_index[6] = 10;
                sprt_index[7] = 8;
                for(unsigned char i = 0; i < 8; i++){
                    bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 - 8 + 16*(i%2), y * 32  + 24 - 16*((i/2)%4), sprt_index[i]);
                    sprite.set_horizontal_flip(true);
                    sprite.set_camera(cam);
                    sprite.set_bg_priority(1);
                    _sprites.push_back(sprite);
                }
                _para = jv::para(x * 32, y * 32 + 20, 34, 16, 8);
                break;
            default:
                break;
        }
        para_vector.push_back(_para);
    }
    
private:
    bn::vector<bn::sprite_ptr, 8> _sprites;
    jv::para _para;
};

class Floor : public Block{
public:
    ~Floor(){};
    Floor(int x, int y, bn::camera_ptr& cam, unsigned char option)
    {
        unsigned char sprt_index[4];
        switch(option){
            case 0:
                for(unsigned char i = 0; i < 4; i++){
                    bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 - 8 + 16*(i%2), y * 32 + 8 - 16*(i>=2), 20);
                    sprite.set_camera(cam);
                    sprite.set_bg_priority(1);
                    _sprites.push_back(sprite);
                }
                break;
            case 1:
                sprt_index[0] = 22;
                sprt_index[1] = 20;
                sprt_index[2] = 16;
                sprt_index[3] = 17;
                for(unsigned char i = 0; i < 4; i++){
                    bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 - 8 + 16*(i%2), y * 32 + 8 - 16*(i>=2), sprt_index[i]);
                    sprite.set_camera(cam);
                    sprite.set_bg_priority(1);
                    _sprites.push_back(sprite);
                }
                break;
            case 2:
                sprt_index[0] = 20;
                sprt_index[1] = 22;
                sprt_index[2] = 17;
                sprt_index[3] = 16;
                for(unsigned char i = 0; i < 4; i++){
                    bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 - 8 + 16*(i%2), y * 32 + 8 - 16*(i>=2), sprt_index[i]);
                    sprite.set_horizontal_flip(true);
                    sprite.set_camera(cam);
                    sprite.set_bg_priority(1);
                    _sprites.push_back(sprite);
                }
                break;
            case 3:
                sprt_index[0] = 19;
                sprt_index[1] = 20;
                sprt_index[2] = 16;
                sprt_index[3] = 17;
                for(unsigned char i = 0; i < 4; i++){
                    bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 - 8 + 16*(i%2), y * 32 + 8 - 16*(i>=2), sprt_index[i]);
                    sprite.set_camera(cam);
                    sprite.set_bg_priority(1);
                    _sprites.push_back(sprite);
                }
                break;
            case 4:
                sprt_index[0] = 20;
                sprt_index[1] = 19;
                sprt_index[2] = 17;
                sprt_index[3] = 16;
                for(unsigned char i = 0; i < 4; i++){
                    bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 - 8 + 16*(i%2), y * 32 + 8 - 16*(i>=2), sprt_index[i]);
                    sprite.set_horizontal_flip(true);
                    sprite.set_camera(cam);
                    sprite.set_bg_priority(1);
                    _sprites.push_back(sprite);
                }
                break;
            case 5:
                sprt_index[0] = 23;
                sprt_index[1] = 20;
                sprt_index[2] = 23;
                sprt_index[3] = 20;
                for(unsigned char i = 0; i < 4; i++){
                    bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 - 8 + 16*(i%2), y * 32 + 8 - 16*(i>=2), sprt_index[i]);
                    sprite.set_camera(cam);
                    sprite.set_bg_priority(1);
                    _sprites.push_back(sprite);
                }
                break;
            case 6:
                sprt_index[0] = 20;
                sprt_index[1] = 23;
                sprt_index[2] = 20;
                sprt_index[3] = 23;
                for(unsigned char i = 0; i < 4; i++){
                    bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 - 8 + 16*(i%2), y * 32 + 8 - 16*(i>=2), sprt_index[i]);
                    sprite.set_horizontal_flip(true);
                    sprite.set_camera(cam);
                    sprite.set_bg_priority(1);
                    _sprites.push_back(sprite);
                }
                break;
            case 7:
                sprt_index[0] = 20;
                sprt_index[1] = 20;
                sprt_index[2] = 21;
                sprt_index[3] = 21;
                for(unsigned char i = 0; i < 4; i++){
                    bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 - 8 + 16*(i%2), y * 32 + 8 - 16*(i>=2), sprt_index[i]);
                    if(i == 3 && sprt_index[3] == 18){
                        sprite.set_horizontal_flip(true);
                    }
                    sprite.set_camera(cam);
                    sprite.set_bg_priority(1);
                    _sprites.push_back(sprite);
                }
                break;
            case 8:
                sprt_index[0] = 20;
                sprt_index[1] = 20;
                sprt_index[2] = 21;
                sprt_index[3] = 18;
                for(unsigned char i = 0; i < 4; i++){
                    bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 - 8 + 16*(i%2), y * 32 + 8 - 16*(i>=2), sprt_index[i]);
                    if(i == 3 && sprt_index[3] == 18){
                        sprite.set_horizontal_flip(true);
                    }
                    sprite.set_camera(cam);
                    sprite.set_bg_priority(1);
                    _sprites.push_back(sprite);
                }
                break;
            case 9:
                sprt_index[0] = 20;
                sprt_index[1] = 20;
                sprt_index[2] = 18;
                sprt_index[3] = 21;
                for(unsigned char i = 0; i < 4; i++){
                    bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 - 8 + 16*(i%2), y * 32 + 8 - 16*(i>=2), sprt_index[i]);
                    if(i == 3 && sprt_index[3] == 18){
                        sprite.set_horizontal_flip(true);
                    }
                    sprite.set_camera(cam);
                    sprite.set_bg_priority(1);
                    _sprites.push_back(sprite);
                }
                break;
            case 10:
                sprt_index[0] = 20;
                sprt_index[1] = 20;
                sprt_index[2] = 18;
                sprt_index[3] = 18;
                for(unsigned char i = 0; i < 4; i++){
                    bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 - 8 + 16*(i%2), y * 32 + 8 - 16*(i>=2), sprt_index[i]);
                    if(i == 3 && sprt_index[3] == 18){
                        sprite.set_horizontal_flip(true);
                    }
                    sprite.set_camera(cam);
                    sprite.set_bg_priority(1);
                    _sprites.push_back(sprite);
                }
                break;
            case 11:
                sprt_index[0] = 24;
                sprt_index[1] = 20;
                sprt_index[2] = 23;
                sprt_index[3] = 20;
                for(unsigned char i = 0; i < 4; i++){
                    bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 - 8 + 16*(i%2), y * 32 + 8 - 16*(i>=2), sprt_index[i]);
                    sprite.set_camera(cam);
                    sprite.set_bg_priority(1);
                    _sprites.push_back(sprite);
                }
                break;
            case 12:
                sprt_index[0] = 20;
                sprt_index[1] = 24;
                sprt_index[2] = 20;
                sprt_index[3] = 23;
                for(unsigned char i = 0; i < 4; i++){
                    bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 - 8 + 16*(i%2), y * 32 + 8 - 16*(i>=2), sprt_index[i]);
                    if(i%2 == 1){
                        sprite.set_horizontal_flip(true);
                    }
                    sprite.set_camera(cam);
                    sprite.set_bg_priority(1);
                    _sprites.push_back(sprite);
                }
                break;
            case 13:
                sprt_index[0] = 42;
                sprt_index[1] = 42;
                sprt_index[2] = 20;
                sprt_index[3] = 20;
                for(unsigned char i = 0; i < 4; i++){
                    bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 - 8 + 16*(i%2), y * 32 + 8 - 16*(i>=2), sprt_index[i]);
                    sprite.set_camera(cam);
                    sprite.set_bg_priority(1);
                    _sprites.push_back(sprite);
                }
                break;
            case 14:
                sprt_index[0] = 41;
                sprt_index[1] = 42;
                sprt_index[2] = 20;
                sprt_index[3] = 20;
                for(unsigned char i = 0; i < 4; i++){
                    bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 - 8 + 16*(i%2), y * 32 + 8 - 16*(i>=2), sprt_index[i]);
                    sprite.set_camera(cam);
                    sprite.set_bg_priority(1);
                    _sprites.push_back(sprite);
                }
                break;
            case 15:
                sprt_index[0] = 42;
                sprt_index[1] = 41;
                sprt_index[2] = 20;
                sprt_index[3] = 20;
                for(unsigned char i = 0; i < 4; i++){
                    bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 - 8 + 16*(i%2), y * 32 + 8 - 16*(i>=2), sprt_index[i]);
                    if(i == 1){
                        sprite.set_horizontal_flip(true);
                    }
                    sprite.set_camera(cam);
                    sprite.set_bg_priority(1);
                    _sprites.push_back(sprite);
                }
                break;
            case 16:
                sprt_index[2] = 36;
                sprt_index[3] = 37;
                for(unsigned char i = 2; i < 4; i++){
                    bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 - 8 + 16*(i%2), y * 32 + 8 - 16*(i>=2), sprt_index[i]);
                    sprite.set_camera(cam);
                    sprite.set_bg_priority(1);
                    _sprites.push_back(sprite);
                }
                _sprites.shrink(2);
                break;
            case 17:
                sprt_index[2] = 37;
                sprt_index[3] = 36;
                for(unsigned char i = 2; i < 4; i++){
                    bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 - 8 + 16*(i%2), y * 32 + 8 - 16*(i>=2), sprt_index[i]);
                    sprite.set_horizontal_flip(true);
                    sprite.set_camera(cam);
                    sprite.set_bg_priority(1);
                    _sprites.push_back(sprite);
                }
                _sprites.shrink(2);
                break;
            case 18:
                sprt_index[0] = 36;
                sprt_index[1] = 37;
                sprt_index[2] = 38;
                sprt_index[3] = 20;
                for(unsigned char i = 0; i < 4; i++){
                    bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 - 8 + 16*(i%2), y * 32 + 8 - 16*(i>=2), sprt_index[i]);
                    sprite.set_camera(cam);
                    sprite.set_bg_priority(1);
                    _sprites.push_back(sprite);
                }
                break;
            case 19:
                sprt_index[0] = 37;
                sprt_index[1] = 36;
                sprt_index[2] = 20;
                sprt_index[3] = 38;
                for(unsigned char i = 0; i < 4; i++){
                    bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 - 8 + 16*(i%2), y * 32 + 8 - 16*(i>=2), sprt_index[i]);
                    sprite.set_horizontal_flip(true);
                    sprite.set_camera(cam);
                    sprite.set_bg_priority(1);
                    _sprites.push_back(sprite);
                }
                break;
            case 20:
                for(unsigned char i = 2; i < 4; i++){
                    bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 - 8 + 16*(i%2), y * 32 + 8 - 16*(i>=2), 42);
                    sprite.set_camera(cam);
                    sprite.set_bg_priority(1);
                    _sprites.push_back(sprite);
                }
                _sprites.shrink(2);
                break;
            case 21:
                sprt_index[2] = 41;
                sprt_index[3] = 42;
                for(unsigned char i = 2; i < 4; i++){
                    bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 - 8 + 16*(i%2), y * 32 + 8 - 16*(i>=2), sprt_index[i]);
                    sprite.set_camera(cam);
                    sprite.set_bg_priority(1);
                    _sprites.push_back(sprite);
                }
                break;
            case 22:
                sprt_index[2] = 42;
                sprt_index[3] = 41;
                for(unsigned char i = 2; i < 4; i++){
                    bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 - 8 + 16*(i%2), y * 32 + 8 - 16*(i>=2), sprt_index[i]);
                    sprite.set_horizontal_flip(true);
                    sprite.set_camera(cam);
                    sprite.set_bg_priority(1);
                    _sprites.push_back(sprite);
                }
                _sprites.shrink(2);
                break;
            case 23:
                for(unsigned char i = 2; i < 4; i++){
                    bn::sprite_ptr sprite = bn::sprite_items::tileset.create_sprite(x * 32 - 8 + 16*(i%2), y * 32 + 8 - 16*(i>=2), 42);
                    sprite.set_camera(cam);
                    sprite.set_bg_priority(1);
                    _sprites.push_back(sprite);
                }
                _sprites.shrink(2);
                break;
            default:
                break;
        }
    }

private:
    bn::vector<bn::sprite_ptr, 4> _sprites;
};

/*class LevelGenerator{
public:
    ~LevelGenerator(){};
    LevelGenerator(unsigned char width, unsigned char height, bn::camera_ptr& cam, bn::vector<jv::para, 40>& para_vector, bn::vector<jv::Block*, 36>& block_holder, unsigned char* block_array){
        unsigned char i = 0;
        for(size_t y = 0; y < height; y++){
            for(size_t x = 0; x < width; x++){
                jv::Block* newblock;
                if(block_array[i] <= 14){
                    newblock = new Wall2(x, y, cam, para_vector, block_array[i]);
                }else if(block_array[i] <= 18){
                    newblock = new Wall4(x, y, cam, para_vector, block_array[i] - 15);
                }else if(block_array[i] <= 19){
                    newblock = new Wall6(x, y, cam, para_vector);
                }else if(block_array[i] <= 23){
                    newblock = new Wall8(x, y, cam, para_vector, block_array[i] - 20);
                }else{
                    newblock = new Floor(x, y, cam, block_array[i] - 24);
                }
                block_holder.push_back(newblock);
                i++;
            }
        }
    }
};*/

}

#endif