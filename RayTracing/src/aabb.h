#pragma once

#include "Utilities.h"
#include <iostream>
class aabb {
public:
    aabb() {}
    aabb(const point3& a, const point3& b) { minimum = a; maximum = b; }

    point3 min() const { return minimum; }
    point3 max() const { return maximum; }
    static aabb surrounding_box(const aabb& box0, aabb box1) { 
        point3 small(fmin(box0.min().x, box1.min().x),
                     fmin(box0.min().y, box1.min().y),
                     fmin(box0.min().z, box1.min().z));

        point3 big(fmax(box0.max().x, box1.max().x),
                   fmax(box0.max().y, box1.max().y),
                   fmax(box0.max().z, box1.max().z));
            
        return aabb(small,big);
    }
  
    bool hit(const ray& r, float t_min, float t_max) const {
        
         auto t0 = (minimum.x - r.orig.x) * r.invDir.x;
         auto t1 = (maximum.x - r.orig.x) * r.invDir.x;
       
       
        t_min = std::max((r.invDir.x < 0)? t1:t0, t_min);
        t_max = std::min((r.invDir.x < 0) ? t0:t1, t_max);
        if (t_max <= t_min)
            return false;
        t0 = (minimum.y - r.orig.y) * r.invDir.y;
        t1 = (maximum.y - r.orig.y) * r.invDir.y;
       
        t_min = std::max((r.invDir.y < 0) ? t1 : t0, t_min);
        t_max = std::min((r.invDir.y < 0) ? t0 : t1, t_max);
        if (t_max <= t_min)
            return false;
        t0 = (minimum.z - r.orig.z) * r.invDir.z;
        t1 = (maximum.z - r.orig.z) * r.invDir.z;
      
        t_min = std::max((r.invDir.z < 0) ? t1 : t0, t_min);
        t_max = std::min((r.invDir.z < 0) ? t0 : t1, t_max);
        if (t_max <= t_min)
            return false;
              
        return true;
    }
    point3 minimum;
    point3 maximum;
};