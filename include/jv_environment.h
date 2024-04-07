#ifndef JV_ENVIRONMENT_H
#define JV_ENVIRONMENT_H

#include "bn_sprite_ptr.h"
#include "bn_sprite_tiles_ptr.h"
#include "bn_sprite_builder.h"

#include "bn_sprite_items_big_floor.h"
#include "bn_sprite_items_small_floor.h"
#include "bn_sprite_items_huge_wall.h"
#include "bn_sprite_items_big_wall.h"
#include "bn_sprite_items_small_wall.h"

#include "jv_constants.h"
#include "jv_math.h"

namespace jv{
class Block{
public:
    ~Block(){}
    Block(int x, int y, bn::camera_ptr& cam, unsigned char option, int z_order = 0);
    // Getters
    [[nodiscard]] int x() const{
        return _x;
    }
    [[nodiscard]] int y() const{
        return _y;
    }
    [[nodiscard]] bn::point xy() const{
        return bn::point(_x, _y);
    }
    [[nodiscard]] jv::para get_para(){
        return _para;
    }
    // Setters
    void set_camera(bn::camera_ptr& new_cam);
    void set_block(int x, int y, bn::camera_ptr& cam, unsigned char option, int z_order = 0);
protected:
    bn::sprite_ptr sprite_factory(int x, int y, unsigned char option){
        switch(option){
            case 1:{
                bn::sprite_builder builder(bn::sprite_items::small_wall, 0);
                builder.set_position(x + 8, y);
                return builder.build();
            }
            case 2:{
                bn::sprite_builder builder(bn::sprite_items::small_wall, 0);
                builder.set_position(x - 8, y);
                builder.set_horizontal_flip(true);
                return builder.build();
            }
            case 3:{
                bn::sprite_builder builder(bn::sprite_items::small_wall, 1);
                builder.set_position(x + 8, y);
                return builder.build();
            }
            case 4:{
                bn::sprite_builder builder(bn::sprite_items::small_wall, 1);
                builder.set_position(x - 8, y);
                builder.set_horizontal_flip(true);
                return builder.build();
            }
            case 5:{
                bn::sprite_builder builder(bn::sprite_items::small_wall, 2);
                builder.set_position(x + 8, y);
                return builder.build();
            }
            case 6:{
                bn::sprite_builder builder(bn::sprite_items::small_wall, 2);
                builder.set_position(x - 8, y);
                builder.set_horizontal_flip(true);
                return builder.build();
            }
            case 7:{
                bn::sprite_builder builder(bn::sprite_items::small_wall, 3);
                builder.set_position(x + 8, y);
                return builder.build();
            }
            case 8:{
                bn::sprite_builder builder(bn::sprite_items::small_wall, 3);
                builder.set_position(x - 8, y);
                builder.set_horizontal_flip(true);
                return builder.build();
            }
            case 9:{
                bn::sprite_builder builder(bn::sprite_items::small_wall, 4);
                builder.set_position(x + 8, y);
                return builder.build();
            }
            case 10:{
                bn::sprite_builder builder(bn::sprite_items::small_wall, 4);
                builder.set_position(x - 8, y);
                builder.set_horizontal_flip(true);
                return builder.build();
            }
            case 11:{
                bn::sprite_builder builder(bn::sprite_items::small_wall, 5);
                builder.set_position(x + 8, y);
                return builder.build();
            }
            case 12:{
                bn::sprite_builder builder(bn::sprite_items::small_wall, 5);
                builder.set_position(x - 8, y);
                builder.set_horizontal_flip(true);
                return builder.build();
            }
            case 13:{   // Black Diagonal
                bn::sprite_builder builder(bn::sprite_items::big_wall, 0);
                builder.set_position(x, y + 16);
                return builder.build();
            }
            case 14:{   // Black Diagonal
                bn::sprite_builder builder(bn::sprite_items::big_wall, 0);
                builder.set_position(x, y + 16);
                builder.set_horizontal_flip(true);
                return builder.build();
            }
            case 15:{
                bn::sprite_builder builder(bn::sprite_items::big_wall, 0);
                builder.set_position(x, y);
                return builder.build();
            }
            case 16:{
                bn::sprite_builder builder(bn::sprite_items::big_wall, 0);
                builder.set_position(x, y);
                builder.set_horizontal_flip(true);
                return builder.build();
            }
            case 17:{   // Black Upward facing wall
                bn::sprite_builder builder(bn::sprite_items::small_wall, 6);
                builder.set_position(x, y - 25);
                builder.set_rotation_angle(90);
                return builder.build();
            }
            case 18:{
                bn::sprite_builder builder(bn::sprite_items::big_wall, 1);
                builder.set_position(x, y);
                return builder.build();
            }
            case 19:{
                bn::sprite_builder builder(bn::sprite_items::big_wall, 1);
                builder.set_position(x, y);
                builder.set_horizontal_flip(true);
                return builder.build();
            }
            case 20:{
                bn::sprite_builder builder(bn::sprite_items::big_wall, 2);
                builder.set_position(x, y);
                return builder.build();
            }
            case 21:{
                bn::sprite_builder builder(bn::sprite_items::big_wall, 3);
                builder.set_position(x, y);
                return builder.build();
            }
            case 22:{
                bn::sprite_builder builder(bn::sprite_items::big_wall, 3);
                builder.set_position(x, y);
                builder.set_horizontal_flip(true);
                return builder.build();
            }
            case 23:{
                bn::sprite_builder builder(bn::sprite_items::small_wall, 6);
                builder.set_position(x, y - 9);
                builder.set_rotation_angle(90);
                return builder.build();
            }
            case 24:{   // Huge Walls
                bn::sprite_builder builder(bn::sprite_items::huge_wall, 2);
                builder.set_position(x, y);
                return builder.build();
            }
            case 25:{
                bn::sprite_builder builder(bn::sprite_items::huge_wall, 2);
                builder.set_position(x, y);
                builder.set_horizontal_flip(true);
                return builder.build();
            }
            case 26:{
                bn::sprite_builder builder(bn::sprite_items::huge_wall, 1);
                builder.set_position(x, y);
                return builder.build();
            }
            case 27:{
                bn::sprite_builder builder(bn::sprite_items::huge_wall, 1);
                builder.set_position(x, y);
                builder.set_horizontal_flip(true);
                return builder.build();
            }
            case 28:{   // down facing horizontal wall
                bn::sprite_builder builder(bn::sprite_items::huge_wall, 0);
                builder.set_position(x, y - 16);
                return builder.build();
            }
            
            // Floors
            case W_COUNT + 1:{
                bn::sprite_builder builder(bn::sprite_items::big_floor, 0);
                builder.set_position(x, y);
                return builder.build();
            }
            case W_COUNT + 2:{
                bn::sprite_builder builder(bn::sprite_items::big_floor, 0);
                builder.set_position(x, y);
                builder.set_horizontal_flip(true);
                return builder.build();
            }
            case W_COUNT + 3:{
                bn::sprite_builder builder(bn::sprite_items::big_floor, 0);
                builder.set_position(x, y);
                builder.set_vertical_flip(true);
                return builder.build();
            }
            case W_COUNT + 4:{
                bn::sprite_builder builder(bn::sprite_items::big_floor, 0);
                builder.set_position(x, y);
                builder.set_vertical_flip(true);
                builder.set_horizontal_flip(true);
                return builder.build();
            }
            case W_COUNT + 5:{
                bn::sprite_builder builder(bn::sprite_items::big_floor, 1);
                builder.set_position(x, y);
                return builder.build();
            }
            case W_COUNT + 6:{
                bn::sprite_builder builder(bn::sprite_items::big_floor, 1);
                builder.set_position(x, y);
                builder.set_horizontal_flip(true);
                return builder.build();
            }
            case W_COUNT + 7:{
                bn::sprite_builder builder(bn::sprite_items::big_floor, 1);
                builder.set_position(x, y);
                builder.set_vertical_flip(true);
                return builder.build();
            }
            case W_COUNT + 8:{
                bn::sprite_builder builder(bn::sprite_items::big_floor, 1);
                builder.set_position(x, y);
                builder.set_vertical_flip(true);
                builder.set_horizontal_flip(true);
                return builder.build();
            }
            case W_COUNT + 9:{
                bn::sprite_builder builder(bn::sprite_items::big_floor, 2);
                builder.set_position(x, y);
                return builder.build();
            }
            case W_COUNT + 10:{
                bn::sprite_builder builder(bn::sprite_items::big_floor, 2);
                builder.set_position(x, y);
                builder.set_horizontal_flip(true);
                return builder.build();
            }
            case W_COUNT + 11:{
                bn::sprite_builder builder(bn::sprite_items::big_floor, 2);
                builder.set_position(x, y);
                builder.set_vertical_flip(true);
                return builder.build();
            }
            case W_COUNT + 12:{
                bn::sprite_builder builder(bn::sprite_items::big_floor, 2);
                builder.set_position(x, y);
                builder.set_vertical_flip(true);
                builder.set_horizontal_flip(true);
                return builder.build();
            }
            case W_COUNT + 13:{
                bn::sprite_builder builder(bn::sprite_items::big_floor, 3);
                builder.set_position(x, y);
                return builder.build();
            }
            case W_COUNT + 14:{
                bn::sprite_builder builder(bn::sprite_items::big_floor, 3);
                builder.set_position(x, y);
                builder.set_vertical_flip(true);
                return builder.build();
            }
            case W_COUNT + 15:    // Side Floors
                {
                bn::sprite_builder builder(bn::sprite_items::big_floor, 5);
                builder.set_position(x, y);
                return builder.build();
            }
            case W_COUNT + 16:{
                bn::sprite_builder builder(bn::sprite_items::big_floor, 5);
                builder.set_position(x, y);
                builder.set_horizontal_flip(true);
                return builder.build();
            }
            case W_COUNT + 17:    // Side Floors
                {
                bn::sprite_builder builder(bn::sprite_items::big_floor, 5);
                builder.set_position(x, y);
                builder.set_vertical_flip(true);
                return builder.build();
            }
            case W_COUNT + 18:{
                bn::sprite_builder builder(bn::sprite_items::big_floor, 5);
                builder.set_position(x, y);
                builder.set_vertical_flip(true);
                builder.set_horizontal_flip(true);
                return builder.build();
            }
            case W_COUNT + 19:    // Side Floors
                {
                bn::sprite_builder builder(bn::sprite_items::big_floor, 6);
                builder.set_position(x, y);
                return builder.build();
            }
            case W_COUNT + 20:{
                bn::sprite_builder builder(bn::sprite_items::big_floor, 6);
                builder.set_position(x, y);
                builder.set_horizontal_flip(true);
                return builder.build();
            }
            case W_COUNT + 21:{
                bn::sprite_builder builder(bn::sprite_items::big_floor, 10);
                builder.set_position(x, y);
                return builder.build();
            }
            case W_COUNT + 22:{
                bn::sprite_builder builder(bn::sprite_items::big_floor, 10);
                builder.set_position(x, y);
                builder.set_vertical_flip(true);
                return builder.build();
            }
            case W_COUNT + 23:{
                bn::sprite_builder builder(bn::sprite_items::big_floor, 4);
                builder.set_position(x, y);
                return builder.build();
            }
            case W_COUNT + 24:{
                bn::sprite_builder builder(bn::sprite_items::big_floor, 4);
                builder.set_position(x, y);
                builder.set_horizontal_flip(true);
                return builder.build();
            }
            case W_COUNT + 25:{
                bn::sprite_builder builder(bn::sprite_items::big_floor, 7);
                builder.set_position(x, y);
                return builder.build();
            }
            case W_COUNT + 26:{
                bn::sprite_builder builder(bn::sprite_items::big_floor, 8);
                builder.set_position(x, y);
                return builder.build();
            }
            case W_COUNT + 27:{
                bn::sprite_builder builder(bn::sprite_items::big_floor, 9);
                builder.set_position(x, y);
                return builder.build();
            }
            case W_COUNT + 28:{
                bn::sprite_builder builder(bn::sprite_items::big_floor, 15);
                builder.set_position(x, y);
                return builder.build();
            }
            case W_COUNT + 29:{
                bn::sprite_builder builder(bn::sprite_items::small_floor, 1);
                builder.set_position(x, y + 8);
                return builder.build();
            }
            case W_COUNT + 30:{
                bn::sprite_builder builder(bn::sprite_items::small_floor, 1);
                builder.set_position(x, y - 8);
                builder.set_vertical_flip(true);
                return builder.build();
            }
            case W_COUNT + 31:{
                bn::sprite_builder builder(bn::sprite_items::small_floor, 2);
                builder.set_position(x, y + 8);
                return builder.build();
            }
            case W_COUNT + 32:{
                bn::sprite_builder builder(bn::sprite_items::small_floor, 2);
                builder.set_position(x, y + 8);
                builder.set_horizontal_flip(true);
                return builder.build();
            }
            case W_COUNT + 33:{
                bn::sprite_builder builder(bn::sprite_items::small_floor, 2);
                builder.set_position(x, y + 8);
                builder.set_vertical_flip(true);
                return builder.build();
            }
            case W_COUNT + 34:{
                bn::sprite_builder builder(bn::sprite_items::small_floor, 2);
                builder.set_position(x, y - 8);
                builder.set_vertical_flip(true);
                builder.set_horizontal_flip(true);
                return builder.build();
            }
            case W_COUNT + 35:{
                bn::sprite_builder builder(bn::sprite_items::big_floor, 13);
                builder.set_position(x, y);
                return builder.build();
            }
            case W_COUNT + 36:{
                bn::sprite_builder builder(bn::sprite_items::big_floor, 13);
                builder.set_position(x, y);
                builder.set_vertical_flip(true);
                return builder.build();
            }
            case W_COUNT + 37:{
                bn::sprite_builder builder(bn::sprite_items::small_floor, 3);
                builder.set_position(x, y - 8);
                builder.set_vertical_flip(true);
                return builder.build();
            }
            case W_COUNT + 38:{
                bn::sprite_builder builder(bn::sprite_items::small_floor, 3);
                builder.set_position(x, y - 8);
                builder.set_vertical_flip(true);
                builder.set_horizontal_flip(true);
                return builder.build();
            }
            case W_COUNT + 39:{
                bn::sprite_builder builder(bn::sprite_items::small_floor, 4);
                builder.set_position(x, y + 8);
                return builder.build();
            }
            case W_COUNT + 40:{
                bn::sprite_builder builder(bn::sprite_items::small_floor, 4);
                builder.set_position(x, y - 8);
                builder.set_vertical_flip(true);
                return builder.build();
            }
            case W_COUNT + 41:{
                bn::sprite_builder builder(bn::sprite_items::big_floor, 17);
                builder.set_position(x, y);
                return builder.build();
            }
            case W_COUNT + 42:{
                bn::sprite_builder builder(bn::sprite_items::big_floor, 16);
                builder.set_position(x, y);
                return builder.build();
            }
            case W_COUNT + 43:{
                bn::sprite_builder builder(bn::sprite_items::big_floor, 16);
                builder.set_position(x, y);
                builder.set_horizontal_flip(true);
                return builder.build();
            }
            case W_COUNT + 44:{
                bn::sprite_builder builder(bn::sprite_items::big_floor, 11);
                builder.set_position(x, y);
                return builder.build();
            }
            case W_COUNT + 45:{
                bn::sprite_builder builder(bn::sprite_items::big_floor, 12);
                builder.set_position(x, y);
                return builder.build();
            }
            case W_COUNT + 46:{
                bn::sprite_builder builder(bn::sprite_items::big_floor, 12);
                builder.set_position(x, y);
                builder.set_horizontal_flip(true);
                return builder.build();
            }
            case W_COUNT + 47:{
                bn::sprite_builder builder(bn::sprite_items::big_floor, 12);
                builder.set_position(x, y);
                builder.set_vertical_flip(true);
                return builder.build();
            }
            case W_COUNT + 48:{
                bn::sprite_builder builder(bn::sprite_items::big_floor, 12);
                builder.set_position(x, y);
                builder.set_horizontal_flip(true);
                builder.set_vertical_flip(true);
                return builder.build();
            }
            case W_COUNT + 49:{
                bn::sprite_builder builder(bn::sprite_items::big_floor, 14);
                builder.set_position(x, y);
                return builder.build();
            }
            case W_COUNT + 50:{
                bn::sprite_builder builder(bn::sprite_items::big_floor, 14);
                builder.set_position(x, y);
                builder.set_horizontal_flip(true);
                return builder.build();
            }
            default:{
                bn::sprite_builder builder(bn::sprite_items::small_floor, 7);
                builder.set_visible(false);
                builder.set_position(0, 0);
                return builder.build();
            }
        }
    }

