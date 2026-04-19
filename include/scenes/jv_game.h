#ifndef JV_GAME_H
#define JV_GAME_H

#include "bn_log.h"
#include "bn_span.h"
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

#include "bn_regular_bg_tiles_items_fortress_tiles.h"
#include "bn_regular_bg_tiles_items_fortress_torch.h"
#include "bn_bg_palette_items_fortress_palette.h"

#include "bn_regular_bg_tiles_items_jungle_tiles.h"
#include "bn_bg_palette_items_jungle_palette.h"
#include "bn_regular_bg_tiles_items_jungle_vines.h"

#include "bn_regular_bg_items_bg.h"
#include "bn_regular_bg_items_intro1.h"
#include "bn_regular_bg_items_intro_card.h"
#include "bn_regular_bg_items_intro_card_bg.h"

#include "bn_sprite_items_cursor.h"

#if DEV_ENABLED
    #include "bn_log.h"
    static_assert(DEV_ENABLED, "Log is not enabled");
    #include "jv_debug.h"
#endif

namespace jv {
using NPCs_vector_t = bn::vector<jv::NPC, 1>;
using enemies_vector_t = bn::vector<jv::Enemy*, MAX_ENEMIES>;
using items_vector_t = bn::vector<jv::Item*, MAX_ENEMIES>;
using projectiles_vector_t = bn::vector<jv::Projectile*, MAX_ENEMIES>;

class LevelGenerator{
public:
    static void Generate(jv::stairs* s, jv::iFog* f, jv::NPCs_vector_t* npcs, jv::enemies_vector_t* e, const bn::point level_dimensions){
        LevelGenerator(s, f, npcs, e, level_dimensions);
    }
private:
    LevelGenerator(jv::stairs* s, jv::iFog* f, jv::NPCs_vector_t* npcs, jv::enemies_vector_t* e, const bn::point level_dimensions):
        _stairs(s), _fog(f), _v_npcs(npcs), _v_enemies(e), zone(level_dimensions)
        {
            generate_rooms();
            generate_corridors();
            
            jv::Interface::Log_zone_layout(zone);
            //BN_LOG("Enemies percentage: ", 100*bn::fixed(_v_enemies.size())/_v_enemies.max_size());
        }
    
    struct PointNum {
        int option;
        bn::point top_left;
    };

    enum RoomTag {Empty, Small1, Tall1, Tall2, Tall3, Wide1, Wide2, Big1, Big2};
    enum CorridorTag {V_Corr, H_Corr};
    static constexpr uint8_t UNIQUECOUNT = 3;   // Count of unique entities to place in the level

    void block_factory(const bool blockFlip){
        const int block_index = (blockConfig.option < BLOCK_TOTAL) ? blockConfig.option : 0;
        
        if(blockConfig.option == 1){
            blocks::cell_span_t span = jv::blocks::get_block(block_index);
            bn::array<GameMap::cell_type, 16> arr = {0}; 
            if(Global::Random().get_int(32) < 6){
                for(int i = 0; i < 16; i++) arr[i] = span[i] + 16;
            }else{
                for(int i = 0; i < 16; i++) arr[i] = span[i];
            }
            Global::Map().insert_data(4, 4, arr, blockConfig.top_left);
        }else{
            Global::Map().insert_data(4, 4, jv::blocks::get_block(block_index), blockConfig.top_left, blockFlip);
        }
    }

    void insert_room(bn::ivector<bn::point>& v_walkBlocks){
        int mapIndex = roomConfig.option - 1;
        bn::point target;
        bool flip;

        for(int y = 0; y < prefab_maps::data[mapIndex].height; y++){
            for(int x = 0; x < prefab_maps::data[mapIndex].width; x++){
                const uint16_t index = x + y*prefab_maps::data[mapIndex].width;
                target = {x + roomConfig.top_left.x()*7, y + roomConfig.top_left.y()*7};

                blockConfig.option = prefab_maps::data[mapIndex].cell(index);
                blockConfig.top_left = (target*4) - bn::point(2, 2);
                flip = prefab_maps::data[mapIndex].horizontal_flip(index);

                block_factory(flip);

                if(blockConfig.option == 1) v_walkBlocks.push_back(target*32);
            }
        }
        
    }

