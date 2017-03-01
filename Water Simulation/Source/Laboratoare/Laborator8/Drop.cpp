#include "Drop.h"

using namespace std;

Drop::Drop() {}

Drop::~Drop() {}

void Drop::setPosition(float x, float y, float z) {
	this->position.x = x;
	this->position.y = y;
	this->position.z = z;
}