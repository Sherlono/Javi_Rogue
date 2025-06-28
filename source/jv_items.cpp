#include "jv_items.h"

#include "jv_actors.h"

namespace jv{

[[nodiscard]] bool Item::on_screen() const {
    const uint8_t halfWidth = 8, halfHeight = 8;
    constexpr uint8_t x_offset = 120 + halfWidth, y_offset = halfHeight + 80;
    bool up = y() > jv::Common::Camera().y() - y_offset, down = y() < jv::Common::Camera().y() + y_offset;
    bool left = x() > jv::Common::Camera().x() - x_offset, right = x() < jv::Common::Camera().x() + x_offset;
    return left && right && up && down;
}

void Item::update(){
    if(!_gotten && on_screen()){
        if(!_sprite.has_value()){
            bn::sprite_builder builder(bn::sprite_items::scene_items, get_id());
            builder.set_position(x(), y());
            builder.set_camera(jv::Common::Camera());
            builder.set_bg_priority(1);
            
            _sprite = builder.release_build(); 
        }

        if(jv::Common::Player().sprite().y() > _sprite->y() - 4){ _sprite->set_z_order(jv::Common::Player().sprite().z_order() + 1);}
        else{ _sprite->set_z_order(jv::Common::Player().sprite().z_order() - 1);}

        if(jv::Common::Player().get_state() == Actor::State::NORMAL && !jv::Common::Player().is_attacking()){
            if(bn::keypad::a_pressed() && jv::Common::Player().rect().contains(_point) && jv::Common::Player().can_interact()){
                get_item();
                jv::Common::Player().set_interact_token(false);
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
    builder.set_camera(jv::Common::Camera());
    builder.set_bg_priority(1);
    
    _sprite = builder.release_build();
}

void Key::get_item() {
    BN_LOG("Key get!");
    jv::Common::Player()._inventory.gain_item(Item::IDs::KEY);
    _gotten = true;
}

Potion::Potion(int x, int y): Item(x, y)
{
    bn::sprite_builder builder(bn::sprite_items::scene_items, get_id());
    builder.set_position(this->x(), this->y());
    builder.set_camera(jv::Common::Camera());
    builder.set_bg_priority(1);
    
    _sprite = builder.release_build();
}

void Potion::get_item() {
    jv::Common::Player().heal(1);
    _gotten = true;
}

}