#pragma once

#ifndef SVO_HPP_
#define SVO_HPP_

#include <vector>

class SVO {
public:
    SVO(uint16_t depth = 2);
    ~SVO();

    void Build(const std::vector<float>& heightMap);

    struct Node {
		int32_t data;
	};
protected:
private:
    void Build_Internal(const std::vector<float>& heightMap, uint16_t depth, uint16_t parentX, uint16_t parentY, uint16_t parentZ);

    uint16_t _Depth;
    int32_t _Size;
    std::vector<Node> _Octree;
};


#endif /* !SVO_HPP_ */