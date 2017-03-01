#pragma once
#include <Component/SimpleScene.h>
#include <Component/Transform/Transform.h>
#include <Core/GPU/Mesh.h>
#include <string>
#include <vector>
#include <include/glm.h>
#include "pugixml.hpp"
#include "Player.h"
#include "Enemy.h"
#include "Animation.h"
#include "Frame.h"

class Laborator9 : public SimpleScene
{
	public:
		Laborator9();
		~Laborator9();

		struct ViewportSpace
		{
			ViewportSpace() : x(0), y(0), width(1), height(1) {}
			ViewportSpace(int x, int y, int width, int height)
				: x(x), y(y), width(width), height(height) {}
			int x;
			int y;
			int width;
			int height;
		};

		void Init() override;

	private:
		void FrameStart() override;
		void Update(float deltaTimeSeconds) override;
		void FrameEnd() override;

		void RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix, Texture2D* texture1 = NULL, Texture2D* texture2 = NULL);
		void RecursiveParsePlayer(pugi::xml_node tools);
		void RecursiveParseEnemy(pugi::xml_node tools);
		void CreateMeshWithTextureCoords(std::string name, std::vector<glm::vec2> textureCoords);
		bool DetectCollision(glm::vec2 player_position, glm::vec2 enemy_position);

		void SetViewportArea(const ViewportSpace & viewSpace, glm::vec3 colorColor, bool clear);

		void OnInputUpdate(float deltaTime, int mods) override;
		void OnKeyPress(int key, int mods) override;
		void OnKeyRelease(int key, int mods) override;
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
		void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
		void OnWindowResize(int width, int height) override;

		int const fixedZ = -2;
		int const backgroundArenaZ = -3;
		float width = 2.0f;
		float height = 3.0f;
		std::unordered_map<std::string, Texture2D*> mapTextures;

		ViewportSpace viewSpace;
		GLuint randomTextureID;

		int factor = 1;

		Player *player;
		Enemy *enemy;
		Frame *frame = nullptr;
		Animation *animation = nullptr;
};