    jv::para para_factory(int x, int y, unsigned char option){
        switch(option){
            case 1:
                return jv::para(x + 12, y + 4, 16, 34);
            case 2:
                return jv::para(x - 12, y + 4, 16, 34);
            case 3:
                return jv::para(x + 12, y + 8, 16, 16);
            case 4:
                return jv::para(x - 12, y + 8, 16, 16);
            case 5:
                return jv::para(x + 12, y + 4, 16, 34);
            case 6:
                return jv::para(x - 12, y + 4, 16, 34);
            case 7:
                return jv::para(x + 12, y + 4, 16, 34);
            case 8:
                return jv::para(x - 12, y + 4, 16, 34);
            case 9:
                return jv::para(x + 12, y + 4, 16, 34);
            case 10:
                return jv::para(x - 12, y + 4, 16, 34);
            case 11:
                return jv::para(x + 12, y - 4, 16, 16);
            case 12:
                return jv::para(x - 12, y - 4, 16, 16);
            case 13:    // Black Diagonal
                return jv::para(x, y + 18, 34, 16, 8);
            case 14:    // Black Diagonal
                return jv::para(x, y + 18, 34, 16, -8);
            case 15:
                return jv::para(x, y + 2, 34, 16, 8);
            case 16:
                return jv::para(x, y + 2, 34, 16, -8);
            case 17:    // Black Upward facing wall
                return jv::para(x, y - 22, 34, 16);
            case 18:
                return jv::para(x, y + 3, 34, 34);
            case 19:
                return jv::para(x, y + 3, 34, 34);
            case 20:
                return jv::para(x, y + 10, 34, 18);
            case 21:
                return jv::para(x, y + 12, 34, 22);
            case 22:
                return jv::para(x, y + 12, 34, 22);
            case 23:
                return jv::para(x, y - 8, 34, 12);
            case 24:    // Huge Walls
                return jv::para(x, y + 20, 34, 16, -8);
            case 25:
                return jv::para(x, y + 20, 34, 16, 8);
            case 26:
                return jv::para(x, y + 20, 34, 16, -8);
            case 27:
                return jv::para(x, y + 20, 34, 16, 8);
            case 28:     // Down facing horizontal wall
                return jv::para(x, y + 3, 34, 34);
            default:
                return jv::para(0, 0, 0, 0);
        }
    }
private:
    bn::sprite_ptr _sprite;
    jv::para _para;
    int _x, _y;
};

}

#endif