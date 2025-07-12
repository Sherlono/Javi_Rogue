#ifndef JV_TILED_BG_H
#define JV_TILED_BG_H

#include "bn_point.h"
#include "bn_unique_ptr.h"
#include "bn_camera_ptr.h"

#include "jv_global.h"
#include "jv_map_classes.h"

#include "bn_regular_bg_tiles_items_fortress_tiles.h"
#include "bn_bg_palette_items_fortress_palette.h"

class game_map;

namespace jv{
class tiled_bg{
public:
    tiled_bg(game_map m, const uint8_t priority = 0);
    
    // Getters
    [[nodiscard]] int width() const { return map.x();}
    [[nodiscard]] int height() const { return map.y();}
    [[nodiscard]] int size() const { return map.x()*map.y();}
    [[nodiscard]] bn::fixed_point position() const { return bg.position();}

    // Setters
    void set_camera(bn::camera_ptr& camera){
        bg.set_camera(camera);
    }
    void remove_camera(){
        bg.remove_camera();
    }
    void set_priority(int priority){
        bg.set_priority(priority);
    }
    void set_visible(bool visible){ bg.set_visible(visible);}
    void set_blending_enabled(bool isBlend){ bg.set_blending_enabled(isBlend);}

    // Functionality
    void update();
    void init();

    game_map map;
private:
    int prev_x, prev_y;
    bn::unique_ptr<bg_map> bg_m_ptr;
    bn::regular_bg_ptr bg;
    bn::regular_bg_map_ptr bg_m;
};

class animated_prop{
public:
    virtual ~animated_prop() = default;
    animated_prop(): _time(0) {}

    virtual void update(game_map& map) = 0;
protected:
    uint8_t _time;
};

class torch : public animated_prop{
/*######
  ##╭╮##
  ##┑┍##
  ######*/
public:
    torch(int x, int y, game_map& map): animated_prop(), _top_left(x, y)
    {
        // Bottom half
        map.set_cell(x, y+1, 97);
        map.set_cell(x+1, y+1, 98);
        // Top half
        map.set_cell(x, y, 99);
        map.set_cell(x+1, y, 100);
    }

    void update(game_map& map) override{
        _time++;
        if(_time < _wait_frames){
            map.set_cell(_top_left.x(), _top_left.y(), 99);
            map.set_cell(_top_left.x() + 1, _top_left.y(), 100);
        }else if(_time < _wait_frames*2){
            map.set_cell(_top_left.x(), _top_left.y(), 101);
            map.set_cell(_top_left.x() + 1, _top_left.y(), 102);
        }else if(_time < _wait_frames*3){
            map.set_cell(_top_left.x(), _top_left.y(), 103);
            map.set_cell(_top_left.x() + 1, _top_left.y(), 104);
        }else{
            _time = 0;
        }
    }
    
private:
    static constexpr uint8_t _wait_frames = 4;
    bn::point _top_left;
};

}

#endif