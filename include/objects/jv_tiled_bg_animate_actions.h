#ifndef JV_TILED_BG_ANIMATE_ACTION_H
#define JV_TILED_BG_ANIMATE_ACTION_H

#include "bn_point.h"
#include "bn_array.h"
#include "bn_vector.h"
#include "bn_limits.h"
#include "bn_regular_bg_ptr.h"
#include "bn_regular_bg_tiles_ptr.h"
#include "bn_regular_bg_tiles_item.h"

class GameMap;

namespace jv{
using tile_span = bn::span<const bn::tile>;

class itiled_bg_animate_action{
public:
    itiled_bg_animate_action(const itiled_bg_animate_action& other) = delete;

    void update()
    {
        BN_ASSERT(! done(), "Action is done");

        if(_current_wait_updates)
        {
            --_current_wait_updates;
        }
        else
        {
            const bn::ivector<uint16_t>& graphics_indexes = this->graphics_indexes();
            int current_graphics_indexes_index = _current_graphics_indexes_index;
            int current_graphics_index = graphics_indexes[current_graphics_indexes_index];
            _current_wait_updates = _wait_updates;

            if(current_graphics_indexes_index == 0 ||
                    graphics_indexes[current_graphics_indexes_index - 1] != current_graphics_index)
            {
                const bn::tile& tile = _tiles[current_graphics_index];
                _tiles_ref->overwrite_tile(_tile_index, tile);
            }

            if(_forever && current_graphics_indexes_index == graphics_indexes.size() - 1)
            {
                _current_graphics_indexes_index = 0;
            }
            else
            {
                ++_current_graphics_indexes_index;
            }
        }
    }

    [[nodiscard]] bool done() const
    {
        return _current_graphics_indexes_index == _graphics_indexes_ref->size();
    }

    void reset()
    {
        _current_graphics_indexes_index = 0;
        _current_wait_updates = 0;
    }

    [[nodiscard]] const bn::regular_bg_tiles_ptr& regular_bg() const
    {
        return *_tiles_ref;
    }

    [[nodiscard]] int wait_updates() const
    {
        return _wait_updates;
    }

    void set_wait_updates(int wait_updates){
        BN_ASSERT(wait_updates >= 0, "Invalid wait updates: ", wait_updates);
        BN_ASSERT(wait_updates <= std::numeric_limits<decltype(_wait_updates)>::max(),
                "Too many wait updates: ", wait_updates);

        _wait_updates = uint16_t(wait_updates);

        if(wait_updates < _current_wait_updates)
        {
            _current_wait_updates = uint16_t(wait_updates);
        }
    }

    [[nodiscard]] int next_change_updates() const
    {
        return _current_wait_updates;
    }

    void set_next_change_updates(int next_change_updates){
        BN_ASSERT(next_change_updates >= 0 && next_change_updates <= _wait_updates,
                "Invalid next change updates: ", next_change_updates, " - ", _wait_updates);

        _current_wait_updates = next_change_updates;
    }

    void set_tile_index(int index){
        _tile_index = index;
    }

    [[nodiscard]] const tile_span& tiles_item() const
    {
        return _tiles;
    }

    [[nodiscard]] const bn::ivector<uint16_t>& graphics_indexes() const
    {
        return *_graphics_indexes_ref;
    }

    [[nodiscard]] bool update_forever() const
    {
        return _forever;
    }

    [[nodiscard]] int current_index() const
    {
        return _current_graphics_indexes_index;
    }

    void set_current_index(int current_index){
        const bn::ivector<uint16_t>& graphics_indexes = this->graphics_indexes();
        int num_graphics_indexes = graphics_indexes.size();

        if(_forever)
        {
            BN_ASSERT(current_index >= 0 && current_index < num_graphics_indexes,
                    "Invalid current index: ", current_index, " - ", num_graphics_indexes);

            _current_graphics_indexes_index = current_index;
        }
        else
        {
            BN_ASSERT(current_index >= 0 && current_index <= num_graphics_indexes,
                    "Invalid current index: ", current_index, " - ", num_graphics_indexes);

            _current_graphics_indexes_index = current_index;

            if(current_index == num_graphics_indexes) --current_index;
        }

        int current_graphics_indexes_index = _current_graphics_indexes_index;
        int current_graphics_index = graphics_indexes[current_graphics_indexes_index];

        const bn::tile& tile = _tiles[current_graphics_index];
        _tiles_ref->overwrite_tile(_tile_index, tile);
    }

    [[nodiscard]] int current_graphics_index() const
    {
        return graphics_indexes()[_current_graphics_indexes_index];
    }

protected:
    itiled_bg_animate_action() = default;

