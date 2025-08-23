#ifndef JV_GAME_H
#define JV_GAME_H

#include "bn_string.h"
#include "bn_vector.h"
#include "bn_colors.h"
#include "bn_sprites.h"
#include "bn_bg_palettes.h"
#include "bn_music_items.h"
#include "bn_music_actions.h"
#include "bn_sprite_palettes.h"
#include "bn_blending_actions.h"
#include "bn_sprite_text_generator.h"
#include "bn_sprite_palette_actions.h"
#include "common_variable_8x8_sprite_font.h"

#include "jv_fog.h"
#include "jv_math.h"
#include "jv_items.h"
#include "jv_actors.h"
#include "jv_stairs.h"
#include "jv_global.h"
#include "jv_tiled_bg.h"
#include "jv_constants.h"
#include "jv_healthbar.h"
#include "jv_interface.h"
#include "jv_projectile.h"
#include "jv_blocks_data.h"
#include "jv_tiled_bg_animate_actions.h"

#include "bn_regular_bg_items_bg.h"
#include "bn_regular_bg_items_intro1.h"
#include "bn_regular_bg_items_intro_card.h"
#include "bn_regular_bg_items_intro_card_bg.h"
#include "bn_regular_bg_items_intro_movie_forest.h"

#include "bn_sprite_items_cursor.h"

#if DEV_ENABLED
    #include "bn_log.h"
    static_assert(DEV_ENABLED, "Log is not enabled");
    #include "jv_debug.h"
#endif

namespace jv::scenes{
class MainGame{
    static constexpr bn::fixed cam_lerp_value = bn::fixed(0.13);
    using NPCs_vector_t = bn::vector<jv::NPC, 1>;
    using enemies_vector_t = bn::vector<jv::Enemy*, MAX_ENEMIES>;
    using items_vector_t = bn::vector<jv::Item*, MAX_ENEMIES>;
    using projectiles_vector_t = bn::vector<jv::Projectile*, MAX_ENEMIES>;
    enum RoomTag {Empty, Small, Tall1, Tall2, Wide1, Wide2, Big1, Big2, V_Corr, H_Corr};

public:
    MainGame(bn::random& r):text_generator(common::variable_8x8_sprite_font),
    randomizer(r),
    _background(bn::regular_bg_items::bg.create_bg(0, 0)),
    _map(((MAX_ROOM_ROWS*7) - 1)*4, ((MAX_ROOM_COLUMNS*7) - 1)*4),
    _tiles(bn::regular_bg_tiles_items::fortress_tiles1,
           bn::bg_palette_items::fortress_palette,
           _map),
    _tiles_items{bn::regular_bg_tiles_items::fortress_tiles1,
                 bn::regular_bg_tiles_items::fortress_tiles2,
                 bn::regular_bg_tiles_items::fortress_tiles3},
    _bg_animation(jv::create_tiled_bg_animate_action_forever(_tiles.background(), 15, _tiles_items, 0, 1, 0, 2)),

    _cam(bn::camera_ptr::create(0, 0)),
    _healthbar(),
    _cat(bn::point(0, 0), _cam, &_v_enemies),
    _stairs(),
    cam_x_target(0),
    cam_y_target(0),
    floor(0), gameover_delay(0),
    next_level(false), game_over(false), Objective(true)
    #if DEV_ENABLED
    ,options{jv::menu_option(&_cat.invulnerable, "Invuln."),
            jv::menu_option(&FullHeal, "Fully heal"),
            jv::menu_option(&Noclip, "Noclip"),
            jv::menu_option(&next_level, "Next level"),
            jv::menu_option(&Die, "Die"),
            jv::menu_option(&Clear, "Clear"),
            jv::menu_option(&NoFog, "No Fog"),}
    #endif    
    {
        bn::music_items::cyberrid.play(0.2);

        text_generator.set_bg_priority(0);
        _background.set_camera(_cam);
        _tiles.set_camera(_cam);

        bn::sprites::set_blending_bottom_enabled(false);
        bn::blending::set_transparency_alpha(0.8);

        jv::Global::initialize(&_cam, &_map, &_cat, &randomizer, &_v_projectiles);
        
        _healthbar.init();
        text_generator.generate(64, -70, "Floor", _txt_sprts);

        #if DEV_ENABLED
            _cat.invulnerable = true;
            FullHeal = false;
            Noclip = false;
            Die = false;
            Clear = false;
            NoFog = false;
            text_generator.generate(-32, -70, "CPU: ", cpu_sprts);
            cpu_sprts[0].set_bg_priority(0);
        #endif

        while(!game_over){
            update();
        }

        bn::sprites::set_blending_bottom_enabled(true);
        bn::music::stop();
    }

private:
    void fade(const bool fadeIn, const unsigned char speed, const bool fademusic){
        bn::fixed progress;
        bn::color black = bn::colors::black;

        if(fadeIn){
            progress = 1.0;
            bn::fixed max;
            for(int i = 0; progress >= 0; i++){
                progress = 1 - bn::fixed(i)/speed;
                max = bn::max(progress, bn::fixed(0));
                bn::sprite_palettes::set_fade(black, max);
                bn::bg_palettes::set_fade(black, max);
                bn::core::update();
            }
        }else{
            bn::fixed volume, volume_decrement;
            if(fademusic && bn::music::playing()){
                volume_decrement = bn::music::volume()/speed;
            }
            progress = 0.0;
            bn::fixed min;

            for(int i = 0; progress <= 1; i++){
                progress = bn::fixed(i)/speed;
                min = bn::min(progress, bn::fixed(1));
                bn::sprite_palettes::set_fade(black, min);
                bn::bg_palettes::set_fade(black, min);
                if(fademusic && bn::music::playing()){
                    volume = bn::music::volume();
                    bn::music::set_volume(bn::max(volume - volume_decrement, bn::fixed(0)));
                }
                bn::core::update();
            }
        }
    }

