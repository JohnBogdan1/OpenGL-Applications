#pragma once
class Enemy : public GameObject {
public:
	Enemy();

	~Enemy();

	void setPosition(float x, float y);

	Mesh* Enemy::CreateObject(std::string name, glm::vec3 leftBottomCorner, float length, glm::vec3 color);

public:
	float enemyScaleStep = 0.0f;
};
