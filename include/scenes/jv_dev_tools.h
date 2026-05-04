#ifndef JV_DEV_TOOLS
#define JV_DEV_TOOLS

#include "bn_log.h"
#include "bn_core.h"
#include "bn_point.h"
#include "bn_string.h"
#include "bn_colors.h"
#include "bn_vector.h"
#include "bn_keypad.h"
#include "bn_camera_ptr.h"
#include "bn_sprite_ptr.h"
#include "bn_bg_palettes.h"
#include "bn_sprite_palettes.h"
#include "bn_sprite_palette_ptr.h"
#include "bn_sprite_text_generator.h"

#include "jv_math.h"
#include "jv_global.h"
#include "jv_tiled_bg.h"
#include "jv_interface.h"
#include "jv_blocks_data.h"

#include "bn_sprite_items_cursor.h"
#include "bn_sprite_items_good_cat.h"
#include "bn_regular_bg_tiles_items_fortress_tiles.h"
#include "bn_bg_palette_items_fortress_palette.h"
#include "bn_regular_bg_tiles_items_jungle_tiles.h"
#include "bn_bg_palette_items_jungle_palette.h"

#include "common_variable_8x8_sprite_font.h"

namespace jv{
namespace Scenes{
class BlocksScene{
public:
    static void Start(){
        Scenes::BlocksScene instance;
    }
private:
    ~BlocksScene() = default;

    BlocksScene():
        cursor(bn::sprite_items::cursor.create_sprite(9, 0)),
        arrowUp(bn::sprite_items::cursor.create_sprite(-1, -8)),
        arrowDown(bn::sprite_items::cursor.create_sprite(0, 8)),
        cam(bn::camera_ptr::create(4, 4)),
        text_generator(common::variable_8x8_sprite_font),
        t_bg(bn::regular_bg_tiles_items::jungle_tiles, bn::bg_palette_items::jungle_palette, mapSize.x()*4, mapSize.y()*4)
        {
            configure();
            generate_numbers();

            while(!exit()){
                update();
            }

            bn::bg_palettes::set_transparent_color(bn::color(0, 0, 0));
            Global::reset();
        }

    void Log_block_data(){
        for(int block_y = 0; block_y < 7; block_y++){
            for(int block_x = 0; block_x < 6; block_x++){
                if(block_x + block_y*6 >= BLOCK_TOTAL) break; 

                const int start_x = block_x*8, start_y = block_y*4;
                BN_LOG("//block ", block_x + block_y*6);
                for(int y = start_y; y < start_y + 4; y++){
                    bn::string_view line = "";
                    for(int x = start_x; x < start_x + 4; x++){
                        int num = Global::Map().raw_cell(x, y);
                        //if(num >= 17) num += 16;
                        //if(num >= 41 && num <= 118) num += 32;
                        //else if(num >= 119 && num <= 150) num -= (119 - 41);
                        line = line + bn::to_string<32>(num) + ", ";
                    }
                    BN_LOG(line);
                }
            }
        }
    }

    void GenerateDevLevel(){
        Global::Map().clear();

        for(int y = 0; y < height; y++){
            for(int x = 0; x < width; x++){
                int index = x + y*width;

                const int  block_index = (index>>1 < BLOCK_TOTAL) ? index>>1 : 0;
                Global::Map().insert_data(4, 4, jv::blocks::get_block(block_index), bn::point(x*4, y*4), index%2);
            }
        }
    }

    void configure(){
        bn::bg_palettes::set_transparent_color(bn::color(31, 0, 31));
        bn::sprite_palettes::set_fade(bn::colors::black, bn::fixed(0.0));
        bn::bg_palettes::set_fade(bn::colors::black, bn::fixed(0.0));
        
        t_bg.set_priority(3);
        cursor.set_bg_priority(1);
        arrowDown.set_bg_priority(1);
        arrowDown.set_rotation_angle(90);
        arrowDown.set_visible(false);
        arrowUp.set_bg_priority(1);
        arrowUp.set_rotation_angle(270);
        arrowUp.set_visible(false);

        t_bg.set_camera(cam);
        text_generator.set_camera(cam);

        Global::init(&cam, &t_bg);
        Global::autoCamControl = false;
        GenerateDevLevel();

        t_bg.init();
    }

    void generate_numbers(){
        for(int y = 0; y < height; y++){
            for(int x = 0; x < width; x++){
                if(bamod(x, 2) == 0){
                    int num = (x + y*width)/2;
                    if(num >= BLOCK_TOTAL){ break;}
                    bn::string_view text = bn::to_string<8>(num);
                    text_generator.generate(x*32, 4 + y*32, text, numbers);
                }
            }
        }       
    }

