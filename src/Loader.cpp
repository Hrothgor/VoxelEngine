#include "Loader.hpp"
#include <fstream>
#include "Entities/Shape.hpp"
#include "Logger.hpp"
    
Loader::VoxChunkHeader Loader::ReadVoxChunkHeader(std::ifstream &file)
{
	VoxChunkHeader chunk;
	file.read(chunk.Id, sizeof(1*4));
	file.read(reinterpret_cast<char*>(&chunk.BytesOfContent), sizeof(4));
	file.read(reinterpret_cast<char*>(&chunk.BytesOfChildren), sizeof(4));
	return chunk;
}

void Loader::ReadVoxChunkPACK(std::ifstream &file, VoxContent &voxContent)
{
	uint32_t numModels;
	file.read(reinterpret_cast<char*>(&numModels), sizeof(4));
	voxContent.NumModels = numModels;
	voxContent.Models = new VoxModel[numModels];
}

void Loader::ReadVoxChunkSIZE(std::ifstream &file, VoxContent &voxContent, int modelIndex)
{
	if (voxContent.NumModels == 1)
		voxContent.Models = new VoxModel[1];

	uint32_t sizeX, sizeY, sizeZ;
	file.read(reinterpret_cast<char*>(&sizeX), sizeof(4));
	file.read(reinterpret_cast<char*>(&sizeY), sizeof(4));
	file.read(reinterpret_cast<char*>(&sizeZ), sizeof(4));
	voxContent.Models[modelIndex].SizeX = sizeX;
	voxContent.Models[modelIndex].SizeY = sizeY;
	voxContent.Models[modelIndex].SizeZ = sizeZ;
}

void Loader::ReadVoxChunkXYZI(std::ifstream &file, VoxContent &voxContent, int modelIndex)
{
	uint32_t numVoxels;
	file.read(reinterpret_cast<char*>(&numVoxels), sizeof(4));
	voxContent.Models[modelIndex].NumVoxels = numVoxels;
	voxContent.Models[modelIndex].Voxels = new uint32_t[numVoxels];
	file.read(reinterpret_cast<char*>(voxContent.Models[modelIndex].Voxels), numVoxels * sizeof(4));
}

void Loader::ReadVoxChunkRGBA(std::ifstream &file, VoxContent &voxContent)
{
	voxContent.Palette = new uint32_t[256];
	file.read(reinterpret_cast<char*>(voxContent.Palette), 256 * sizeof(4));
}

Shape *Loader::ConvertVoxToShape(const VoxContent &voxContent)
{
	Shape *shape = new Shape();

	// Switch Y and Z axis
	std::swap(voxContent.Models[0].SizeY, voxContent.Models[0].SizeZ);

	bool voxXoffset = false;
	bool voxYoffset = false;
	bool voxZoffset = false;
	if (voxContent.Models[0].SizeX % 4 != 0)
	{
		voxContent.Models[0].SizeX += 4 - (voxContent.Models[0].SizeX % 4);
		voxXoffset = true;
	}
	if (voxContent.Models[0].SizeY % 4 != 0)
	{
		voxContent.Models[0].SizeY += 4 - (voxContent.Models[0].SizeY % 4);
		voxYoffset = true;
	}
	if (voxContent.Models[0].SizeZ % 4 != 0)
	{
		voxContent.Models[0].SizeZ += 4 - (voxContent.Models[0].SizeZ % 4);
		voxZoffset = true;
	}

	shape->SetSize(glm::u16vec3(voxContent.Models[0].SizeX, voxContent.Models[0].SizeY, voxContent.Models[0].SizeZ));
	
	std::vector<int8_t> data(voxContent.Models[0].SizeX * voxContent.Models[0].SizeY * voxContent.Models[0].SizeZ, 0);
	for (int i = 0; i < voxContent.Models[0].NumVoxels; i++)
	{
		uint32_t voxel = voxContent.Models[0].Voxels[i];
		uint32_t x = voxel >> 0 & 255;
		uint32_t y = voxel >> 8 & 255;
		uint32_t z = voxel >> 16 & 255;
		std::swap(y, z);
		uint32_t colorIndex = voxel >> 24 & 255;
		data[(x + voxXoffset) +
			(y + voxYoffset) * voxContent.Models[0].SizeX +
			(z + voxZoffset) * voxContent.Models[0].SizeX * voxContent.Models[0].SizeY] = colorIndex;
	}
	shape->SetData(data);

	for (int i = 0; i < 255; i++)
	{
		shape->SetMaterial(i, Material{glm::vec4(
			((voxContent.Palette[i] >> 0) & 255) / 255.0f,
			((voxContent.Palette[i] >> 8) & 255) / 255.0f,
			((voxContent.Palette[i] >> 16) & 255) / 255.0f,
			((voxContent.Palette[i] >> 24) & 255) / 255.0f
		)});
	}
	return shape;
}

Loader::VoxContent Loader::LoadFromVoxFile(const std::string &path)
{
	VoxContent voxContent;
    std::ifstream file(path, std::ios::binary);

	Logger::Get()->Log(Logger::LogType::INFO, "Loading .vox file: %s", path.c_str());

    if (!file.is_open())
    {
        Logger::Get()->Log(Logger::LogType::ERROR, "Failed to open file: %s", path.c_str());
        return voxContent;
    }

	// Read header
    char magic[4];
    file.read(magic, 4);
    if (magic[0] != 'V' || magic[1] != 'O' || magic[2] != 'X' || magic[3] != ' ') {
        Logger::Get()->Log(Logger::LogType::ERROR, "Invalid .vox format: %s", path.c_str());
        return voxContent;
    }
	// Read version
	uint32_t version;
    file.read(reinterpret_cast<char*>(&version), sizeof(version));
	Logger::Get()->Log(Logger::LogType::INFO, "Version: %d", version);

	// MAIN chunk
	VoxChunkHeader mainChunkHeader = ReadVoxChunkHeader(file);
	if (strncmp(mainChunkHeader.Id, "MAIN", 4) != 0)
	{
		Logger::Get()->Log(Logger::LogType::ERROR, "Can't find MAIN chunk");
		return voxContent;
	}
	voxContent.NumModels = 1;
	int currentModelIndex = 0;
	int currentPos = file.tellg();
	while (file.tellg() < mainChunkHeader.BytesOfChildren + currentPos)
	{
		VoxChunkHeader chunkHeader = ReadVoxChunkHeader(file);
		if (strncmp(chunkHeader.Id, "PACK", 4) == 0)
		{
			ReadVoxChunkPACK(file, voxContent);
		} 
		else if (strncmp(chunkHeader.Id, "SIZE", 4) == 0)
		{
			ReadVoxChunkSIZE(file, voxContent, currentModelIndex);
		}
		else if (strncmp(chunkHeader.Id, "XYZI", 4) == 0)
		{
			ReadVoxChunkXYZI(file, voxContent, currentModelIndex);
			currentModelIndex++;
		}
		else if (strncmp(chunkHeader.Id, "RGBA", 4) == 0)
		{
			ReadVoxChunkRGBA(file, voxContent);
		}
		else
		{
			file.seekg(chunkHeader.BytesOfContent, std::ios::cur);
		}
	}
	
	Logger::Get()->Log(Logger::LogType::SUCCESS, "Successfully loaded .vox file.");

    file.close();
    return voxContent;
}