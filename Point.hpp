#ifndef __Point_hpp__
#define __Point_hpp__

#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>

#include <glm/glm.hpp>

/* Container type around a real number, to easiliy switch between float and double */
typedef double Real_t;

/**
    Check if a and b are equal, within a certain margin
*/
template<typename T> bool Equal(const T& a, const T& b, T e = std::numeric_limits<T>::epsilon()) {
    return std::abs(a - b) <= e;
}

/**
    A point in K dimensional space
*/
template<int K>
class Point {
public:
    Point() {
        coordinates_ = std::vector<Real_t>(K, 0);
    }

    Point(Real_t a) {
        coordinates_ = std::vector<Real_t>(K, a);
    }

    Point(std::vector<Real_t> coordinates) {
        coordinates_ = coordinates;
    }

    Real_t Norm() {
        Real_t a = 0;
        for (size_t i = 0; i < K; i++) {
            a += coordinates_[i] * coordinates_[i];
        }
        return sqrt(a);
    }

    void Normalise() {
        Real_t norm = Norm();
        for (size_t i = 0; i < K; i++) {
            coordinates_[i] = coordinates_[i] / norm;
        }
    }

    Point& operator+=(const Point& rhs) {
        for (size_t i = 0; i < K; i++)
            coordinates_[i] += rhs.coordinates_[i];
        return *this;
    }

    friend Point operator+(Point lhs, const Point& rhs) {
        lhs += rhs;
        return lhs; 
    }

    Point& operator-=(const Point& rhs) {
        for (size_t i = 0; i < K; i++)
            coordinates_[i] -= rhs.coordinates_[i];
        return *this;
    }

    friend Point operator-(Point lhs, const Point& rhs) {
        lhs -= rhs;
        return lhs;
    }

    bool operator==(const Point& rhs) {
        bool ret = true;
        for (size_t i = 0; i < K; i++) {
            ret = ret & Equal(coordinates_[i], rhs.coordinates_[i]);
        }
        return ret;
    }

    Real_t& operator[](size_t i) {
        return coordinates_[i];
    }

    friend std::ostream& operator<<(std::ostream& os, Point const& h) {
        os << "[";
        for (size_t i = 0; i < K-1; i++) {
            os << h.coordinates_[i] << ", ";
        }
        os << h.coordinates_[0] << "]";
        return os;
    }

private:
    std::vector<Real_t> coordinates_;

};
/* The octree is going to used 3D points */
typedef Point<3> Point3D;

#endif