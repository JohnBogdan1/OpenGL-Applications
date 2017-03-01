#include "GameObject.h"
#include "Player.h"

using namespace std;

Player::Player() {}

Player::~Player() {}

void Player::setPosition(float x, float y) {
	this->positionX = x;
	this->positionY = y;
}

Mesh* Player::CreateObject(std::string name, glm::vec3 leftBottomCorner, float length, glm::vec3 color) {
	glm::vec3 corner = leftBottomCorner;

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(corner, color),
		VertexFormat(corner + glm::vec3(length, 0, 0), color),
		VertexFormat(corner + glm::vec3(length, length, 0), color),
		VertexFormat(corner + glm::vec3(0, length, 0), color),
		VertexFormat(corner + glm::vec3(length / 2, length / 2, 0), color)
	};

	Mesh* player = new Mesh(name);
	std::vector<unsigned short> indices = {
		0, 1, 4,
		4, 3, 2,
	};

	player->InitFromData(vertices, indices);
	return player;
}