    void populate(bn::ivector<bn::point>& v_walkBlocks, bool spawnEnemies = true){
        enum EntityTag {Cat, Cow, Stairs};
        
        // Populate Other entities
        for(int i = 0; i < entity_checks[1].size(); i++){
            int index = Global::Random().get_int(v_walkBlocks.size());

            switch(entity_checks[1][i]){
                case EntityTag::Cat:{
                    Global::Player().set_position(v_walkBlocks[index]);
                    Global::Player().reset();
                    Global::Camera().set_position(Global::Player().get_hitbox().position());
                    Global::update();
                    spawnEnemies = false;
                    break;
                }
                case EntityTag::Cow:{
                    _v_npcs->push_back(jv::NPC(v_walkBlocks[index], Global::Camera()));
                    break;
                }
                case EntityTag::Stairs:{
                    _stairs->set_position(v_walkBlocks[index]);
                    break;
                }
                default:{
                    BN_ERROR("Invalid entity tag: ", entity_checks[1][i]);
                    break;
                }
            }
            v_walkBlocks.erase(v_walkBlocks.begin() + index);
            entity_checks[1].erase(entity_checks[1].begin() + i);
        }
        
        // Populate Enemies
        if(spawnEnemies){
            for(int i = 0; i < v_walkBlocks.size(); i++){
                if(_v_enemies->full()) break;

                int rand = Global::Random().get_int(128);
                if(rand < 16){
                    _v_enemies->push_back(new jv::BadCat(v_walkBlocks[i], Global::Camera()));
                    v_walkBlocks.erase(v_walkBlocks.begin() + i);
                }else if(rand < 24){
                    _v_enemies->push_back(new jv::PaleTongue(v_walkBlocks[i], Global::Camera()));
                    v_walkBlocks.erase(v_walkBlocks.begin() + i);
                }else if(rand < 28){
                    _v_enemies->push_back(new jv::PaleFinger(v_walkBlocks[i], Global::Camera()));
                    v_walkBlocks.erase(v_walkBlocks.begin() + i);
                }
            }
        }
    }

