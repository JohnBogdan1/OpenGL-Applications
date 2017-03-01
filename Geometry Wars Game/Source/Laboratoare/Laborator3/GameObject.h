#pragma once

#include <string>
#include <Core/Engine.h>
#include <include/glm.h>
#include <Core/GPU/Mesh.h>

class GameObject
{
private:
	virtual Mesh* GameObject::CreateObject(std::string name, glm::vec3 leftBottomCorner, float length, glm::vec3 color) = 0;
public:
	glm::mat3 modelMatrix;
	float positionX, positionY;
	glm::vec2 scale;
	float speed;
	float rotation;
	float length;
	std::string name;
	int healthPoints;
	glm::vec3 color;
};