#pragma once

#ifndef SVO_HPP_
#define SVO_HPP_

#include "Include.hpp"

class SVO {
public:
    SVO(uint16_t depth = 2);
    ~SVO();

    void Build(const std::vector<float>& heightMap);
    void TempDisplay();

    struct Node {
		int32_t data = 0;
	};
protected:
private:
    void Build_Internal(const std::vector<float>& heightMap, uint16_t depth, uint16_t rootX, uint16_t rootY, uint16_t rootZ);
    void TempDisplay_Internal();

    uint16_t _Depth;
    int32_t _Size;
    std::vector<Node> _Octree;
};


#endif /* !SVO_HPP_ */