    void room_factory(){
        int mapIndex = roomConfig.option - 1;
        
        switch(roomConfig.option){
            case Empty:{
                break;
            }
            case Small1:{
                bn::vector<bn::point, 49> v_walkBlocks;
                insert_room(v_walkBlocks);
                populate(v_walkBlocks);

                if(_fog->visible()){
                    _fog->create_room(bn::rect(-16 + (roomConfig.top_left.x()*224 + 1*112),
                                              -16 + (roomConfig.top_left.y()*224 + 1*112),
                                              (prefab_maps::data[mapIndex].width - 1)*32,
                                              (prefab_maps::data[mapIndex].height - 1)*32 + 16));
                }
                break;
            }
            case Tall1:{
                bn::vector<bn::point, 49*2> v_walkBlocks;
                insert_room(v_walkBlocks);     
                populate(v_walkBlocks);

                if(_fog->visible()){
                    _fog->create_room(bn::rect(-16 + (roomConfig.top_left.x()*224 + 1*112),
                                              -16 + (roomConfig.top_left.y()*224 + 2*112),
                                              (prefab_maps::data[mapIndex].width - 1)*32,
                                              (prefab_maps::data[mapIndex].height - 1)*32 + 16));
                }
                break;
            }
            case Tall2:{
                bn::vector<bn::point, 49*2> v_walkBlocks;
                insert_room(v_walkBlocks);       
                populate(v_walkBlocks);

                if(_fog->visible()){
                    _fog->create_room(bn::rect(-16 + (roomConfig.top_left.x()*224 + 1*112),
                                              -16 + (roomConfig.top_left.y()*224 + 1*112),
                                              (prefab_maps::data[mapIndex].width - 1)*32,
                                              ((prefab_maps::data[mapIndex].height>>1) - 1)*32 + 16));
                    _fog->create_room(bn::rect(-16 + (roomConfig.top_left.x()*224 + 1*112),
                                              -16 + (roomConfig.top_left.y()*224 + 3*112),
                                              (prefab_maps::data[mapIndex].width - 1)*32,
                                              ((prefab_maps::data[mapIndex].height>>1) - 1)*32 + 16));
                }
                break;
            }
            case Tall3:{
                bn::vector<bn::point, 49*2> v_walkBlocks;
                insert_room(v_walkBlocks);
                populate(v_walkBlocks);

                if(_fog->visible()){
                    _fog->create_room(bn::rect(-16 + (roomConfig.top_left.x()*224 + 1*112),
                                              -16 + (roomConfig.top_left.y()*224 + 2*112),
                                              (prefab_maps::data[mapIndex].width - 1)*32,
                                              (prefab_maps::data[mapIndex].height - 1)*32 + 16));
                }
                break;
            }
            case Wide1:{
                bn::vector<bn::point, 49*2> v_walkBlocks;
                insert_room(v_walkBlocks);
                populate(v_walkBlocks);

                if(_fog->visible()){
                    _fog->create_room(bn::rect(-16 + (roomConfig.top_left.x()*224 + 2*112),
                                              -16 + (roomConfig.top_left.y()*224 + 1*112),
                                              (prefab_maps::data[mapIndex].width - 1)*32,
                                              (prefab_maps::data[mapIndex].height - 1)*32 + 16));
                }
                break;
            }
            case Wide2:{
                bn::vector<bn::point, 49*2> v_walkBlocks;
                insert_room(v_walkBlocks);
                populate(v_walkBlocks);

                if(_fog->visible()){
                    _fog->create_room(bn::rect(0 + (roomConfig.top_left.x()*224 + 1*112),
                                              -16 + (roomConfig.top_left.y()*224 + 1*112),
                                              ((prefab_maps::data[mapIndex].width>>1) )*32,
                                              (prefab_maps::data[mapIndex].height - 1)*32 + 16));
                    _fog->create_room(bn::rect(-16 + (roomConfig.top_left.x()*224 + 3*112),
                                              -16 + (roomConfig.top_left.y()*224 + 1*112),
                                              ((prefab_maps::data[mapIndex].width>>1) - 1)*32,
                                              (prefab_maps::data[mapIndex].height - 1)*32 + 16));
                }
                break;
            }
            case Big1:{
                bn::vector<bn::point, 49*4> v_walkBlocks;
                insert_room(v_walkBlocks);
                populate(v_walkBlocks);

                if(_fog->visible()){
                    _fog->create_room(bn::rect(-16 + (roomConfig.top_left.x()*224 + 2*112),
                                              -16 + (roomConfig.top_left.y()*224 + 2*112),
                                              (prefab_maps::data[mapIndex].width - 1)*32,
                                              (prefab_maps::data[mapIndex].height - 1)*32 + 16));
                }
                break;
            }
            case Big2:{
                bn::vector<bn::point, 49*4> v_walkBlocks;
                insert_room(v_walkBlocks);
                populate(v_walkBlocks);

                if(_fog->visible()){
                    _fog->create_room(bn::rect(-16 + (roomConfig.top_left.x()*224 + 2*112),
                                              -16 + (roomConfig.top_left.y()*224 + 2*112),
                                              (prefab_maps::data[mapIndex].width - 1)*32,
                                              (prefab_maps::data[mapIndex].height - 1)*32 + 16));
                }
                break;
            }
            default:{
                BN_ERROR("Invalid room option: ", roomConfig.option);
                break;
            }
        }
        
    }

