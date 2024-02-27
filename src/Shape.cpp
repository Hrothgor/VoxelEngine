#include "Shape.hpp"
#include "Logger.hpp"
#include <FastNoise/FastNoise.h>

Shape::Shape(int size)
{
	_Size = size;
	_Size = std::clamp(_Size, 1, 256);
	_Data.resize(_Size * _Size * _Size, 0);
}

Shape::~Shape()
{
}

void Shape::Build()
{
	auto start = std::chrono::high_resolution_clock::now();

	float *noiseOutput = new float[_Size * _Size * _Size];
	auto fnSimplex = FastNoise::New<FastNoise::Perlin>();
	fnSimplex->GenUniformGrid3D(noiseOutput, 0, 0, 0, _Size, _Size, _Size, 0.02, 1337);
	
	for (int i = 0; i < _Size * _Size * _Size; i++)
	{
		_Data[i] = noiseOutput[i] > 0.0 ? 1 : 0;
	}

	delete[] noiseOutput;

	Logger::Get()->Log(Logger::INFO, "Shape built --------");
	Logger::Get()->Log(Logger::INFO, "\tSize:  %d", _Size);
	Logger::Get()->Log(Logger::INFO, "\tMax size:     %d", _Size * _Size * _Size);
	Logger::Get()->Log(Logger::INFO, "\tMemory size (KB): %d KB", _Data.size() / 1024);
	Logger::Get()->Log(Logger::INFO, "\tMemory size (MB): %d MB", _Data.size() / 1024 / 1024);
	Logger::Get()->Log(Logger::INFO, "-----------------");

    auto finish = std::chrono::high_resolution_clock::now();
    Logger::Get()->Log(Logger::LogType::INFO, "Time to build Shape with noise: %.3f ms", std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(finish - start).count());
}