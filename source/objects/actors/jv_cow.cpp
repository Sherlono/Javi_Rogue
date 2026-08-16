#include "jv_cow.h"

#include "bn_log.h"

#include "jv_math.h"
#include "jv_global.h"
#include "jv_dialog.h"
#include "jv_stairs.h"
#include "jv_tiled_bg.h"

#include "bn_sprite_items_cow.h"

namespace jv{
// Destructor
Cow::~Cow() {}

// Constructor
Cow::Cow(bn::point position):   
    NPC(Actor_data::Id::Cow, position)
    {
        //BN_LOG("Cow Constructor called.");
        if(is_on_screen()) load_graphics(animation::Id::Walk);
    }

void Cow::update(){
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

    }else{
        if(Global::Graphics_Manager().find(graphics_key)){
            Global::Graphics_Manager().erase_sprite(graphics_key);
        }
    }
}

}
