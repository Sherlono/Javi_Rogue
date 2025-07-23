#ifndef JV_fog_bg_H
#define JV_fog_bg_H

#include "bn_rect.h"
#include "bn_assert.h"
#include "bn_utility.h"
#include "bn_display.h"
#include "bn_blending.h"
#include "bn_camera_ptr.h"
#include "bn_type_traits.h"
#include "bn_regular_bg_ptr.h"
#include "bn_rect_window_actions.h"
#include "bn_rect_window_boundaries_hbe_ptr.h"

#include "jv_constants.h"

#include "bn_regular_bg_items_darkness.h"

#if LOGS_ENABLED
    #include "bn_log.h"
    #include "bn_string.h"
    static_assert(LOGS_ENABLED, "Log is not enabled");
#endif

namespace jv{

class iFog{
    using h_bounds_type = bn::array<bn::pair<bn::fixed, bn::fixed>, bn::display::height()>;
    
public:
    ~iFog() noexcept = default;

    // Constructors
    iFog(const iFog& other) = delete;

    iFog(bn::rect& data, int max_size):
        _rooms(&data),
        _max_size(max_size),
        current_room(-1), _x(0), _y(0), _width(0), _height(0),
        _window(bn::window::internal()),
        _internal_window(bn::rect_window::internal()),
        _horizontal_boundaries_hbe(bn::rect_window_boundaries_hbe_ptr::create_horizontal(_internal_window, _horizontal_boundaries)),
        _fog_bg(bn::regular_bg_items::darkness.create_bg(0, 0))
    {
        _fog_bg.set_priority(0);
        _fog_bg.set_blending_enabled(true);
        _window.set_show_bg(_fog_bg, false);
    }

    // Setters
    void set_position(int x, int y){
        _x = x;
        _y = y;
    }

    void set_position(bn::point position){
        _x = position.x();
        _y = position.y();
    }

    void set_visible(bool visible){
        _fog_bg.set_visible(visible);
    }

    void set_boundaries(int w, int a){
        _width = w;
        _height = a;
    }
    
    void set_shape(bn::point position, int w, int a){
        set_position(position.x(), position.y());
        set_boundaries(w, a);
    }
    
    void set_shape(bn::rect shape){
        set_position(shape.x(), shape.y());
        set_boundaries(shape.width()>>1, shape.height()>>1);
    }

    // Getters
    [[nodiscard]] bn::rect* end()
    {
        return _rooms + _size;
    }
    [[nodiscard]] int size() const
    {
        return _size;
    }

    [[nodiscard]] bool full() const
    {
        return _size == _max_size;
    }

    [[nodiscard]] bool visible(){ return _fog_bg.visible();}

    // Functionality
    void clear()
    {
        _size = 0;
    }

    void reset(){
        current_room = -1;
        clear();
    }

    void create_room(const bn::rect& value)
    {
        BN_BASIC_ASSERT(! full(), "Vector is full");

        ::new(static_cast<void*>(end())) bn::rect(value);
        ++_size;
    }

    void update();

protected:
    void _assign(const iFog& other)
    {
        bn::rect* data = _rooms;
        const bn::rect* other_rooms = other._rooms;
        int other_size = other._size;
        _size = other_size;

        for(int index = 0; index < other_size; ++index)
        {
            ::new(static_cast<void*>(data + index)) bn::rect(other_rooms[index]);
        }
    }

    void _assign(iFog&& other)
    {
        bn::rect* data = _rooms;
        bn::rect* other_rooms = other._rooms;
        int other_size = other._size;
        _size = other_size;

        for(int index = 0; index < other_size; ++index)
        {
            ::new(static_cast<void*>(data + index)) bn::rect(move(other_rooms[index]));
        }

        other.clear();
    }

    bn::rect* _rooms;
    int _size = 0;
    int _max_size;
    
    int8_t current_room;
    int _x, _y;
    int _width, _height;
    bn::window _window;
    bn::rect_window _internal_window;
    bn::rect_window_boundaries_hbe_ptr _horizontal_boundaries_hbe;
    bn::regular_bg_ptr _fog_bg;
    h_bounds_type _horizontal_boundaries;
    
    static constexpr int half_display_height = bn::display::height()>>1;
};

template <int MaxSize>
class Fog: public iFog{
    static_assert(MaxSize > 0);
public:
    ~Fog(){ clear();}
    
    Fog():
        iFog(*reinterpret_cast<bn::rect*>(_storage_buffer), MaxSize)
        {}
        
private:
    alignas(bn::rect) char _storage_buffer[sizeof(bn::rect) * MaxSize];
};

}

#endif