    void corridor_factory(){
        switch(roomConfig.option){
            // Corridors
            case V_Corr:{
                const uint8_t width = 3, height = 5;
                constexpr uint16_t size = width*height;

                const uint8_t blockArr[size] = {
                    0, 1, 0,
                    27,1,27,
                    0, 1, 0,
                    0, 1, 0,
                    0, 1, 0,};

                for(int y = 0; y < height; y++){
                    for(int x = 0; x < width; x++){
                        const int index = x + y*width;
                        blockConfig.option = blockArr[index];
                        blockConfig.top_left = {(x + roomConfig.top_left.x())*4 - 2, (y + roomConfig.top_left.y())*4 - 2};
                        block_factory(index == 5);
                    }
                }
            
                GameMap::cell_type auxBlockArr[4] = {
                    139, 140,
                    136, 137};

                bool x_equals_1;
                for(int x = 0; x < 2; x++){
                    x_equals_1 = x == 1;
                    blockConfig.top_left = {roomConfig.top_left.x()*4 + (x_equals_1 ? 6 : 0), (2 + roomConfig.top_left.y())*4 - 2};
                    Global::Map().insert_data(2, 2, auxBlockArr, blockConfig.top_left, x_equals_1);
                }
                break;
            }
            case H_Corr:{
                const uint8_t width = 2, height = 4;
                constexpr uint16_t size = width*height;

                const uint8_t blockArr[size] = {
                    15,15,
                     9, 9,
                     4, 4,
                    25,25,};

                for(int y = 0; y < height; y++){
                    for(int x = 0; x < width; x++){
                        const int index = x + y*width;
                        blockConfig.option = blockArr[index];
                        blockConfig.top_left = {(x + 1 + roomConfig.top_left.x())*4 - 2, (y + roomConfig.top_left.y())*4 - 4};
                        block_factory(false);
                    }
                }
                break;
            }
            default:{
                BN_ERROR("Invalid corridor: ", roomConfig.option);
                break;
            }
        }

    }

    void generate_rooms(){
        using rooms_type = bn::vector<uint8_t, prefab_map::ROOM_PREFAB_COUNT>;
        rooms_type validRooms;
        uint8_t emptyCount = 0;

        Global::Map().reset();
        _fog->reset();
        
        // Choosing room shape and location
        for(int y = 0; y < zone.height(); y++){
            for(int x = 0; x < zone.width(); x++){
                if(zone.cell(x, y) == true) continue;
                validRooms.clear();
                
                // Valid room selection
                validRooms.push_back(Small1);

                if(emptyCount < (zone.width()*zone.height())/3){
                    bool Margin = !(x > 0 && x + 1 < zone.width() && y > 0 && y + 1 < zone.height());
                    bool Corners = zone.cell(x+1, y+1) && zone.cell(x+1, y-1) && zone.cell(x-1, y+1) && zone.cell(x-1, y-1);

                    if(emptyCount == 0 || Margin || (!Margin && Corners)){
                        validRooms.push_back(Empty);
                    }
                }

                if(zone.height() > 1 && zone.width() > 1) [[likely]] {
                    if(x+y == 0 || ((y + 1 < zone.height() && x + 1 < zone.width()) && !zone.cell(x+1, y) && !zone.raw_cell(x+1, y+1))){
                        validRooms.push_back(Big1);
                        validRooms.push_back(Big2);
                    }
                }
                if(zone.width() > 1){
                    if(x+y == 0 || (x + 1 < zone.width() && !zone.cell(x+1, y))){
                        validRooms.push_back(Wide1);
                        if((zone.cell(x-1, y) || zone.cell(x, y-1)) && (zone.cell(x+1, y-1) || zone.cell(x+1, y+1))){
                            validRooms.push_back(Wide2);
                        }
                    }
                }
                if (zone.height() > 1){
                    if(y + 1 < zone.height()){
                        validRooms.push_back(Tall1);
                        validRooms.push_back(Tall3);
                        if((zone.cell(x, y-1) || zone.cell(x-1, y) || zone.cell(x+1, y)) && (zone.cell(x-1, y+1) || zone.cell(x+1, y+1))){
                            validRooms.push_back(Tall2);
                        }
                    }
                }
                
                roomConfig.top_left = {x, y};
                roomConfig.option = validRooms[Global::Random().get_int(0, validRooms.size())];

                if(roomConfig.option != Empty) v_roomConfigs.push_back(roomConfig);
                else emptyCount++;

                // Sectors update
                const bn::point occupied = roomConfig.option != Empty ? prefab_maps::data[roomConfig.option - 1].zones : bn::point(0, 0);
                for(int row = y; row < y + occupied.y(); row++){
                    for(int column = x; column < x + occupied.x(); column++){
                        const int index = column + (row*zone.width());
                        zone.set_cell(index, true);
                    }
                }
                
            }
        }

        // Room generation and population
        entity_checks[0].clear();
        for(uint8_t i = 0; i < UNIQUECOUNT; i++) { entity_checks[0].push_back(i);}
        
        for(int k = 0; k < v_roomConfigs.size(); k++){
            for(uint8_t i = 0; i < entity_checks[0].size(); i++) {
                const uint8_t value = entity_checks[0][i];
                if(Global::Random().get_int(v_roomConfigs.size() - k) == 0){
                    entity_checks[1].push_back(value);
                    entity_checks[0].erase(entity_checks[0].begin() + i);
                }
            }

            roomConfig = v_roomConfigs[k];
            room_factory();
        }
    }

