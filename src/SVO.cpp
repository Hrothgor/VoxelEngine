#include "SVO.h"

SVO::SVO(uint16_t depth)
{
	_Depth = depth == 0 ? 1 : depth;
	_Size = 1 << depth;
}

SVO::~SVO()
{
}

void SVO::Build(const std::vector<float>& heightMap)
{
	_Octree.clear();


}

void SVO::Build_Internal(const std::vector<float>& heightMap, uint16_t depth, uint16_t parentX, uint16_t parentY, uint16_t parentZ)
{

}