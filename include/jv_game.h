#ifndef JV_GAME_H
#define JV_GAME_H

#include "bn_log.h"

#include "jv_environment.h"
#include "jv_actors.h"
#include "jv_interface.h"
#include "bn_regular_bg_items_hud_item.h"

namespace jv::game{

void game_scene(bn::camera_ptr& cam, bn::random* random_ptr){
    // Player init
    const bn::point start_pos (16 + 1*32,16 + 1*32);
    cam.set_position(start_pos.x(), start_pos.y());
    jv::Player cat(start_pos.x(), start_pos.y(), random_ptr);
    
    // Background
    bn::regular_bg_ptr background = bn::regular_bg_items::bg.create_bg(0, 0);
    background.set_camera(cam);
    
    bn::vector<bn::sprite_ptr, MAX_BLOCKS> block_v;

    //jv::LevelMaker::init(map1, cam, block_v);

    int prev_skipped = 0;
    
    bn::unique_ptr<bg_map> bg_map_ptr(new bg_map());
    bn::regular_bg_item bg_item(
                bn::regular_bg_tiles_items::floor_tiles, bn::bg_palette_items::floor_palette, bg_map_ptr->map_item);
    bn::regular_bg_ptr bg = bg_item.create_bg(0, 0);
    bn::regular_bg_map_ptr bg_map = bg.map();
    bg.set_camera(cam);

    bn::array<bool, 64> blockFlip = {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
                                     0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
                                     0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
                                     0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1};
    
    int offset = 0;

    for(int y = 0; y < 8; y++){
        for(int x = 0; x < 8; x++){
            int aux = x + y*8;
            FloorFactory(bn::point(x, y), (aux + offset)/2, blockFlip[aux], bg_map_ptr);
        }
    }
    bg_map.reload_cells_ref();

    
    while(true){
        /*badcat.update(cat);
        cow.update(cat);*/
        cat.update(cam);

        
        if(bn::keypad::a_pressed()){
            offset++;
            for(int y = 0; y < 8; y++){
                for(int x = 0; x < 8; x++){
                        int aux = x + y*8;
                        FloorFactory(bn::point(x, y), (aux + offset)/2, blockFlip[aux], bg_map_ptr);
                }
            }
            bg_map.reload_cells_ref();
        }

        int skipped = bn::core::last_missed_frames();
        if(prev_skipped != skipped && skipped != 0){ BN_LOG("Frames skipped: ", skipped);}
        prev_skipped = skipped;

        //jv::LevelMaker::update(map1, cam, block_v);
        jv::resetcombo();
        bn::core::update();
    }
}
}
#endif