    void generate_corridors(){
        // Vertical corridors
        if(zone.height() > 1) [[likely]] {
            for(int y = 0; y < zone.height() - 1; y++){
                for(int x = 0; x < zone.width(); x++){
                    const int next_cell_x = (2 + x*7)*4, next_cell_y = (7 + y*7)*4 + 1, halfway_cell_y = (6 + y*7)*4 + 1;
                    // Cell not occupied   // No room exists in the next cell.                     Something between current and next cell
                    if(!zone.cell(x, y) || !Global::Map().cell(next_cell_x, next_cell_y) || Global::Map().cell(next_cell_x, halfway_cell_y)) [[unlikely]]{
                        continue;
                    }
                    roomConfig = PointNum{V_Corr, bn::point(2 + x*7, 5 + y*7)};
                    corridor_factory();
                }
            }
        }

        // Horizontal corridors
        if(zone.width() > 1) [[likely]] {
            GameMap::cell_type auxBlockArr[2][4] = {
                {184, 144,
                 183, 147,},
                {149, 140,
                 150, 137,}};

            for(int y = 0; y < zone.height(); y++){
                for(int x = 0; x < zone.width() - 1; x++){
                    uint8_t x_times_7 = x*7, y_times_7 = y*7;
                    int next_cell_x = (7 + x_times_7)*4 + 1, next_cell_y = (2 + y_times_7)*4, halfway_cell_x = (6 + x_times_7)*4 + 1;
                    // Cell not occupied   // No room exists in the next cell.        Something between current and next cell
                    if(!zone.cell(x, y) || !Global::Map().cell(next_cell_x, next_cell_y) || Global::Map().cell(halfway_cell_x, next_cell_y)) [[unlikely]] {
                        continue;
                    }

                    roomConfig = PointNum{H_Corr, bn::point(5 + x_times_7, 2 + y_times_7)};
                    corridor_factory();

                    const int x_times_28 = x*28, y_times_28 = y*28;
                    bn::point checkPoint(22 + x_times_28, 18 + y_times_28);
                    bn::point targetPoint(checkPoint.x(), 16 + y_times_28);

                    bool cellCheck = Global::Map().cell(checkPoint) == 140;
                    if(cellCheck || Global::Map().cell(checkPoint) == 147) Global::Map().insert_data(2, 2, auxBlockArr[cellCheck], targetPoint, true);
                    
                    checkPoint.set_x(29 + x_times_28);
                    targetPoint.set_x(28 + x_times_28);
                    
                    cellCheck = Global::Map().cell(checkPoint) == 140;
                    if(cellCheck || Global::Map().cell(checkPoint) == 147)  Global::Map().insert_data(2, 2, auxBlockArr[cellCheck], targetPoint);
                }
            }
        }
    }

    jv::stairs* _stairs;
    jv::iFog* _fog;
    jv::NPCs_vector_t* _v_npcs;
    jv::enemies_vector_t* _v_enemies;