    void _set_refs(bn::regular_bg_tiles_ptr& bg, const tile_span& tiles_item, bn::ivector<uint16_t>& graphics_indexes){
        _tiles_ref = &bg;
        _tiles = tiles_item;
        _graphics_indexes_ref = &graphics_indexes;
    }

    void _assign(const itiled_bg_animate_action& other){
        _wait_updates = other._wait_updates;
        _current_graphics_indexes_index = other._current_graphics_indexes_index;
        _current_wait_updates = other._current_wait_updates;
        _forever = other._forever;
    }

    void _set_update_forever(bool forever)
    {
        _forever = forever;
    }

    void _assign_graphics_indexes(const bn::span<const uint16_t>& graphics_indexes){
        BN_ASSERT(graphics_indexes.size() > 1 && graphics_indexes.size() <= _graphics_indexes_ref->max_size(),
                "Invalid graphics indexes count: ", graphics_indexes.size(), " - ", _graphics_indexes_ref->max_size());

        for(uint16_t graphics_index : graphics_indexes)
        {
            _graphics_indexes_ref->push_back(graphics_index);
        }
    }

    void _assign_graphics_indexes(const bn::ivector<uint16_t>& graphics_indexes){
        BN_ASSERT(graphics_indexes.size() > 1 && graphics_indexes.size() <= _graphics_indexes_ref->max_size(),
                "Invalid graphics indexes count: ", graphics_indexes.size(), " - ", _graphics_indexes_ref->max_size());

        *_graphics_indexes_ref = graphics_indexes;
    }

private:
    bn::regular_bg_tiles_ptr* _tiles_ref = nullptr;
    tile_span _tiles;

    bn::ivector<uint16_t>* _graphics_indexes_ref = nullptr;
    uint16_t _wait_updates = 0;
    uint16_t _tile_index = 0;
    uint16_t _current_graphics_indexes_index = 0;
    uint16_t _current_wait_updates = 0;
    bool _forever = true;
};


template<int MaxSize>
class tiled_bg_animate_action: public itiled_bg_animate_action{
    static_assert(MaxSize > 1);
public:
    [[nodiscard]] static tiled_bg_animate_action once(
            const bn::regular_bg_tiles_ptr& regular_bg, int wait_updates, int tile_index, const tile_span& tiles_item,
            const bn::span<const uint16_t>& graphics_indexes)
    {
        return tiled_bg_animate_action(regular_bg, wait_updates, tile_index, tiles_item, false, graphics_indexes);
    }

    [[nodiscard]] static tiled_bg_animate_action once(
            bn::regular_bg_tiles_ptr&& regular_bg, int wait_updates, int tile_index, const tile_span& tiles_item,
            const bn::span<const uint16_t>& graphics_indexes)
    {
        return tiled_bg_animate_action(std::move(regular_bg), wait_updates, tile_index, tiles_item, false, graphics_indexes);
    }

    [[nodiscard]] static tiled_bg_animate_action forever(
            const bn::regular_bg_tiles_ptr& regular_bg, int wait_updates, int tile_index, const tile_span& tiles_item,
            const bn::span<const uint16_t>& graphics_indexes)
    {
        return tiled_bg_animate_action(regular_bg, wait_updates, tile_index, tiles_item, true, graphics_indexes);
    }

    [[nodiscard]] static tiled_bg_animate_action forever(
            bn::regular_bg_tiles_ptr&& regular_bg, int wait_updates, int tile_index, const tile_span& tiles_item,
            const bn::span<const uint16_t>& graphics_indexes)
    {
        return tiled_bg_animate_action(std::move(regular_bg), wait_updates, tile_index, tiles_item, true, graphics_indexes);
    }

    tiled_bg_animate_action(const tiled_bg_animate_action& other) :
        _regular_bg_tiles(other._regular_bg_tiles),
        _graphics_indexes(other._graphics_indexes)
    {
        this->_set_refs(_regular_bg_tiles, _tiles, _graphics_indexes);
        this->_assign(other);
    }

    tiled_bg_animate_action(tiled_bg_animate_action&& other) noexcept :
        _regular_bg_tiles(std::move(other._regular_bg_tiles)),
        _graphics_indexes(other._graphics_indexes)
    {
        this->_set_refs(_regular_bg_tiles, _tiles, _graphics_indexes);
        this->_assign(other);
    }

