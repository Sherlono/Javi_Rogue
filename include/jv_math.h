#ifndef JV_MATH_H
#define JV_MATH_H

#include "bn_point.h"
#include "bn_fixed.h"

namespace jv{
bn::fixed xproduct(bn::fixed_point a, bn::fixed_point b, bn::fixed_point t){  // Keep in mind the Y axis is inverted
    if (a.x() != b.x()){
        return (b.x() - a.x()) * (-t.y() + a.y()) + (b.y() - a.y()) * (t.x() - a.x());  // > 0 = above
    }else{
        return a.x() - t.x();   // > 0 = left
    }
}

class para{     // Paralelogram
public:
    ~para(){}
    para(){}
    para(bn::fixed x, bn::fixed y, bn::fixed width, bn::fixed height, bn::fixed vshear = 0):_x(x), _y(y){
        _top_left = bn::fixed_point(_x - (width/2), _y - vshear - (height/2));
        _top_right = bn::fixed_point(_x + (width/2), _y + vshear - (height/2));
        _bottom_left = bn::fixed_point(_x - (width/2), _y - vshear + (height/2));
        _bottom_right = bn::fixed_point(_x + (width/2), _y + vshear + (height/2));
    }

    [[nodiscard]] bn::fixed x(){
        return _x;
    }
    [[nodiscard]] bn::fixed y(){
        return _y;
    }
    [[nodiscard]] bn::fixed_point xy(){
        return bn::fixed_point(_x, _y);
    }

    [[nodiscard]] bn::fixed_point top_left(){
        return _top_left;
    }
    [[nodiscard]] bn::fixed_point top_right(){
        return _top_right;
    }
    [[nodiscard]] bn::fixed_point bottom_left(){
        return _bottom_left;
    }
    [[nodiscard]] bn::fixed_point bottom_right(){
        return _bottom_right;
    }
    
    [[nodiscard]] bool contains_top_left(jv::para other){
        if(xproduct(this->_top_left, this->_top_right, other.top_left()) <= 0 && xproduct(this->_top_left, this->_bottom_left, other.top_left()) <= 0){
            if(xproduct(this->_bottom_left, this->_bottom_right, other.top_left()) >= 0 && xproduct(this->_top_right, this->_bottom_right, other.top_left()) >= 0){
                return true;
            }
        }
        return false;
    }
    [[nodiscard]] bool contains_top_right(jv::para other){
        if(xproduct(this->_top_left, this->_top_right, other.top_right()) <= 0 && xproduct(this->_top_left, this->_bottom_left, other.top_right()) <= 0){
            if(xproduct(this->_bottom_left, this->_bottom_right, other.top_right()) >= 0 && xproduct(this->_top_right, this->_bottom_right, other.top_right()) >= 0){
                return true;
            }
        }
        return false;
    }
    [[nodiscard]] bool contains_bottom_left(jv::para other){
        if(xproduct(this->_top_left, this->_top_right, other.bottom_left()) <= 0 && xproduct(this->_top_left, this->_bottom_left, other.bottom_left()) <= 0){
            if(xproduct(this->_bottom_left, this->_bottom_right, other.bottom_left()) >= 0 && xproduct(this->_top_right, this->_bottom_right, other.bottom_left()) >= 0){
                return true;
            }
        }
        return false;
    }
    [[nodiscard]] bool contains_bottom_right(jv::para other){
        if(xproduct(this->_top_left, this->_top_right, other.bottom_right()) <= 0 && xproduct(this->_top_left, this->_bottom_left, other.bottom_right()) <= 0){
            if(xproduct(this->_bottom_left, this->_bottom_right, other.bottom_right()) >= 0 && xproduct(this->_top_right, this->_bottom_right, other.bottom_right()) >= 0){
                return true;
            }
        }
        return false;
    }

    [[nodiscard]] bool contains(bn::fixed_point point){
        if(xproduct(this->_top_left, this->_top_right, point) <= 0 && xproduct(this->_top_left, this->_bottom_left, point) <= 0){
            if(xproduct(this->_bottom_left, this->_bottom_right, point) >= 0 && xproduct(this->_top_right, this->_bottom_right, point) >= 0){
                return true;
            }
        }
        return false;
    }

    [[nodiscard]] bool touches(jv::para other){
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
    [[nodiscard]] bool touches(bn::rect other){
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

    void set_position(bn::fixed new_x, bn::fixed new_y){
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
        this->_top_left.set_x(other.top_left().x());
        this->_top_left.set_y(other.top_left().y());
        this->_top_right.set_x(other.top_right().x());
        this->_top_right.set_y(other.top_right().y());
        this->_bottom_left.set_x(other.bottom_left().x());
        this->_bottom_left.set_y(other.bottom_left().y());
        this->_bottom_right.set_x(other.bottom_right().x());
        this->_bottom_right.set_y(other.bottom_right().y());
    }

protected:
    void set_points(bn::fixed_point new_TL, bn::fixed_point new_TR, bn::fixed_point new_BL, bn::fixed_point new_BR){
        _top_left = new_TL;
        _top_right = new_TR;
        _bottom_left = new_BL;
        _bottom_right = new_BR;
    }
    
private:
    bn::fixed _x, _y;
    bn::fixed_point _top_left, _top_right, _bottom_left, _bottom_right;
};
}

#endif