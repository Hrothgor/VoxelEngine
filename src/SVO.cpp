#include "SVO.hpp"
#include <GLFW/glfw3.h>
#include <iostream>
#include "Logger.hpp"

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

	Logger::Get()->Log(Logger::INFO, "SVO built --------");
	Logger::Get()->Log(Logger::INFO, "\tDepth: %d", _Depth);
	Logger::Get()->Log(Logger::INFO, "\tSize: %d", _Size);
	Logger::Get()->Log(Logger::INFO, "\tMax size: %d", _Size * _Size * _Size);
	Logger::Get()->Log(Logger::INFO, "\tOctree size: %d", _Octree.size());
	Logger::Get()->Log(Logger::INFO, "\tOctree memory size (KB): %d KB", _Octree.size() * sizeof(Node) / 1024);
	Logger::Get()->Log(Logger::INFO, "\tOctree memory size (MB): %d MB", _Octree.size() * sizeof(Node) / 1024 / 1024);
}

// heightMap is a 2D array of size _Size * _Size
// retrun the index of the added node
int SVO::Build_Internal(const std::vector<float>& heightMap, uint16_t depth, uint16_t rootX, uint16_t rootY, uint16_t rootZ)
{
	float MySize = _Size / (1 << depth);

	//check if this cube is full of same element in the heightmap
	bool IsSame = true;

	// int FirstElement = heightMap[rootX + rootY * MySize + rootZ * MySize * MySize] >= 0.0 ? 1 : 0;
	// for (uint16_t x = rootX; x < rootX + MySize; x++)
	// {
	// 	for (uint16_t y = rootY; y < rootY + MySize; y++)
	// 	{
	// 		for (uint16_t z = rootZ; z < rootZ + MySize; z++)
	// 		{
	// 			int ActualElement = heightMap[x + y * MdySize + z * MySize * MySize] >= 0.0 ? 1 : 0;
	// 			if (ActualElement != FirstElement)
	// 			{
	// 				IsSame = false;
	// 				break;
	// 			}
	// 		}
	// 	}
	// }

	int FirstElement = ((heightMap[rootX + rootZ * MySize] + 1) / 2) * (_Size / 8) + (_Size / 4) >= rootY ? 1 : 0;
	for (uint16_t x = rootX; x < rootX + MySize; x++)
	{
		for (uint16_t z = rootZ; z < rootZ + MySize; z++)
		{
			for (uint16_t y = rootY; y < rootY + MySize; y++)
			{
				int ActualElement = ((heightMap[x + z * MySize] + 1) / 2) * (_Size / 8) + (_Size / 4) >= y ? 1 : 0;
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
		Node node;
		node.data |= FirstElement; // set the first bit to 1 if the element is filled
		node.data |= 1 << 31; // set the last bit to 1 to indicate that this node is a leaf

		_Octree.push_back(node);
		return (_Octree.size() - 1);
	} else {
		Node node;
		node.data = FirstElement;

		_Octree.push_back(node);
		int index = _Octree.size() - 1;

		_Octree[index].children[0] = Build_Internal(heightMap, depth + 1, rootX				, rootY				, rootZ);
		_Octree[index].children[1] = Build_Internal(heightMap, depth + 1, rootX + MySize / 2, rootY				, rootZ);
		_Octree[index].children[2] = Build_Internal(heightMap, depth + 1, rootX				, rootY + MySize / 2, rootZ);
		_Octree[index].children[3] = Build_Internal(heightMap, depth + 1, rootX + MySize / 2, rootY + MySize / 2, rootZ);

		_Octree[index].children[4] = Build_Internal(heightMap, depth + 1, rootX				, rootY				, rootZ + MySize / 2);
		_Octree[index].children[5] = Build_Internal(heightMap, depth + 1, rootX + MySize / 2, rootY				, rootZ + MySize / 2);
		_Octree[index].children[6] = Build_Internal(heightMap, depth + 1, rootX				, rootY + MySize / 2, rootZ + MySize / 2);
		_Octree[index].children[7] = Build_Internal(heightMap, depth + 1, rootX + MySize / 2, rootY + MySize / 2, rootZ + MySize / 2);

		return (index);
	}
}

std::vector<SVO::Node> SVO::GetOctree() const
{
	return (_Octree);
}