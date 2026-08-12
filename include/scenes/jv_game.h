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

#include "jv_fog.h"
#include "jv_cow.h"
#include "jv_fox.h"
#include "jv_math.h"
#include "jv_global.h"
#include "jv_tiled_bg.h"
#include "jv_interface.h"
#include "jv_blocks_data.h"
#include "jv_game_assets.h"
#include "jv_level_generator.h"
#include "jv_tiled_bg_animate_actions.h"

#include "jv_tiled_bg_item_fortress_fwd.h"
#include "jv_tiled_bg_item_jungle_fwd.h"

#include "bn_regular_bg_items_bg.h"
#include "bn_regular_bg_items_intro1.h"
#include "bn_regular_bg_items_intro_card.h"
#include "bn_regular_bg_items_intro_card_bg.h"

#include "bn_sprite_items_cursor.h"
#include "common_variable_8x8_sprite_font.h"

#if DEV_ENABLED
    #include "bn_log.h"
    #include "jv_debug.h"
    static_assert(DEV_ENABLED, "Log is not enabled");
#endif

namespace jv::Scenes{
enum class Tag {Restart, Main, Credits, Blocks, Tiles};
struct MainGame{
    static void Start(bn::random& r){
        MainGame instance(r);
    }
private:
    ~MainGame() = default;
    MainGame(bn::random& r):
        _cam(bn::camera_ptr::create(0, 0)),
        text_generator(common::variable_8x8_sprite_font),
        _gameAssets(r),
        _backdrop(bn::regular_bg_items::bg.create_bg(0, 0)),
        _tiled_bg(Global::environment_id == Global::Fortress ? bn::regular_bg_tiles_items::fortress_tiles : bn::regular_bg_tiles_items::jungle_tiles,
                  Global::environment_id == Global::Fortress ? bn::bg_palette_items::fortress_palette : bn::bg_palette_items::jungle_palette,
                  ((MAX_ROOM_ROWS*7) - 1)*4, ((MAX_ROOM_COLUMNS*7) - 1)*4),
        _tiles_item(Global::environment_id == Global::Fortress ? bn::regular_bg_tiles_items::fortress_torch.tiles_ref() : bn::regular_bg_tiles_items::jungle_vines.tiles_ref()),
        _bg_animation{jv::create_tiled_bg_animate_action_forever(_tiled_bg.tiles(), 15, 89, _tiles_item, 0, 2, 0, 4),
                      jv::create_tiled_bg_animate_action_forever(_tiled_bg.tiles(), 15, 90, _tiles_item, 1, 3, 1, 5)}
        #if DEV_ENABLED
        ,options{jv::menu_option(&_gameAssets.cat.invulnerable, "Invuln."),
                 jv::menu_option(&_gameAssets.cat.invisible, "Invisible"),
                 jv::menu_option(&FullHeal, "FullHeal"),
                 jv::menu_option(&_gameAssets.cat.noClip, "Noclip"),
                 jv::menu_option(&_next_level, "NextFloor"),
                 jv::menu_option(&Die, "Die"),
                 jv::menu_option(&Clear, "Clear"),
                 jv::menu_option(&NoFog, "NoFog"),
                 jv::menu_option(&openStairs, "OpenStairs"),}
        #endif
        {
            bn::music_items::cyberrid.play(0.2);
            text_generator.set_bg_priority(0);
            _backdrop.set_camera(_cam);
            _tiled_bg.set_camera(_cam);

            bn::sprites::set_blending_bottom_enabled(false);
            bn::blending::set_transparency_alpha(0.8);
            
            Global::init(&_cam, &_tiled_bg, &_gameAssets);

            _gameAssets.cat.load_graphics(jv::animation::Id::Walk);
            _gameAssets.healthbar.init();

            text_generator.generate(64, -70, "Floor", _txt_sprts);

            #if DEV_ENABLED
            _gameAssets.cat.invulnerable = true;
            text_generator.generate(-32, -70, "CPU: ", cpu_sprts);
            cpu_sprts[0].set_bg_priority(0);
            #endif
            
            while(!_game_over){
                level_start();
                update();
                level_end();
            }

            bn::sprites::set_blending_bottom_enabled(true);
            bn::music::stop();
            Global::reset();
        }

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
                
                _bg_animation[0].update();
                _bg_animation[1].update();
                
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
                