    void update(){
        Global::update();

        // Movement
        if(!selected){
            tile_grid_move(mapSize.x());
            if(bn::keypad::start_pressed()) Log_block_data();
        }
        // Change tile
        else tile_change();
        
        // Hide block index
        if(bn::keypad::select_pressed()){
            for(bn::sprite_ptr sprite : numbers){
                sprite.set_visible(!sprite.visible());
            }
        }else if(bn::keypad::a_pressed() && bamod(_x>>2, 2) == 0){// Select tile
            selected = !selected;
            arrowDown.set_visible(!arrowDown.visible());
            arrowUp.set_visible(!arrowUp.visible());
        }else if(bn::keypad::l_pressed()){                          // Copy
            tile_copy = current_tile;
        }else if(bn::keypad::r_pressed() && bamod(_x>>2, 2) == 0){   // Paste
            t_bg.game_map().set_cell(_x, _y, tile_copy);
            int value = tile_copy + jv::GameMap::FLPTHD*(1 - 2*t_bg.game_map().horizontal_flip(_x + _y*t_bg.game_map().width()));
            t_bg.game_map().set_cell(_x + 7 - 2*(_x%4), _y, value);
            t_bg.init();
        }

        jv::Interface::resetcombo();
        bn::core::update();
    }

    void tile_grid_move(const int map_x){
        if(bn::keypad::up_pressed() && _y > 0){
            _y--;
            Global::Camera().set_y(Global::Camera().y() - 8);
            Global::Tiled_Bg().init();
            current_block = (_x>>3) + (_y*map_x/80)*6;
            current_tile = Global::Map()._data[_x + _y*Global::Map().width()];
        }else if(bn::keypad::down_pressed() && _y < height*4 - 1){
            _y++;
            Global::Camera().set_y(Global::Camera().y() + 8);
            Global::Tiled_Bg().init();
            current_block = (_x>>3) + (_y*map_x/80)*6;
            current_tile = Global::Map()._data[_x + _y*Global::Map().width()];
        }
        if(bn::keypad::left_pressed() && _x > 0){
            _x--;
            Global::Camera().set_x(Global::Camera().x() - 8);
            Global::Tiled_Bg().init();
            current_block = (_x>>3) + (_y*map_x/80)*6;
            current_tile = Global::Map()._data[_x + _y*Global::Map().width()];
            if(bamod(_x>>2, 2) == 0){ cursor.set_palette(bn::sprite_items::cursor.palette_item().create_palette());}
            else{ cursor.set_palette(bn::sprite_items::good_cat.palette_item().create_palette());}
        }else if(bn::keypad::right_pressed() && _x < width*4 - 1){
            _x++;
            Global::Camera().set_x(Global::Camera().x() + 8);
            Global::Tiled_Bg().init();
            current_block = (_x>>3) + (_y*map_x/80)*6;
            current_tile = Global::Map()._data[_x + _y*Global::Map().width()];
            if(bamod(_x>>2, 2) == 0){ cursor.set_palette(bn::sprite_items::cursor.palette_item().create_palette());}
            else{ cursor.set_palette(bn::sprite_items::good_cat.palette_item().create_palette());}
        }
    }

    void tile_change(){
        if(bn::keypad::up_pressed()){
            current_tile = Global::Map().cell(_x, _y);
            Global::Map().set_cell(_x, _y, current_tile + 1);
            int value = current_tile + 1 + jv::GameMap::FLPTHD*(1 - 2*Global::Map().horizontal_flip(_x + _y*Global::Map().width()));
            Global::Map().set_cell(_x + 7 - 2*(_x%4), _y, value);
            Global::Tiled_Bg().init();
        }else if(bn::keypad::down_pressed() && current_tile - 1 >= 0){
            current_tile = Global::Map().cell(_x, _y);
            Global::Map().set_cell(_x, _y, current_tile - 1);
            int value = current_tile - 1 + jv::GameMap::FLPTHD*(1 - 2*Global::Map().horizontal_flip(_x + _y*Global::Map().width()));
            Global::Map().set_cell(_x + 7 - 2*(_x%4), _y, value);
            Global::Tiled_Bg().init();
        }
    }

    [[nodiscard]] bool exit() const {
        return bn::keypad::a_held() && bn::keypad::b_held();
    }

    bool selected = false;
    int _x = 0, _y = 0, width = 12, height = 7, current_tile = 0, current_block = 0, tile_copy = 0;
    bn::sprite_ptr cursor, arrowUp, arrowDown;

    bn::camera_ptr cam;
    bn::vector<bn::sprite_ptr, 64> numbers;
    bn::sprite_text_generator text_generator;
    jv::tiled_bg t_bg;
    