    void clear_objects(){
        for(auto enemy : _v_enemies) delete enemy;
        _v_enemies.clear();
        for(auto item : _v_scene_items) delete item;
        _v_scene_items.clear();
        for(auto projectile : _v_projectiles) delete projectile;
        _v_projectiles.clear();
    }

    void scene_items_update(){
        for(int i = 0; i < _v_scene_items.size(); i++){
            if(!_v_scene_items[i]->gotten()){
                _v_scene_items[i]->update();
            }else{
                delete _v_scene_items[i];
                _v_scene_items.erase(_v_scene_items.begin() + i);
            }
        }
    }

    void enemies_update(){
        for(int i = 0; i < _v_enemies.size(); i++){
            _v_enemies[i]->update();
            Objective = Objective && !_v_enemies[i]->alive();
            if(_v_enemies[i]->get_state() == Actor::State::DEAD){
                int item_check = randomizer.get_int(0, 3);
                if(item_check == 1){
                    _v_scene_items.push_back(new jv::Potion(_v_enemies[i]->x(), _v_enemies[i]->y()));
                }else if(item_check == 2){
                    _v_scene_items.push_back(new jv::Key(_v_enemies[i]->x(), _v_enemies[i]->y()));
                }
                delete _v_enemies[i];
                _v_enemies.erase(_v_enemies.begin() + i);
            }
        }
    }

    void projectiles_update(){
        for(int i = 0; i < _v_projectiles.size(); i++){
            if(_v_projectiles[i]->update()){
                delete _v_projectiles[i];
                _v_projectiles.erase(_v_projectiles.begin() + i);
            }
        }
    }

    void npcs_set_visible(bool visible){
        for(auto npc : _v_npcs) npc.set_visible(visible);
    }
    void enemies_set_visible(bool visible){
        for(auto enemy : _v_enemies) enemy->set_visible(visible);
    }
    void items_set_visible(bool visible){
        for(auto item : _v_scene_items) item->set_visible(visible);
    }
    void projectiles_set_visible(bool visible){
        for(auto projectile : _v_projectiles){ projectile->set_visible(visible);}
    }

    void set_hide_all(bool hide){
        _healthbar.set_visible(!hide);
        _background.set_visible(!hide);
        _tiles.set_visible(!hide);
        _cat.set_visible(!hide);
        npcs_set_visible(!hide);
        enemies_set_visible(!hide);
        items_set_visible(!hide);
        projectiles_set_visible(!hide);
        for(bn::sprite_ptr sprite : _txt_sprts){ sprite.set_visible(!hide);}
    }

    [[nodiscard]] bn::point pop_point(bn::point* points, int& size, const int  index){
        BN_ASSERT(size > 0, "Invalid size: ", size);
        BN_ASSERT(index < size, "Invalid index: ", index);
        bn::point out = bn::point(points[index].x(), points[index].y());

        points[index] = points[size-1];
        size = size - 1;
        return out;
    }

    void random_coords(bn::ivector<bn::point>& points_out){
        bn::point* valid_points = nullptr;
        int pointCount = 0;

        int current_size = 0;
        int width = (_map.width() - 2)>>2, height = (_map.height() - 3)>>2;
        int tileIndex[2][2] = {{0, 0}, {0, 0}};

        // Finding coordinates with floor in them
        for(int y = 2; y < height; y++){
            for(int x = 1; x < width; x++){
                bool walkable_check = true;
                tileIndex[0][0] = x*4 - 1;
                tileIndex[1][0] = x*4 + 2;
                tileIndex[0][1] = y*4 - 1;
                tileIndex[1][1] = y*4 + 2;

                for(int i = 0; i < 4; i++){
                    uint8_t value = _map.cell(tileIndex[i%2][0], tileIndex[i/2][1]);
                    walkable_check = walkable_check && (value > 0 && value < WTILES_COUNT);
                }
                
                if(walkable_check){
                    // Simple dinamically growing array code. Double size when capped
                    pointCount++;
                    if(valid_points == nullptr){
                        valid_points = new bn::point[1];
                        valid_points[0] = bn::point(32*x, 32*y);
                        current_size = 1;
                    }else if(pointCount >= current_size){
                        current_size = current_size*2;
                        bn::point* temp = new bn::point[current_size];
                        for(int i = 0; i < pointCount - 1; i++){
                            temp[i] = valid_points[i];
                        }
                        temp[pointCount - 1] = bn::point(32*x, 32*y);

                        delete[] valid_points;
                        valid_points = temp;
                    }else{
                        valid_points[pointCount - 1] = bn::point(32*x, 32*y);
                    }
                }
            }
        }

        // Storing random valid coordinates
        for(int i = 0; i < points_out.max_size(); i++){
            int pointIndex = randomizer.get_int(0, pointCount);
            points_out.push_back(pop_point(valid_points, pointCount, pointIndex));
        }
        delete[] valid_points;
    }

