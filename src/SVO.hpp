#pragma once

#ifndef SVO_HPP_
#define SVO_HPP_

#include "Include.hpp"

class SVO {
public:
    struct Node {
        uint32_t data = 0;
        uint32_t children[8];
    };

    SVO(uint16_t depth = 2);
    ~SVO();

    void Build(const std::vector<float>& heightMap);
    std::vector<SVO::Node> GetOctree() const;

protected:
private:
    int Build_Internal(const std::vector<float>& heightMap, uint16_t depth, uint16_t rootX, uint16_t rootY, uint16_t rootZ);

    uint16_t _Depth;
    int32_t _Size;
    std::vector<Node> _Octree;
};


#endif /* !SVO_HPP_ */