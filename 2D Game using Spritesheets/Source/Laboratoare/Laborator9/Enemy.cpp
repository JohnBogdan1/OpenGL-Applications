#include "Enemy.h"

using namespace std;

Enemy::Enemy() {}

Enemy::~Enemy() {}

void Enemy::setPosition(float x, float y) {
	this->position.x = x;
	this->position.y = y;
}