#ifndef __UniformGrid_hpp__
#define __UniformGrid_hpp__

#include <iostream>
#include <stdexcept>
#include <vector>
#include <math.h>

template<typename T> static T map_to_range(T in_a, T in_b, T in_t, T out_a, T out_b) {
    return out_a + ((out_b - out_a) / (in_b - in_a)) * (in_t - in_a);
}

template<typename T, size_t D>
class UniformGrid {
public:
    UniformGrid(size_t dimension_size) {
        dimension_size_ = dimension_size;
        array_ = std::vector<T>(static_cast<size_t>(std::pow(dimension_size, D)));
    }

    ~UniformGrid() {
        array_.clear();
    }

    template<typename I, typename ... Args>
    T& at(I first, Args ... rest) {
        const int size = sizeof...(rest) + 1;
        if (size != D) {
            std::string error("Wrong number of arguments");
            throw std::invalid_argument(error);
        }

        size_t index = unpack(D-1, first, rest...);
        return array_[index];
    }

private:
    std::vector<T> array_;
    size_t dimension_size_;
    
    template<typename I>
    size_t unpack(size_t level, I last) {
        return last * static_cast<size_t>(std::pow(dimension_size_, level));
    }

    template<typename I, typename ... Args>
    size_t unpack(size_t level, I first, Args ... rest) {
        return first * static_cast<size_t>(std::pow(dimension_size_, level)) + unpack(level - 1, rest...);
    }

};

#endif