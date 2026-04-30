#ifndef JV_DEBUG_H
#define JV_DEBUG_H

#include "bn_fixed.h"
#include "bn_array.h"
#include "bn_vector.h"
#include "bn_string.h"
#include "bn_keypad.h"
#include "bn_sprite_ptr.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_text_generator.h"
#include "common_variable_8x8_sprite_font.h"

#include "jv_constants.h"
#include "jv_interface.h"
#include "jv_global.h"
#include "jv_healthbar.h"
#include "jv_tiled_bg.h"

#include "bn_sprite_items_cursor.h"

#if DEV_ENABLED
    #include "bn_log.h"
    static_assert(DEV_ENABLED, "Log is not enabled");
#endif

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
                *_f = *_f + bn::fixed(0.1);
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
                *_f = *_f - bn::fixed(0.1);
                break;
            case isBool:
                *_b = !*_b;
                break;
            default:
                break;
        }
    }
    void print(int x, int y, bn::ivector<bn::sprite_ptr>& v_text, bn::sprite_text_generator& text_generator){
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

class Debug{
public:
    static void Start(jv::menu_option* options, uint8_t const options_size){
        Debug instance(options, options_size);
    }
private:
    static int index;
    ~Debug() = default;
    Debug(jv::menu_option* o, uint8_t const o_size): 
    options(o), options_size(o_size),
    text_generator(common::variable_8x8_sprite_font),
    cursor(bn::sprite_items::cursor.create_sprite(36 + X_OFFSET, -70 + 9*index))
    {
        for(int i = 0; i < options_size; i++){
            maxStringSize = maxStringSize < options[i].text().length() ? options[i].text().length() : maxStringSize;
        }

        text_generator.set_bg_priority(0);
        cursor.set_bg_priority(0);
        cursor.set_x(cursor.x() + 8*maxStringSize);

        for(int i = 0; i < options_size; i++){
            text_generator.generate(14 + X_OFFSET, -70 + 9*i, options[i].text(), v_text);
            options[i].print(X_OFFSET + 8*maxStringSize, -70 + 9*i, v_text, text_generator);
        }

        bn::core::update();

        while(!bn::keypad::select_pressed()){
            if(bn::keypad::down_pressed()){
                if(index < options_size - 1){
                    index++;
                    cursor.set_position(cursor.x(), cursor.y() + 9);
                }
            }else if(bn::keypad::up_pressed()){
                if(index > 0){
                    index--;
                    cursor.set_position(cursor.x(), cursor.y() - 9);
                }
            }
            
            if(bn::keypad::a_pressed()){ debug_update(true);}
            else if(bn::keypad::b_pressed()){ debug_update(false);}

            if(bn::keypad::a_held() && !options[index].is_Bool()){
                hold++;
                if(hold > 6){
                    debug_update(true);
                    hold = 0;
                }
            }else if(bn::keypad::b_held() && !options[index].is_Bool()){
                hold++;
                if(hold > 6){
                    debug_update(false);
                    hold = 0;
                }
            }


            if(bn::keypad::a_released() || bn::keypad::b_released()){hold = 0;}

            jv::Interface::resetcombo();
            bn::core::update();
        }
        
        // Print debug values
        /*for(int i = 0; i < options_size; i++){
            if(options[i].is_Int()){ BN_LOG(options[i].text(), ": ", options[i].getInt());}
            else if(options[i].is_Float()){ BN_LOG(options[i].text(), ": ", options[i].getFloat());}
            else if(options[i].is_Bool()){ BN_LOG(options[i].text(), ": ", options[i].getBool());}
        }*/
    }
    
    void debug_update(const bool increase){
        if(increase){ options[index].increase();}
        else{ options[index].decrease();}

        v_text.clear();
        for(int i = 0; i < options_size; i++){
            text_generator.generate(14 + X_OFFSET, -70 + 9*i, options[i].text(), v_text);
            options[i].print(X_OFFSET + 8*maxStringSize, -70 + 9*i, v_text, text_generator);
        }
    }
    static constexpr int8_t X_OFFSET = -124;
    int maxStringSize = 0;
    jv::menu_option* options;

    uint8_t hold = 0, options_size;

    bn::sprite_text_generator text_generator;
    bn::sprite_ptr cursor;
    bn::vector<bn::sprite_ptr, 32> v_text;
};

int Debug::index = 0;
}

#endif