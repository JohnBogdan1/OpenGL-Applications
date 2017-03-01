#pragma once

class Gift : public GameObject {
public:
	Gift();

	~Gift();

	void setPosition(float x, float y);

	Mesh* Gift::CreateObject(std::string name, glm::vec3 leftBottomCorner, float length, glm::vec3 color);

};