#pragma once
#pragma comment(lib, "winmm.lib")

#include <Component/SimpleScene.h>
#include <string>
#include <Core/Engine.h>
#include "GameObject.h"
#include "Player.h"
#include "Enemy.h"
#include "HealthBar.h"
#include "Projectile.h"
#include "Gift.h"
#include <vector>
#include <mciapi.h>
#include <Mmsystem.h>

class Laborator3_Vis2D : public SimpleScene
{
public:
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

	struct LogicSpace
	{
		LogicSpace() : x(0), y(0), width(1), height(1) {}
		LogicSpace(float x, float y, float width, float height)
			: x(x), y(y), width(width), height(height) {}
		float x;
		float y;
		float width;
		float height;
	};

public:
	Laborator3_Vis2D();
	~Laborator3_Vis2D();

	void Init() override;

private:
	void FrameStart() override;
	void Update(float deltaTimeSeconds) override;
	void FrameEnd() override;

	void RenderHealthPoints();
	void RenderProjectile(std::vector<Projectile*> projectiles, float deltaTimeSeconds);
	bool DetectCollisionPlayerEnemy(Player *player, Enemy *enemy);
	bool DetectCollisionProjectileEnemy(Projectile *projectile, GameObject *enemy);
	bool DetectCollisionPlayerGift(Player *player, Gift *gift);
	void LogicalMousePosition(int mouseX, int mouseY);
	float RandomGenerator(float min, float max);
	glm::vec2 GeneratePointInEllipse();

	void OnInputUpdate(float deltaTime, int mods) override;
	void OnKeyPress(int key, int mods) override;
	void OnKeyRelease(int key, int mods) override;
	void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
	void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;

	// Sets the logic space and view space
	// logicSpace: { x, y, width, height }
	// viewSpace: { x, y, width, height }
	glm::mat3 VisualizationTransf2D(const LogicSpace &logicSpace, const ViewportSpace &viewSpace);
	glm::mat3 VisualizationTransf2DUnif(const LogicSpace &logicSpace, const ViewportSpace &viewSpace);

	void SetViewportArea(const ViewportSpace &viewSpace, glm::vec3 colorColor = glm::vec3(0), bool clear = true);

protected:

	// vectorii in care tin inamicii, proiectilele si proiectilele boss enemy-ului
	std::vector<Enemy*> enemies;
	std::vector<Projectile*> projectiles;
	std::vector<Projectile*> bossProjectiles;

	// vectorul in care tin obiecte cadou
	std::vector<Gift*> gifts;

	// instante pentru: player, enemy, boss enemy, obiect cadou, bara de viata si proiectil
	// pe care le voi folosi pentru a creea cate un obiect de tipul respectiv
	Player *player;
	Enemy *enemy, *bossEnemy;
	Gift *gift;
	HealthBar *hb;
	Projectile *projectile;

	// variabile in care salvez pozitia logica a mouse-ului
	float mouseX, mouseY;
	float length;

	// variabila cu ajutorul careia schimb culoarea navei galbene printr-o animatie
	float redColorStep = 0.0f;

	// folosite pentru generarea inamicilor intr-un anumit interval de timp
	double preTime, currTime, offset, extracter;

	// folosite pentru generarea proiectilelor boss-ului la un anumit interval de timp
	double bossProjectilePreTime, bossProjectileCurrTime;
	bool endGameState, bossShowedUp;

	// folosite pentru a genera inamici, proiectile(nume distincte - optional)
	int enemyId = 0, projectileId = 0, bossProjectileId = 0;
	int killedEnemies;
	bool bossYDirection, bossXDirection;
	bool bossInitialTime;
	bool generatedHPGift, generatedInvincibilityGift;
	ViewportSpace viewSpace;
	LogicSpace logicSpace;
	glm::mat3 modelMatrix, visMatrix;
};