    PointNum roomConfig, blockConfig;
    bn::vector<uint8_t, 3> entity_checks[2]; // Indexes for Player, Stairs, NPCs and other planned unique entities.
    bn::vector<PointNum, MAX_ROOMS> v_roomConfigs;
    GameMap zone;
};

namespace scenes{
class MainGame{
    static constexpr bn::fixed cam_lerp_value = bn::fixed(0.16);

public:
    MainGame(bn::random& r):
        text_generator(common::variable_8x8_sprite_font),
        randomizer(r),
        _backdrop(bn::regular_bg_items::bg.create_bg(0, 0)),
        _tiled_bg(bn::regular_bg_tiles_items::fortress_tiles,
                  bn::bg_palette_items::fortress_palette,
                  ((MAX_ROOM_ROWS*7) - 1)*4, ((MAX_ROOM_COLUMNS*7) - 1)*4),
        _tiles_item(bn::regular_bg_tiles_items::fortress_torch.tiles_ref()),
        _bg_animation{jv::create_tiled_bg_animate_action_forever(_tiled_bg.tiles(), 15, 89, _tiles_item, 0, 2, 0, 4),
                      jv::create_tiled_bg_animate_action_forever(_tiled_bg.tiles(), 15, 90, _tiles_item, 1, 3, 1, 5)},
        _cam(bn::camera_ptr::create(0, 0)),
        _cat(bn::point(0, 0), _cam, &_v_enemies)
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
            _backdrop.set_camera(_cam);
            _tiled_bg.set_camera(_cam);

            bn::sprites::set_blending_bottom_enabled(false);
            bn::blending::set_transparency_alpha(0.8);
            
            jv::Global::initialize(&_cam, _tiled_bg.game_map_ptr(), &_cat, &randomizer, &_v_projectiles);
            
            _healthbar.init();
            text_generator.generate(64, -70, "Floor", _txt_sprts);

