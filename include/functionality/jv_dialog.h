#ifndef JV_DIALOG_H
#define JV_DIALOG_H

#include "bn_string.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "common_variable_8x8_sprite_font.h"

#include "jv_interface.h"

#include "bn_sprite_items_dialog_window.h"

#define WINDOW_SPRITES_COUNT 14
#define WINDOW_WIDTH 7

namespace jv::Dialog{
inline void init(const bn::string_view line_1, const bn::string_view line_2 = "", const bn::string_view line_3 = ""){
    uint8_t graphic_index[WINDOW_SPRITES_COUNT] = {
        0, 1, 1, 1, 1, 1, 0,
        0, 1, 1, 1, 1, 1, 0};
    bn::vector<bn::sprite_ptr, WINDOW_SPRITES_COUNT> txt_window;
    for(int y = 0; y < 2; y++){
        for(int x = 0; x < WINDOW_WIDTH; x++){
            const uint8_t i = x + y*WINDOW_WIDTH;
            txt_window.push_back(bn::sprite_items::dialog_window.create_sprite((x%WINDOW_WIDTH)*32 - (96), y*32 + 36, graphic_index[i]));
            txt_window[i].set_bg_priority(0);
            if(x == WINDOW_WIDTH - 1){
                txt_window[i].set_horizontal_flip(true);
            }
            if(y == 1){
                txt_window[i].set_vertical_flip(true);
            }
        }
    }

    bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);
    bn::vector<bn::sprite_ptr, 8> txt_sprts[3];
    bn::string<64> text = "";

    for(char c : line_1){
        txt_sprts[0].clear();
        text = text + c;
        text_generator.generate(-88, 42, text, txt_sprts[0]);
        for(int i = 0; i < txt_sprts[0].size(); i++){
            txt_sprts[0][i].set_bg_priority(0);
        }
        bn::core::update();
    }
    while(!bn::keypad::a_pressed() && !bn::keypad::b_pressed()){
        bn::core::update();
    }

    text = "";
    for(char c : line_2){
        txt_sprts[1].clear();
        text = text + c;
        text_generator.generate(-88, 52, text, txt_sprts[1]);
        for(int i = 0; i < txt_sprts[1].size(); i++){
            txt_sprts[1][i].set_bg_priority(0);
        }
        bn::core::update();
    }
    while(!bn::keypad::a_pressed() && !bn::keypad::b_pressed()){
        bn::core::update();
    }

    text = "";
    for(char c : line_3){
        txt_sprts[2].clear();
        text = text + c;
        text_generator.generate(-88, 62, text, txt_sprts[2]);
        for(int i = 0; i < txt_sprts[2].size(); i++){
            txt_sprts[2][i].set_bg_priority(0);
        }
        bn::core::update();
    }

    jv::Interface::Log_resources();
    while(!bn::keypad::a_pressed() && !bn::keypad::b_pressed()){
        bn::core::update();
    }


    txt_sprts[0].clear();
    txt_sprts[1].clear();
    txt_sprts[2].clear();
    txt_window.clear();
}
}

#endif