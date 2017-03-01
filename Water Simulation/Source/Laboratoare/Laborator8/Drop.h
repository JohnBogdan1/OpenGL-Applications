#pragma once

#include <string>
#include <include/glm.h>

class Drop {
public:
	Drop();

	~Drop();

	void setPosition(float x, float y, float z);

public:
	glm::vec3 position;
	glm::vec3 scale;
	float speed;
	bool hitGround;
	int bouncing;
	float newHeight;
	float factor = 1.0f;
	std::string name;
	int healthPoints;
};