#ifndef __Ray_hpp__
#define __Ray_hpp__

#include "Point.hpp"

/**
    A ray in K dimensional space
*/
template<int K>
class Ray {
public:
    Ray(Point<K> origin, Point<K> direction) : origin_(origin), direction_(direction) {
        direction_.Normalise();
    };

    Point<K>& Origin() {
        return origin_;
    }

    Point<K>& Direction() {
        return direction_;
    }

private:
    Point<K> origin_;
    Point<K> direction_;
};
/* The octree is going to use 3D rays */
typedef Ray<3> Ray3D;


#endif
