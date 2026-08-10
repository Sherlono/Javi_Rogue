#include "jv_cow.h"

#include "jv_math.h"
#include "jv_global.h"
#include "jv_dialog.h"
#include "jv_stairs.h"
#include "jv_tiled_bg.h"

#include "bn_sprite_items_cow.h"

namespace jv{
Cow::Cow(bn::point position):   // Constructor
    NPC(Actor_data::Id::Cow, position)
    {
        //if(is_on_screen()) load_graphics(animation::Id::Walk);
    }

void Cow::update(){
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
                if(!Global::Stairs().isOpen){
                    jv::Dialog::init("Thanks for finding me!", "The stairs are open now!");
                    Global::Stairs().set_open(true);
                    Global::Tiled_Bg().init();
                }else{
                    jv::Dialog::init("Find the stairs! They're open!");
                }
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