                _bg_animation[0].update();
                _bg_animation[1].update();
            
                jv::Interface::resetcombo();
                bn::core::update();
            }
        }
    }

    void set_all_visible(bool visible){
        _backdrop.set_visible(visible);
        _tiled_bg.set_visible(visible);
        _gameAssets.set_all_visible(visible);
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

    void level_start(){
        _gameover_delay = 0;
        _next_level = false;

        _gameAssets.fog.set_visible(Global::environment_id == Global::Jungle);
        
        LevelGenerator::Generate(4 + Global::Random().get_int(MAX_ROOM_COLUMNS - 3),
                                 4 + Global::Random().get_int(MAX_ROOM_ROWS - 3));
        /*LevelGenerator::Generate(4, 4);*/

        // Initialize visuals
        _tiled_bg.init();
        _gameAssets.stairs.set_open(false);
        if(_gameAssets.fog.visible()) _gameAssets.fog.update();
        //for(auto enemy : _gameAssets.v_enemies) if(enemy.is_on_screen() && !enemy.has_graphics) enemy.update();
        
        #if DEV_ENABLED
        jv::Interface::Log_resources();
        while(cpu_sprts.size() > 1){ cpu_sprts.erase(cpu_sprts.end() - 1); }
        text_generator.generate(-4, -70, bn::to_string<7>(bn::core::last_cpu_usage()), cpu_sprts);
        bn::core::update();
        jv::Interface::Log_skipped_frames();
        #endif

        text_generator.generate(94, -70, bn::to_string<3>(_floor), _txt_sprts);
        fade(FADE_IN, fadespeed::MEDIUM, false);
    }

    void level_end(){
        const bool FADE_MUSIC = ! _gameAssets.cat.alive();
        int fade_speed = _gameAssets.cat.alive() ? fadespeed::MEDIUM : fadespeed::SLOW;
        fade(FADE_OUT, fade_speed, FADE_MUSIC);
        
        _floor--;
        //Global::environment_id = DEV_ENABLED ? bn::abs(_floor)%2 : (bn::abs(_floor)/3)%2;
        Global::environment_id = bn::abs(_floor)%2;
        
        load_bg_assets();

        // Flush and reset objects
        _gameAssets.clear_objects();
        _txt_sprts.erase(_txt_sprts.begin() + 1);

        #if DEV_ENABLED
        while(cpu_sprts.size() > 1) cpu_sprts.erase(cpu_sprts.end() - 1);
        #endif
    }

    void update(){ // Main gameplay loop
        while(!_next_level){
            Global::update();

            // Bg stuff
            _bg_animation[0].update();
            _bg_animation[1].update();
            
            // Player update
            _gameAssets.cat.update();

            if(_gameAssets.cat.alive()) [[likely]] {
                _next_level = _gameAssets.stairs.climb();
                _gameAssets.scene_items_update();

                // Debug Stuff
                #if DEV_ENABLED
                if(bn::keypad::l_pressed()) [[unlikely]] {
                    int value = _tiled_bg.game_map().cell(_gameAssets.cat.x()>>3, (_gameAssets.cat.y()+4)>>3);
                    BN_LOG("x: ", _gameAssets.cat.x()>>3, " y: ", (_gameAssets.cat.x()+4)>>3, " Value: ", value);
                }else if(bn::keypad::select_pressed()) [[unlikely]] {
                    set_all_visible(false);
                    jv::Debug::Start(options.data(), options.size());
                    set_all_visible(true);
                    
                    if(FullHeal){
                        _gameAssets.cat.heal( _gameAssets.cat.get_maxhp());
                        FullHeal = false;
                    }
                    if(Die){
                        _gameAssets.cat.invulnerable = false;
                        _gameAssets.cat.got_hit( _gameAssets.cat.get_hp(), true);
                    }
                    if(Clear){
                        _gameAssets.clear_objects(false);
                        Clear = false;
                    }
                    if(Global::environment_id == Global::Jungle){
                        if(NoFog){
                            if(_gameAssets.fog.visible()) _gameAssets.fog.set_visible(false);
                        }else{
                            if(!_gameAssets.fog.visible()) _gameAssets.fog.set_visible(true);
                        }
                    }
                    if(openStairs){
                        _gameAssets.stairs.set_open(true);
                        openStairs = false;
                    }
                }
                #endif

            }else{  // Death sequence
                if(_gameover_delay == 120){
                    _game_over = true;
                    break;
                }
                _gameover_delay++;
            }

            if(_gameAssets.cat.get_state() != Actor::State::DEAD && _gameAssets.cat.moved()){    // Backdrop movement
                _backdrop.set_position(_backdrop.x() + (_gameAssets.cat.prev_position().x() - _gameAssets.cat.x())*bn::fixed(0.15),
                                       _backdrop.y() + (_gameAssets.cat.prev_position().y() - _gameAssets.cat.y())*bn::fixed(0.15));
            }
            if(_gameAssets.fog.visible() && Global::cam_moved()) _gameAssets.fog.update();
            
            _gameAssets.update();
            
            #if DEV_ENABLED
            while(cpu_sprts.size() > 1){
                cpu_sprts.erase(cpu_sprts.end() - 1);
            }
            text_generator.generate(-4, -70, bn::to_string<7>(bn::core::last_cpu_usage()), cpu_sprts);
            #endif

            if(bn::keypad::r_held()) for(int i = 0; i < 8; i++) bn::core::update(); // Slow down game

            jv::Interface::resetcombo();
            bn::core::update();
        }
    }

    bn::camera_ptr _cam;
    bn::sprite_text_generator text_generator;
    bn::vector<bn::sprite_ptr, 2> _txt_sprts;
    
    jv::GameAssets _gameAssets;

    // Metadata
    uint8_t _gameover_delay = 0;
    int _floor = 0;
    bool _game_over = false, _next_level = false;

    // Bg Graphics
    bn::regular_bg_ptr _backdrop;
    jv::tiled_bg _tiled_bg;
    jv::tile_span _tiles_item;
    jv::tiled_bg_animate_action<4> _bg_animation[2];

    #if DEV_ENABLED
    bn::vector<bn::sprite_ptr, 3> cpu_sprts;
    bn::array<jv::menu_option, 9> options;
    bool FullHeal = false, Die = false, Clear = false, NoFog = false, openStairs = false;
    #endif
};

