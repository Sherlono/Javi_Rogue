#ifndef LEVEL_GENERATOR_H
#define LEVEL_GENERATOR_H

#include "jv_global.h"

#if DEV_ENABLED
    #include "bn_log.h"
    #include "bn_string.h"
    static_assert(DEV_ENABLED, "Log is not enabled");
#endif

namespace jv{
class LevelGenerator{
public:
    static void Generate(const int level_width, const int level_height){
        LevelGenerator instance(level_width, level_height);
    }
private:
    LevelGenerator(const int level_width, const int level_height): zone(level_width, level_height)
        {
            generate_rooms();
            generate_corridors();
            
            #if DEV_ENABLED
            jv::Interface::Log_zone_layout(zone);
            BN_LOG("Enemies percentage: ", 100*bn::fixed(Global::Enemies().size())/Global::Enemies().max_size());
            #endif
        }
    
    struct NumPoint {
        int option;
        bn::point top_left;
    };

    enum RoomTag {Empty, Small1, Tall1, Tall2, Tall3, Wide1, Wide2, Big1, Big2};
    enum CorridorTag {V_Corr, H_Corr};
    static constexpr uint8_t UNIQUECOUNT = 4;   // Count of unique entities to place in the level

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
        enum EntityTag {Cat, Cow, Fox, Stairs};
        enemy_ref_t enemies_ref = Global::Enemies();
        NPCs_ref_t npcs_ref = Global::NPCs();
            
        // Populate unique entities
        for(int i = 0; i < entity_checks[1].size(); i++){
            int index = Global::Random().get_int(v_walkBlocks.size());

            switch(entity_checks[1][i]){
                case EntityTag::Cat:
                    Global::Player().reset_at(v_walkBlocks[index]);
                    Global::Camera().set_position(Global::Player().get_hitbox().position());
                    Global::update();
                    spawnEnemies = false;
                    #if DEV_ENABLED
                    BN_LOG("Player was positioned.");
                    #endif
                    break;
                case EntityTag::Cow:
                    npcs_ref.push_back(new jv::Cow(v_walkBlocks[index]));
                    #if DEV_ENABLED
                    BN_LOG("Cow was created.");
                    #endif
                    break;
                case EntityTag::Fox:
                    npcs_ref.push_back(new jv::Fox(v_walkBlocks[index]));
                    #if DEV_ENABLED
                    BN_LOG("Fox was created.");
                    #endif
                    break;
                case EntityTag::Stairs:{
                    Global::Stairs().set_position(v_walkBlocks[index]);
                    #if DEV_ENABLED
                    BN_LOG("Stairs were positioned.");
                    #endif
                    break;
                }
                default:
                    BN_ERROR("Invalid entity tag: ", entity_checks[1][i]);
                    break;
            }
            v_walkBlocks.erase(v_walkBlocks.begin() + index);
            entity_checks[1].erase(entity_checks[1].begin() + i);
        }
        
        // Populate Enemies
        if(spawnEnemies){
            for(int i = 0; i < v_walkBlocks.size(); i++){
                if(Global::Enemies().full()) break;
                int rand = Global::Random().get_int(128);
                switch(Global::environment_id){
                    case Global::Environments::Fortress:{
                        if(rand < 16){
                            enemies_ref.push_back(new jv::Enemy(Actor_data::Id::Bad_Cat, v_walkBlocks[i]));
                            v_walkBlocks.erase(v_walkBlocks.begin() + i);
                        }else if(rand < 24){
                            enemies_ref.push_back(new jv::Enemy(Actor_data::Id::Pale_Finger, v_walkBlocks[i]));
                            v_walkBlocks.erase(v_walkBlocks.begin() + i);
                        }else if(rand < 28){
                            enemies_ref.push_back(new jv::Enemy(Actor_data::Id::Pale_Tongue, v_walkBlocks[i]));
                            v_walkBlocks.erase(v_walkBlocks.begin() + i);
                        }/**/
                        break;
                    }
                    case Global::Environments::Jungle:{
                        if(rand < 16){
                            enemies_ref.push_back(new jv::Enemy(Actor_data::Id::Snakes, v_walkBlocks[i]));
                            v_walkBlocks.erase(v_walkBlocks.begin() + i);
                        }
                        break;
                    }
                    default:{
                        BN_BASIC_ASSERT("Invalid Environment id", Global::environment_id);
                        break;
                    }
                }
            }
        }
    }

