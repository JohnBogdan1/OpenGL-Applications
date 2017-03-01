#pragma once

class Enemy : public GameObject {
public:
	Enemy();

	~Enemy();

	void setPosition(float x, float y, float z);

public:
	glm::vec3 target = { 10.0f, 0.0f, -10.0f };
	float OxAngle = 0.0f;
};