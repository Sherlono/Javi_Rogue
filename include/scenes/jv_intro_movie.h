#ifndef JV_INTRO_MOVIE_H
#define JV_INTRO_MOVIE_H

#include "jv_constants.h"

#include "bn_regular_bg_items_intro_movie_bushes.h"
#include "bn_regular_bg_items_intro_movie_tail_peak_1.h"
#include "bn_regular_bg_items_intro_movie_shot_2_cat.h"

#include "bn_sprite_items_intro_cat_1.h"
#include "bn_sprite_items_intro_shot_3_sprites.h"
#include "bn_sprite_items_intro_movie_big_bush.h"

namespace jv::scenes{
class IntroMovie{
public:
    IntroMovie(): _interrupted(false)
        {
            {Shot_1 start_1(_interrupted);}
            if(!_interrupted){Shot_2 start_2(_interrupted);}
            if(!_interrupted){Shot_3 start_3(_interrupted);}
        }
private:
    class Shot_1{
    public:
        Shot_1(bool& interrupted):
            _fade_speed(fadespeed::SLOW),
            _duration(500),
            _fade_color(bn::colors::black),
            cat_sprites{bn::sprite_items::intro_cat_1.create_sprite(-32, 36, 3),
                        bn::sprite_items::intro_cat_1.create_sprite(-34, 13)},
            cat_animations{bn::sprite_animate_action<4>::forever(cat_sprites[0], 16, bn::sprite_items::intro_cat_1.tiles_item(), bn::array<const short unsigned int, 4>{3, 4, 3, 5}),
                           bn::sprite_animate_action<4>::forever(cat_sprites[1], 16, bn::sprite_items::intro_cat_1.tiles_item(), bn::array<const short unsigned int, 4>{0, 1, 0, 2})},
            bgs{bn::regular_bg_items::intro_movie_forest.create_bg(0, 32),
                bn::regular_bg_items::intro_movie_bushes.create_bg(0, 32)}
            {
                start(interrupted);
            }
        

    private:
        void update(){
            for(bn::sprite_ptr sprite : cat_sprites) sprite.set_x(sprite.x() + bn::fixed(0.15));
            bgs[0].set_x(bgs[0].x() - bn::fixed(0.6));
            bgs[1].set_x(bgs[1].x() - bn::fixed(0.9));
            for(auto& animation : cat_animations) animation.update();
            bn::core::update();
        }

        void start(bool& interrupted){
            for(bn::sprite_ptr sprite : cat_sprites) sprite.set_bg_priority(3);
            bgs[1].set_priority(0);

            bn::music_items::cyberrid.play(0.2);
            for(int i = 0; i < _duration; i++){
                if(bn::keypad::any_pressed()) {
                    interrupted = true;
                    break;
                }

                if(i <= _fade_speed){
                    bn::fixed max;
                    _fade_progress = 1 - bn::fixed(i)/_fade_speed;
                    max = bn::max(_fade_progress, bn::fixed(0));

                    bn::sprite_palettes::set_fade(_fade_color, max);
                    bn::bg_palettes::set_fade(_fade_color, max);
                    update();
                }else if(i < _duration - _fade_speed){
                    update();
                }else{
                    bn::fixed min;
                    _fade_progress = bn::fixed(i - _duration + _fade_speed)/_fade_speed;
                    min = bn::min(_fade_progress, bn::fixed(1));

                    bn::sprite_palettes::set_fade(_fade_color, min);
                    bn::bg_palettes::set_fade(_fade_color, min);
                    
                    update();
                }
            }
        }

        uint8_t _fade_speed;
        const int  _duration;
        bn::color _fade_color;
        bn::fixed _volume_decrement;
        bn::fixed _fade_progress;

        bn::array<bn::sprite_ptr, 2> cat_sprites;
        bn::array<bn::sprite_animate_action<4>, 2> cat_animations;
        bn::array<bn::regular_bg_ptr, 2> bgs;
    };

