#ifndef JV_ENVIRONMENT_H
#define JV_ENVIRONMENT_H

#include "bn_point.h"
#include "bn_fixed.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_tiles_ptr.h"

#include "bn_sprite_items_big_floor.h"
#include "bn_sprite_items_small_floor.h"
#include "bn_sprite_items_big_wall.h"
#include "bn_sprite_items_small_wall.h"

#include "jv_math.h"

namespace jv{
class Block{
public:
    virtual ~Block(){}
    [[nodiscard]] virtual jv::para get_para() const{
        return jv::para(0,0,bn::point(0,0),0,0); // Never use this
    }
    [[nodiscard]] int x() const{
        return _x;
    }
    [[nodiscard]] int y() const{
        return _y;
    }
    [[nodiscard]] bn::point xy() const{
        return bn::point(_x, _y);
    }
protected:
    void set_xy(int x, int y){
        _x = x;
        _y = y;
    }
private:
    int _x, _y;
};


class Wall1 : public Block{
public:
    ~Wall1(){}
    Wall1(int x, int y, bn::camera_ptr& cam, unsigned char option, int z):
    _sprite(bn::sprite_items::small_wall.create_sprite(x, y))
    {
        switch(option){
            case 1:
                _sprite.set_tiles(bn::sprite_items::small_wall.tiles_item().create_tiles(0));
                _sprite.set_x(x + 8);
                _para = jv::para(x + 12, y + 4, bn::point(x, y), 16, 34);
                break;
            case 2:
                _sprite.set_tiles(bn::sprite_items::small_wall.tiles_item().create_tiles(0));
                _sprite.set_horizontal_flip(true);
                _sprite.set_x(x - 8);
                _para = jv::para(x - 12, y + 4, bn::point(x, y), 16, 34);
                break;
            case 3:
                _sprite.set_tiles(bn::sprite_items::small_wall.tiles_item().create_tiles(1));
                _sprite.set_x(x + 8);
                _para = jv::para(x + 12, y + 8, bn::point(x, y), 16, 16);
                break;
            case 4:
                _sprite.set_tiles(bn::sprite_items::small_wall.tiles_item().create_tiles(1));
                _sprite.set_horizontal_flip(true);
                _sprite.set_x(x - 8);
                _para = jv::para(x - 12, y + 8, bn::point(x, y), 16, 16);
                break;
            case 5:
                _sprite.set_tiles(bn::sprite_items::small_wall.tiles_item().create_tiles(2));
                _sprite.set_x(x + 8);
                _para = jv::para(x + 12, y + 4, bn::point(x, y), 16, 34);
                break;
            case 6:
                _sprite.set_tiles(bn::sprite_items::small_wall.tiles_item().create_tiles(2));
                _sprite.set_horizontal_flip(true);
                _sprite.set_x(x - 8);
                _para = jv::para(x - 12, y + 4, bn::point(x, y), 16, 34);
                break;
            case 7:
                _sprite.set_tiles(bn::sprite_items::small_wall.tiles_item().create_tiles(3));
                _sprite.set_x(x + 8);
                _para = jv::para(x + 12, y + 4, bn::point(x, y), 16, 34);
                break;
            case 8:
                _sprite.set_tiles(bn::sprite_items::small_wall.tiles_item().create_tiles(3));
                _sprite.set_horizontal_flip(true);
                _sprite.set_x(x - 8);
                _para = jv::para(x - 12, y + 4, bn::point(x, y), 16, 34);
                break;
            case 9:
                _sprite.set_tiles(bn::sprite_items::small_wall.tiles_item().create_tiles(4));
                _sprite.set_x(x + 8);
                _para = jv::para(x + 12, y + 4, bn::point(x, y), 16, 34);
                break;
            case 10:
                _sprite.set_tiles(bn::sprite_items::small_wall.tiles_item().create_tiles(4));
                _sprite.set_horizontal_flip(true);
                _sprite.set_x(x - 8);
                _para = jv::para(x - 12, y + 4, bn::point(x, y), 16, 34);
                break;
            case 11:
                _sprite.set_tiles(bn::sprite_items::small_wall.tiles_item().create_tiles(5));
                _sprite.set_x(x + 8);
                _para = jv::para(x + 12, y - 4, bn::point(x, y), 16, 16);
                break;
            case 12:
                _sprite.set_tiles(bn::sprite_items::small_wall.tiles_item().create_tiles(5));
                _sprite.set_horizontal_flip(true);
                _sprite.set_x(x - 8);
                _para = jv::para(x - 12, y - 4, bn::point(x, y), 16, 16);
                break;
            case 13:    // Black Diagonal
                _sprite.set_tiles(bn::sprite_shape_size(32, 32), bn::sprite_items::big_wall.tiles_item().create_tiles(0));
                _sprite.set_y(y + 16);
                _para = jv::para(x, y + 18, bn::point(x, y), 34, 16, 8);
                break;
            case 14:    // Black Diagonal
                _sprite.set_tiles(bn::sprite_shape_size(32, 32), bn::sprite_items::big_wall.tiles_item().create_tiles(0));
                _sprite.set_horizontal_flip(true);
                _sprite.set_y(y + 16);
                _para = jv::para(x, y + 18, bn::point(x, y), 34, 16, -8);
                break;
            case 15:
                _sprite.set_tiles(bn::sprite_shape_size(32, 32), bn::sprite_items::big_wall.tiles_item().create_tiles(0));
                _para = jv::para(x, y + 2, bn::point(x, y), 34, 16, 8);
                break;
            case 16:
                _sprite.set_tiles(bn::sprite_shape_size(32, 32), bn::sprite_items::big_wall.tiles_item().create_tiles(0));
                _sprite.set_horizontal_flip(true);
                _para = jv::para(x, y + 2, bn::point(x, y), 34, 16, -8);
                break;
            case 17:    // Black Upward facing wall
                _sprite.set_tiles(bn::sprite_items::small_wall.tiles_item().create_tiles(7));
                _sprite.set_rotation_angle(90);
                _sprite.set_y(y - 25);
                _para = jv::para(x, y - 22, bn::point(x, y), 34, 16);
                break;
            case 18:
                _sprite.set_tiles(bn::sprite_shape_size(32, 32), bn::sprite_items::big_wall.tiles_item().create_tiles(5));
                _para = jv::para(x, y + 3, bn::point(x, y), 34, 34);
                break;
            case 19:
                _sprite.set_tiles(bn::sprite_shape_size(32, 32), bn::sprite_items::big_wall.tiles_item().create_tiles(5));
                _sprite.set_horizontal_flip(true);
                _para = jv::para(x, y + 3, bn::point(x, y), 34, 34);
                break;
            case 20:
                _sprite.set_tiles(bn::sprite_shape_size(32, 32), bn::sprite_items::big_wall.tiles_item().create_tiles(6));
                _para = jv::para(x, y + 10, bn::point(x, y), 34, 18);
                break;
            case 21:
                _sprite.set_tiles(bn::sprite_shape_size(32, 32), bn::sprite_items::big_wall.tiles_item().create_tiles(7));
                _para = jv::para(x, y + 12, bn::point(x, y), 34, 22);
                break;
            case 22:
                _sprite.set_tiles(bn::sprite_shape_size(32, 32), bn::sprite_items::big_wall.tiles_item().create_tiles(7));
                _sprite.set_horizontal_flip(true);
                _para = jv::para(x, y + 12, bn::point(x, y), 34, 22);
                break;
            case 23:
                _sprite.set_tiles(bn::sprite_items::small_wall.tiles_item().create_tiles(7));
                _sprite.set_rotation_angle(90);
                _sprite.set_y(y - 9);
                _para = jv::para(x, y - 8, bn::point(x, y), 34, 12);
                break;
            default:
                break;
        }
        _sprite.set_camera(cam);
        _sprite.set_bg_priority(3);
        _sprite.set_z_order(z);
        this->set_xy(x, y);
    }
    
