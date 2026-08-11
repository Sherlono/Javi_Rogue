#ifndef JV_FOX_H
#define JV_FOX_H

#include "jv_actors.h"

namespace jv{
class Fox: public NPC{
public:
    ~Fox();
    // Constructor
    Fox(bn::point position);
    
    // Functionality
    void force_move_player();
    void choice();
    void update() override ;
    
private:
};

}

#endif