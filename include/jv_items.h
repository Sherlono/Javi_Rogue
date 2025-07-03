#ifndef JV_ITEMS_H
#define JV_ITEMS_H

#include "bn_point.h"
#include "bn_vector.h"
#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_builder.h"

#include "jv_global.h"

#include "bn_sprite_items_scene_items.h"

namespace jv{
class Actor;
class Player;

class Item{
public:
    enum IDs {KEY, POTION};

    virtual ~Item() = default;
    Item(int x, int y): _gotten(false), _point(bn::point(x, y)){}

    // Getters
    [[nodiscard]] int x() const { return _point.x();}
    [[nodiscard]] int y() const { return _point.y();}
    [[nodiscard]] bool gotten() const { return _gotten;}
    [[nodiscard]] virtual uint8_t get_id() const = 0;

    // Setters
    void set_position(bn::point position){
        _point.set_x(position.x());
        _point.set_y(position.y());
        _sprite->set_position(position.x(), position.y());
    }
    void set_visible(bool visible){
        if(_sprite.has_value()){ _sprite->set_visible(visible);}
    }
    
    // Functionality
    virtual void get_item() = 0;

    void update();

    [[nodiscard]] bool on_screen() const;

protected:
    void reset_graphics(){ _sprite.reset();}

    bool _gotten;
    bn::point _point;
    bn::optional<bn::sprite_ptr> _sprite;
};

class Key: public Item{
public:
    ~Key(){ reset_graphics();};
    Key(int x, int y);

    [[nodiscard]] uint8_t get_id() const override { return IDs::KEY;}

    void get_item() override;
};

class Potion: public Item{
public:
    ~Potion(){ reset_graphics();};
    Potion(int x, int y);

    [[nodiscard]] uint8_t get_id() const override { return IDs::POTION;}

    void get_item() override;
};

}

#endif