    [[nodiscard]] jv::para get_para() const override{
        return _para;
    }

    void set_camera(bn::camera_ptr& new_cam){
        _sprite.set_camera(new_cam);
    }
private:
    bn::sprite_ptr _sprite;
    jv::para _para;
};

class Wall2 : public Block{
public:
    ~Wall2(){}
    Wall2(int x, int y, bn::camera_ptr& cam, unsigned char option, int z):
    _sprite_1(bn::sprite_items::big_wall.create_sprite(x, y)),
    _sprite_2(bn::sprite_items::big_wall.create_sprite(x, y))
    {
        switch(option){
            case W1_COUNT + 1:
                _sprite_1.set_tiles(bn::sprite_items::big_wall.tiles_item().create_tiles(2));
                _sprite_1.set_y(y - 16);
                _sprite_2.set_tiles(bn::sprite_items::big_wall.tiles_item().create_tiles(4));
                _sprite_2.set_y(y + 16);
                _para = jv::para(x, y + 20, bn::point(x, y), 34, 16, -8);
                break;
            case W1_COUNT + 2:
                _sprite_1.set_tiles(bn::sprite_items::big_wall.tiles_item().create_tiles(2));
                _sprite_1.set_horizontal_flip(true);
                _sprite_1.set_y(y - 16);
                _sprite_2.set_tiles(bn::sprite_items::big_wall.tiles_item().create_tiles(4));
                _sprite_2.set_horizontal_flip(true);
                _sprite_2.set_y(y + 16);
                _para = jv::para(x, y + 20, bn::point(x, y), 34, 16, 8);
                break;
            case W1_COUNT + 3:
                _sprite_1.set_tiles(bn::sprite_items::big_wall.tiles_item().create_tiles(3));
                _sprite_1.set_y(y - 16);
                _sprite_2.set_tiles(bn::sprite_items::big_wall.tiles_item().create_tiles(4));
                _sprite_2.set_y(y + 16);
                _para = jv::para(x, y + 20, bn::point(x, y), 34, 16, -8);
                break;
            case W1_COUNT + 4:
                _sprite_1.set_tiles(bn::sprite_items::big_wall.tiles_item().create_tiles(3));
                _sprite_1.set_horizontal_flip(true);
                _sprite_1.set_y(y - 16);
                _sprite_2.set_tiles(bn::sprite_items::big_wall.tiles_item().create_tiles(4));
                _sprite_2.set_horizontal_flip(true);
                _sprite_2.set_y(y + 16);
                _para = jv::para(x, y + 20, bn::point(x, y), 34, 16, 8);
                break;
            case W1_COUNT + 5:     // down facing horizontal wall
                _sprite_1.set_tiles(bn::sprite_items::big_wall.tiles_item().create_tiles(1));
                _sprite_1.set_y(y);
                _sprite_2.set_tiles(bn::sprite_shape_size(16, 32), bn::sprite_items::small_wall.tiles_item().create_tiles(6));
                _sprite_2.set_rotation_angle(90);
                _sprite_2.set_y(y - 25);
                _para = jv::para(x, y + 3, bn::point(x, y), 34, 34);
                break;
            default:
                break;
        }
        _sprite_1.set_camera(cam);
        _sprite_1.set_bg_priority(3);
        _sprite_1.set_z_order(z);
        _sprite_2.set_camera(cam);
        _sprite_2.set_bg_priority(3);
        _sprite_2.set_z_order(z);
        this->set_xy(x, y);
    }
    
