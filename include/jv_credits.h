#ifndef JV_CREDITS_H
#define JV_CREDITS_H

#include "bn_vector.h"
#include "bn_string.h"
#include "bn_sprite_text_generator.h"

#if LOGS_ENABLED
    #include "bn_log.h"
    static_assert(LOGS_ENABLED, "Log is not enabled");
#endif

#define SECTIONTOTAL 11
#define MAXCREDITBODY 10
#define MAXHEADLINES 4
#define MAXBODYLINES 20

namespace jv::credits{
constexpr bn::string_view Header[SECTIONTOTAL] = {
    "Director",
    "Programming",
    "Writing",
    "Game Design",
    "Graphic Design",
    "Character Desig",
    "Background Design",
    "Composers",
    "Audio Design",
    "Playtesters",
    "Special Thanks"};

constexpr bn::string_view Body[SECTIONTOTAL][MAXCREDITBODY] = {
    {"Javier Sánchez"},
    {"Javier Sánchez"},
    {"Javier Sánchez"},
    {"Javier Sánchez"},
    {"Javier Sánchez", "Demian"},
    {"Javier Sánchez"},
    {"Javier Sánchez"},
    {"Looking"},
    {"Looking"},
    {"Javier Sánchez"},
    {"Claudia Andrade", "Freddy Sánchez", "And you!"}};

void generate_credits(bn::sprite_text_generator& text_gen, auto& header_sprts, auto& body_sprts, int section, int const y_offset){
    for(int i = 0; i < MAXHEADLINES; i++){
        if(header_sprts[i].empty()){
            text_gen.generate(0, y_offset, Header[section], header_sprts[i]);
            break;
        }
    }
    
    for(int i = 0; i < MAXCREDITBODY; i++){
        if(Body[section][i] == ""){ break;}
        for(int j = 0; j < MAXBODYLINES; j++){
            if(body_sprts[j].empty()){
                text_gen.generate(0, y_offset + (i+2)*8, Body[section][i], body_sprts[j]);
                break;
            }
        }
    }
}

void credits_scene(){
    bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);
    bn::vector<bn::sprite_ptr, 4> header_sprts[MAXHEADLINES];
    bn::vector<bn::sprite_ptr, 4> body_sprts[MAXBODYLINES];
    const bn::fixed scrollSpeed = 0.1;
    int lowestLine, section = 0;
    bool done = false;

    {// Configs
        text_generator.set_alignment(bn::sprite_text_generator::alignment_type::CENTER);
        bn::bg_palettes::set_fade(bn::colors::black, 0);
        bn::sprite_palettes::set_fade(bn::colors::black, 0);
    }
    
    while(!done){
        lowestLine = 0;

        if(section != 0){
            for(int line = 0; line < MAXHEADLINES; line++){
                if(!header_sprts[line].empty()){
                    lowestLine = bn::max(lowestLine, header_sprts[line][0].y().integer());

                    for(int i = 0; i < header_sprts[line].size(); i++){   // Move line sprites up
                        header_sprts[line][i].set_y(header_sprts[line][i].y() - scrollSpeed);
                    }
                    if(header_sprts[line][0].y() < -90){  // If line passed upper limit then clear it
                        header_sprts[line].clear();
                        line--;
                    }
                }
            }
            
            for(int line = 0; line < MAXBODYLINES; line++){
                if(!body_sprts[line].empty()){
                    lowestLine = bn::max(lowestLine, body_sprts[line][0].y().integer());
                    
                    for(int i = 0; i < body_sprts[line].size(); i++){   // Move line sprites up
                        body_sprts[line][i].set_y(body_sprts[line][i].y() - scrollSpeed);
                    }
                    if(!body_sprts[line].empty() && body_sprts[line][0].y() < -90){  // If line passed upper limit then clear it
                        body_sprts[line].clear();
                        line--;
                    }
                }
            }
            
        }

        if(section < SECTIONTOTAL){
            if(lowestLine < 50){
                generate_credits(text_generator, header_sprts, body_sprts, section, 90);
                section++;
            }
        }else{
            bool headsEmpty = true, bodiesEmpty = true;
            for(int line = 0; line < MAXHEADLINES; line++){
                headsEmpty = headsEmpty && header_sprts[line].empty();
            }
            for(int line = 0; line < MAXBODYLINES; line++){
                bodiesEmpty = bodiesEmpty && body_sprts[line].empty();
            }
            done = headsEmpty && bodiesEmpty;
        }
        bn::core::update();
    }

}

}

#endif