            #if DEV_ENABLED
                _cat.invulnerable = true;
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
    void fade(const bool fadeIn, const uint8_t speed, const bool fademusic){
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
                jv::Interface::resetcombo();
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
                jv::Interface::resetcombo();
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
            //objective = objective && !_v_enemies[i]->alive();
            if(_v_enemies[i]->get_state() == Actor::State::DEAD) [[unlikely]] {
                int item_check = Global::Random().get_int(0, 3);
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

    inline void npcs_set_visible(bool visible){
        for(auto npc : _v_npcs) npc.set_visible(visible);
    }
    inline void enemies_set_visible(bool visible){
        for(auto enemy : _v_enemies) enemy->set_visible(visible);
    }
    inline void items_set_visible(bool visible){
        for(auto item : _v_scene_items) item->set_visible(visible);
    }
    inline void projectiles_set_visible(bool visible){
        for(auto projectile : _v_projectiles){ projectile->set_visible(visible);}
    }

    void set_all_visible(bool visible){
        _healthbar.set_visible(visible);
        _backdrop.set_visible(visible);
        _tiled_bg.set_visible(visible);
        _cat.set_visible(visible);
        npcs_set_visible(visible);
        enemies_set_visible(visible);
        items_set_visible(visible);
        projectiles_set_visible(visible);
        for(bn::sprite_ptr sprite : _txt_sprts){ sprite.set_visible(visible);}

        #if DEV_ENABLED
        for(bn::sprite_ptr& sprite : cpu_sprts){ sprite.set_visible(visible);}
        #endif
    }

    void load_bg_assets(){
        bn::regular_bg_tiles_ptr tiles = _tiled_bg.tiles();
        switch (Global::environment_id){
            case Global::Fortress:{
                tiles.set_tiles_ref(bn::regular_bg_tiles_items::fortress_tiles);
                _tiled_bg.set_palette(bn::bg_palette_items::fortress_palette);
                _tiles_item = bn::regular_bg_tiles_items::fortress_torch.tiles_ref();
                break;
            }
            case Global::Jungle:{
                tiles.set_tiles_ref(bn::regular_bg_tiles_items::jungle_tiles);
                _tiled_bg.set_palette(bn::bg_palette_items::jungle_palette);
                _tiles_item = bn::regular_bg_tiles_items::jungle_vines.tiles_ref();
                break;
            }
            default:{
                BN_ERROR("Invalid Environment id: ", Global::environment_id);
                break;
            }
        }
        _bg_animation[0] = jv::create_tiled_bg_animate_action_forever(_tiled_bg.tiles(), 15, 89, _tiles_item, 0, 2, 0, 4);
        _bg_animation[1] = jv::create_tiled_bg_animate_action_forever(_tiled_bg.tiles(), 15, 90, _tiles_item, 1, 3, 1, 5);
    }

    void update(){
        bn::fixed_point cam_target;
        bn::point prev_cam_pos, prev_cat_pos;
        bool cam_moved;
        uint8_t gameover_delay = 0;

        _fog.set_visible(Global::environment_id == Global::Jungle);
        text_generator.generate(94, -70, bn::to_string<3>(floor), _txt_sprts);
        next_level = false;
        
        LevelGenerator::Generate(&_stairs, &_fog, &_v_npcs, &_v_enemies, bn::point(3 + Global::Random().get_int(MAX_ROOM_COLUMNS - 3),
                                                                                   3 + Global::Random().get_int(MAX_ROOM_ROWS - 3)));

        // Initialize level visuals
        _tiled_bg.init();
        if(_fog.visible()) _fog.update();
        
        #if DEV_ENABLED
        //jv::Interface::Log_resources();
        while(cpu_sprts.size() > 1){ cpu_sprts.erase(cpu_sprts.end() - 1); }
        text_generator.generate(-4, -70, bn::to_string<7>(bn::core::last_cpu_usage()), cpu_sprts);
        bn::core::update();
        jv::Interface::Log_skipped_frames();
        #endif

        fade(FADE_IN, fadespeed::MEDIUM, false);

        while(!next_level){
            prev_cam_pos = jv::Global::cam_pos();
            cam_target = {lerp(_cam.x(), _cat.get_hitbox().x(), cam_lerp_value),
                          lerp(_cam.y(), _cat.get_hitbox().y() + 4, cam_lerp_value)};
            _cam.set_position(cam_target);
            
            jv::Global::update();
            cam_moved = (prev_cam_pos.x() != jv::Global::cam_pos().x()) || (prev_cam_pos.y() != jv::Global::cam_pos().y());

            if(cam_moved) _tiled_bg.update();
            _bg_animation[0].update();
            _bg_animation[1].update();
            
            // Player update
            prev_cat_pos = _cat.position();
            #if DEV_ENABLED
            _cat.update(Noclip);
            #else 
            _cat.update();
            #endif

            if(prev_cat_pos.x() != _cat.x() || prev_cat_pos.y() != _cat.y()){
                _backdrop.set_position(_backdrop.x() + (prev_cat_pos.x() - _cat.x())*bn::fixed(0.15),
                                       _backdrop.y() + (prev_cat_pos.y() - _cat.y())*bn::fixed(0.15));
            }
                                    
            if(_fog.visible() && cam_moved) _fog.update();
            
            if(_cat.alive()) [[likely]] {
                next_level = _stairs.climb();
                scene_items_update();

                // Debug menu
                #if DEV_ENABLED
                if(bn::keypad::select_pressed()) [[unlikely]] {
                    set_all_visible(false);
                    jv::Debug::Start(options.data(), options.size());
                    set_all_visible(true);
                    
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
                    if(Global::environment_id != Global::Fortress){
                        if(NoFog){
                            if(_fog.visible()) _fog.set_visible(false);
                        }else{
                            if(!_fog.visible()) _fog.set_visible(true);
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

            objective = true;
            enemies_update();
            projectiles_update();
            _healthbar.update();
            
            for(int i = 0; i < _v_npcs.size(); i++){
                _v_npcs[i].update(_stairs, _tiled_bg, objective);
                // Other NPC updates
            }

            #if DEV_ENABLED
            while(cpu_sprts.size() > 1){
                cpu_sprts.erase(cpu_sprts.end() - 1);
            }
            text_generator.generate(-4, -70, bn::to_string<7>(bn::core::last_cpu_usage()), cpu_sprts);
            #endif

            jv::Interface::resetcombo();
            bn::core::update();
        }

        const bool FADE_MUSIC = ! _cat.alive();
        int fade_speed =  _cat.alive() ? fadespeed::MEDIUM : fadespeed::SLOW;
        fade(FADE_OUT, fade_speed, FADE_MUSIC);
        
        floor--;
        //Global::environment_id = (bn::abs(floor)/2)%2;
        Global::environment_id = bn::abs(floor)%2;
        load_bg_assets();

        // Flush and reset objects
        clear_objects();
        _v_npcs.clear();
        _fog.reset();
        _txt_sprts.erase(_txt_sprts.begin() + 1);

        #if DEV_ENABLED
        while(cpu_sprts.size() > 1) cpu_sprts.erase(cpu_sprts.end() - 1);
        #endif
    }

    bn::sprite_text_generator text_generator;
    bn::vector<bn::sprite_ptr, 2> _txt_sprts;
    
    bn::random& randomizer;

    bn::regular_bg_ptr _backdrop;
    jv::tiled_bg _tiled_bg;
    jv::tile_span _tiles_item;
    jv::tiled_bg_animate_action<4> _bg_animation[2];

    NPCs_vector_t _v_npcs;
    enemies_vector_t _v_enemies;
    items_vector_t _v_scene_items;
    projectiles_vector_t _v_projectiles;

    bn::camera_ptr _cam;
    jv::healthbar _healthbar;
    jv::Fog<MAX_ROOMS> _fog;
    jv::Player _cat;
    jv::stairs _stairs;

    int floor = 0;
    bool next_level = false, game_over = false, objective = true;
    
    #if DEV_ENABLED
    bn::vector<bn::sprite_ptr, 3> cpu_sprts;
    bn::array<jv::menu_option, 7> options;
    bool FullHeal = false, Noclip = false, Die = false, Clear = false, NoFog = false;
    #endif
};

void intro_scene(){
    bn::regular_bg_ptr intro1_bg = bn::regular_bg_items::intro1.create_bg(0, 0);
    
    jv::Interface::fade(FADE_IN, fadespeed::MEDIUM, false);
    for(int i = 0; i < 180; i++) {
        jv::Interface::resetcombo();
        bn::core::update();
    }
    jv::Interface::fade(FADE_OUT, fadespeed::MEDIUM, false);
}

int start_scene(bn::random& randomizer){
    enum {Start_game, Start_credits, Start_intro};
    bn::regular_bg_ptr card = bn::regular_bg_items::intro_card.create_bg(0, 0);
    bn::regular_bg_ptr bg = bn::regular_bg_items::intro_card_bg.create_bg(0, -54);
    
    bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);
    bn::vector<bn::sprite_ptr, 15> menu_sprts;
    bn::vector<bn::sprite_ptr, 30> explain_sprts;
    
    int option = 0;
    int x_offset = -32, y_offset = 46;
    [[maybe_unused]] int idle = 0;
    [[maybe_unused]] const int  idle_limit = 600;

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
        //text_generator.generate(-110, y_offset + 16,"Tile test", menu_sprts);
        #else
        bn::string<16> line = "V ";
        line.append(Version);
        text_generator.generate(74, -70, line, menu_sprts);
        text_generator.generate(-110, y_offset + 8, "Credits", menu_sprts);
        #endif

        y_offset = 40;
    }

    #if DEV_ENABLED
    bn::string_view explain_text[2][5] = {
        {"", "A: Interact", "B: Attack", "L: Log tile", "SELECT: Debug menu"},
        {"A: Select tile", "L: Copy tile", "R: Paste tile", "SELECT: Toggle index", "START: Print to log"},
        //{"", "L: Next highlighted tile", "R: Prev. highlighted tile", "SELECT: Toggle index"}
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
        jv::Interface::resetcombo();
        bn::core::update();
    }

    // Selecting a scene
    while(!bn::keypad::a_pressed()){
        #if DEV_ENABLED
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

        #if !DEV_ENABLED
        idle++;
        if(idle == idle_limit) break;
        #endif

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
        jv::Interface::resetcombo();
        bn::core::update();
    }
    #if DEV_ENABLED
    return option;
    #else
    return idle == idle_limit ? Start_intro : option;
    #endif
}

}
}

#endif