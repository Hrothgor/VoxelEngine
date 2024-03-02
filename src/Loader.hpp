#pragma once

#ifndef LOADER_HPP_
#define LOADER_HPP_

#include "Include.hpp"

class Shape;

class Loader {
    public:
        Loader();
        ~Loader();

        Shape *LoadFromVoxFile(const std::string &path);

    protected:
    private:
        struct VoxChunkHeader {
            char Id[4];
            uint32_t  BytesOfContent;
            uint32_t  BytesOfChildren;
        };
        VoxChunkHeader ReadVoxChunkHeader(std::ifstream &file);
        struct VoxModel {
            uint32_t SizeX;
            uint32_t SizeY;
            uint32_t SizeZ;
            uint32_t NumVoxels;
            uint32_t *Voxels;
        };
        struct VoxContent {
            uint32_t NumModels = 1;
            VoxModel *Models;
            uint32_t *Palette;
        };
        void ReadVoxChunkPACK(std::ifstream &file, VoxContent &voxContent);
        void ReadVoxChunkSIZE(std::ifstream &file, VoxContent &voxContent, int modelIndex);
        void ReadVoxChunkXYZI(std::ifstream &file, VoxContent &voxContent, int modelIndex);
        void ReadVoxChunkRGBA(std::ifstream &file, VoxContent &voxContent);

        Shape *ConvertVoxToShape(const VoxContent &voxContent);

// SINGLETON
    protected:
        static Loader *instance;
    public:
        Loader(Loader &other) = delete;
        void operator=(const Loader &) = delete;
        static Loader *Get() {
            if (instance == nullptr)
                instance = new Loader();
            return instance;
        };
};

#endif /* !LOADER_HPP_ */