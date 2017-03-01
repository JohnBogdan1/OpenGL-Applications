#pragma once

class Player : public GameObject {
public:
	Player();

	~Player();

	void setPosition(float x, float y);

	Mesh* Player::CreateObject(std::string name, glm::vec3 leftBottomCorner, float length, glm::vec3 color);

public:
	float invincibilityElapsedTime = 99999;
	bool isInvincible = false;
};