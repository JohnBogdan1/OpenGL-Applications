#include "GameObject.h"
#include "Gift.h"

using namespace std;

Gift::Gift() {}

Gift::~Gift() {}

void Gift::setPosition(float x, float y) {
	this->positionX = x;
	this->positionY = y;
}

Mesh* Gift::CreateObject(std::string name, glm::vec3 leftBottomCorner, float length, glm::vec3 color) {
	glm::vec3 corner = leftBottomCorner;

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(corner, color),
		VertexFormat(corner + glm::vec3(length, 0, 0), color),
		VertexFormat(corner + glm::vec3(length, length, 0), color),
	};

	Mesh* projectile = new Mesh(name);
	std::vector<unsigned short> indices = {
		0, 1, 2,
	};

	projectile->InitFromData(vertices, indices);
	return projectile;
}