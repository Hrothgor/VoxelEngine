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
// retrun the index of the added node
int SVO::Build_Internal(const std::vector<float>& heightMap, uint16_t depth, uint16_t rootX, uint16_t rootY, uint16_t rootZ)
{
	float MySize = _Size / (1 << depth);

	//check if this cube is full of same element in the heightmap
	bool IsSame = true;

	int FirstElement = heightMap[rootX + rootY * MySize + rootZ * MySize * MySize] >= 0.0 ? 1 : 0;
	for (uint16_t x = rootX; x < rootX + MySize; x++)
	{
		for (uint16_t y = rootY; y < rootY + MySize; y++)
		{
			for (uint16_t z = rootZ; z < rootZ + MySize; z++)
			{
				int ActualElement = heightMap[x + y * MySize + z * MySize * MySize] >= 0.0 ? 1 : 0;
				if (ActualElement != FirstElement)
				{
					IsSame = false;
					break;
				}
			}
		}
	}

	// int FirstElement = ((heightMap[rootX + rootY * MySize] + 1) / 2) * (_Size / 2) + (_Size / 4) >= rootZ ? 1 : 0;
	// for (uint16_t x = rootX; x < rootX + MySize; x++)
	// {
	// 	for (uint16_t y = rootY; y < rootY + MySize; y++)
	// 	{
	// 		for (uint16_t z = rootZ; z < rootZ + MySize; z++)
	// 		{
	// 			int ActualElement = ((heightMap[x + y * MySize] + 1) / 2) * (_Size / 2) + (_Size / 4) >= z ? 1 : 0;
	// 			if (ActualElement != FirstElement)
	// 			{
	// 				IsSame = false;
	// 				break;
	// 			}
	// 		}
	// 	}
	// }
			
	if (IsSame)
	{
		Node node;
		node.data |= FirstElement; // set the first bit to 1 if the element is filled
		node.data |= 1 << 31; // set the last bit to 1 to indicate that this node is a leaf

		_Octree.push_back(node);
		return (_Octree.size() - 1);
	} else {
		Node node;
		node.data = 0;

		node.children[0] = Build_Internal(heightMap, depth + 1, rootX, rootY, rootZ);
		node.children[1] = Build_Internal(heightMap, depth + 1, rootX + MySize / 2, rootY, rootZ);
		node.children[2] = Build_Internal(heightMap, depth + 1, rootX, rootY + MySize / 2, rootZ);
		node.children[3] = Build_Internal(heightMap, depth + 1, rootX + MySize / 2, rootY + MySize / 2, rootZ);

		node.children[4] = Build_Internal(heightMap, depth + 1, rootX, rootY, rootZ + MySize / 2);
		node.children[5] = Build_Internal(heightMap, depth + 1, rootX + MySize / 2, rootY, rootZ + MySize / 2);
		node.children[6] = Build_Internal(heightMap, depth + 1, rootX, rootY + MySize / 2, rootZ + MySize / 2);
		node.children[7] = Build_Internal(heightMap, depth + 1, rootX + MySize / 2, rootY + MySize / 2, rootZ + MySize / 2);

		_Octree.push_back(node);
		return (_Octree.size() - 1);
	}
}

std::vector<SVO::Node> SVO::GetOctree() const
{
	return (_Octree);
}