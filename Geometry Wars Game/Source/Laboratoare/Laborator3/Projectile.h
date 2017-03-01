#pragma once

class Projectile : public GameObject {
public:
	Projectile();

	~Projectile();

	void setPosition(float x, float y);

	Mesh* Projectile::CreateObject(std::string name, glm::vec3 leftBottomCorner, float length, glm::vec3 color);

};