#ifndef JV_INVENTORY_H
#define JV_INVENTORY_H

#include "jv_items.h"
#include "jv_constants.h"

namespace jv{

class Inventory{
public:
    ~Inventory() = default;
    Inventory(): _data{0} {}

    void gain_item(uint8_t const option){
        if(_data[option] < 255){ _data[option]++;}
    }
    void lose_item(uint8_t const option){
        if(_data[option] > 0){ _data[option]--;}
    }

    uint8_t keys_count() const { return _data[0];}

private:
    uint8_t _data[1];
};

}

#endif