void IntroScene(){
    bn::regular_bg_ptr intro1_bg = bn::regular_bg_items::intro1.create_bg(0, 0);
    
    jv::Interface::fade(FADE_IN, fadespeed::MEDIUM, false);
    for(int i = 0; i < 180; i++) {
        jv::Interface::resetcombo();
        bn::core::update();
    }
    jv::Interface::fade(FADE_OUT, fadespeed::MEDIUM, false);
}

Tag StartScene(bn::random& randomizer){
    bn::regular_bg_ptr card = bn::regular_bg_items::intro_card.create_bg(0, 0);
    bn::regular_bg_ptr bg = bn::regular_bg_items::intro_card_bg.create_bg(0, -54);
    
    bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);
    bn::vector<bn::sprite_ptr, 15> menu_sprts;
    bn::vector<bn::sprite_ptr, 30> explain_sprts;
    
    int x_offset = -32, y_offset = 46, option = 0;
    [[maybe_unused]] int idle_time = 0;
    [[maybe_unused]] const int  idle_limit = 600, max_options = 1;

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
        if(bn::keypad::down_pressed() && option < max_options){
            option++;
            cursor.set_y(cursor.y() + 8);
            explain_sprts.clear();
            for(int i = 0; i < 5; i++){
                text_generator.generate(x_offset, y_offset + i*8, explain_text[int(option)][i], explain_sprts);
            }
            idle_time = 0;
        }else if(bn::keypad::up_pressed() && option > 0){
            option--;
            cursor.set_y(cursor.y() - 8);
            explain_sprts.clear();
            for(int i = 0; i < 5; i++){
                text_generator.generate(x_offset, y_offset + i*8, explain_text[int(option)][i], explain_sprts);
            }
            idle_time = 0;
        }

        #if !DEV_ENABLED
        idle_time++;
        if(idle_time == idle_limit) break;
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
    if(option == 0){
        return Tag::Main;
    }else if (option == 1){
        return Tag::Blocks;
    }else{
        return Tag::Tiles;
    }
    #else
    if(idle_time == idle_limit){
        return Tag::Restart;
    }else{
        if(option == 0){
            return Tag::Main;
        }else{
            return Tag::Credits;
        }
    }
    #endif
}

}

#endif