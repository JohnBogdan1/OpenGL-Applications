#include "GameObject.h"
#include "HealthBar.h"

using namespace std;

HealthBar::HealthBar() {}

HealthBar::~HealthBar() {}

Mesh* HealthBar::CreateObject(std::string name, glm::vec3 leftBottomCorner, float length, glm::vec3 color) {
	glm::vec3 corner = leftBottomCorner;

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(corner, color),
		VertexFormat(corner + glm::vec3(length, 0, 0), color),
		VertexFormat(corner + glm::vec3(length, length, 0), color),
		VertexFormat(corner + glm::vec3(0, length, 0), color),
	};

	Mesh* hb = new Mesh(name);
	std::vector<unsigned short> indices = {
		0, 1, 2,
		2, 3, 0,
	};

	hb->InitFromData(vertices, indices);
	return hb;
}