    class Shot_2{
    public:
        Shot_2(bool& interrupted):
            _fade_speed(fadespeed::SLOW),
            _frame(0),
            _duration(320),
            _cat_bg(bn::regular_bg_items::intro_movie_shot_2_cat.create_bg(0, 20)),
            _bush_sprites_left{ bn::sprite_items::intro_movie_big_bush.create_sprite(-32 - 64 + 64*0, -64 + 64*0, 0),
                                bn::sprite_items::intro_movie_big_bush.create_sprite(-32 - 64 + 64*1, -64 + 64*0, 1),
                                bn::sprite_items::intro_movie_big_bush.create_sprite(-32 - 64 + 64*2, -64 + 64*0, 2),
                                bn::sprite_items::intro_movie_big_bush.create_sprite(-32 - 64 + 64*0, -64 + 64*1, 3),
                                bn::sprite_items::intro_movie_big_bush.create_sprite(-32 - 64 + 64*1, -64 + 64*1, 4),
                                bn::sprite_items::intro_movie_big_bush.create_sprite(-32 - 64 + 64*2, -64 + 64*1, 5),
                                bn::sprite_items::intro_movie_big_bush.create_sprite(-32 - 64 + 64*0, -64 + 64*2, 6),
                                bn::sprite_items::intro_movie_big_bush.create_sprite(-32 - 64 + 64*1, -64 + 64*2, 7),
                                bn::sprite_items::intro_movie_big_bush.create_sprite(-32 - 64 + 64*2, -64 + 64*2, 8),},
            _bush_sprites_right{bn::sprite_items::intro_movie_big_bush.create_sprite( 32 - 64 + 64*0, -64 + 64*0, 2),
                                bn::sprite_items::intro_movie_big_bush.create_sprite( 32 - 64 + 64*1, -64 + 64*0, 1),
                                bn::sprite_items::intro_movie_big_bush.create_sprite( 32 - 64 + 64*2, -64 + 64*0, 0),
                                bn::sprite_items::intro_movie_big_bush.create_sprite( 32 - 64 + 64*0, -64 + 64*1, 5),
                                bn::sprite_items::intro_movie_big_bush.create_sprite( 32 - 64 + 64*1, -64 + 64*1, 4),
                                bn::sprite_items::intro_movie_big_bush.create_sprite( 32 - 64 + 64*2, -64 + 64*1, 3),
                                bn::sprite_items::intro_movie_big_bush.create_sprite( 32 - 64 + 64*0, -64 + 64*2, 8),
                                bn::sprite_items::intro_movie_big_bush.create_sprite( 32 - 64 + 64*1, -64 + 64*2, 7),
                                bn::sprite_items::intro_movie_big_bush.create_sprite( 32 - 64 + 64*2, -64 + 64*2, 6),}
            {
                start(interrupted);
            }
        
    private:
        void update(){
            if(_frame > 80 && _frame < _duration - 130){
                for(bn::sprite_ptr sprite : _bush_sprites_left)  sprite.set_x(sprite.x() - bn::fixed(1.5));
                for(bn::sprite_ptr sprite : _bush_sprites_right) sprite.set_x(sprite.x() + bn::fixed(1.5));
            }
            if(_frame < _duration - 80){
                _cat_bg.set_y(_cat_bg.y() - bn::fixed(0.1));
            }
            bn::core::update();
        }

        void start(bool& interrupted){
            for(bn::sprite_ptr sprite : _bush_sprites_right)  sprite.set_horizontal_flip(true);
            bn::sprite_palettes::set_fade_intensity(0);
            bn::bg_palettes::set_fade_intensity(0);

            for(int i = 0; i < _duration; i++){
                if(bn::keypad::any_pressed()) {
                    interrupted = true;
                    break;
                }

                update();
                _frame++;
            }
        }

        uint8_t _fade_speed;
        int _frame;
        const int  _duration;
        bn::color _fade_color;
        bn::fixed _volume_decrement, _fade_progress;

        bn::regular_bg_ptr _cat_bg;
        bn::array<bn::sprite_ptr, 9> _bush_sprites_left;
        bn::array<bn::sprite_ptr, 9> _bush_sprites_right;
    };

    class Shot_3{
    public:
        Shot_3(bool& interrupted):
            _fade_speed(fadespeed::SLOW),
            _frame(0),
            _duration(500),
            _fade_color(bn::colors::black),
            _cat_head(bn::sprite_items::intro_shot_3_sprites.create_sprite(0, 58 + 32, 0)),
            _bush_sprites{bn::sprite_items::intro_shot_3_sprites.create_sprite(-96, 64 + 16, 1),
                          bn::sprite_items::intro_shot_3_sprites.create_sprite(-32, 64 + 16, 2),
                          bn::sprite_items::intro_shot_3_sprites.create_sprite( 32, 64 + 16, 3),
                          bn::sprite_items::intro_shot_3_sprites.create_sprite( 96, 64 + 16, 4),},
            _bg{bn::regular_bg_items::intro_movie_tail_peak_1.create_bg(0, 45)}
            {
                start(interrupted);
            }
        

