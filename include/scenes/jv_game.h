#ifndef JV_GAME_H
#define JV_GAME_H

#include "bn_vector.h"
#include "bn_memory.h"
#include "bn_colors.h"
#include "bn_sprites.h"
#include "bn_music_items.h"
#include "bn_bg_palettes.h"
#include "bn_music_actions.h"
#include "bn_sprite_palettes.h"
#include "bn_blending_actions.h"
#include "bn_sprite_palette_actions.h"
#include "common_variable_8x8_sprite_font.h"

#include "jv_fog.h"
#include "jv_math.h"
#include "jv_items.h"
#include "jv_actors.h"
#include "jv_stairs.h"
#include "jv_global.h"
#include "jv_credits.h"
#include "jv_tiled_bg.h"
#include "jv_healthbar.h"
#include "jv_interface.h"
#include "jv_level_generation.h"
#include "jv_tiled_bg_animate_actions.h"

#include "bn_regular_bg_items_bg.h"
#include "bn_regular_bg_items_intro1.h"
#include "bn_regular_bg_items_intro_card.h"
#include "bn_regular_bg_items_intro_card_bg.h"

#include "bn_sprite_items_ball.h"
#include "bn_sprite_items_cursor.h"

#if LOGS_ENABLED
    #include "bn_log.h"
    #include "bn_string.h"
    static_assert(LOGS_ENABLED, "Log is not enabled");
#endif
#if DEV_ENABLED
    #include "jv_debug.h"
#endif

namespace jv::game{
void intro_scene(){
    bn::regular_bg_ptr intro1_bg = bn::regular_bg_items::intro1.create_bg(0, 0);
    
    jv::Interface::fade(true);
    for(int i = 0; i < 180; i++){ bn::core::update();}
    jv::Interface::fade(false);
}

int start_scene(bn::random& randomizer){
    bn::regular_bg_ptr card = bn::regular_bg_items::intro_card.create_bg(0, 0);
    bn::regular_bg_ptr bg = bn::regular_bg_items::intro_card_bg.create_bg(0, -54);
    
    bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);
    bn::vector<bn::sprite_ptr, 45> menu_sprts;
    bn::vector<bn::sprite_ptr, 83> explain_sprts;
    
    int option = 0;
    int x_offset = -16, y_offset = 50;

    bn::sprite_ptr cursor = bn::sprite_items::cursor.create_sprite(-34, y_offset);
    
    {// Configs
        card.set_priority(0);
        cursor.set_bg_priority(1);
        bg.set_priority(3);

        text_generator.set_bg_priority(0);
        text_generator.generate(-96, y_offset - 16, "Select scene", menu_sprts);
        text_generator.generate(-100, y_offset,     "Start game", menu_sprts);
        #if DEV_ENABLED
            text_generator.generate(-100, y_offset + 8, "Block test", menu_sprts);
            text_generator.generate(-100, y_offset + 16,"Tile test", menu_sprts);
        #else
            text_generator.generate(-100, y_offset + 8, "Credits", menu_sprts);
        #endif

        y_offset = 40;
    }

    #if !DEV_ENABLED
        bn::string_view explain_text[3][5] = {
            {"", "A: Interact", "B: Attack"}
        };
    #else
        bn::string_view explain_text[3][5] = {
            {"", "A: Interact", "B: Attack", "SELECT: Debug menu"},
            {"A: Select tile", "L: Copy tile", "R: Paste tile", "SELECT: Toggle index", "START: Print to log"},
            {"", "L: Next highlighted tile", "R: Prev. highlighted tile", "SELECT: Toggle index"}
        };
    #endif

    for(int i = 0; i < 5; i++){
        text_generator.generate(x_offset, y_offset + i*8, explain_text[int(option)][i], explain_sprts);
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

    bn::sprite_palettes::set_fade(bn::colors::black, bn::fixed(0.0));
    bn::bg_palettes::set_fade(bn::colors::black, bn::fixed(0.0));
    
    return option;
}

void game_scene(bn::random& randomizer){
    // Text generator
    bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);

    // Music
    bn::music_items::cyberrid.play(0.2);

    // *** Level Background ***
    bn::regular_bg_ptr background = bn::regular_bg_items::bg.create_bg(0, 0);
    const uint8_t zone_x = 5, zone_y = 5;
    constexpr uint8_t zoneSize = zone_x*zone_y;
    int tileDatasize = ((zone_x*7) - 1)*4 * ((zone_y*7) - 1)*4;
    
    uint8_t* tileData = new uint8_t[tileDatasize];
    jv::tiled_bg Fortress(bn::regular_bg_tiles_items::fortress_tiles1, bn::bg_palette_items::fortress_palette, game_map(((zone_x*7) - 1)*4, ((zone_y*7) - 1)*4, tileData));

    bn::regular_bg_tiles_item tiles_items[3] = 
        {bn::regular_bg_tiles_items::fortress_tiles1,
         bn::regular_bg_tiles_items::fortress_tiles2,
         bn::regular_bg_tiles_items::fortress_tiles3};
    jv::tiled_bg_animate_action<4> fort_animation = jv::create_tiled_bg_animate_action_forever(Fortress.background(), 15, tiles_items, 0, 1, 0, 2);