    void BlockFactory(const bn::point top_left, const uint8_t option, const bool blockFlip){
        const int  block_index = (option < BLOCK_TOTAL) ? option : 0;
        _map.insert_data(4, 4, (uint8_t*)jv::blocks::data[block_index], top_left, blockFlip);
    }

    bn::point InsertRoom(const bn::point top_left, const uint8_t option){
        bn::point target;   // Target Map cell to insert block
        switch(option){
            // Rooms
            case Small:{
                const uint8_t width = 7, height = 7;
                constexpr uint16_t size = width*height;

                uint8_t blockArr[size] = {
                        0,18,15,15,15,18, 0,
                        19,16, 9, 9, 9,16,19,
                        20, 8, 2, 2, 2, 8,20,
                        20, 1, 1, 1, 1, 1,20,
                        20, 1, 1, 1, 1, 1,20,
                        22, 6, 5, 5, 5, 6,22,
                        0,26,25,25,25,26, 0 };
                bool flipArr[size] = {
                        0, 0, 0, 0, 0, 1, 0,
                        0, 0, 0, 0, 0, 1, 1,
                        0, 0, 0, 0, 0, 1, 1,
                        0, 0, 0, 0, 0, 0, 1,
                        0, 0, 0, 0, 0, 0, 1,
                        0, 0, 0, 0, 0, 1, 1,
                        0, 0, 0, 0, 0, 1, 0 };
                
                for(int y = 0; y < height; y++){
                    for(int x = 0; x < width; x++){
                        int index = x + y*width;
                        target.set_x((x + top_left.x()*7)*4 - 2);
                        target.set_y((y + top_left.y()*7)*4 - 2);
                        BlockFactory(target, blockArr[index], flipArr[index]);
                    }
                }
            
                _fog.create_room(bn::rect(-16 + (top_left.x()*224 + 1*112), -16 + (top_left.y()*224 + 1*112),
                                        (width - 1)*32, (height - 1)*32 + 16));
                
                return bn::point(1, 1);
            }
            case Tall1:{
                const uint8_t width = 7, height = 14;
                constexpr uint16_t size = width*height;

                uint8_t blockArr[size] = {
                        0,18,15,15,15,18, 0,
                        19,16, 9, 9, 9,16,19,
                        20, 8, 2, 2, 2, 8,20,
                        20, 1, 1, 1, 1, 1,20,
                        20, 1, 1, 1, 1, 1,20,
                        20, 1, 1, 1, 1, 1,20,
                        20, 1, 1, 1, 1, 1,20,
                        20, 1, 1, 1, 1, 1,20,
                        20, 1, 1, 1, 1, 1,20,
                        20, 1, 1, 1, 1, 1,20,
                        20, 1, 1, 1, 1, 1,20,
                        20, 1, 1, 1, 1, 1,20,
                        22, 6, 5, 5, 5, 6,22,
                        0,26,25,25,25,26, 0 };
                bool flipArr[size] = {
                        0, 0, 0, 0, 0, 1, 0,
                        0, 0, 0, 0, 0, 1, 1,
                        0, 0, 0, 0, 0, 1, 1,
                        0, 0, 0, 0, 0, 0, 1,
                        0, 0, 0, 0, 0, 0, 1,
                        0, 0, 0, 0, 0, 0, 1,
                        0, 0, 0, 0, 0, 0, 1,
                        0, 0, 0, 0, 0, 0, 1,
                        0, 0, 0, 0, 0, 0, 1,
                        0, 0, 0, 0, 0, 0, 1,
                        0, 0, 0, 0, 0, 0, 1,
                        0, 0, 0, 0, 0, 0, 1,
                        0, 0, 0, 0, 0, 1, 1,
                        0, 0, 0, 0, 0, 1, 0 };
                
                for(int y = 0; y < height; y++){
                    for(int x = 0; x < width; x++){
                        int index = x + y*width;
                        target.set_x((x + top_left.x()*7)*4 - 2);
                        target.set_y((y + top_left.y()*7)*4 - 2);
                        BlockFactory(target, blockArr[index], flipArr[index]);
                    }
                }
            
                _fog.create_room(bn::rect(-16 + (top_left.x()*224 + 1*112), -16 + (top_left.y()*224 + 2*112),
                                        (width - 1)*32, (height - 1)*32 + 16));
                
                return bn::point(1, 2);
            }
            case Tall2:{
                const uint8_t width = 7, height = 14;
                constexpr uint16_t size = width*height;

                uint8_t blockArr[size] = {
                        0,18,15,15,15,18, 0,
                        19,16, 9, 9, 9,16,19,
                        20, 8, 2, 2, 2, 8,20,
                        20, 1, 1, 1, 1, 1,20,
                        20, 1, 1, 1, 1, 1,20,
                        20, 1, 1, 1, 1, 1,20,
                        20,29,29,29,29,29,20,
                        20, 9, 9, 9, 9, 9,20,
                        20, 2, 2, 2, 2, 2,20,
                        20, 1, 1, 1, 1, 1,20,
                        20, 1, 1, 1, 1, 1,20,
                        20, 1, 1, 1, 1, 1,20,
                        22, 6, 5, 5, 5, 6,22,
                        0,26,25,25,25,26, 0 };
                bool flipArr[size] = {
                        0, 0, 0, 0, 0, 1, 0,
                        0, 0, 0, 0, 0, 1, 1,
                        0, 0, 0, 0, 0, 1, 1,
                        0, 0, 0, 0, 0, 0, 1,
                        0, 0, 0, 0, 0, 0, 1,
                        0, 0, 0, 0, 0, 0, 1,
                        0, 0, 0, 0, 0, 0, 1,
                        0, 0, 0, 0, 0, 0, 1,
                        0, 0, 0, 0, 0, 0, 1,
                        0, 0, 0, 0, 0, 0, 1,
                        0, 0, 0, 0, 0, 0, 1,
                        0, 0, 0, 0, 0, 0, 1,
                        0, 0, 0, 0, 0, 1, 1,
                        0, 0, 0, 0, 0, 1, 0 };
                
                for(int y = 0; y < height; y++){
                    for(int x = 0; x < width; x++){
                        int index = x + y*width;
                        target.set_x((x + top_left.x()*7)*4 - 2);
                        target.set_y((y + top_left.y()*7)*4 - 2);
                        BlockFactory(target, blockArr[index], flipArr[index]);
                    }
                }
            
                _fog.create_room(bn::rect(-16 + (top_left.x()*224 + 1*112), -16 + (top_left.y()*224 + 1*112),
                                        (width - 1)*32, ((height>>1) - 1)*32 + 16));
                _fog.create_room(bn::rect(-16 + (top_left.x()*224 + 1*112), -16 + (top_left.y()*224 + 3*112),
                                        (width - 1)*32, ((height>>1) - 1)*32 + 16));
                
                return bn::point(1, 2);
            }
            case Wide1:{
                const uint8_t width = 14, height = 7;
                constexpr uint16_t size = width*height;
                
                uint8_t blockArr[size] = {
                        0,18,15,15,15,15,15,15,15,15,15,15,18, 0,
                        19,16, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,16,19,
                        20, 8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 8,20,
                        20, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,20,
                        20, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,20,
                        22, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6,22,
                        0,26,25,25,25,25,25,25,25,25,25,25,26, 0 };
                bool flipArr[size] = {
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 };
                
                for(int y = 0; y < height; y++){
                    for(int x = 0; x < width; x++){
                        int index = x + y*width;
                        target.set_x((x + top_left.x()*7)*4 - 2);
                        target.set_y((y + top_left.y()*7)*4 - 2);
                        BlockFactory(target, blockArr[index], flipArr[index]);
                    }
                }
            
                _fog.create_room(bn::rect(-16 + (top_left.x()*224 + 2*112), -16 + (top_left.y()*224 + 1*112),
                                        (width - 1)*32, (height - 1)*32 + 16));
                
                return bn::point(2, 1);
            }
            case Wide2:{
                const uint8_t width = 14, height = 7;
                constexpr uint16_t size = width*height;
                
                uint8_t blockArr[size] = {
                        0,18,15,15,15,15,18, 0,18,15,15,15,18, 0,
                        19,16, 9, 9, 9, 9,16,31,16, 9, 9, 9,16,19,
                        20, 8, 2, 2, 2, 2, 8,28, 8, 2, 2, 2, 8,20,
                        20, 1, 1, 1, 1, 1, 1,28, 1, 1, 1, 1, 1,20,
                        20, 1, 1, 1, 1, 1, 1,28, 1, 1, 1, 1, 1,20,
                        22, 6, 5, 5, 5, 5, 6,30, 6, 5, 5, 5, 6,22,
                        0,26,25,25,25,25,26, 0,26,25,25,25,26, 0 };
                bool flipArr[size] = {
                        0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0,
                        0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1,
                        0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                        0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1,
                        0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0 };
                
                for(int y = 0; y < height; y++){
                    for(int x = 0; x < width; x++){
                        int index = x + y*width;
                        target.set_x((x + top_left.x()*7)*4 - 2);
                        target.set_y((y + top_left.y()*7)*4 - 2);
                        BlockFactory(target, blockArr[index], flipArr[index]);
                    }
                }
            
                _fog.create_room(bn::rect(0 + (top_left.x()*224 + 1*112), -16 + (top_left.y()*224 + 1*112),
                                        ((width>>1) )*32, (height - 1)*32 + 16));
                _fog.create_room(bn::rect(-16 + (top_left.x()*224 + 3*112), -16 + (top_left.y()*224 + 1*112),
                                        ((width>>1) - 1)*32, (height - 1)*32 + 16));
                
                return bn::point(2, 1);
            }
            case Big1:{
                const uint8_t width = 14, height = 14;
                constexpr uint16_t size = width*height;

                uint8_t blockArr[size] = {
                        0,18,15,15,15,15,15,15,15,15,15,15,18, 0,
                        19,16, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,16,19,
                        20, 8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 8,20,
                        20, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,20,
                        20, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,20,
                        20, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,20,
                        20, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,20,
                        20, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,20,
                        20, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,20,
                        20, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,20,
                        20, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,20,
                        20, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,20,
                        22, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6,22,
                        0,26,25,25,25,25,25,25,25,25,25,25,26, 0 };
                bool flipArr[size] = {
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 };
                
                for(int y = 0; y < height; y++){
                    for(int x = 0; x < width; x++){
                        int index = x + y*width;
                        target.set_x((x + top_left.x()*7)*4 - 2);
                        target.set_y((y + top_left.y()*7)*4 - 2);
                        BlockFactory(target, blockArr[index], flipArr[index]);
                    }
                }

                _fog.create_room(bn::rect(-16 + (top_left.x()*224 + 2*112), -16 + (top_left.y()*224 + 2*112),
                                        (width - 1)*32, (height - 1)*32 + 16));
                
                return bn::point(2, 2);
            }
            case Big2:{
                const uint8_t width = 14, height = 14;
                constexpr uint16_t size = width*height;

                uint8_t blockArr[size] = {
                        0,18,15,15,15,15,15,15,36,15,15,15,18, 0,
                        19,16, 9, 9, 9, 9, 9, 9,28, 9, 9, 9,16,19,
                        20, 8, 2, 2, 2, 2, 2, 2,28, 2, 2, 2, 8,20,
                        20, 1, 1, 1, 1, 1, 1, 1,28, 1, 1, 1, 1,20,
                        20, 1, 1, 1, 1, 1, 1, 1,28, 1, 1, 1, 1,20,
                        20, 1, 1, 1, 1,28, 1, 1,28, 1, 1, 1, 1,20,
                        20, 1, 1, 1, 1,28, 1, 1,28, 1, 1, 1, 1,20,
                        20, 1, 1, 1, 1,28, 1, 1,24, 1, 1, 1, 1,20,
                        20, 1, 1, 1, 1,28, 1, 1, 9, 1, 1, 1, 1,20,
                        20, 1, 1, 1, 1,28, 1, 1, 2, 1, 1, 1, 1,20,
                        20, 1, 1, 1, 1,28, 1, 1, 1, 1, 1, 1, 1,20,
                        20, 1, 1, 1, 1,28, 1, 1, 1, 1, 1, 1, 1,20,
                        22, 6, 5, 5, 5,28, 5, 5, 5, 5, 5, 5, 6,22,
                        0,26,25,25,25, 0,25,25,25,25,25,25,26, 0 };
                bool flipArr[size] = {
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                        0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                        0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0 };
                
                for(int y = 0; y < height; y++){
                    for(int x = 0; x < width; x++){
                        int index = x + y*width;
                        target.set_x((x + top_left.x()*7)*4 - 2);
                        target.set_y((y + top_left.y()*7)*4 - 2);
                        BlockFactory(target, blockArr[index], flipArr[index]);
                    }
                }

                _fog.create_room(bn::rect(-16 + (top_left.x()*224 + 2*112), -16 + (top_left.y()*224 + 2*112),
                                        (width - 1)*32, (height - 1)*32 + 16));
                
                return bn::point(2, 2);
            }
            // Corridors
            case V_Corr:{
                const uint8_t width = 3, height = 5;
                constexpr uint16_t size = width*height;

                uint8_t blockArr[size] = {
                        0, 1, 0,
                        27, 1,27,
                        0, 1, 0,
                        0, 1, 0,
                        0, 1, 0,};

                for(int y = 0; y < height; y++){
                    for(int x = 0; x < width; x++){
                        int index = x + y*width;
                        target.set_x((x + top_left.x())*4 - 2);
                        target.set_y((y + top_left.y())*4 - 2);
                        BlockFactory(target, blockArr[index], index == 5);
                    }
                }
            
                uint8_t aux_blockArr[4] = {
                        81,82,
                        83,84,};
                for(int x = 0; x < 2; x++){
                    target.set_x(top_left.x()*4 + 6*(x == 1));
                    target.set_y((2 + top_left.y())*4 - 2);
                    _map.insert_data(2, 2, aux_blockArr, target, (x == 1));
                }
                break;
            }
            case H_Corr:{
                const uint8_t width = 4, height = 4;
                constexpr uint16_t size = width*height;

                uint8_t blockArr[size] = {
                        0,15,15, 0,
                        0, 9, 9, 0,
                        0, 4, 4, 0,
                        0,25,25, 0 };

                for(int y = 0; y < height; y++){
                    for(int x = 0; x < width; x++){
                        int index = x + y*width;
                        target.set_x((x + top_left.x())*4 - 2);
                        target.set_y((y + top_left.y())*4 - 4);
                        BlockFactory(target, blockArr[index], false);
                    }
                }
                break;
            }
            default:{
                BN_ASSERT(false, "Invalid room option.", option);
                break;
            }
        }
        return bn::point(0, 0);
    }