    private:
        void update(){
            if(_frame > 80 && _frame < _duration - 260){
                _cat_head.set_y(_cat_head.y() - bn::fixed(0.19));
                for(bn::sprite_ptr sprite : _bush_sprites) sprite.set_y(sprite.y() - bn::fixed(0.1));
                _bg.set_y(_bg.y() - bn::fixed(0.6));
            }
            bn::core::update();
        }

        void start(bool& interrupted){
            for(int i = 0; i < _duration; i++){
                if(bn::keypad::any_pressed()) {
                    interrupted = true;
                    break;
                }

                if(i <= _fade_speed){
                    bn::fixed max;
                    _fade_progress = 1 - bn::fixed(i)/_fade_speed;
                    max = bn::max(_fade_progress, bn::fixed(0));

                    bn::sprite_palettes::set_fade(_fade_color, max);
                    bn::bg_palettes::set_fade(_fade_color, max);

                    update();
                }else if(i < _duration - _fade_speed){
                    update();
                }else{
                    bn::fixed min;
                    _fade_progress = bn::fixed(i - _duration + _fade_speed)/_fade_speed;
                    min = bn::min(_fade_progress, bn::fixed(1));

                    bn::sprite_palettes::set_fade(_fade_color, min);
                    bn::bg_palettes::set_fade(_fade_color, min);

                    update();
                }
                _frame++;
            }
        }

        uint8_t _fade_speed;
        int _frame;
        const int  _duration;
        bn::color _fade_color;
        bn::fixed _volume_decrement, _fade_progress;

        bn::sprite_ptr _cat_head;
        bn::array<bn::sprite_ptr, 4> _bush_sprites;
        bn::regular_bg_ptr _bg;
    };

    class Shot_4{
    public:
        Shot_4(bool& interrupted):
            _fade_speed(fadespeed::SLOW),
            _frame(0),
            _duration(500),
            _fade_color(bn::colors::black),
            _cat_head(bn::sprite_items::intro_shot_3_sprites.create_sprite(0, 58 + 32, 0)),
            _bush_sprites{bn::sprite_items::intro_shot_3_sprites.create_sprite(-96, 64 + 16, 1),
                          bn::sprite_items::intro_shot_3_sprites.create_sprite(-32, 64 + 16, 2),
                          bn::sprite_items::intro_shot_3_sprites.create_sprite( 32, 64 + 16, 3),
                          bn::sprite_items::intro_shot_3_sprites.create_sprite( 96, 64 + 16, 4),},
            _bg{bn::regular_bg_items::intro_movie_tail_peak_1.create_bg(0, 45)}
            {
                start(interrupted);
            }
        

    private:
        void update(){
            if(_frame > 80 && _frame < _duration - 260){
                _cat_head.set_y(_cat_head.y() - bn::fixed(0.19));
                for(bn::sprite_ptr sprite : _bush_sprites) sprite.set_y(sprite.y() - bn::fixed(0.1));
                _bg.set_y(_bg.y() - bn::fixed(0.6));
            }
            bn::core::update();
        }

        void start(bool& interrupted){
            for(int i = 0; i < _duration; i++){
                if(bn::keypad::any_pressed()) {
                    interrupted = true;
                    break;
                }

                if(i <= _fade_speed){
                    bn::fixed max;
                    _fade_progress = 1 - bn::fixed(i)/_fade_speed;
                    max = bn::max(_fade_progress, bn::fixed(0));

                    bn::sprite_palettes::set_fade(_fade_color, max);
                    bn::bg_palettes::set_fade(_fade_color, max);

                    update();
                }else if(i < _duration - _fade_speed){
                    update();
                }else{
                    bn::fixed min;
                    _fade_progress = bn::fixed(i - _duration + _fade_speed)/_fade_speed;
                    min = bn::min(_fade_progress, bn::fixed(1));

                    bn::sprite_palettes::set_fade(_fade_color, min);
                    bn::bg_palettes::set_fade(_fade_color, min);

                    update();
                }
                _frame++;
            }
        }

        uint8_t _fade_speed;
        int _frame;
        const int  _duration;
        bn::color _fade_color;
        bn::fixed _volume_decrement, _fade_progress;

        bn::sprite_ptr _cat_head;
        bn::array<bn::sprite_ptr, 4> _bush_sprites;
        bn::regular_bg_ptr _bg;
    };

    bool _interrupted;
};
}

#endif