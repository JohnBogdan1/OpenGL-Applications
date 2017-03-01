#pragma once

class Projectile : public GameObject {
public:
	Projectile();

	~Projectile();

	void setPosition(float x, float y, float z);

public:
	float distance;
	glm::vec3 direction;
	int power;
	glm::vec3 initialPosition;
	float time = 0.0f;
};