    void Generate(){
        using rooms_type = bn::vector<uint8_t, ROOM_PREFAB_COUNT>;

        _map.reset();
        bool* zData = new bool[zone_x*zone_y];
        Zone zone(zone_x, zone_y, zData);
        _fog.reset();

        rooms_type validRooms;
        bn::point top_left(0, 0);
        uint8_t emptyCount = 0;
        
        // Creating rooms
        for(int y = 0; y < zone._height; y++){
            for(int x = 0; x < zone._width; x++){
                if(zone.cell(x, y) == true){ continue;}
                
                // Valid room selection
                validRooms.clear();
                
                /*if(emptyCount < 2){
                    bool Margin = !(x > 0 && x + 1 < zone._width && y > 0 && y + 1 < zone._height);
                    bool Corners = zone.cell(x+1, y+1) && zone.cell(x+1, y-1) && zone.cell(x-1, y+1) && zone.cell(x-1, y-1);

                    if(emptyCount == 0 || Margin || (!Margin && Corners)){
                        validRooms.push_back(Empty);
                    }
                }*/

                validRooms.push_back(Small);
                if(y + 1 < zone._height){
                    validRooms.push_back(Tall1);
                    if((zone.cell(x, y-1) || zone.cell(x-1, y) || zone.cell(x+1, y)) && (zone.cell(x-1, y+1) || zone.cell(x+1, y+1))){
                        validRooms.push_back(Tall2);
                    }
                }
                if(x+y == 0 || (x + 1 < zone._width && !zone.cell(x+1, y))){
                    validRooms.push_back(Wide1);
                    if((zone.cell(x-1, y) || zone.cell(x, y-1)) && (zone.cell(x+1, y-1) || zone.cell(x+1, y+1))){
                        validRooms.push_back(Wide2);
                    }
                }
                if(x+y == 0 || ((y + 1 < zone._height && x + 1 < zone._width) && !zone.cell(x+1, y) && !zone.cell(x+1, y+1))){
                    //validRooms.push_back(Big1);
                    validRooms.push_back(Big2);
                }

                uint8_t selectedRoom = validRooms[randomizer.get_int(0, validRooms.size())];
                if(selectedRoom == Empty){
                    emptyCount++;
                }

                top_left.set_x(x);
                top_left.set_y(y);

                bn::point occupied = InsertRoom(top_left, selectedRoom);

                // Sectors update
                for(int row = y; row < y + occupied.y(); row++){
                    for(int column = x; column < x + occupied.x(); column++){
                        int index = column + (row*zone._width);
                        zone.data[index] = true;
                    }
                }

            }
        }

        // Vertical corridors
        for(int y = 0; y < zone._height - 1; y++){
            for(int x = 0; x < zone._width; x++){
                int next_cell_x = (2 + x*7)*4, next_cell_y = (7 + y*7)*4 + 1, halfway_cell_y = (6 + y*7)*4 + 1;
                // Cell not occupied   // No room exists in the next cell.        Something between current and next cell
                if(!zone.cell(x, y) || !_map.cell(next_cell_x, next_cell_y) || _map.cell(next_cell_x, halfway_cell_y)){ continue;}
                InsertRoom(bn::point(2 + x*7, 5 + y*7), V_Corr);
            }
        }
        
        // Horizontal corridors
        uint8_t cornerFix[4] =
                {77, 82,
                78, 84,};

        for(int y = 0; y < zone._height; y++){
            for(int x = 0; x < zone._width - 1; x++){
                int next_cell_x = (7 + x*7)*4 + 1, next_cell_y = (2 + y*7)*4, halfway_cell_x = (6 + x*7)*4 + 1;
                // Cell not occupied   // No room exists in the next cell.        Something between current and next cell
                if(!zone.cell(x, y) || !_map.cell(next_cell_x, next_cell_y) || _map.cell(halfway_cell_x, next_cell_y)){ continue;}
                InsertRoom(bn::point(5 + x*7, 2 + y*7), H_Corr);

                if(_map.cell(22 + x*28, 18 + y*28) == 82){
                    _map.insert_data(2, 2, cornerFix, bn::point(22 + x*28, 16 + y*28), true);
                }
                if(_map.cell(29 + x*28, 18 + y*28) == 82){
                    _map.insert_data(2, 2, cornerFix, bn::point(28 + x*28, 16 + y*28));
                }
            }
        }
        
        jv::Interface::Log_zone_layout(zone);
        delete[] zData;
    }

