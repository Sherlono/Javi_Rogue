#ifndef JV_ITEMS_H
#define JV_ITEMS_H

#include "bn_point.h"
#include "bn_vector.h"
#include "bn_optional.h"
#include "bn_sprite_builder.h"

#include "jv_constants.h"
#include "jv_common.h"

#include "bn_sprite_items_potion.h"

namespace jv{

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
    virtual void get_item() = 0;

    void update();

    [[nodiscard]] bool on_screen() const;

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
    Potion(int x, int y);

    void get_item() override;
};

}

#endif