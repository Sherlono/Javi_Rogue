#ifndef JV_DIALOG_H
#define JV_DIALOG_H

#include "bn_sprite_ptr.h"
#include "common_variable_8x8_sprite_font.h"
#include "bn_sprite_text_generator.h"
#include "bn_string.h"

#include "bn_regular_bg_items_dialog_window.h"

namespace jv{
namespace Dialog{
void init(const bn::string_view line1, const bn::string_view line2, const bn::string_view line3){
    static bn::regular_bg_ptr d_window = bn::regular_bg_items::dialog_window.create_bg(0,0);
    d_window.set_visible(true);
    d_window.set_priority(0);
    bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);
    static bn::vector<bn::sprite_ptr, 32> sprites1, sprites2, sprites3;
    bn::string<64> text = "";

    for(char c : line1){
        sprites1.clear();
        text = text + c;
        text_generator.generate(-6*16 + 8, 2*16 + 2, text, sprites1);
        for(int i = 0; i < sprites1.size(); i++){
            sprites1[i].set_bg_priority(0);
        }
        bn::core::update();
    }
    while(!bn::keypad::a_pressed() && !bn::keypad::b_pressed()){
        bn::core::update();
    }

    text = "";
    for(char c : line2){
        sprites2.clear();
        text = text + c;
        text_generator.generate(-6*16 + 8, 2*16 + 2 + 10, text, sprites2);
        for(int i = 0; i < sprites2.size(); i++){
            sprites2[i].set_bg_priority(0);
        }
        bn::core::update();
    }
    while(!bn::keypad::a_pressed() && !bn::keypad::b_pressed()){
        bn::core::update();
    }

    text = "";
    for(char c : line3){
        sprites3.clear();
        text = text + c;
        text_generator.generate(-6*16 + 8, 2*16 + 2 + 10*2, text, sprites3);
        for(int i = 0; i < sprites3.size(); i++){
            sprites3[i].set_bg_priority(0);
        }
        bn::core::update();
    }

    while(!bn::keypad::a_pressed() && !bn::keypad::b_pressed()){
        bn::core::update();
    }
    d_window.set_visible(false);
    sprites1.clear();
    sprites2.clear();
    sprites3.clear();
}
/*void update(){

}*/
}

}

#endif