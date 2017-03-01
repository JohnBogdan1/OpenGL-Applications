#pragma once
#include <Component/SimpleScene.h>
#include <Component/Transform/Transform.h>
#include <Core/GPU/Mesh.h>
#include "Drop.h"

class Laborator8 : public SimpleScene
{
public:
	Laborator8();
	~Laborator8();

	void Init() override;

private:
	void FrameStart() override;
	void Update(float deltaTimeSeconds) override;
	void FrameEnd() override;

	void RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix, bool wave, float deltaTime, const glm::vec3 &color = glm::vec3(1));
	Mesh* CreateMesh(const char *name, const std::vector<VertexFormat> &vertices, const std::vector<unsigned short> &indices);

	void OnInputUpdate(float deltaTime, int mods) override;
	void OnKeyPress(int key, int mods) override;
	void OnKeyRelease(int key, int mods) override;
	void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
	void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
	void OnWindowResize(int width, int height) override;

	void Laborator8::GenerateGrid();

	glm::vec3 lightPosition;
	glm::vec3 lightDirection;
	std::vector<Drop*> drops;
	Drop *drop;
	int dropId = 0;
	unsigned int materialShininess;
	float materialKd;
	float materialKs;
	GLint light_type = 0;
	GLfloat angle = 30.0f;
	std::vector<VertexFormat> vertices;
	std::vector<unsigned short> indices;
	int gridSize = 101;
	glm::vec3 ballPosition;
	bool showGridEdges = false;
	float g = 9.80665f;
	int wavesNumber = 0;

	std::vector<float> A;
	std::vector<glm::vec2> C;
	std::vector<float> circle_radius;
	std::vector<float> desired_wave_length;

	float change_speed = 1.0f;

	bool bonus = false;
};
