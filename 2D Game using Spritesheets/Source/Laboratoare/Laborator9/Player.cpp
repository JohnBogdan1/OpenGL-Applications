#include "Player.h"

using namespace std;

Player::Player() {}

Player::~Player() {}

void Player::setPosition(float x, float y) {
	this->position.x = x;
	this->position.y = y;
}