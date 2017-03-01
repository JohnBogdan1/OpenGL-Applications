#include "GameObject.h"
#include "Projectile.h"

using namespace std;

Projectile::Projectile() {}

Projectile::~Projectile() {}

void Projectile::setPosition(float x, float y, float z) {
	this->position.x = x;
	this->position.y = y;
	this->position.z = z;
}