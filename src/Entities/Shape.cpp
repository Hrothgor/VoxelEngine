#include "Shape.hpp"
#include "Logger.hpp"
#include "Profiler.hpp"
#include <FastNoise/FastNoise.h>

Shape::Shape()
{
}

Shape::~Shape()
{
}

void Shape::Build()
{
	Profiler::Get()->StartFrame("Shape::Build");

	float *noiseVolume = new float[_Size.x * _Size.y * _Size.z];
	auto fnPerlin = FastNoise::New<FastNoise::Perlin>();
	fnPerlin->GenUniformGrid3D(noiseVolume, 0, 0, 0, _Size.x, _Size.y, _Size.z, 0.02, 1337);

	for (int i = 0; i < _Size.x * _Size.y * _Size.z; i++)
	{
		if (noiseVolume[i] > 0.0)
		{
			_Data[i] = (rand() % 10 + 1);
		}
		else
		{
			_Data[i] = 0;
		}
	}

	delete[] noiseVolume;

	Logger::Get()->Log(Logger::INFO, "Shape built --------");
	Logger::Get()->Log(Logger::INFO, "\tSize: %d %d %d", _Size.x, _Size.y, _Size.z);
	Logger::Get()->Log(Logger::INFO, "\tMax size:     %d", _Size.x * _Size.y * _Size.z);
	Logger::Get()->Log(Logger::INFO, "\tMemory size (KB): %d KB", _Data.size() / 1024);
	Logger::Get()->Log(Logger::INFO, "\tMemory size (MB): %d MB", _Data.size() / 1024 / 1024);
	Logger::Get()->Log(Logger::INFO, "-----------------");

	Profiler::Get()->EndFrame();
    Logger::Get()->Log(Logger::LogType::INFO, "Time to build Shape with noise: %.3f ms", Profiler::Get()->GetFrameTime("Shape::Build"));
}

void Shape::GenTexture()
{
	// Check if size is divisible by 4
	if (_Size.x % 4 != 0 || _Size.y % 4 != 0 || _Size.z % 4 != 0)
	{
		Logger::Get()->Log(Logger::ERROR, "Size of the shape is not divisible by 4");
		return;
	}

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
    glTexImage3D(GL_TEXTURE_3D, 0, GL_R8, _Size.x, _Size.y, _Size.z, 0, GL_RED, GL_UNSIGNED_BYTE, _Data.data());
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_3D, 0);

    // load material into a 2D texture
	glGenTextures(1, &_MaterialTexture);
    glBindTexture(GL_TEXTURE_2D, _MaterialTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, 8, MAX_MATERIALS, 0, GL_RED, GL_FLOAT, _MaterialPalette);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);


}

void Shape::SetMaterial(int index, const Material &material)
{
	if (index < 0 || index > MAX_MATERIALS)
	{
		Logger::Get()->Log(Logger::ERROR, "Material index out of range: %d", index);
		return;
	}
	_MaterialPalette[index] = material;
}