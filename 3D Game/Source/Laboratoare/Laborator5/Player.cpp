#include "GameObject.h"
#include "Player.h"

using namespace std;

Player::Player() {}

Player::~Player() {}

void Player::setPosition(float x, float y, float z) {
	this->position.x = x;
	this->position.y = y;
	this->position.z = z;
}