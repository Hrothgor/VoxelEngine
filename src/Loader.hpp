#pragma once

#ifndef LOADER_HPP_
#define LOADER_HPP_

#include "Include.hpp"

class Shape;

class Loader {
    private:
        struct VoxChunkHeader {
            char Id[4];
            uint32_t  BytesOfContent;
            uint32_t  BytesOfChildren;
        };
        struct VoxModel {
            uint32_t SizeX;
            uint32_t SizeY;
            uint32_t SizeZ;
            uint32_t NumVoxels;
            uint32_t *Voxels;
        };
    public:
        struct VoxContent {
            uint32_t NumModels = 0;
            VoxModel *Models;
            uint32_t *Palette;
        };

    private:
        static VoxChunkHeader ReadVoxChunkHeader(std::ifstream &file);
        static void ReadVoxChunkPACK(std::ifstream &file, VoxContent &voxContent);
        static void ReadVoxChunkSIZE(std::ifstream &file, VoxContent &voxContent, int modelIndex);
        static void ReadVoxChunkXYZI(std::ifstream &file, VoxContent &voxContent, int modelIndex);
        static void ReadVoxChunkRGBA(std::ifstream &file, VoxContent &voxContent);

    public:
        static VoxContent LoadFromVoxFile(const std::string &path);
        static Shape *ConvertVoxToShape(const VoxContent &voxContent);
};

#endif /* !LOADER_HPP_ */