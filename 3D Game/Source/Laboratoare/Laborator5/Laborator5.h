#pragma once
#include <Component/SimpleScene.h>
#include "LabCamera.h"
#include "GameObject.h"
#include "Player.h"
#include "Enemy.h"
#include "Tower.h"
#include "Projectile.h"
#include "Transform3D.h"

class Laborator5 : public SimpleScene
{
public:
	Laborator5();
	~Laborator5();

	void Init() override;

private:
	void FrameStart() override;
	void Update(float deltaTimeSeconds) override;
	void FrameEnd() override;

	void RenderMesh(Mesh * mesh, Shader * shader, const glm::mat4 & modelMatrix) override;
	void myRenderMesh(Mesh * mesh, Shader * shader, const glm::mat4 & modelMatrix);

	void OnInputUpdate(float deltaTime, int mods) override;
	void OnKeyPress(int key, int mods) override;
	void OnKeyRelease(int key, int mods) override;
	void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
	void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
	void OnWindowResize(int width, int height) override;

	void RenderPlayer(float deltaTimeSeconds);
	void GenerateAndRenderEnemies(float deltaTimeSeconds);
	void RenderTowers(float deltaTimeSeconds);
	void RenderHealthPoints(float deltaTimeSeconds);
	void RenderWeapon();
	void RenderProjectiles(std::vector<Projectile*> projectiles, float deltaTimeSeconds);
	void RenderTowerProjectiles(std::vector<Projectile*> projectiles, float deltaTimeSeconds);
	void CreateProjectile();
	glm::vec3 GetObjectInitialSize(std::vector<glm::vec3> positions);
	bool DetectCollisionProjectileEnemy(Projectile *projectile, Enemy *enemy);
	void CreateTowerProjectile(glm::vec3 position, float rotation);

protected:
	// cameras
	Laborator::Camera *camera, *topCamera;

	// the model Matrix
	glm::mat4 modelMatrix;

	// objects used for the world's objects
	Player *player;
	Enemy *enemy;
	Tower *tower1, *tower2, *tower3;
	Projectile *pistolProjectile, *rifleProjectile, *grenadeLauncherProjectile, *towerProjectile;

	// arrays where to store those objects
	std::vector<Enemy*> enemies;
	std::vector<Projectile*> pistolProjectiles, rifleProjectiles, grenadeLauncherProjectiles, towerProjectiles;
	std::vector<glm::vec3> mazeCorners = { {-10.0f, 0.0f, -10.0f}, {10.0f, 0.0f, -10.0f}, {-10.0f, 0.0f, 10.0f}, {10.0f, 0.0f, 10.0f} };
	float angularStepOX = 0, angularStepOY = 0, angularStepOZ = 0;

	// used for the enemy apparition and projectile creation
	double preTime, currTime, offset, extracter;
	float projectilePreTime = 0.0f, projectileCurrTime, projectileOfsset;

	int enemyId = 0, initialEnemyHP = 1, pistolProjectileId = 0, rifleProjectileId = 0, grenadeLauncherProjectileId = 0, towerProjectileId = 0;
	float initialScaleFactor = 1.0f;
	float apparitionTime = -0.5f;

	// tower range
	float towerCircleRadius = 6.0f;

	// when the players dies, use this for his rotation
	float OxPlayerAngle = 0.0f;

	// to know what weapon is hold by the player
	bool zoom = false, renderPistol = false, renderGrenadeLauncher = false;

	// field of view - perspective camera
	float fov = RADIANS(60);

	// used for the orthographic camera
	float bottom = -10.0f, top = 10.0f;

	// box used for the minimap
	Mesh *minimapBoxMesh;

	glm::mat4 projectionMatrix, minimapProjectionMatrix;
};
