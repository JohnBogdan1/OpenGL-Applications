#include "GameObject.h"
#include "Projectile.h"

using namespace std;

Projectile::Projectile() {}

Projectile::~Projectile() {}

void Projectile::setPosition(float x, float y) {
	this->positionX = x;
	this->positionY = y;
}

Mesh* Projectile::CreateObject(std::string name, glm::vec3 leftBottomCorner, float length, glm::vec3 color) {
	glm::vec3 corner = leftBottomCorner;

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(corner, color),
		VertexFormat(corner + glm::vec3(length, 0, 0), color),
		VertexFormat(corner + glm::vec3(length, length, 0), color),
		VertexFormat(corner + glm::vec3(0, length, 0), color),
	};

	Mesh* projectile = new Mesh(name);
	std::vector<unsigned short> indices = {
		0, 1, 3,
		3, 1, 2,
	};

	projectile->InitFromData(vertices, indices);
	return projectile;
}