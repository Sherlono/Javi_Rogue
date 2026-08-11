#include "jv_items.h"

#include "bn_sound_items.h"

#include "jv_actors.h"
#include "jv_global.h"


namespace jv{

[[nodiscard]] bool Item::is_on_screen() const {
    const uint8_t halfWidth = 8, halfHeight = 8;
    constexpr uint8_t x_offset = 120 + halfWidth, y_offset = halfHeight + 80;
    bool up = y() > Global::cam_pos().y() - y_offset, down = y() < Global::cam_pos().y() + y_offset;
    bool left = x() > Global::cam_pos().x() - x_offset, right = x() < Global::cam_pos().x() + x_offset;
    return left && right && up && down;
}

void Item::update(){
    if(!_gotten && is_on_screen()){
        if(!_sprite.has_value()){
            bn::sprite_builder builder(bn::sprite_items::scene_items, get_id());
            builder.set_position(x(), y());
            builder.set_camera(Global::Camera());
            builder.set_bg_priority(1);
            
            _sprite = builder.release_build(); 
        }

        if(Global::Player().sprite().y() > _sprite->y() - 4){ _sprite->set_z_order(Global::Player().sprite().z_order() + 1);}
        else{ _sprite->set_z_order(Global::Player().sprite().z_order() - 1);}

        if(Global::Player().get_state() == Actor::State::NORMAL && !Global::Player().is_attacking()){
            if(bn::keypad::a_pressed() && Global::Player().rect().contains(_point) && Global::Player().can_interact()){
                get_item();
                Global::Player().spend_interact_token();
            }
        }

    }else{
        if(_sprite.has_value()){
            _sprite.reset();
        }
    }
}

Key::Key(int x, int y): Item(x, y)
{
    bn::sprite_builder builder(bn::sprite_items::scene_items, get_id());
    builder.set_position(this->x(), this->y());
    builder.set_camera(Global::Camera());
    builder.set_bg_priority(1);
    
    _sprite = builder.release_build();
}

void Key::get_item() {
    bn::sound_items::cure.play(0.5);
    Global::Player().playerInventory.gain_item(Item::IDs::KEY);
    _gotten = true;
}

Potion::Potion(int x, int y): Item(x, y)
{
    bn::sprite_builder builder(bn::sprite_items::scene_items, get_id());
    builder.set_position(this->x(), this->y());
    builder.set_camera(Global::Camera());
    builder.set_bg_priority(1);
    
    _sprite = builder.release_build();
}

void Potion::get_item() {
    bn::sound_items::cure.play(0.5);
    Global::Player().heal(1);
    _gotten = true;
}

}