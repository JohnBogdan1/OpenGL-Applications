#pragma once

class HealthBar : public GameObject {
public:
	HealthBar();

	~HealthBar();

	Mesh* HealthBar::CreateObject(std::string name, glm::vec3 leftBottomCorner, float length, glm::vec3 color);

};