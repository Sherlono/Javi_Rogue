#include "jv_tree.h"
#include "jv_global.h"

namespace jv{
    
void JungleTree::_displace(const bn::fixed speed, bn::fixed_point position){
    // If direction is valid
    if(move_dir != NEUTRAL && move_dir < 9){
        bn::fixed x_offset = 0, y_offset = 0;
        // Move if move_dir not obstructed
        if((move_dir == NORTH || move_dir == NORTHWEST || move_dir == NORTHEAST) && _map_obstacle(NORTH)){          // UP
            bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(move_dir == NORTHWEST || move_dir == NORTHEAST);
            y_offset = -speed*diagonal; 
        }else if((move_dir == SOUTH || move_dir == SOUTHWEST || move_dir == SOUTHEAST) && _map_obstacle(SOUTH)){  // DOWN
            bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(move_dir == SOUTHWEST || move_dir == SOUTHEAST);
            y_offset = speed*diagonal; 
        }
        if((move_dir == WEST || move_dir == NORTHWEST || move_dir == SOUTHWEST) && _map_obstacle(WEST)){  // LEFT
            bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(move_dir == NORTHWEST || move_dir == SOUTHWEST);
            x_offset = - speed*diagonal; 
        }else if((move_dir == EAST || move_dir == NORTHEAST || move_dir == SOUTHEAST) && _map_obstacle(EAST)){ // RIGHT
            bn::fixed diagonal = 1 - ONEMSQRTTWODTWO*(move_dir == NORTHEAST || move_dir == SOUTHEAST);
            x_offset = speed*diagonal; 
        }
        set_position(position.x() + x_offset, position.y() + y_offset); 
    }
}

void JungleTree::update(){
    if(is_on_screen()){
        if(!Global::Graphics_Manager().find(graphics_key)) load_graphics(animation::Id::Walk);
            
        Graphics& my_graphics = Global::Graphics_Manager()[graphics_key];

        if(_idle_time == 0){
            move_dir = Global::Random().get_int(12);
            _idle_time++;
        }else if(_idle_time <= 1*60 + move_dir*2){
            _idle_time++;
        }else{
            _idle_time = 0;
        }

        _displace(bn::fixed(0.3), my_graphics.position());
    }
    else{
        if(Global::Graphics_Manager().find(graphics_key)) Global::Graphics_Manager().erase_sprite(graphics_key);
    }
}

}