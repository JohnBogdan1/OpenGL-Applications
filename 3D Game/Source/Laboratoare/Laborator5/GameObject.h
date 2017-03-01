#pragma once

#include <string>
#include <include/glm.h>

class GameObject
{
public:
	glm::mat4 modelMatrix;
	glm::vec3 position;
	glm::vec3 scale;
	float speed;
	float rotation;
	glm::vec3 size;
	std::string name;
	int healthPoints;
};