    tiled_bg_animate_action(const itiled_bg_animate_action& other) :
        _regular_bg_tiles(other.regular_bg()),
        _graphics_indexes(other.graphics_indexes())
    {
        BN_ASSERT(other.graphics_indexes().size() <= MaxSize,
                  "Too many graphics indexes: ", other.graphics_indexes().size(), " - ", MaxSize);

        this->_set_refs(_regular_bg_tiles, _tiles, _graphics_indexes);
        this->_assign(other);
    }

    tiled_bg_animate_action& operator=(const tiled_bg_animate_action& other)
    {
        if(this != &other)
        {
            _regular_bg_tiles = other._regular_bg_tiles;
            _graphics_indexes = other._graphics_indexes;
            this->_assign(other);
        }

        return *this;
    }

    tiled_bg_animate_action& operator=(tiled_bg_animate_action&& other) noexcept
    {
        if(this != &other)
        {
            _regular_bg_tiles = std::move(other._regular_bg_tiles);
            _graphics_indexes = other._graphics_indexes;
            this->_assign(other);
        }

        return *this;
    }

    tiled_bg_animate_action& operator=(const itiled_bg_animate_action& other)
    {
        static_cast<itiled_bg_animate_action&>(*this) = other;
        return *this;
    }

    tiled_bg_animate_action& operator=(itiled_bg_animate_action&& other) noexcept
    {
        static_cast<itiled_bg_animate_action&>(*this) = std::move(other);
        return *this;
    }


private:
    bn::regular_bg_tiles_ptr _regular_bg_tiles;
    bn::vector<uint16_t, MaxSize> _graphics_indexes;

    tiled_bg_animate_action(const bn::regular_bg_tiles_ptr& regular_bg, int wait_updates, int tile_index, const tile_span& tiles_item,
                          bool forever, const bn::span<const uint16_t>& graphics_indexes) :
        _regular_bg_tiles(regular_bg)
    {
        this->_set_refs(_regular_bg_tiles, tiles_item, _graphics_indexes);
        this->_set_update_forever(forever);
        this->set_wait_updates(wait_updates);
        this->set_tile_index(tile_index);
        this->_assign_graphics_indexes(graphics_indexes);
    }

    tiled_bg_animate_action(bn::regular_bg_tiles_ptr&& regular_bg, int wait_updates, int tile_index, const tile_span& tiles_item, bool forever,
                          const bn::span<const uint16_t>& graphics_indexes) :
        _regular_bg_tiles(std::move(regular_bg))
    {
        this->_set_refs(_regular_bg_tiles, tiles_item, _graphics_indexes);
        this->_set_update_forever(forever);
        this->set_wait_updates(wait_updates);
        this->set_tile_index(tile_index);
        this->_assign_graphics_indexes(graphics_indexes);
    }
};



template<typename ...Args>
[[nodiscard]] auto create_tiled_bg_animate_action_once(
        const bn::regular_bg_tiles_ptr& regular_bg, int wait_updates, int tile_index, const tile_span& tiles_item, Args ...graphics_indexes)
{
    return tiled_bg_animate_action<sizeof...(Args)>::once(
                regular_bg, wait_updates, tile_index, tiles_item,
                bn::array<uint16_t, sizeof...(Args)>{{ uint16_t(graphics_indexes)... }});
}


template<typename ...Args>
[[nodiscard]] auto create_tiled_bg_animate_action_once(
        bn::regular_bg_tiles_ptr&& regular_bg, int wait_updates, int tile_index, const tile_span& tiles_item, Args ...graphics_indexes)
{
    return tiled_bg_animate_action<sizeof...(Args)>::once(
                std::move(regular_bg), wait_updates, tile_index, tiles_item,
                bn::array<uint16_t, sizeof...(Args)>{{ uint16_t(graphics_indexes)... }});
}


template<typename ...Args>
[[nodiscard]] auto create_tiled_bg_animate_action_forever(
        const bn::regular_bg_tiles_ptr& regular_bg, int wait_updates, int tile_index, const tile_span& tiles_item, Args ...graphics_indexes)
{
    return tiled_bg_animate_action<sizeof...(Args)>::forever(
                regular_bg, wait_updates, tile_index, tiles_item,
                bn::array<uint16_t, sizeof...(Args)>{{ uint16_t(graphics_indexes)... }});
}


template<typename ...Args>
[[nodiscard]] auto create_tiled_bg_animate_action_forever(
        bn::regular_bg_tiles_ptr&& regular_bg, int wait_updates, int tile_index, const tile_span& tiles_item, Args ...graphics_indexes)
{
    return tiled_bg_animate_action<sizeof...(Args)>::forever(
                std::move(regular_bg), wait_updates, tile_index, tiles_item,
                bn::array<uint16_t, sizeof...(Args)>{{ uint16_t(graphics_indexes)... }});
}


}

#endif