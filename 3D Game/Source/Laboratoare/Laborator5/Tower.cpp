#include "GameObject.h"
#include "Tower.h"

using namespace std;

Tower::Tower() {}

Tower::~Tower() {}

void Tower::setPosition(float x, float y, float z) {
	this->position.x = x;
	this->position.y = y;
	this->position.z = z;
}