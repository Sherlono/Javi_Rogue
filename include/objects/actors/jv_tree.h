#ifndef JV_TREE_H
#define JV_TREE_H

#include "bn_rect.h"
#include "bn_fixed_point.h"

#include "jv_actors.h"

namespace jv{
class JungleTree: public Actor{
public:
    JungleTree(const bn::point position):
        Actor(Actor_data::Tree, position) {}
    void update();
private:
    void _displace(const bn::fixed speed, bn::fixed_point position);
    uint8_t _idle_time = 0, move_dir = 0;
};
}


#endif