    [[nodiscard]] jv::para get_para() const override{
        return _para;
    }

    void set_camera(bn::camera_ptr& new_cam){
        _sprite_1.set_camera(new_cam);
        _sprite_2.set_camera(new_cam);
    }
private:
    bn::sprite_ptr _sprite_1, _sprite_2;
    jv::para _para;
};

class Floor : public Block{
public:
    ~Floor(){};
    Floor(int x, int y, bn::camera_ptr& cam, unsigned char option, int z):
    _sprite(bn::sprite_items::big_floor.create_sprite(x, y))
    {
        switch(option){
            case W1W2_COUNT + 1:
                _sprite.set_tiles(bn::sprite_items::big_floor.tiles_item().create_tiles(0));
                break;
            case W1W2_COUNT + 2:
                _sprite.set_tiles(bn::sprite_items::big_floor.tiles_item().create_tiles(0));
                _sprite.set_horizontal_flip(true);
                break;
            case W1W2_COUNT + 3:
                _sprite.set_tiles(bn::sprite_items::big_floor.tiles_item().create_tiles(0));
                _sprite.set_vertical_flip(true);
                break;
            case W1W2_COUNT + 4:
                _sprite.set_tiles(bn::sprite_items::big_floor.tiles_item().create_tiles(0));
                _sprite.set_vertical_flip(true);
                _sprite.set_horizontal_flip(true);
                break;
            case W1W2_COUNT + 5:
                _sprite.set_tiles(bn::sprite_items::big_floor.tiles_item().create_tiles(1));
                break;
            case W1W2_COUNT + 6:
                _sprite.set_tiles(bn::sprite_items::big_floor.tiles_item().create_tiles(1));
                _sprite.set_horizontal_flip(true);
                break;
            case W1W2_COUNT + 7:
                _sprite.set_tiles(bn::sprite_items::big_floor.tiles_item().create_tiles(1));
                _sprite.set_vertical_flip(true);
                break;
            case W1W2_COUNT + 8:
                _sprite.set_tiles(bn::sprite_items::big_floor.tiles_item().create_tiles(1));
                _sprite.set_vertical_flip(true);
                _sprite.set_horizontal_flip(true);
                break;
            case W1W2_COUNT + 9:
                _sprite.set_tiles(bn::sprite_items::big_floor.tiles_item().create_tiles(2));
                break;
            case W1W2_COUNT + 10:
                _sprite.set_tiles(bn::sprite_items::big_floor.tiles_item().create_tiles(2));
                _sprite.set_horizontal_flip(true);
                break;
            case W1W2_COUNT + 11:
                _sprite.set_tiles(bn::sprite_items::big_floor.tiles_item().create_tiles(2));
                _sprite.set_vertical_flip(true);
                break;
            case W1W2_COUNT + 12:
                _sprite.set_tiles(bn::sprite_items::big_floor.tiles_item().create_tiles(2));
                _sprite.set_vertical_flip(true);
                _sprite.set_horizontal_flip(true);
                break;
            case W1W2_COUNT + 13:
                _sprite.set_tiles(bn::sprite_items::big_floor.tiles_item().create_tiles(3));
                break;
            case W1W2_COUNT + 14:
                _sprite.set_tiles(bn::sprite_items::big_floor.tiles_item().create_tiles(3));
                _sprite.set_vertical_flip(true);
                break;
            case W1W2_COUNT + 15:    // Side Floors
                _sprite.set_tiles(bn::sprite_items::big_floor.tiles_item().create_tiles(5));
                break;
            case W1W2_COUNT + 16:
                _sprite.set_tiles(bn::sprite_items::big_floor.tiles_item().create_tiles(5));
                _sprite.set_horizontal_flip(true);
                break;
            case W1W2_COUNT + 17:    // Side Floors
                _sprite.set_tiles(bn::sprite_items::big_floor.tiles_item().create_tiles(5));
                _sprite.set_vertical_flip(true);
                break;
            case W1W2_COUNT + 18:
                _sprite.set_tiles(bn::sprite_items::big_floor.tiles_item().create_tiles(5));
                _sprite.set_vertical_flip(true);
                _sprite.set_horizontal_flip(true);
                break;
            case W1W2_COUNT + 19:    // Side Floors
                _sprite.set_tiles(bn::sprite_items::big_floor.tiles_item().create_tiles(6));
                break;
            case W1W2_COUNT + 20:
                _sprite.set_tiles(bn::sprite_items::big_floor.tiles_item().create_tiles(6));
                _sprite.set_horizontal_flip(true);
                break;
            case W1W2_COUNT + 21:
                _sprite.set_tiles(bn::sprite_items::big_floor.tiles_item().create_tiles(10));
                break;
            case W1W2_COUNT + 22:
                _sprite.set_tiles(bn::sprite_items::big_floor.tiles_item().create_tiles(10));
                _sprite.set_vertical_flip(true);
                break;
            case W1W2_COUNT + 23:
                _sprite.set_tiles(bn::sprite_items::big_floor.tiles_item().create_tiles(4));
                break;
            case W1W2_COUNT + 24:
                _sprite.set_tiles(bn::sprite_items::big_floor.tiles_item().create_tiles(4));
                _sprite.set_horizontal_flip(true);
                break;
            case W1W2_COUNT + 25:
                _sprite.set_tiles(bn::sprite_items::big_floor.tiles_item().create_tiles(7));
                break;
            case W1W2_COUNT + 26:
                _sprite.set_tiles(bn::sprite_items::big_floor.tiles_item().create_tiles(8));
                break;
            case W1W2_COUNT + 27:
                _sprite.set_tiles(bn::sprite_items::big_floor.tiles_item().create_tiles(9));
                break;
            case W1W2_COUNT + 28:
                _sprite.set_tiles(bn::sprite_items::big_floor.tiles_item().create_tiles(15));
                break;
            case W1W2_COUNT + 29:
                _sprite.set_tiles(bn::sprite_shape_size(32, 16), bn::sprite_items::small_floor.tiles_item().create_tiles(1));
                _sprite.set_y(y + 8);
                break;
            case W1W2_COUNT + 30:
                _sprite.set_tiles(bn::sprite_shape_size(32, 16), bn::sprite_items::small_floor.tiles_item().create_tiles(1));
                _sprite.set_vertical_flip(true);
                _sprite.set_y(y - 8);
                break;
            case W1W2_COUNT + 31:
                _sprite.set_tiles(bn::sprite_shape_size(32, 16), bn::sprite_items::small_floor.tiles_item().create_tiles(2));
                _sprite.set_y(y + 8);
                break;
            case W1W2_COUNT + 32:
                _sprite.set_tiles(bn::sprite_shape_size(32, 16), bn::sprite_items::small_floor.tiles_item().create_tiles(2));
                _sprite.set_horizontal_flip(true);
                _sprite.set_y(y + 8);
                break;
            case W1W2_COUNT + 33:
                _sprite.set_tiles(bn::sprite_shape_size(32, 16), bn::sprite_items::small_floor.tiles_item().create_tiles(2));
                _sprite.set_vertical_flip(true);
                _sprite.set_y(y - 8);
                break;
            case W1W2_COUNT + 34:
                _sprite.set_tiles(bn::sprite_shape_size(32, 16), bn::sprite_items::small_floor.tiles_item().create_tiles(2));
                _sprite.set_vertical_flip(true);
                _sprite.set_horizontal_flip(true);
                _sprite.set_y(y - 8);
                break;
            case W1W2_COUNT + 35:
                _sprite.set_tiles(bn::sprite_items::big_floor.tiles_item().create_tiles(13));
                break;
            case W1W2_COUNT + 36:
                _sprite.set_tiles(bn::sprite_shape_size(32, 16), bn::sprite_items::small_floor.tiles_item().create_tiles(13));
                _sprite.set_vertical_flip(true);
                break;
            case W1W2_COUNT + 37:
                _sprite.set_tiles(bn::sprite_shape_size(32, 16), bn::sprite_items::small_floor.tiles_item().create_tiles(3));
                _sprite.set_vertical_flip(true);
                _sprite.set_y(y - 8);
                break;
            case W1W2_COUNT + 38:
                _sprite.set_tiles(bn::sprite_shape_size(32, 16), bn::sprite_items::small_floor.tiles_item().create_tiles(3));
                _sprite.set_vertical_flip(true);
                _sprite.set_horizontal_flip(true);
                _sprite.set_y(y - 8);
                break;
            case W1W2_COUNT + 39:
                _sprite.set_tiles(bn::sprite_shape_size(32, 16), bn::sprite_items::small_floor.tiles_item().create_tiles(4));
                _sprite.set_y(y + 8);
                break;
            case W1W2_COUNT + 40:
                _sprite.set_tiles(bn::sprite_shape_size(32, 16), bn::sprite_items::small_floor.tiles_item().create_tiles(4));
                _sprite.set_vertical_flip(true);
                _sprite.set_y(y - 8);
                break;
            case W1W2_COUNT + 41:
                _sprite.set_tiles(bn::sprite_items::big_floor.tiles_item().create_tiles(17));
                break;
            case W1W2_COUNT + 42:
                _sprite.set_tiles(bn::sprite_items::big_floor.tiles_item().create_tiles(16));
                break;
            case W1W2_COUNT + 43:
                _sprite.set_tiles(bn::sprite_items::big_floor.tiles_item().create_tiles(16));
                _sprite.set_horizontal_flip(true);
                break;
            case W1W2_COUNT + 44:
                _sprite.set_tiles(bn::sprite_items::big_floor.tiles_item().create_tiles(11));
                break;
            case W1W2_COUNT + 45:
                _sprite.set_tiles(bn::sprite_items::big_floor.tiles_item().create_tiles(12));
                break;
            case W1W2_COUNT + 46:
                _sprite.set_tiles(bn::sprite_items::big_floor.tiles_item().create_tiles(12));
                _sprite.set_horizontal_flip(true);
                break;
            case W1W2_COUNT + 47:
                _sprite.set_tiles(bn::sprite_items::big_floor.tiles_item().create_tiles(12));
                _sprite.set_vertical_flip(true);
                break;
            case W1W2_COUNT + 48:
                _sprite.set_tiles(bn::sprite_items::big_floor.tiles_item().create_tiles(12));
                _sprite.set_horizontal_flip(true);
                _sprite.set_vertical_flip(true);
                break;
            case W1W2_COUNT + 49:
                _sprite.set_tiles(bn::sprite_items::big_floor.tiles_item().create_tiles(14));
                break;
            case W1W2_COUNT + 50:
                _sprite.set_tiles(bn::sprite_items::big_floor.tiles_item().create_tiles(14));
                _sprite.set_horizontal_flip(true);
                break;
            default:
                break;
        }
        _sprite.set_camera(cam);
        _sprite.set_bg_priority(3);
        _sprite.set_z_order(z);
        this->set_xy(x, y);
    }

    void set_camera(bn::camera_ptr& new_cam){
        _sprite.set_camera(new_cam);
    }

private:
    bn::sprite_ptr _sprite;
};

}

#endif