    void Populate(){
        const uint8_t pointsSize = 3 + MAX_ENEMIES; // positions for Player, npc, stairs and enemies
        bn::vector<bn::point, pointsSize> v_points;
        random_coords(v_points);
        
        _cat.set_position(v_points[0], 8);
        _cat.reset();
        _cam.set_position(_cat.get_hitbox().x(), _cat.get_hitbox().y() + 4);
        _stairs.set_position(v_points[1]);

        _v_npcs.push_back(jv::NPC(v_points[2], _cam));

        bn::vector<uint8_t, 2> enemyCountList;
        enemyCountList.push_back(randomizer.get_int(MAX_ENEMIES));
        enemyCountList.push_back(randomizer.get_int(MAX_ENEMIES - enemyCountList[0]));
        uint8_t enemyCount[2];
        for(int i = 0; i < 2; i++){
            int index = randomizer.get_int(enemyCountList.size());
            enemyCount[i] = enemyCountList[index];
            enemyCountList.erase(enemyCountList.begin()+index);
        }

        for(int i = 0; i < enemyCount[0]; i++){
            _v_enemies.push_back(new jv::BadCat(v_points[3+i], _cam));
        }
        for(int i = 0; i < enemyCount[1]; i++){
            _v_enemies.push_back(new jv::PaleTongue(v_points[3+enemyCount[0]+i], _cam));
        }
        for(int i = 0; i < MAX_ENEMIES - enemyCount[0] - enemyCount[1]; i++){
            _v_enemies.push_back(new jv::PaleFinger(v_points[3+enemyCount[0]+enemyCount[1]+i], _cam));
        }
        
        jv::Global::update();
    }

