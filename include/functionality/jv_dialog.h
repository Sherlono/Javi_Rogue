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
    // Creating dialog window
    constexpr uint8_t GRAPHIC_WIDTH = bn::sprite_items::dialog_window.shape_size().width();
    constexpr uint8_t GRAPHIC_HEIGHT = bn::sprite_items::dialog_window.shape_size().height();

    bn::vector<bn::sprite_ptr, WINDOW_SPRITES_COUNT> txt_window;
    constexpr uint8_t graphic_index[WINDOW_SPRITES_COUNT] = 
        {0, 1, 1, 1, 1, 1, 0,
         0, 1, 1, 1, 1, 1, 0};

    for(int y = 0; y < 2; y++){
        for(int x = 0; x < WINDOW_WIDTH; x++){
            const uint8_t i = x + y*WINDOW_WIDTH;
            txt_window.push_back(bn::sprite_items::dialog_window.create_sprite((x%WINDOW_WIDTH)*GRAPHIC_WIDTH - 96, y*GRAPHIC_HEIGHT + 36, graphic_index[i]));
            txt_window[i].set_bg_priority(0);

            if(x == WINDOW_WIDTH - 1){
                txt_window[i].set_horizontal_flip(true);
            }
            if(y == 1){
                txt_window[i].set_vertical_flip(true);
            }
        }
    }

    // Managing text
    bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);
    bn::vector<bn::sprite_ptr, 8> txt_sprts[3];
    bn::string<64> text = "";

    text_generator.set_bg_priority(0);

    for(char letter : line_1){
        txt_sprts[0].clear();
        text = text + letter;
        text_generator.generate(-88, 42, text, txt_sprts[0]);
        bn::core::update();
    }
    while(!bn::keypad::a_pressed() && !bn::keypad::b_pressed()){
        bn::core::update();
    }

    text.clear();
    for(char letter : line_2){
        txt_sprts[1].clear();
        text = text + letter;
        text_generator.generate(-88, 52, text, txt_sprts[1]);
        bn::core::update();
    }
    while(!bn::keypad::a_pressed() && !bn::keypad::b_pressed()){
        bn::core::update();
    }

    text.clear();
    for(char letter : line_3){
        txt_sprts[2].clear();
        text = text + letter;
        text_generator.generate(-88, 62, text, txt_sprts[2]);
        bn::core::update();
    }

    //jv::Interface::Log_resources();
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