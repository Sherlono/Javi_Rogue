#ifndef JV_MATH_H
#define JV_MATH_H

#include "bn_point.h"
#include "bn_rect.h"
#include "bn_fixed.h"

namespace jv{
int xproduct(bn::point a, bn::point b, bn::point t){  // Keep in mind the Y axis is inverted
    if (a.x() != b.x()){
        return (b.x() - a.x()) * (-t.y() + a.y()) + (b.y() - a.y()) * (t.x() - a.x());  // > 0 = above
    }else{
        return a.x() - t.x();   // > 0 = left
    }
}

class para{     // para is for Paralelogram
public:
    ~para(){}
    para(){}
    para(int x, int y, int width, int height, int vshear = 0):_x(x), _y(y){
        _top_left = bn::point(_x - (width/2), _y - vshear - (height/2));
        _top_right = bn::point(_x + (width/2), _y + vshear - (height/2));
        _bottom_left = bn::point(_x - (width/2), _y - vshear + (height/2));
        _bottom_right = bn::point(_x + (width/2), _y + vshear + (height/2));
    }

    [[nodiscard]] int x() const{ // Adding true in the arguments returns the "id" (simplified position) instead
        return _x;
    }
    [[nodiscard]] int y() const{
        return _y;
    }
    [[nodiscard]] bn::point xy() const{
        return bn::point(this->x(), this->y());
    }

    [[nodiscard]] bn::point top_left() const{
        return _top_left;
    }
    [[nodiscard]] bn::point top_right() const{
        return _top_right;
    }
    [[nodiscard]] bn::point bottom_left() const{
        return _bottom_left;
    }
    [[nodiscard]] bn::point bottom_right() const{
        return _bottom_right;
    }
    
    [[nodiscard]] bool contains_top_left(jv::para other) const{
        if(xproduct(this->_top_left, this->_top_right, other.top_left()) <= 0 && xproduct(this->_top_left, this->_bottom_left, other.top_left()) <= 0){
            if(xproduct(this->_bottom_left, this->_bottom_right, other.top_left()) >= 0 && xproduct(this->_top_right, this->_bottom_right, other.top_left()) >= 0){
                return true;
            }
        }
        return false;
    }
    [[nodiscard]] bool contains_top_right(jv::para other) const{
        if(xproduct(this->_top_left, this->_top_right, other.top_right()) <= 0 && xproduct(this->_top_left, this->_bottom_left, other.top_right()) <= 0){
            if(xproduct(this->_bottom_left, this->_bottom_right, other.top_right()) >= 0 && xproduct(this->_top_right, this->_bottom_right, other.top_right()) >= 0){
                return true;
            }
        }
        return false;
    }
    [[nodiscard]] bool contains_bottom_left(jv::para other) const{
        if(xproduct(this->_top_left, this->_top_right, other.bottom_left()) <= 0 && xproduct(this->_top_left, this->_bottom_left, other.bottom_left()) <= 0){
            if(xproduct(this->_bottom_left, this->_bottom_right, other.bottom_left()) >= 0 && xproduct(this->_top_right, this->_bottom_right, other.bottom_left()) >= 0){
                return true;
            }
        }
        return false;
    }
    [[nodiscard]] bool contains_bottom_right(jv::para other) const{
        if(xproduct(this->_top_left, this->_top_right, other.bottom_right()) <= 0 && xproduct(this->_top_left, this->_bottom_left, other.bottom_right()) <= 0){
            if(xproduct(this->_bottom_left, this->_bottom_right, other.bottom_right()) >= 0 && xproduct(this->_top_right, this->_bottom_right, other.bottom_right()) >= 0){
                return true;
            }
        }
        return false;
    }

    [[nodiscard]] bool contains(bn::point point) const{
        if(xproduct(this->_top_left, this->_top_right, point) <= 0 && xproduct(this->_top_left, this->_bottom_left, point) <= 0){
            if(xproduct(this->_bottom_left, this->_bottom_right, point) >= 0 && xproduct(this->_top_right, this->_bottom_right, point) >= 0){
                return true;
            }
        }
        return false;
    }

    [[nodiscard]] bool touches(jv::para other) const{
        if(this->contains(other.top_left())){           // Upper left corner check
            return true;
        }else if(this->contains(other.top_right())){    // Upper right corner check
            return true;
        }else if(this->contains(other.bottom_left())){  // Lower left corner check
            return true;
        }else if(this->contains(other.bottom_right())){ // Lower right corner check
            return true;
        }
        return false;
    }
    [[nodiscard]] bool touches(bn::rect other) const{
        if(this->contains(other.top_left())){           // Upper left corner check
            return true;
        }else if(this->contains(other.top_right())){    // Upper right corner check
            return true;
        }else if(this->contains(other.bottom_left())){  // Lower left corner check
            return true;
        }else if(this->contains(other.bottom_right())){ // Lower right corner check
            return true;
        }
        return false;
    }

    void set_position(int new_x, int new_y){
        _top_left.set_x(new_x + (_top_left.x() - _x));
        _top_left.set_y(new_y + (_top_left.y() - _y));
        _top_right.set_x(new_x + (_top_right.x() - _x));
        _top_right.set_y(new_y + (_top_right.y() - _y));
        _bottom_left.set_x(new_x + (_bottom_left.x() - _x));
        _bottom_left.set_y(new_y + (_bottom_left.y() - _y));
        _bottom_right.set_x(new_x + (_bottom_right.x() - _x));
        _bottom_right.set_y(new_y + (_bottom_right.y() - _y));

        _x = new_x;
        _y = new_y;
    }

    void copy(para other){
        this->set_points(other.top_left(), other.top_right(), other.bottom_left(), other.bottom_right());
        _x = other.x();
        _y = other.y();
    }

protected:
    void set_points(bn::point new_TL, bn::point new_TR, bn::point new_BL, bn::point new_BR){
        _top_left = new_TL;
        _top_right = new_TR;
        _bottom_left = new_BL;
        _bottom_right = new_BR;
    }
    
private:
    int _x, _y;
    bn::point _top_left, _top_right, _bottom_left, _bottom_right;
};
}

#endif