#pragma once

class Player : public GameObject {
public:
	Player();

	~Player();

	void setPosition(float x, float y, float z);
};