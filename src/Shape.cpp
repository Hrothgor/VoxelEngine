#include "Shape.hpp"
#include "Logger.hpp"
#include <FastNoise/FastNoise.h>

Shape::Shape(int size)
{
	_Size = size;
	_Size = std::clamp(_Size, 1, 256);
	_Data.resize(_Size * _Size * _Size, 0);


	// Test Material
	_MaterialPalette[0].color = glm::vec4(0.41, 0.18, 0.02, 1.0);
	_MaterialPalette[1].color = glm::vec4(0.45, 0.28, 0.16, 1.0);
	_MaterialPalette[2].color = glm::vec4(0.65, 0.26, 0.12, 1.0);
	_MaterialPalette[3].color = glm::vec4(0.71, 0.36, 0.13, 1.0);
	_MaterialPalette[4].color = glm::vec4(0.75, 0.47, 0.14, 1.0);
	_MaterialPalette[5].color = glm::vec4(0.80, 0.50, 0.16, 1.0);
	_MaterialPalette[6].color = glm::vec4(0.85, 0.55, 0.20, 1.0);
	_MaterialPalette[7].color = glm::vec4(0.90, 0.60, 0.25, 1.0);
	_MaterialPalette[8].color = glm::vec4(0.95, 0.65, 0.30, 1.0);
	_MaterialPalette[9].color = glm::vec4(1.00, 0.70, 0.35, 1.0);
}

Shape::~Shape()
{
}

void Shape::Build()
{
	auto start = std::chrono::high_resolution_clock::now();

	float *noiseVolume = new float[_Size * _Size * _Size];
	auto fnPerlin = FastNoise::New<FastNoise::Perlin>();
	fnPerlin->GenUniformGrid3D(noiseVolume, 0, 0, 0, _Size, _Size, _Size, 0.02, 1337);

	for (int i = 0; i < _Size * _Size * _Size; i++)
	{
		if (noiseVolume[i] > 0.0)
		{
			_Data[i] = rand() % 10 + 1;
		}
		else
		{
			_Data[i] = 0;
		}
	}

	delete[] noiseVolume;

	Logger::Get()->Log(Logger::INFO, "Shape built --------");
	Logger::Get()->Log(Logger::INFO, "\tSize:  %d", _Size);
	Logger::Get()->Log(Logger::INFO, "\tMax size:     %d", _Size * _Size * _Size);
	Logger::Get()->Log(Logger::INFO, "\tMemory size (KB): %d KB", _Data.size() / 1024);
	Logger::Get()->Log(Logger::INFO, "\tMemory size (MB): %d MB", _Data.size() / 1024 / 1024);
	Logger::Get()->Log(Logger::INFO, "-----------------");

    auto finish = std::chrono::high_resolution_clock::now();
    Logger::Get()->Log(Logger::LogType::INFO, "Time to build Shape with noise: %.3f ms", std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(finish - start).count());
}

void Shape::GenTexture()
{
	if (_VolumeTexture != 0)
	{
		glDeleteTextures(1, &_VolumeTexture);
	}
	if (_MaterialTexture != 0)
	{
		glDeleteTextures(1, &_MaterialTexture);
	}

    // load data into a 3D texture
    glGenTextures(1, &_VolumeTexture);
    glBindTexture(GL_TEXTURE_2D, _VolumeTexture);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_R8, _Size, _Size, _Size, 0, GL_RED, GL_UNSIGNED_BYTE, _Data.data());
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_3D, 0);

    // load material into a 2D texture
	glGenTextures(1, &_MaterialTexture);
    glBindTexture(GL_TEXTURE_2D, _MaterialTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, 8, 255, 0, GL_RED, GL_FLOAT, _MaterialPalette);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);


}

void Shape::SetMaterial(int index, const Material &material)
{
	if (index < 0 || index > 255)
	{
		Logger::Get()->Log(Logger::ERROR, "Material index out of range: %d", index);
		return;
	}
	_MaterialPalette[index] = material;
}