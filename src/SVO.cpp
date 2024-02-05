#include "SVO.hpp"
#include <GLFW/glfw3.h>
#include <iostream>

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

	Build_Internal(heightMap, 0, 0, 0, 0);

	std::cout << "Depth: " << _Depth << std::endl;
	std::cout << "Size: " << _Size << std::endl;
	std::cout << "Max size: " << _Size * _Size * _Size << std::endl;
	std::cout << "Octree size: " << _Octree.size() << std::endl;
}

// heightMap is a 2D array of size _Size * _Size
void SVO::Build_Internal(const std::vector<float>& heightMap, uint16_t depth, uint16_t rootX, uint16_t rootY, uint16_t rootZ)
{
	float MySize = _Size / (1 << depth);

	//check if this cube is full of same element in the heightmap
	bool IsSame = true;

	int FirstElement = ((heightMap[rootX + rootY * MySize] + 1) / 2) * (_Size / 2) + (_Size / 4) >= rootZ ? 1 : 0;
	for (uint16_t x = rootX; x < rootX + MySize; x++)
	{
		for (uint16_t y = rootY; y < rootY + MySize; y++)
		{
			for (uint16_t z = rootZ; z < rootZ + MySize; z++)
			{
				int ActualElement = ((heightMap[x + y * MySize] + 1) / 2) * (_Size / 2) + (_Size / 4) >= z ? 1 : 0;
				if (ActualElement != FirstElement)
				{
					IsSame = false;
					break;
				}
			}
		}
	}
			
	if (IsSame)
	{
		uint32_t node;
		node |= 1 << 31; // set the first bit to 1 to indicate that this node is a leaf
		node |= FirstElement <= 0 ? 0 : 1; // set the second bit to 1 if the element is filled
		_Octree.push_back(node);
		return;
	} else {
		uint32_t node;
		node |= 0 << 31; // set the first bit to 0 to indicate that this node is not a leaf
		_Octree.push_back(node);

		Build_Internal(heightMap, depth + 1, rootX, rootY, rootZ);
		Build_Internal(heightMap, depth + 1, rootX + MySize / 2, rootY, rootZ);
		Build_Internal(heightMap, depth + 1, rootX, rootY + MySize / 2, rootZ);
		Build_Internal(heightMap, depth + 1, rootX + MySize / 2, rootY + MySize / 2, rootZ);

		Build_Internal(heightMap, depth + 1, rootX, rootY, rootZ + MySize / 2);
		Build_Internal(heightMap, depth + 1, rootX + MySize / 2, rootY, rootZ + MySize / 2);
		Build_Internal(heightMap, depth + 1, rootX, rootY + MySize / 2, rootZ + MySize / 2);
		Build_Internal(heightMap, depth + 1, rootX + MySize / 2, rootY + MySize / 2, rootZ + MySize / 2);
	}
}

std::vector<int32_t> SVO::GetOctree() const
{
	return (_Octree);
}