    void update(){
        text_generator.generate(94, -70, bn::to_string<3>(floor), _txt_sprts);
        next_level = false;
        gameover_delay = 0;

        // Level generation
        zone_x = 2 + randomizer.get_int(MAX_ROOM_ROWS - 2);
        zone_y = 2 + randomizer.get_int(MAX_ROOM_COLUMNS - 2);
        
        Generate();
        Populate();
        
        // Initialize level visuals
        _tiles.init();
        _fog.update();
        
        #if DEV_ENABLED
        jv::Interface::Log_resources();
        while(cpu_sprts.size() > 1){
            cpu_sprts.erase(cpu_sprts.end() - 1);
        }
        text_generator.generate(-4, -70, bn::to_string<7>(bn::core::last_cpu_usage()), cpu_sprts);
        bn::core::update();
        jv::Interface::Log_skipped_frames();
        #endif

        fade(FADE_IN, fadespeed::MEDIUM, false);

        while(!next_level){
            jv::Global::update();
            _tiles.update();
            _bg_animation.update();
            Objective = true;

            // Player update
            #if DEV_ENABLED
            _cat.update(Noclip);
            #else 
            _cat.update();
            #endif
            
            cam_x_target = lerp(_cam.x(), _cat.get_hitbox().x(), cam_lerp_value);
            cam_y_target = lerp(_cam.y(), _cat.get_hitbox().y() + 4, cam_lerp_value);
            _cam.set_position(cam_x_target, cam_y_target);
            
            _fog.update();
            
            if( _cat.alive()){
                next_level = _stairs.climb();
                scene_items_update();

                // Debug menu
                #if DEV_ENABLED
                if(bn::keypad::select_pressed()){
                    set_hide_all(true);
                    for(bn::sprite_ptr& sprite : cpu_sprts){ sprite.set_visible(false);}
                    jv::Debug::Start(options.data(), options.size());
                    set_hide_all(false);
                    for(bn::sprite_ptr& sprite : cpu_sprts){ sprite.set_visible(true);}
                    
                    if(FullHeal){
                        _cat.heal( _cat.get_maxhp());
                        FullHeal = false;
                    }
                    if(Die){
                        _cat.invulnerable = false;
                        _cat.got_hit( _cat.get_hp(), true);
                    }
                    if(Clear){
                        clear_objects();
                        Clear = false;
                    }
                    if(NoFog){
                        if(_fog.visible()){
                            _fog.set_visible(false);
                        }
                    }else{
                        if(!_fog.visible()){
                            _fog.set_visible(true);
                        }
                    }
                }
                #endif

            }else{  // Death sequence
                if(gameover_delay == 120){
                    game_over = true;
                    break;
                }
                gameover_delay++;
            }

            enemies_update();
            projectiles_update();
            _healthbar.update();
            
            for(int i = 0; i < _v_npcs.size(); i++){ _v_npcs[i].update(_stairs, _tiles, Objective);}

            #if DEV_ENABLED
            while(cpu_sprts.size() > 1){
                cpu_sprts.erase(cpu_sprts.end() - 1);
            }
            text_generator.generate(-4, -70, bn::to_string<7>(bn::core::last_cpu_usage()), cpu_sprts);
            jv::Interface::Log_skipped_frames();
            #endif

            jv::Interface::resetcombo();
            bn::core::update();
        }

        floor--;
        
        const bool FADE_MUSIC = ! _cat.alive();
        int fade_speed =  _cat.alive() ? fadespeed::MEDIUM : fadespeed::SLOW;
        fade(FADE_OUT, fade_speed, FADE_MUSIC);
        
        // Flush and reset objects
        clear_objects();
        _v_npcs.clear();
        _stairs.set_open(false);
        _fog.reset();
        _txt_sprts.erase(_txt_sprts.begin() + 1);

        #if DEV_ENABLED
        while(cpu_sprts.size() > 1){
            cpu_sprts.erase(cpu_sprts.end() - 1);
        }
        #endif
        
    }