    static constexpr bn::point mapSize = bn::point(20, 20);
};

}

/*void tile_scene(){
    bn::vector<bn::sprite_ptr, 64> numbers;
    bn::vector<bn::sprite_ptr, 1> tile_index_sprite;
    bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);

    // Background
    bn::bg_palettes::set_transparent_color(bn::color(31, 0, 31));
    constexpr bn::point mapSize(20, 20);
    jv::tiled_bg Fortress(bn::regular_bg_tiles_items::fortress_tiles, bn::bg_palette_items::fortress_palette, mapSize.x()*4, mapSize.y()*4);
    
    // ******** Camera ********
    bn::camera_ptr cam = bn::camera_ptr::create(4, 4);

    // ****** Other data ******
    int current_tile = 0, timer = 0;
    bool prev_toggle = false, toggle = false;
    const int x_offset = -110, y_offset = -70;
    const int width = 12, height = 7;

    { // Configs
        Fortress.set_priority(3);
        text_generator.set_camera(cam);
        Fortress.set_camera(cam);

        // **** Number sprites ****
        for(int y = 0; y < height; y++){
            for(int x = 0; x < width; x++){
                if(bamod(x, 2) == 0){
                    int num = (x + y*width)/2;
                    if(num >= BLOCK_TOTAL){ break;}
                    bn::string_view text = bn::to_string<8>(num);
                    text_generator.generate(x*32, 4 + y*32, text, numbers);
                }
            }
        }
        
        text_generator.remove_camera();
        text_generator.generate(x_offset, y_offset, bn::to_string<3>(current_tile), tile_index_sprite);

        Global::init(&cam, &Fortress.game_map());
        jv::dev::GenerateDevLevel(Fortress.game_map());

        Fortress.init();
        bn::sprite_palettes::set_fade(bn::colors::black, bn::fixed(0.0));
        bn::bg_palettes::set_fade(bn::colors::black, bn::fixed(0.0));
    }
    
    while(true){
        Global::update();

        if(bn::keypad::up_held()){
            bn::fixed target_y = cam.y() - 2;
            cam.set_position(cam.x(), target_y);
        }else if(bn::keypad::down_held()){
            bn::fixed target_y = cam.y() + 2;
            cam.set_position(cam.x(), target_y);
        }
        if(bn::keypad::left_held()){
            bn::fixed target_x = cam.x() - 2;
            cam.set_position(target_x, cam.y());
        }else if(bn::keypad::right_held()){
            bn::fixed target_x = cam.x() + 2;
            cam.set_position(target_x, cam.y());
        }

        if(bn::keypad::select_pressed()){
            for(bn::sprite_ptr sprite : numbers) sprite.set_visible(!sprite.visible());
        }else if(bn::keypad::l_pressed() && current_tile > 0){
            current_tile--;
            tile_index_sprite.clear();
            text_generator.generate(x_offset, y_offset, bn::to_string<3>(current_tile), tile_index_sprite);
            jv::dev::GenerateDevLevel(Fortress.game_map());
            for(int y = 0; y < Fortress.game_map().height(); y++){
                for(int x = 0; x < Fortress.game_map().width(); x++){
                    if(Fortress.game_map().cell(x, y) == current_tile && x < width*4 && y < height*4){
                        Fortress.game_map().set_cell(x, y, GameMap::FLPTHD - 1);
                    }
                }
            }
            Fortress.init();
        }else if(bn::keypad::r_pressed() && current_tile < GameMap::FLPTHD - 1){
            current_tile++;
            tile_index_sprite.clear();
            text_generator.generate(x_offset, y_offset, bn::to_string<3>(current_tile), tile_index_sprite);
            jv::dev::GenerateDevLevel(Fortress.game_map());
            for(int y = 0; y < Fortress.game_map().height(); y++){
                for(int x = 0; x < Fortress.game_map().width(); x++){
                    if(Fortress.game_map().cell(x, y) == current_tile && x < width*4 && y < height*4){
                        Fortress.game_map().set_cell(x, y, GameMap::FLPTHD - 1);
                    }
                }
            }
            Fortress.init();
        }

        toggle = bamod(timer>>6, 2);
        
        if(prev_toggle != toggle){
            if(toggle){
                for(int y = 0; y < Fortress.game_map().height(); y++){
                    for(int x = 0; x < Fortress.game_map().width(); x++){
                        if(Fortress.game_map().cell(x, y) == current_tile && x < width*4 && y < height*4){
                            Fortress.game_map().set_cell(x, y, GameMap::FLPTHD - 1);
                        }
                    }
                }
                Fortress.init();
            }else{
                jv::dev::GenerateDevLevel(Fortress.game_map());
                Fortress.init();
            }
        }

        prev_toggle = toggle;
        timer++;
        
        Fortress.update();
        jv::Interface::resetcombo();
        bn::core::update();
    }
}*/

}

#endif