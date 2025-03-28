#ifndef JV_DEBUG_H
#define JV_DEBUG_H

#include "bn_log.h"
#include "bn_fixed.h"
#include "bn_vector.h"
#include "bn_string.h"
#include "bn_keypad.h"
#include "bn_sprite_ptr.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_text_generator.h"
#include "common_variable_8x8_sprite_font.h"

#include "jv_constants.h"
#include "jv_interface.h"

#include "bn_sprite_items_cursor.h"

namespace jv{
struct menu_option{
    enum {isInt, isFloat, isBool};
    menu_option(int* d, bn::string_view t):_text(t),  _i(d), _type(isInt){}
    menu_option(bn::fixed* d, bn::string_view t): _text(t), _f(d), _type(isFloat){}
    menu_option(bool* d, bn::string_view t): _text(t), _b(d), _type(isBool){}

    // Getters
    [[nodiscard]] int getInt() const { return *_i;}
    [[nodiscard]] bn::fixed getFloat() const { return *_f;}
    [[nodiscard]] bool getBool() const { return *_b;}
    [[nodiscard]] bn::string_view text() const { return _text;}
    [[nodiscard]] char type() const { return _type;}

    [[nodiscard]] bool is_Int() const { return _type == isInt;}
    [[nodiscard]] bool is_Float() const { return _type == isFloat;}
    [[nodiscard]] bool is_Bool() const { return _type == isBool;}

    void increase(){
        switch(_type){
            case isInt:
                *_i = *_i + 1;
                break;
            case isFloat:
                *_f = *_f + 0.1f;
                break;
            case isBool:
                *_b = !*_b;
                break;
            default:
                break;
        }
    }
    void decrease(){
        switch(_type){
            case isInt:
                *_i = *_i - 1;
                break;
            case isFloat:
                *_f = *_f - 0.1f;
                break;
            case isBool:
                *_b = !*_b;
                break;
            default:
                break;
        }
    }
    void print(int x, int y, bn::vector<bn::sprite_ptr, 128>& v_text, bn::sprite_text_generator& text_generator){
        switch(_type){
            case isInt:
                text_generator.generate(x, y, bn::to_string<16>(*_i), v_text);
                break;
            case isFloat:
                text_generator.generate(x, y, bn::to_string<16>(*_f), v_text);
                break;
            case isBool:
                text_generator.generate(x, y, bn::to_string<16>(*_b), v_text);
                break;
            default:
                break;
        }
    }
    
private:
    bn::string_view _text;
    union{ int* _i; bn::fixed* _f; bool* _b;};
    char _type;
};
namespace Debug{
void debug_update(auto& options, bn::vector<bn::sprite_ptr, 128>& v_text, bn::sprite_text_generator& text_generator, const int index, const bool increase);
void Start(auto& options, bn::vector<bn::regular_bg_ptr, 4> bgs, bn::vector<bn::sprite_ptr, 120> sprts){
    // Hide all previous graphics
    for(int i = 0; i < sprts.size(); i++){
        if(i < bgs.size()){ bgs[i].set_visible(false);}
        sprts[i].set_visible(false);
    }

    static int index = 0;
    uchar_t hold = 0;
    bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);
    bn::sprite_ptr cursor = bn::sprite_items::cursor.create_sprite(-20, -70 + 9*index);
    bn::vector<bn::sprite_ptr, 128> v_text;

    for(int i = 0; i < options.size(); i++){
        text_generator.generate(-110, -70 + 9*i, options[i].text(), v_text);
        options[i].print(-50, -70 + 9*i, v_text, text_generator);
    }

    bn::core::update();

    while(!bn::keypad::start_pressed()){
        if(bn::keypad::down_pressed()){
            if(index < options.size() - 1){
                index++;
                cursor.set_position(cursor.x(), cursor.y() + 9);
            }
        }else if(bn::keypad::up_pressed()){
            if(index > 0){
                index--;
                cursor.set_position(cursor.x(), cursor.y() - 9);
            }
        }
        
        if(bn::keypad::a_pressed()){ debug_update(options, v_text, text_generator, index, true);}
        else if(bn::keypad::b_pressed()){ debug_update(options, v_text, text_generator, index, false);}

        if(bn::keypad::a_held() && !options[index].is_Bool()){
            hold++;
            if(hold > 6){
                debug_update(options, v_text, text_generator, index, true);
                hold = 0;
            }
        }else if(bn::keypad::b_held() && !options[index].is_Bool()){
            hold++;
            if(hold > 6){
                debug_update(options, v_text, text_generator, index, false);
                hold = 0;
            }
        }

        if(bn::keypad::a_released() || bn::keypad::b_released()){hold = 0;}

        jv::resetcombo();
        bn::core::update();
    }
    
    // Unhide all previous graphics
    for(int i = 0; i < sprts.size(); i++){
        if(i < bgs.size()){ bgs[i].set_visible(true);}
        sprts[i].set_visible(true);
    }

    // Print debug values
    for(int i = 0; i < options.size(); i++){
        if(options[i].is_Int()){ BN_LOG(options[i].text(), ": ", options[i].getInt());}
        else if(options[i].is_Float()){ BN_LOG(options[i].text(), ": ", options[i].getFloat());}
        else if(options[i].is_Bool()){ BN_LOG(options[i].text(), ": ", options[i].getBool());}
    }
}

void debug_update(auto& options, bn::vector<bn::sprite_ptr, 128>& v_text, bn::sprite_text_generator& text_generator, const int index, const bool increase){
    if(increase){ options[index].increase();
    }else{ options[index].decrease();}

    v_text.clear();
    for(int i = 0; i < options.size(); i++){
        text_generator.generate(-110, -70 + 9*i, options[i].text(), v_text);
        options[i].print(-50, -70 + 9*i, v_text, text_generator);
    }
}
}
}

#endif