#include "GameObject.h"
#include "Enemy.h"

using namespace std;

Enemy::Enemy() {}

Enemy::~Enemy() {}

void Enemy::setPosition(float x, float y, float z) {
	this->position.x = x;
	this->position.y = y;
	this->position.z = z;
}