    bn::sprite_text_generator text_generator;
    bn::random& randomizer;

    bn::regular_bg_ptr _background;
    GameMap _map;
    jv::tiled_bg _tiles;

    bn::regular_bg_tiles_item _tiles_items[3];
    jv::tiled_bg_animate_action<4> _bg_animation;

    NPCs_vector_t _v_npcs;
    enemies_vector_t _v_enemies;
    items_vector_t _v_scene_items;
    projectiles_vector_t _v_projectiles;

    bn::camera_ptr _cam;
    jv::healthbar _healthbar;
    jv::Fog<MAX_ROOMS> _fog;
    jv::Player _cat;
    jv::stairs _stairs;

    bn::vector<bn::sprite_ptr, 2> _txt_sprts;
    
    bn::fixed cam_x_target, cam_y_target;
    uint8_t zone_x, zone_y;
    int floor, gameover_delay;
    bool next_level, game_over, Objective;
    
    #if DEV_ENABLED
    bn::vector<bn::sprite_ptr, 3> cpu_sprts;
    bn::array<jv::menu_option, 7> options;
    bool FullHeal, Noclip, Die, Clear, NoFog;
    #endif
};

void intro_scene(){
    bn::regular_bg_ptr intro1_bg = bn::regular_bg_items::intro1.create_bg(0, 0);
    
    jv::Interface::fade(FADE_IN, fadespeed::MEDIUM, false);
    for(int i = 0; i < 180; i++) bn::core::update();
    jv::Interface::fade(FADE_OUT, fadespeed::MEDIUM, false);
}

int start_scene(bn::random& randomizer){
    enum {Start_game, Start_credits, Start_intro};
    bn::regular_bg_ptr card = bn::regular_bg_items::intro_card.create_bg(0, 0);
    bn::regular_bg_ptr bg = bn::regular_bg_items::intro_card_bg.create_bg(0, -54);
    
    bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);
    bn::vector<bn::sprite_ptr, 15> menu_sprts;
    bn::vector<bn::sprite_ptr, 30> explain_sprts;
    