    void room_factory(){
        int mapIndex = roomConfig.option - 1;
        
        switch(roomConfig.option){
            case Empty:
                break;
            case Small1:{
                bn::vector<bn::point, 49> v_walkBlocks;
                insert_room(v_walkBlocks);
                populate(v_walkBlocks);

                if(Global::Fog().visible()){
                    Global::Fog().create_room(bn::rect(-16 + (roomConfig.top_left.x()*224 + 1*112),
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

                if(Global::Fog().visible()){
                    Global::Fog().create_room(bn::rect(-16 + (roomConfig.top_left.x()*224 + 1*112),
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

                if(Global::Fog().visible()){
                    Global::Fog().create_room(bn::rect(-16 + (roomConfig.top_left.x()*224 + 1*112),
                                              -16 + (roomConfig.top_left.y()*224 + 1*112),
                                              (prefab_maps::data[mapIndex].width - 1)*32,
                                              ((prefab_maps::data[mapIndex].height>>1) - 1)*32 + 16));
                    Global::Fog().create_room(bn::rect(-16 + (roomConfig.top_left.x()*224 + 1*112),
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

                if(Global::Fog().visible()){
                    Global::Fog().create_room(bn::rect(-16 + (roomConfig.top_left.x()*224 + 1*112),
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

                if(Global::Fog().visible()){
                    Global::Fog().create_room(bn::rect(-16 + (roomConfig.top_left.x()*224 + 2*112),
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

                if(Global::Fog().visible()){
                    Global::Fog().create_room(bn::rect(0 + (roomConfig.top_left.x()*224 + 1*112),
                                              -16 + (roomConfig.top_left.y()*224 + 1*112),
                                              ((prefab_maps::data[mapIndex].width>>1) )*32,
                                              (prefab_maps::data[mapIndex].height - 1)*32 + 16));
                    Global::Fog().create_room(bn::rect(-16 + (roomConfig.top_left.x()*224 + 3*112),
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

                if(Global::Fog().visible()){
                    Global::Fog().create_room(bn::rect(-16 + (roomConfig.top_left.x()*224 + 2*112),
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

                if(Global::Fog().visible()){
                    Global::Fog().create_room(bn::rect(-16 + (roomConfig.top_left.x()*224 + 2*112),
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
                        blockConfig = {blockArr[index], {(x + roomConfig.top_left.x())*4 - 2, (y + roomConfig.top_left.y())*4 - 2}};
                        block_factory(index == 5);
                    }
                }
            
                GameMap::cell_type auxBlockArr[4] = {
                    139, 140,
                    136, 137};

                bool x_equals_1;
                for(int x = 0; x < 2; x++){
                    x_equals_1 = x == 1;
                    bn::point top_left = {roomConfig.top_left.x()*4 + (x_equals_1 ? 6 : 0), (2 + roomConfig.top_left.y())*4 - 2};
                    Global::Map().insert_data(2, 2, auxBlockArr, top_left, x_equals_1);
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
                        blockConfig = {blockArr[index], {(x + 1 + roomConfig.top_left.x())*4 - 2, (y + roomConfig.top_left.y())*4 - 4}};
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
        using rooms_type = bn::vector<uint8_t, jv::prefab_map::ROOM_PREFAB_COUNT>;
        rooms_type validRooms;
        uint8_t emptyCount = 0;

        Global::clear_bg_map();
        Global::Fog().reset();
        
        // Choosing room shape and location
        for(int y = 0; y < zone.height(); y++){
            for(int x = 0; x < zone.width(); x++){
                if(zone.cell(x, y)) continue;
                validRooms.clear();
                
                // Valid room selection
                validRooms.push_back(Small1);

                if(emptyCount < (zone.width()*zone.height())/3){
                    bool Margin = !(x > 0 && x + 1 < zone.width() && y > 0 && y + 1 < zone.height());
                    bool Corners = zone.cell(x + 1, y + 1) && zone.cell(x + 1, y - 1) && zone.cell(x - 1, y + 1) && zone.cell(x - 1, y - 1);

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
                        if((zone.cell(x, y - 1) || zone.cell(x - 1, y) || zone.cell(x + 1, y)) && (zone.cell(x - 1, y + 1) || zone.cell(x + 1, y + 1))){
                            validRooms.push_back(Tall2);
                        }
                    }
                }
                
                roomConfig.top_left = {x, y};
                roomConfig.option = validRooms[Global::Random().get_int(0, validRooms.size())];

                if(roomConfig.option != Empty){
                    v_roomConfigs.push_back(roomConfig);

                    // Sectors update
                    const bn::point occupied = prefab_maps::data[roomConfig.option - 1].zones;
                    for(int row = y; row < y + occupied.y(); row++){
                        for(int column = x; column < x + occupied.x(); column++){
                            zone.set_cell(column, row, roomConfig.option);
                        }
                    }
                }
                else { emptyCount++;}
            }
        }

        // Room generation and population
        entity_checks[0].clear();
        for(uint8_t i = 0; i < UNIQUECOUNT; i++)  entity_checks[0].push_back(i);
        
        for(int k = 0; k < v_roomConfigs.size(); k++){
            for(uint8_t i = 0; i < entity_checks[0].size(); i++){
                const uint8_t entity_index = entity_checks[0][i];
                if(Global::Random().get_int(v_roomConfigs.size() - k) == 0){
                    entity_checks[1].push_back(entity_index);
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
                    roomConfig = NumPoint{V_Corr, bn::point(2 + x*7, 5 + y*7)};
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

                    roomConfig = NumPoint{H_Corr, bn::point(5 + x_times_7, 2 + y_times_7)};
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

    NumPoint roomConfig, blockConfig;
    bn::vector<uint8_t, 4> entity_checks[2]; // Indexes for Player, NPCs, Stairs and other planned unique entities.
    bn::vector<NumPoint, MAX_ROOMS> v_roomConfigs;
    GameMap zone;
};

}

#endif