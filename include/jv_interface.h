#ifndef JV_INTERFACE_H
#define JV_INTERFACE_H

#include "bn_sprite_text_generator.h"
#include "bn_math.h"

#include "jv_environment.h"
#include "jv_actors.h"
#include "jv_math.h"

const bn::fixed speed = 1.0;

namespace jv{
void move_player(bn::camera_ptr& cam, jv::Player& player, bn::vector<jv::para, 64>& para_v){
    bool tl1 = false;
    bool tr1 = false;
    bool bl1 = false;
    bool br1 = false;

    for(unsigned char i = 0; i < para_v.size(); i++){
        bn::fixed x_distance = bn::abs(para_v[i].x() - player.x());
        bn::fixed y_distance = bn::abs(para_v[i].y() -  player.y());
        if(x_distance < 24 && y_distance < 24){
            tl1 = tl1 + para_v[i].contains_top_left(player.get_para());
            tr1 = tr1 + para_v[i].contains_top_right(player.get_para());
            bl1 = bl1 + para_v[i].contains_bottom_left(player.get_para());
            br1 = br1 + para_v[i].contains_bottom_right(player.get_para());
        }
    }

    if(bn::keypad::up_held() && !(tl1 && tr1)){
        cam.set_position(cam.x(), cam.y() - speed);
        player.set_position(cam.x(), cam.y() - speed);
    }else if(bn::keypad::down_held() && !(bl1 && br1)){
        cam.set_position(cam.x(), cam.y() + speed);
        player.set_position(cam.x(), cam.y() + speed);
    }
    if(bn::keypad::left_held() && !(tl1 && bl1)){
        cam.set_position(cam.x() - speed, cam.y());
        player.set_position(cam.x() - speed, cam.y());
    }else if(bn::keypad::right_held() && !(tr1 && br1)){
        cam.set_position(cam.x() + speed, cam.y());
        player.set_position(cam.x() + speed, cam.y());
    }

    if(bn::keypad::up_held() || bn::keypad::down_held() || bn::keypad::left_held() || bn::keypad::right_held()){
        player.walk_update();
    }else{
        player.wait();
    }
}

}

#endif