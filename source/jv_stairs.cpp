#include "jv_stairs.h"

#include "jv_actors.h"

namespace jv{
bool stairs::climb(){
    bool isOnStairs = Common::Player().rect().intersects(_rect) && bn::keypad::a_pressed();
    if(isOpen && Common::Player().get_state() == Actor::State::NORMAL && isOnStairs){
        return true;
    }else{
        return false;
    }
}
}