#ifndef JV_ITEMS_H
#define JV_ITEMS_H

#include "bn_point.h"
#include "bn_vector.h"
#include "bn_optional.h"
#include "bn_sprite_builder.h"

#include "jv_actors.h"
#include "jv_constants.h"

#include "bn_sprite_items_potion.h"

namespace jv{
class Player;

class Item{
public:
    enum IDs {POTION};
    virtual ~Item() = default;
    Item(int x, int y): _gotten(false), _point(bn::point(x, y)){}

    // Getters
    [[nodiscard]] int x() const { return _point.x();}
    [[nodiscard]] int y() const { return _point.y();}
    [[nodiscard]] bool gotten() const { return _gotten;}

    // Setters
    void set_position(bn::point position){
        _point.set_x(position.x());
        _point.set_y(position.y());
        _sprite->set_position(position.x(), position.y());
    }
    
    // Functionality
    virtual void get_item(jv::Player& player) = 0;

    void update(jv::Player& player, bn::camera_ptr cam){
        if(!_gotten && on_screen(cam)){
            if(!_sprite.has_value()){
                bn::sprite_builder builder(bn::sprite_items::potion);
                builder.set_position(x(), y());
                builder.set_camera(cam);
                builder.set_bg_priority(1);
                
                _sprite = builder.release_build(); 
            }

            if(player.sprite().y() > _sprite->y() - 4){ _sprite->set_z_order(player.sprite().z_order() + 1);}
            else{ _sprite->set_z_order(player.sprite().z_order() - 1);}

            if(player.get_state() == Actor::State::NORMAL && !player.is_attacking()){
                if(bn::keypad::a_pressed() && player.rect().contains(_point) && player._interact_token){
                    get_item(player);
                    player._interact_token = false;
                }
            }

        }else{
            if(_sprite.has_value()){
                _sprite.reset();
            }
        }
    }

    [[nodiscard]] inline bool on_screen(bn::camera_ptr& cam) const {
        const uint8_t halfWidth = 8, halfHeight = 8;
        constexpr uint8_t x_offset = 120 + halfWidth, y_offset = halfHeight + 80;
        bool up = y() > cam.y() - y_offset, down = y() < cam.y() + y_offset;
        bool left = x() > cam.x() - x_offset, right = x() < cam.x() + x_offset;
        return left && right && up && down;
    }

protected:
    void reset_graphics(){ _sprite.reset();}

    uint8_t _id;
    bool _gotten;
    bn::point _point;
    bn::optional<bn::sprite_ptr> _sprite;
};

class Potion: public Item{
public:
    ~Potion(){ reset_graphics();};
    Potion(int x, int y, bn::camera_ptr& cam): Item(x, y)
    {
        _id = IDs::POTION;
        bn::sprite_builder builder(bn::sprite_items::potion);
        builder.set_position(this->x(), this->y());
        builder.set_camera(cam);
        builder.set_bg_priority(1);
        
        _sprite = builder.release_build();
    }

    void get_item(jv::Player& player) override{
        player.heal(1);
        _gotten = true;
    }
};

}

#endif