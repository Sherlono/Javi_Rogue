#ifndef JV_DIALOG_H
#define JV_DIALOG_H

#include "bn_string.h"
#include "bn_sprite_ptr.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_text_generator.h"
#include "common_variable_8x8_sprite_font.h"

#include "bn_regular_bg_items_dialog_window.h"

namespace jv::Dialog{
inline void init(const bn::string_view line1, const bn::string_view line2 = "", const bn::string_view line3 = ""){
    bn::regular_bg_ptr dialog_window = bn::regular_bg_items::dialog_window.create_bg(0,0);
    dialog_window.set_visible(true);
    dialog_window.set_priority(0);
    bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);
    bn::vector<bn::sprite_ptr, 32> v_sprts[3];
    bn::string<64> text = "";

    for(char c : line1){
        v_sprts[0].clear();
        text = text + c;
        text_generator.generate(-88, 42, text, v_sprts[0]);
        for(int i = 0; i < v_sprts[0].size(); i++){
            v_sprts[0][i].set_bg_priority(0);
        }
        bn::core::update();
    }
    while(!bn::keypad::a_pressed() && !bn::keypad::b_pressed()){
        bn::core::update();
    }

    text = "";
    for(char c : line2){
        v_sprts[1].clear();
        text = text + c;
        text_generator.generate(-88, 52, text, v_sprts[1]);
        for(int i = 0; i < v_sprts[1].size(); i++){
            v_sprts[1][i].set_bg_priority(0);
        }
        bn::core::update();
    }
    while(!bn::keypad::a_pressed() && !bn::keypad::b_pressed()){
        bn::core::update();
    }

    text = "";
    for(char c : line3){
        v_sprts[2].clear();
        text = text + c;
        text_generator.generate(-88, 62, text, v_sprts[2]);
        for(int i = 0; i < v_sprts[2].size(); i++){
            v_sprts[2][i].set_bg_priority(0);
        }
        bn::core::update();
    }

    while(!bn::keypad::a_pressed() && !bn::keypad::b_pressed()){
        bn::core::update();
    }

    dialog_window.set_visible(false);
    v_sprts[0].clear();
    v_sprts[1].clear();
    v_sprts[2].clear();
}
}

#endif