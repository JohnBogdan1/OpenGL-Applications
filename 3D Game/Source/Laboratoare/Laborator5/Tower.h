#pragma once

#include "Enemy.h"

class Tower : public GameObject {
public:
	Tower();

	~Tower();

	void setPosition(float x, float y, float z);

public:
	Enemy *targetedEnemy = nullptr;
	float projectileFreqTime;
};