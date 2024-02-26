#pragma once

#ifndef SHAPE_HPP_
#define SHAPE_HPP_

#include "Include.hpp"

class Shape {
public:
    Shape(int size = 256);
    ~Shape();

    void Build();
    
    int GetSize() const { return _Size; }
    const std::vector<int8_t>& GetData() const { return _Data; }

    std::vector<int8_t> _Data; // 0 = empty, 1 = full
protected:
private:
    int _Size;
};


#endif /* !SHAPE_HPP_ */