    int option = 0, idle = 0;
    int x_offset = -32, y_offset = 46;
    const int  idle_limit = 600;

    bn::sprite_ptr cursor = bn::sprite_items::cursor.create_sprite(-44, y_offset);
    
    {// Configs
        card.set_priority(0);
        cursor.set_bg_priority(1);
        bg.set_priority(3);

        text_generator.set_bg_priority(0);
        text_generator.generate(-96, y_offset - 16, "Select scene", menu_sprts);
        text_generator.generate(-110, y_offset,     "Start game", menu_sprts);
        
        #if DEV_ENABLED
        text_generator.generate(64, -70, "Dev. Mode", menu_sprts);
        text_generator.generate(-110, y_offset + 8, "Block test", menu_sprts);
        text_generator.generate(-110, y_offset + 16,"Tile test", menu_sprts);
        #else
        bn::string<16> line = "V ";
        line.append(Version);
        text_generator.generate(74, -70, line, menu_sprts);
        text_generator.generate(-110, y_offset + 8, "Credits", menu_sprts);
        #endif

        y_offset = 40;
    }

    #if DEV_ENABLED
    bn::string_view explain_text[3][5] = {
        {"", "A: Interact", "B: Attack", "L: Log tile", "SELECT: Debug menu"},
        {"A: Select tile", "L: Copy tile", "R: Paste tile", "SELECT: Toggle index", "START: Print to log"},
        {"", "L: Next highlighted tile", "R: Prev. highlighted tile", "SELECT: Toggle index"}
    };
    #else
    bn::string_view explain_text[3][5] = {
        {"", "A: Interact", "B: Attack"}
    };
    #endif

    for(int i = 0; i < 5; i++){
        text_generator.generate(x_offset, y_offset + i*8, explain_text[option][i], explain_sprts);
    }

    // Fade in
    bn::fixed fadeProgress = 1.0, scrollSpeed = 0.25, start_y = -48, end_y = start_y + 96;
    for(int i = 0; fadeProgress >= 0; i++){
        fadeProgress = 1 - bn::fixed(i)/fadespeed::MEDIUM;
        bn::sprite_palettes::set_fade(bn::colors::black, bn::max(fadeProgress, bn::fixed(0)));
        bn::bg_palettes::set_fade(bn::colors::black, bn::max(fadeProgress, bn::fixed(0)));
        bg.set_y(bg.y() + scrollSpeed);
        if(bg.y() == end_y){ bg.set_y(start_y);}
        bn::core::update();
    }

    // Selecting a scene
    while(!bn::keypad::a_pressed()){
        #if DEV_ENABLED
        if(bn::keypad::down_pressed() && option < 2){
            option++;
            cursor.set_y(cursor.y() + 8);
            explain_sprts.clear();
            for(int i = 0; i < 5; i++){
                text_generator.generate(x_offset, y_offset + i*8, explain_text[int(option)][i], explain_sprts);
            }
        }else if(bn::keypad::up_pressed() && option > 0){
            option--;
            cursor.set_y(cursor.y() - 8);
            explain_sprts.clear();
            for(int i = 0; i < 5; i++){
                text_generator.generate(x_offset, y_offset + i*8, explain_text[int(option)][i], explain_sprts);
            }
        }
        #else
        if(bn::keypad::down_pressed() && option < 1){
            option++;
            cursor.set_y(cursor.y() + 8);
            explain_sprts.clear();
            for(int i = 0; i < 5; i++){
                text_generator.generate(x_offset, y_offset + i*8, explain_text[int(option)][i], explain_sprts);
            }
        }else if(bn::keypad::up_pressed() && option > 0){
            option--;
            cursor.set_y(cursor.y() - 8);
            explain_sprts.clear();
            for(int i = 0; i < 5; i++){
                text_generator.generate(x_offset, y_offset + i*8, explain_text[int(option)][i], explain_sprts);
            }
        }
        #endif

        idle++;
        if(idle == idle_limit) break;

        jv::Interface::resetcombo();
        randomizer.update();
        bg.set_y(bg.y() + scrollSpeed);
        if(bg.y() == end_y){ bg.set_y(start_y);}
        bn::core::update();
    }

    // Fade out
    for(int i = 0; fadeProgress <= 1; i++){
        fadeProgress = bn::fixed(i)/fadespeed::MEDIUM;
        bn::sprite_palettes::set_fade(bn::colors::black, bn::min(fadeProgress, bn::fixed(1)));
        bn::bg_palettes::set_fade(bn::colors::black, bn::min(fadeProgress, bn::fixed(1)));
        bg.set_y(bg.y() + scrollSpeed);
        if(bg.y() == end_y){ bg.set_y(start_y);}
        bn::core::update();
    }
    
    return idle == idle_limit ? Start_intro : option;
}

}

#endif