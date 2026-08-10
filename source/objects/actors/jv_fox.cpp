#include "jv_fox.h"

#include "bn_log.h"

#include "jv_math.h"
#include "jv_global.h"
#include "jv_dialog.h"

#include "bn_sprite_items_fox.h"
#include "bn_sprite_items_cursor.h"

namespace jv{
Fox::Fox(bn::point position):   // Constructor
    NPC(Actor_data::Id::Fox, position)
    {
        //if(is_on_screen()) load_graphics(animation::Id::Walk);
    }

void Fox::force_move_player(){
    bn::fixed player_speed = Global::Player().get_speed()/2;
    bn::fixed_point player_target = this->position() + bn::point(0, 14);
    bn::fixed_point player_direction = jv::normalize(player_target - Global::Graphics_Manager()[Global::Player().get_graphics_key()].position());
    
    while(true){
        Global::update();
        bn::fixed target_x = Global::Graphics_Manager()[Global::Player().get_graphics_key()].x(), target_y = Global::Graphics_Manager()[Global::Player().get_graphics_key()].y();
        if((player_direction.x() > 0 && _map_obstacle(EAST)) || (player_direction.x() < 0 && _map_obstacle(WEST))){
            target_x += player_direction.x()*player_speed;
        }
        if((player_direction.y() > 0 && _map_obstacle(SOUTH)) || (player_direction.y() < 0 && _map_obstacle(NORTH))){
            target_y += player_direction.y()*player_speed;
        }
        Global::Player().set_position(target_x, target_y);

        if(Global::Graphics_Manager()[Global::Player().get_graphics_key()].y() > sprite().y()){
            sprite().set_z_order(Global::Graphics_Manager()[Global::Player().get_graphics_key()].z_order() + 1);
        }else{
            sprite().set_z_order(Global::Graphics_Manager()[Global::Player().get_graphics_key()].z_order() - 1);
        }
        
        Global::Graphics_Manager()[Global::Player().get_graphics_key()].animation.update();
        Global::Graphics_Manager()[graphics_key].animation.update();

        if(player_target.y() - Global::Graphics_Manager()[Global::Player().get_graphics_key()].y() < 0){
            Global::Player().set_position(player_target);
            Global::Graphics_Manager()[Global::Player().get_graphics_key()].configure_animation(Actor_data::Id::Player, NORTH, animation::Id::Walk, Global::Player().position());
            break;
        }
        bn::core::update();
    }
}

void Fox::choice(){
    bn::sprite_text_generator text_gen(common::variable_8x8_sprite_font);
    bn::vector<bn::sprite_ptr, 9> txt_sprts;
    bn::sprite_ptr cursor = bn::sprite_items::cursor.create_sprite(-10, 0);

    text_gen.set_bg_priority(0);
    text_gen.generate(-64, 0, "Your left     His right", txt_sprts);
    cursor.set_bg_priority(0);

    bn::core::update();

    while(!bn::keypad::a_pressed()){
        Global::update();
        if(bn::keypad::right_pressed()){
            if(cursor.x() < 0){
                cursor.set_x(-cursor.x());
                cursor.set_horizontal_flip(!cursor.horizontal_flip());
            }
        }else if(bn::keypad::left_pressed()){
            if(cursor.x() > 0){
                cursor.set_x(-cursor.x());
                cursor.set_horizontal_flip(!cursor.horizontal_flip());
            }
        }

        Global::Graphics_Manager()[Global::Player().get_graphics_key()].animation.update();
        Global::Graphics_Manager()[graphics_key].animation.update();
        bn::core::update();
    }
}

void Fox::update(){
    if(is_on_screen()){
        if(!has_graphics) load_graphics(animation::Id::Walk);

        if(Global::Graphics_Manager()[Global::Player().get_graphics_key()].y() > sprite().y()){
            sprite().set_z_order(Global::Graphics_Manager()[Global::Player().get_graphics_key()].z_order() + 1);
        }else{
            sprite().set_z_order(Global::Graphics_Manager()[Global::Player().get_graphics_key()].z_order() - 1);
        }
        
        if(Global::Player().get_state() == State::NORMAL && !Global::Player().is_attacking()) [[likely]] {
            // Dialog
            if(bn::keypad::a_pressed() && Global::Player().rect().intersects(rect()) && Global::Player().can_interact()){
                Global::Player().get_hitbox().set_position(Global::Graphics_Manager()[graphics_key].x().floor_integer(), Global::Graphics_Manager()[graphics_key].y().floor_integer());

                force_move_player();
                
                jv::Dialog::init("Kekeke. Hello there delver. I got", "something you might like. All you", "have to do is guess correctly.");
                
                Global::Graphics_Manager()[graphics_key].animation = bn::create_sprite_animate_action_once(sprite(), 8, bn::sprite_items::fox.tiles_item(), 3, 4, 5, 6, 7);

                while(!Global::Graphics_Manager()[graphics_key].animation.done()){
                    Global::update();
                    Global::Graphics_Manager()[Global::Player().get_graphics_key()].animation.update();
                    Global::Graphics_Manager()[graphics_key].animation.update();
                    bn::core::update();
                }
                for(int stall = 0; stall < 30; stall++){
                    Global::update();
                    Global::Graphics_Manager()[Global::Player().get_graphics_key()].animation.update();
                    bn::core::update();
                }

                jv::Dialog::init("So tell me. Is it in your left or", "my right?.");
                Global::Graphics_Manager()[graphics_key].animation = bn::create_sprite_animate_action_forever(sprite(), 8, bn::sprite_items::fox.tiles_item(), 8, 9, 8, 10);

                choice();

                Global::Graphics_Manager()[graphics_key].animation = bn::create_sprite_animate_action_forever(sprite(), 8, bn::sprite_items::fox.tiles_item(), 7, 7, 7);
                jv::Dialog::init("Sorry pal. Wrong paw. Better luck", "next time ;)");
                Global::Graphics_Manager()[graphics_key].animation = bn::create_sprite_animate_action_forever(sprite(), 8, bn::sprite_items::fox.tiles_item(), 0, 1, 0, 2);
                
                Global::Player().look_at({0, -1});
                Global::Player().spend_interact_token();
            }
        }

        //Global::Actor_Graphic(graphics_id).animation.update();
    }else{
        if(has_graphics){
            Global::Graphics_Manager().erase_sprite(graphics_key);
            has_graphics = false;
        }
    }
}

}