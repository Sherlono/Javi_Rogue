#ifndef JV_CREDITS_H
#define JV_CREDITS_H

#include "bn_vector.h"
#include "bn_string.h"
#include "bn_sprite_text_generator.h"

#ifndef SECTION_TOTAL
    #define SECTION_TOTAL 11
    #define MAX_SECTION_SIZE 10
    #define MAX_HEAD_LINES 4
    #define MAX_BODY_LINES 20
#endif

namespace jv::Scenes{
struct CreditsScene{
    static void Start(){
        CreditsScene instance;
    }
private:
    CreditsScene():
        text_generator(common::variable_8x8_sprite_font)//,
        {
            text_generator.set_alignment(bn::sprite_text_generator::alignment_type::CENTER);
            bn::bg_palettes::set_fade(bn::colors::black, 0);
            bn::sprite_palettes::set_fade(bn::colors::black, 0);

            while(!done){
                update();
            }
        }

    void generate_credits(const int y_offset){
        for(int i = 0; i < MAX_HEAD_LINES; i++){
            if(header_sprts[i].empty()){
                text_generator.generate(0, y_offset, Header[section], header_sprts[i]);
                break;
            }
        }
        
        for(int i = 0; i < MAX_SECTION_SIZE; i++){
            if(Body[section][i] == ""){ break;}
            for(int j = 0; j < MAX_BODY_LINES; j++){
                if(body_sprts[j].empty()){
                    text_generator.generate(0, y_offset + (i+2)*8, Body[section][i], body_sprts[j]);
                    break;
                }
            }
        }
    }
    
    void update(){
        lowestLine = 0;
        
        if(section != 0){
            for(int line = 0; line < MAX_HEAD_LINES; line++){
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
            
            for(int line = 0; line < MAX_BODY_LINES; line++){
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

        if(section < SECTION_TOTAL){
            if(lowestLine < 50){
                generate_credits(90);
                section++;
            }
        }else{
            bool headsEmpty = true, bodiesEmpty = true;
            for(int line = 0; line < MAX_HEAD_LINES; line++){
                headsEmpty = headsEmpty && header_sprts[line].empty();
            }
            for(int line = 0; line < MAX_BODY_LINES; line++){
                bodiesEmpty = bodiesEmpty && body_sprts[line].empty();
            }
            done = headsEmpty && bodiesEmpty;
        }
        bn::core::update();
    }

    bool done = false;
    int lowestLine, section = 0;
    static constexpr bn::fixed scrollSpeed = 0.2;
    
    bn::sprite_text_generator text_generator;
    bn::vector<bn::sprite_ptr, 4> header_sprts[MAX_HEAD_LINES];
    bn::vector<bn::sprite_ptr, 4> body_sprts[MAX_BODY_LINES];
    
    static constexpr bn::string_view Header[SECTION_TOTAL] = {
        "Director",
        "Programming",
        "Writing",
        "Game Design",
        "Graphic Design",
        "Character Design",
        "Background Design",
        "Composers",
        "Audio Design",
        "Playtesters",
        "Special Thanks"};
    static constexpr bn::string_view Body[SECTION_TOTAL][MAX_SECTION_SIZE] = {
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
};
}

#endif