    // ** Universal entities **
    bn::camera_ptr cam = bn::camera_ptr::create(0, 0);
    jv::Player cat(bn::point(0, 0), cam);
    jv::healthbar healthbar(cat.get_maxhp_ptr(), cat.get_hp_ptr());
    jv::stairs stairs;
    jv::Fog<zoneSize>* fog_ptr = nullptr;
    jv::Fog<zoneSize> fog;

    bn::vector<jv::NPC, 1> v_npcs;
    bn::vector<jv::Enemy*, MAX_ENEMIES> v_enemies;
    bn::vector<jv::Item*, MAX_ENEMIES> v_scene_items;
    bn::vector<jv::Projectile*, MAX_ENEMIES> v_projectiles;

    bn::vector<bn::sprite_ptr, 2> txt_sprts;

    // ****** Level data ******
    int floor = 0, gameover_delay = 0;
    bool next_level = false, game_over = false, Objective = true;
    constexpr bn::fixed cam_lerp_t(0.13);

    // ****** Debug data ******
    #if DEV_ENABLED
        bool Noclip = false;
        bool FullHeal = false;
        bool Die = false;
        bool Clear = false;
        bool NoFog = false;
        bn::array<jv::menu_option, 7> options(
            {jv::menu_option(&cat.invulnerable, "Invuln."),
            jv::menu_option(&FullHeal, "Fully heal"),
            jv::menu_option(&Noclip, "Noclip"),
            jv::menu_option(&next_level, "Next level"),
            jv::menu_option(&Die, "Die"),
            jv::menu_option(&Clear, "Clear"),
            jv::menu_option(&NoFog, "No Fog"),});
    #endif

    {// Configs
        bn::sprites::set_blending_bottom_enabled(false);
        bn::blending::set_transparency_alpha(0.8);

        text_generator.set_bg_priority(0);
        background.set_camera(cam);
        Fortress.set_camera(cam);
        fog_ptr = &fog;

        jv::Global::initialize(&cam, &Fortress.map, &cat, &randomizer, &v_projectiles);
        jv::Global::extra_data_init(&v_npcs, &v_enemies, &v_scene_items);

        text_generator.generate(64, -70, "Floor", txt_sprts);
    }

    while(!game_over){
        text_generator.generate(94, -70, bn::to_string<3>(floor), txt_sprts);
        next_level = false;
        gameover_delay = 0;

        // Level generation
        jv::Level::Generate(zone_x, zone_y, fog_ptr);
        jv::Level::Populate(stairs);
        
        // Initialize level visuals
        Fortress.init();
        if(fog_ptr){ fog_ptr->update();}
        
        jv::Interface::Log_resources();
        
        // Fade in
        jv::Interface::fade(true);

        while(!next_level){
            jv::Global::update();
            Fortress.update();
            fort_animation.update();
            Objective = true;

            // Player update
            #if DEV_ENABLED
                cat.update(Noclip);
            #else 
                cat.update();
            #endif
            bn::fixed cam_x_target = lerp(Global::Camera().x(), Global::Player().get_hitbox().x(), cam_lerp_t);
            bn::fixed cam_y_target = lerp(Global::Camera().y(), Global::Player().get_hitbox().y() + 4, cam_lerp_t);
            cam.set_position(cam_x_target, cam_y_target);
            
            if(fog_ptr){ fog_ptr->update();}
            
            if(cat.alive()){
                next_level = stairs.climb();
                Global::scene_items_update();

                // Debug menu
                #if DEV_ENABLED
                    if(bn::keypad::select_pressed()){
                        jv::Interface::set_hide_all(healthbar, background, Fortress, txt_sprts, true);
                        jv::Debug::Start(options.data(), options.size());
                        jv::Interface::set_hide_all(healthbar, background, Fortress, txt_sprts, false);
                        
                        if(FullHeal){
                            cat.heal(cat.get_maxhp());
                            FullHeal = false;
                        }
                        if(Die){
                            cat.invulnerable = false;
                            cat.got_hit(cat.get_hp(), true);
                        }
                        if(Clear){
                            jv::Global::clear_enemies();
                            jv::Global::clear_scene_items();
                            jv::Global::clear_projectiles();
                            Clear = false;
                        }
                        if(fog_ptr){
                            if(NoFog){
                                if(fog_ptr->visible()){
                                    fog_ptr->set_visible(false);
                                }
                            }else{
                                if(!fog_ptr->visible()){
                                    fog_ptr->set_visible(true);
                                }
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

            jv::Global::enemies_update(Objective);
            for(int i = 0; i < v_npcs.size(); i++){ v_npcs[i].update(stairs, Fortress, Objective);}
            
            healthbar.update();

            jv::Interface::Log_skipped_frames();
            jv::Interface::resetcombo();
            bn::core::update();
        }

        floor--;
        
        // Fade out
        jv::Interface::fade(false, cat.alive() ? fadespeed::MEDIUM : fadespeed::SLOW);

        // Reset Stuff
        jv::Global::clear_enemies();
        jv::Global::clear_scene_items();
        jv::Global::clear_projectiles();
        v_npcs.clear();
        stairs.set_open(false);
        if(fog_ptr){ fog_ptr->reset();}
        txt_sprts.erase(txt_sprts.begin() + 1);

        bn::core::update();
    }
    
    jv::Global::reset();
    bn::sprites::set_blending_bottom_enabled(true);
    bn::music::stop();
    delete[] tileData;
}

}

#endif