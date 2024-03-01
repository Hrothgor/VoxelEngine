#include "Transform.hpp"
#include "Logger.hpp"
#include <FastNoise/FastNoise.h>

Transform::Transform(const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &scale)
{
	_Position = position;
	_Rotation = rotation;
	_Scale = scale;
}

Transform::~Transform()
{
}

glm::mat4 Transform::GetTransformMatrix() const
{
	glm::mat4 matrix(1.0f);
	matrix = glm::rotate(matrix, glm::radians(_Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	matrix = glm::rotate(matrix, glm::radians(_Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	matrix = glm::rotate(matrix, glm::radians(_Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	matrix = glm::translate(matrix, -_Position);
	matrix = glm::scale(matrix, _Scale);
	return glm::inverse(matrix);
}