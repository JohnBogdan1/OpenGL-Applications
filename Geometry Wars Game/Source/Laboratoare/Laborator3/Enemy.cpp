#include "GameObject.h"
#include "Enemy.h"

using namespace std;

Enemy::Enemy() {}

Enemy::~Enemy() {}

void Enemy::setPosition(float x, float y) {
	this->positionX = x;
	this->positionY = y;
}

Mesh* Enemy::CreateObject(std::string name, glm::vec3 leftBottomCorner, float length, glm::vec3 color) {
	glm::vec3 corner = leftBottomCorner;

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(corner, color),
		VertexFormat(corner + glm::vec3(length, 0, 0), color),
		VertexFormat(corner + glm::vec3(length, length, 0), color),
		VertexFormat(corner + glm::vec3(0, length, 0), color),
		VertexFormat(corner + glm::vec3(length / 2, length / 2, 0), color)
	};

	Mesh* enemy = new Mesh(name);
	std::vector<unsigned short> indices = {
		2, 3, 4,
		1, 4, 0,
	};

	enemy->InitFromData(vertices, indices);
	return enemy;
}
