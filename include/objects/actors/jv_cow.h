#ifndef JV_COW_H
#define JV_COW_H

#include "jv_actors.h"

namespace jv{
class Cow: public NPC{
public:
    ~Cow() = default;
    // Constructor
    Cow(bn::point position);

    // Functionality
    //void _movement() override {};
    void update() override;
    
private:
};


}

#endif