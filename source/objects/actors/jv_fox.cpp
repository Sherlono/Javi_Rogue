#include "jv_fox.h"

#include "bn_log.h"

#include "jv_math.h"
#include "jv_global.h"
#include "jv_dialog.h"

#include "bn_sprite_items_fox.h"
#include "bn_sprite_items_cursor.h"

namespace jv{
Fox::~Fox(){
    BN_LOG("Fox Destructor called.");
}
Fox::Fox(bn::point position):   // Constructor
    NPC(Actor_data::Id::Fox, position)
    {
        BN_LOG("Fox Constructor called.");
        if(is_on_screen()) load_graphics(animation::Id::Walk);
    }

void Fox::force_move_player(){
    Actor::Graphics& player_graphics = Global::Graphics_Manager()[Global::Player().get_graphics_key()];
    bn::fixed player_speed = Global::Player().get_speed()/2;
    bn::fixed_point player_target = this->position() + bn::point(0, 14);
    bn::fixed_point player_direction = jv::normalize(player_target - player_graphics.position());
    
    while(true){
        Global::update();
        bn::fixed target_x = player_graphics.x(), target_y = player_graphics.y();
        if((player_direction.x() > 0 && _map_obstacle(EAST)) || (player_direction.x() < 0 && _map_obstacle(WEST))){
            target_x += player_direction.x()*player_speed;
        }
        if((player_direction.y() > 0 && _map_obstacle(SOUTH)) || (player_direction.y() < 0 && _map_obstacle(NORTH))){
            target_y += player_direction.y()*player_speed;
        }
        Global::Player().set_position(target_x, target_y);

        if(player_graphics.y() > sprite().y()){
            sprite().set_z_order(player_graphics.z_order() + 1);
        }else{
            sprite().set_z_order(player_graphics.z_order() - 1);
        }
        
        player_graphics.animation.update();
        Global::Graphics_Manager()[graphics_key].animation.update();

        if(player_target.y() - player_graphics.y() < 0){
            Global::Player().set_position(player_target);
            Actor::Graphics::configure_animation(Actor_data::Id::Player, NORTH, animation::Id::Walk, Global::Player().position());
            player_graphics.animation = player_graphics.create_animation();
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
        if(!Global::Graphics_Manager().find(graphics_key)) load_graphics(animation::Id::Walk);

        Graphics& my_graphics = Global::Graphics_Manager()[graphics_key];
        Graphics& player_graphics = Global::Graphics_Manager()[Global::Player().get_graphics_key()];

        if(player_graphics.y() > my_graphics.y()){
            my_graphics.set_z_order(player_graphics.z_order() + 1);
        }else{
            my_graphics.set_z_order(player_graphics.z_order() - 1);
        }
        
        if(Global::Player().get_state() == State::NORMAL && !Global::Player().is_attacking()) [[likely]] {
            // Dialog
            if(bn::keypad::a_pressed() && Global::Player().rect().intersects(rect()) && Global::Player().can_interact()){
                Global::Player().get_hitbox().set_position(my_graphics.x().floor_integer(), my_graphics.y().floor_integer());

                force_move_player();
                
                jv::Dialog::init("Kekeke. Hello there delver. I got", "something you might like. All you", "have to do is guess correctly.");
                
                my_graphics.animation = bn::create_sprite_animate_action_once(my_graphics, 8, bn::sprite_items::fox.tiles_item(), 3, 4, 5, 6, 7);

                while(!my_graphics.animation.done()){
                    Global::update();
                    player_graphics.animation.update();
                    my_graphics.animation.update();
                    bn::core::update();
                }
                for(int stall = 0; stall < 30; stall++){
                    Global::update();
                    player_graphics.animation.update();
                    bn::core::update();
                }

                jv::Dialog::init("So tell me. Is it in your left or", "my right?.");
                my_graphics.animation = bn::create_sprite_animate_action_forever(my_graphics, 8, bn::sprite_items::fox.tiles_item(), 8, 9, 8, 10);

                choice();

                my_graphics.animation = bn::create_sprite_animate_action_forever(my_graphics, 8, bn::sprite_items::fox.tiles_item(), 7, 7, 7);
                jv::Dialog::init("Sorry pal. Wrong paw. Better luck", "next time ;)");
                my_graphics.animation = bn::create_sprite_animate_action_forever(my_graphics, 8, bn::sprite_items::fox.tiles_item(), 0, 1, 0, 2);
                
                Global::Player().look_at({0, -1});
                Global::Player().spend_interact_token();
            }
        }

    }else{
        if(Global::Graphics_Manager().find(graphics_key)){
            Global::Graphics_Manager().erase_sprite(graphics_key);
        }
    }
}

}