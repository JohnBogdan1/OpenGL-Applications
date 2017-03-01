#include "Laborator5.h"

#include <vector>
#include <string>
#include <iostream>

#include <Core/Engine.h>

using namespace std;

Laborator5::Laborator5()
{
}

Laborator5::~Laborator5()
{
}

void Laborator5::Init()
{
	// world camera
	camera = new Laborator::Camera();
	camera->Set(glm::vec3(0.0f, 0.0f, 3.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));

	// camera located above the world, used for the minimap
	topCamera = new Laborator::Camera();
	topCamera->Set(glm::vec3(camera->GetTargetPosition().x, 800.0f, camera->GetTargetPosition().z), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	// used for enemy apparition time
	preTime = 0.0;
	offset = 5.0;
	extracter = 0.05;

	// the box used on the minimap
	{
		minimapBoxMesh = new Mesh("box");
		minimapBoxMesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
		meshes[minimapBoxMesh->GetMeshID()] = minimapBoxMesh;
	}

	// player and enemy meshes
	{
		player = new Player();
		player->name = "player";
		player->healthPoints = 3;
		player->scale = glm::vec3(0.25f);
		player->speed = 2.0f;
		Mesh* playerMesh = new Mesh(player->name);
		playerMesh->LoadMesh(RESOURCE_PATH::MODELS + "Characters/Batman", "batman.obj");
		meshes[playerMesh->GetMeshID()] = playerMesh;

		player->size = GetObjectInitialSize(playerMesh->positions);
		player->size *= (player->scale);
	}

	{
		Mesh* enemyMesh = new Mesh("joker");
		enemyMesh->LoadMesh(RESOURCE_PATH::MODELS + "Characters/Joker", "joker.obj");
		meshes[enemyMesh->GetMeshID()] = enemyMesh;
	}

	// the towers
	{
		tower1 = new Tower();
		tower1->name = "tower1";
		tower1->scale = glm::vec3(0.1f);
		tower1->setPosition(-2.0f, 0.0f, 0.0f);
		Mesh* meshTower1 = new Mesh(tower1->name);
		meshTower1->LoadMesh(RESOURCE_PATH::MODELS + "Characters/Tower", "tower.obj");
		meshes[meshTower1->GetMeshID()] = meshTower1;

		tower1->size = GetObjectInitialSize(meshTower1->positions);
		tower1->size *= tower1->scale;
	}

	{
		tower2 = new Tower();
		tower2->name = "tower2";
		tower2->scale = glm::vec3(0.1f);
		tower2->setPosition(0.0f, 0.0f, -2.0f);
		Mesh* meshTower2 = new Mesh(tower2->name);
		meshTower2->LoadMesh(RESOURCE_PATH::MODELS + "Characters/Tower", "tower.obj");
		meshes[meshTower2->GetMeshID()] = meshTower2;
	}

	{
		tower3 = new Tower();
		tower3->name = "tower3";
		tower3->scale = glm::vec3(0.1f);
		tower3->setPosition(2.0f, 0.0f, 0.0f);
		Mesh* meshTower3 = new Mesh(tower3->name);
		meshTower3->LoadMesh(RESOURCE_PATH::MODELS + "Characters/Tower", "tower.obj");
		meshes[meshTower3->GetMeshID()] = meshTower3;
	}

	{
		// healthpoint sphere mesh
		Mesh* sphereMesh = new Mesh("sphere");
		sphereMesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
		meshes[sphereMesh->GetMeshID()] = sphereMesh;
	}

	{
		// pistol mesh
		Mesh* pistolMesh = new Mesh("pistol");
		pistolMesh->LoadMesh(RESOURCE_PATH::MODELS + "Characters/Pistol", "pistol.obj");
		meshes[pistolMesh->GetMeshID()] = pistolMesh;
	}

	{
		// grenade launcher mesh
		Mesh* grenadeLauncherMesh = new Mesh("grenadeLauncher");
		grenadeLauncherMesh->LoadMesh(RESOURCE_PATH::MODELS + "Characters/Grenade Launcher", "GrenadeLauncher.obj");
		meshes[grenadeLauncherMesh->GetMeshID()] = grenadeLauncherMesh;
	}

	// use a perspective projection
	projectionMatrix = glm::perspective(fov, window->props.aspectRatio, 0.01f, 200.0f);

}

void Laborator5::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Laborator5::Update(float deltaTimeSeconds)
{
	glm::ivec2 resolution = window->GetResolution();

	// sets the screen area where to draw the game
	glViewport(0, 0, resolution.x, resolution.y);

	// Render all the objects in this World every frame
	RenderPlayer(deltaTimeSeconds);
	GenerateAndRenderEnemies(deltaTimeSeconds);
	RenderTowers(deltaTimeSeconds);
	RenderTowerProjectiles(towerProjectiles, deltaTimeSeconds);
	RenderWeapon();
	RenderProjectiles(pistolProjectiles, deltaTimeSeconds);
	RenderProjectiles(rifleProjectiles, deltaTimeSeconds);
	RenderProjectiles(grenadeLauncherProjectiles, deltaTimeSeconds);
	RenderHealthPoints(deltaTimeSeconds);

	// draw the coordinate system for the World
	DrawCoordinatSystem(camera->GetViewMatrix(), projectionMatrix);

	// sets the screen area where to draw the minimap
	glViewport(resolution.x - 250, 50, 200, 200);

	// use a othographic projection
	minimapProjectionMatrix = glm::ortho(-10.0f, 10.0f, bottom, top, 0.0f, 1200.f);

	// render player, enemies and towers as a boxes on minimap
	player->modelMatrix *= Transform3D::Scale(2.0f, 2.0f, 2.0f);
	player->modelMatrix *= Transform3D::RotateOX(RADIANS(90.0f));
	myRenderMesh(meshes["box"], shaders["VertexNormal"], player->modelMatrix);

	for (int i = 0; i < enemies.size(); i++) {

		if ((enemies.at(i))->healthPoints != 0) {
			(enemies.at(i))->modelMatrix *= Transform3D::Scale(2.0f, 2.0f, 2.0f);
			(enemies.at(i))->modelMatrix *= Transform3D::RotateOZ(RADIANS(90.0f));
			myRenderMesh(meshes["box"], shaders["VertexNormal"], (enemies.at(i))->modelMatrix);
		}
		else if ((enemies.at(i))->healthPoints == 0) {
			(enemies.at(i))->modelMatrix *= Transform3D::RotateOZ(RADIANS(90.0f));
			myRenderMesh(meshes["box"], shaders["VertexNormal"], (enemies.at(i))->modelMatrix);
		}
	}
	tower1->modelMatrix *= Transform3D::Scale(6.0f, 6.0f, 6.0f);
	tower2->modelMatrix *= Transform3D::Scale(6.0f, 6.0f, 6.0f);
	tower3->modelMatrix *= Transform3D::Scale(6.0f, 6.0f, 6.0f);
	myRenderMesh(meshes["box"], shaders["VertexNormal"], tower1->modelMatrix);
	myRenderMesh(meshes["box"], shaders["VertexNormal"], tower2->modelMatrix);
	myRenderMesh(meshes["box"], shaders["VertexNormal"], tower3->modelMatrix);

	// draw the coordinate system for minimap
	DrawCoordinatSystem(topCamera->GetViewMatrix(), minimapProjectionMatrix);

}

// update the modelMatrix and render the player
void Laborator5::RenderPlayer(float deltaTimeSeconds) {

	if (player->healthPoints == 0) {
		// stop keyboard/mouse input
		InputController::SetActive(false);

		// the player will fall sideways usingthi angle for the animation, when he has no healthpoints left
		if (DEGREES(OxPlayerAngle) >= -90.0f)
			OxPlayerAngle -= deltaTimeSeconds;
	}

	// update the modelMatrix and render
	player->setPosition(camera->GetTargetPosition().x, 0.1f, camera->GetTargetPosition().z);
	player->rotation = -glm::atan(camera->forward.z, camera->forward.x) + RADIANS(90.0f);
	{
		modelMatrix = glm::mat4(1);
		modelMatrix *= Transform3D::Translate(player->position.x, player->position.y, player->position.z);
		modelMatrix *= Transform3D::RotateOY(player->rotation);
		if (player->healthPoints == 0)
			modelMatrix *= Transform3D::RotateOX(OxPlayerAngle);
		modelMatrix *= Transform3D::Scale(player->scale.x, player->scale.y, player->scale.z);
		player->modelMatrix = modelMatrix;
		if (!zoom)
			RenderMesh(meshes[player->name], shaders["VertexNormal"], modelMatrix);

	}
}

void Laborator5::GenerateAndRenderEnemies(float deltaTimeSeconds) {

	// generate an enemy
	// his size and strength will increase using initialScaleFactor varaiable for the scale, during the game
	currTime = Engine::GetElapsedTime();
	if (currTime >= (preTime + offset)) {

		// enemy properties
		double probability = ((double)rand() / RAND_MAX);
		enemy = new Enemy();
		enemy->name = "enemy" + std::to_string(enemyId);
		enemy->setPosition(mazeCorners[0].x, mazeCorners[0].y, mazeCorners[0].z);
		enemy->scale = { 0.25f * initialScaleFactor, 0.25f * initialScaleFactor, 0.25f * initialScaleFactor };
		enemy->speed = player->speed;
		enemy->healthPoints = initialEnemyHP;

		// mesh for enemy
		Mesh* enemyMesh = new Mesh(enemy->name);
		enemyMesh->LoadMesh(RESOURCE_PATH::MODELS + "Characters/Joker", "Joker.obj");
		meshes[enemyMesh->GetMeshID()] = enemyMesh;

		// get the initial size of the mesh(.obj) and update it
		enemy->size = GetObjectInitialSize(enemyMesh->positions);
		enemy->size *= enemy->scale;
		enemies.push_back(enemy);
		enemyId++;

		if (offset - 0.5f > 0.01f && probability <= 0.5f) {
			offset -= extracter;
			initialEnemyHP++;
			initialScaleFactor += 0.05f;
		}
		preTime = currTime;
	}

	// all towers have the same range
	float tower1Range = towerCircleRadius;
	float tower2Range = towerCircleRadius;
	float tower3Range = towerCircleRadius;

	// update modelMatrix and enemies position
	// enemies will make a z-shaped move on the map
	for (int i = 0; i < enemies.size(); i++) {

		// if enemy is not dead
		if ((enemies.at(i))->healthPoints != 0) {
			(enemies.at(i))->rotation = glm::atan((enemies.at(i))->target.z - (enemies.at(i))->position.z, (enemies.at(i))->target.x - (enemies.at(i))->position.x);
			modelMatrix = glm::mat4(1);
			modelMatrix *= Transform3D::Translate((enemies.at(i))->position.x, (enemies.at(i))->position.y, (enemies.at(i))->position.z);
			modelMatrix *= Transform3D::RotateOY((enemies.at(i))->rotation + (enemies.at(i))->target == mazeCorners[1] || (enemies.at(i))->target == mazeCorners[3] ? RADIANS(90.0f) : RADIANS(-45.0f));
			modelMatrix *= Transform3D::Scale((enemies.at(i))->scale.x, (enemies.at(i))->scale.y, (enemies.at(i))->scale.z);

			// every enemy has a target, to know in which direction he has to move
			if ((enemies.at(i))->position.x >= 10.0f && (enemies.at(i))->position.z == -10.0f)
				(enemies.at(i))->target = mazeCorners[2];
			else if ((enemies.at(i))->position.x <= -10.0f && (enemies.at(i))->position.z >= 10.0f)
				(enemies.at(i))->target = mazeCorners[3];
			else if ((enemies.at(i))->position.x >= 10.0f && (enemies.at(i))->position.z >= 10.0f) { // daca inamicul a ajuns la coltul din dreapta-jos
				(enemies.at(i))->healthPoints = 0;
				if (player->healthPoints > 0)
					player->healthPoints--;
			}

			// when he reached that target of the map, the target changes
			if ((enemies.at(i))->target == mazeCorners[1]) {
				(enemies.at(i))->position.x += glm::cos((enemies.at(i))->rotation) * (enemies.at(i))->speed * deltaTimeSeconds;
			}
			else if ((enemies.at(i))->target == mazeCorners[2]) {
				(enemies.at(i))->position.x += glm::cos((enemies.at(i))->rotation) * (enemies.at(i))->speed * deltaTimeSeconds;
				(enemies.at(i))->position.z += glm::sin((enemies.at(i))->rotation) * (enemies.at(i))->speed * deltaTimeSeconds;
			}
			else if ((enemies.at(i))->target == mazeCorners[3]) {
				(enemies.at(i))->position.x += glm::cos((enemies.at(i))->rotation) * (enemies.at(i))->speed * deltaTimeSeconds;
			}

			if ((enemies.at(i))->healthPoints != 0) {

				// test if there is a collision between projectiles and enemies
				for (int j = 0; j < pistolProjectiles.size(); j++) {

					if ((enemies.at(i))->healthPoints != 0 && (pistolProjectiles.at(j))->healthPoints != 0) {

						// if it is a collision, stop rendering the respective projectile & enemy
						// moreover, the enemy is not longer the target of the towers
						if (DetectCollisionProjectileEnemy(pistolProjectiles.at(j), enemies.at(i))) {

							(enemies.at(i))->healthPoints -= (pistolProjectiles.at(j))->power;

							if ((enemies.at(i))->healthPoints <= 0) {

								// enemy is dead
								(enemies.at(i))->healthPoints = 0;
								tower1->targetedEnemy = nullptr;
								tower2->targetedEnemy = nullptr;
								tower3->targetedEnemy = nullptr;
							}


							// the projectile will not be rendered anymore
							(pistolProjectiles.at(j))->healthPoints = 0;
							meshes.erase((pistolProjectiles.at(j))->name);
						}
					}
				}

				for (int j = 0; j < rifleProjectiles.size(); j++) {

					if ((enemies.at(i))->healthPoints != 0 && (rifleProjectiles.at(j))->healthPoints != 0) {

						// if it is a collision, stop rendering the respective projectile & enemy
						// moreover, the enemy is not longer the target of the towers
						if (DetectCollisionProjectileEnemy(rifleProjectiles.at(j), enemies.at(i))) {

							(enemies.at(i))->healthPoints -= (rifleProjectiles.at(j))->power;

							if ((enemies.at(i))->healthPoints <= 0) {

								// enemy is dead
								(enemies.at(i))->healthPoints = 0;
								tower1->targetedEnemy = nullptr;
								tower2->targetedEnemy = nullptr;
								tower3->targetedEnemy = nullptr;
							}

							// the projectile will not be rendered anymore
							(rifleProjectiles.at(j))->healthPoints = 0;
							meshes.erase((rifleProjectiles.at(j))->name);
						}
					}
				}

				for (int j = 0; j < grenadeLauncherProjectiles.size(); j++) {

					if ((enemies.at(i))->healthPoints != 0 && (grenadeLauncherProjectiles.at(j))->healthPoints != 0) {

						// if it is a collision, stop rendering the respective projectile & enemy
						// moreover, the enemy is not longer the target of the towers
						if (DetectCollisionProjectileEnemy(grenadeLauncherProjectiles.at(j), enemies.at(i))) {

							(enemies.at(i))->healthPoints -= (grenadeLauncherProjectiles.at(j))->power;

							if ((enemies.at(i))->healthPoints <= 0) {

								// enemy is dead
								(enemies.at(i))->healthPoints = 0;
								tower1->targetedEnemy = nullptr;
								tower2->targetedEnemy = nullptr;
								tower3->targetedEnemy = nullptr;
							}

							// the projectile will not be rendered anymore
							(grenadeLauncherProjectiles.at(j))->healthPoints = 0;
							meshes.erase((grenadeLauncherProjectiles.at(j))->name);
						}
					}
				}

				for (int j = 0; j < towerProjectiles.size(); j++) {

					if ((enemies.at(i))->healthPoints != 0 && (towerProjectiles.at(j))->healthPoints != 0) {

						// if it is a collision, stop rendering the respective projectile & enemy
						// moreover, the enemy is not longer the target of the towers
						if (DetectCollisionProjectileEnemy(towerProjectiles.at(j), enemies.at(i))) {

							(enemies.at(i))->healthPoints -= (towerProjectiles.at(j))->power;

							if ((enemies.at(i))->healthPoints <= 0) {

								// enemy is dead
								(enemies.at(i))->healthPoints = 0;
								tower1->targetedEnemy = nullptr;
								tower2->targetedEnemy = nullptr;
								tower3->targetedEnemy = nullptr;
							}

							// the projectile will not be rendered anymore
							(towerProjectiles.at(j))->healthPoints = 0;
							meshes.erase((towerProjectiles.at(j))->name);
						}
					}
				}

			}

			// if the enemy is not dead, render it
			if ((enemies.at(i))->healthPoints != 0) {
				(enemies.at(i))->modelMatrix = modelMatrix;
				RenderMesh(meshes[(enemies.at(i))->name], shaders["VertexNormal"], (enemies.at(i))->modelMatrix);

				// find the closest enemy to each tower every time
				float distanceEnemyTower1 = glm::sqrt(glm::pow(tower1->position.x - (enemies.at(i))->position.x, 2) + glm::pow(tower1->position.z - (enemies.at(i))->position.z, 2));
				float distanceEnemyTower2 = glm::sqrt(glm::pow(tower2->position.x - (enemies.at(i))->position.x, 2) + glm::pow(tower2->position.z - (enemies.at(i))->position.z, 2));
				float distanceEnemyTower3 = glm::sqrt(glm::pow(tower3->position.x - (enemies.at(i))->position.x, 2) + glm::pow(tower3->position.z - (enemies.at(i))->position.z, 2));

				if (distanceEnemyTower1 < tower1Range) {
					tower1Range = distanceEnemyTower1;
					tower1->targetedEnemy = enemies.at(i);
				}

				if (distanceEnemyTower2 < tower2Range) {
					tower2Range = distanceEnemyTower2;
					tower2->targetedEnemy = enemies.at(i);
				}

				if (distanceEnemyTower3 < tower3Range) {
					tower3Range = distanceEnemyTower3;
					tower3->targetedEnemy = enemies.at(i);
				}

			}
		}
		else if ((enemies.at(i))->healthPoints == 0) { // use an animation for the enemy's death
			(enemies.at(i))->rotation = glm::atan((enemies.at(i))->target.z - (enemies.at(i))->position.z, (enemies.at(i))->target.x - (enemies.at(i))->position.x);
			modelMatrix = glm::mat4(1);
			modelMatrix *= Transform3D::Translate((enemies.at(i))->position.x, (enemies.at(i))->position.y, (enemies.at(i))->position.z);
			modelMatrix *= Transform3D::RotateOY((enemies.at(i))->rotation + (enemies.at(i))->target == mazeCorners[1] || (enemies.at(i))->target == mazeCorners[3] ? RADIANS(90.0f) : RADIANS(-45.0f));
			modelMatrix *= Transform3D::RotateOX(-(enemies.at(i))->OxAngle);
			modelMatrix *= Transform3D::Scale((enemies.at(i))->scale.x, (enemies.at(i))->scale.y, (enemies.at(i))->scale.z);

			// he will scale at 0 and will fall sideways
			(enemies.at(i))->scale -= glm::vec3(deltaTimeSeconds / 4);
			(enemies.at(i))->OxAngle += 1.5f * deltaTimeSeconds;
			if ((enemies.at(i))->scale.x >= 0.0f) {
				(enemies.at(i))->modelMatrix = modelMatrix;
				RenderMesh(meshes[(enemies.at(i))->name], shaders["VertexNormal"], modelMatrix);
			}
		}
	}
}

// render the towers
void Laborator5::RenderTowers(float deltaTimeSeconds) {

	// -----------Tower 1------------
	modelMatrix = glm::mat4(1);
	modelMatrix *= Transform3D::Translate(tower1->position.x, tower1->position.y, tower1->position.z);

	// if the tower has a target, rotate in that direction and fire a projectile
	// every projectile is launched at a certain time
	if (tower1->targetedEnemy != nullptr) {

		tower1->rotation = glm::atan((tower1->targetedEnemy)->position.z - tower1->position.z, (tower1->targetedEnemy)->position.x - tower1->position.x);
		modelMatrix *= Transform3D::RotateOY(-tower1->rotation);

		if (tower1->projectileFreqTime == 0.0f)
			CreateTowerProjectile(tower1->position, tower1->rotation);

		tower1->projectileFreqTime += deltaTimeSeconds;

		if (tower1->projectileFreqTime >= 100 * deltaTimeSeconds)
			tower1->projectileFreqTime = 0.0f;

		// if the enemy is not anymore in the tower's target
		float distance = glm::sqrt(glm::pow(tower1->position.x - tower1->targetedEnemy->position.x, 2) + glm::pow(tower1->position.z - tower1->targetedEnemy->position.z, 2));
		if (distance > towerCircleRadius) {
			tower1->targetedEnemy = nullptr;
		}
	}
	modelMatrix *= Transform3D::Scale(tower1->scale.x, tower1->scale.y, tower1->scale.z);
	tower1->modelMatrix = modelMatrix;
	RenderMesh(meshes[tower1->name], shaders["VertexNormal"], tower1->modelMatrix);

	// -----------Tower 2------------
	modelMatrix = glm::mat4(1);
	modelMatrix *= Transform3D::Translate(tower2->position.x, tower2->position.y, tower2->position.z);

	// if the tower has a target, rotate in that direction and fire a projectile
	// every projectile is launched at a certain time
	if (tower2->targetedEnemy != nullptr) {

		tower2->rotation = glm::atan((tower2->targetedEnemy)->position.z - tower2->position.z, (tower2->targetedEnemy)->position.x - tower2->position.x);
		modelMatrix *= Transform3D::RotateOY(-tower2->rotation);

		if (tower2->projectileFreqTime == 0.0f)
			CreateTowerProjectile(tower2->position, tower2->rotation);

		tower2->projectileFreqTime += deltaTimeSeconds;

		if (tower2->projectileFreqTime >= 100 * deltaTimeSeconds)
			tower2->projectileFreqTime = 0.0f;

		// if the enemy is not anymore in the tower's target
		float distance = glm::sqrt(glm::pow(tower2->position.x - tower2->targetedEnemy->position.x, 2) + glm::pow(tower2->position.z - tower2->targetedEnemy->position.z, 2));
		if (distance > towerCircleRadius) {
			tower2->targetedEnemy = nullptr;
		}
	}
	modelMatrix *= Transform3D::Scale(tower2->scale.x, tower2->scale.y, tower2->scale.z);
	tower2->modelMatrix = modelMatrix;
	RenderMesh(meshes[tower2->name], shaders["VertexNormal"], tower2->modelMatrix);

	// -----------Tower 3------------
	modelMatrix = glm::mat4(1);
	modelMatrix *= Transform3D::Translate(tower3->position.x, tower3->position.y, tower3->position.z);

	// if the tower has a target, rotate in that direction and fire a projectile
	// every projectile is launched at a certain time
	if (tower3->targetedEnemy != nullptr) {
		tower3->rotation = glm::atan((tower3->targetedEnemy)->position.z - tower3->position.z, (tower3->targetedEnemy)->position.x - tower3->position.x);
		modelMatrix *= Transform3D::RotateOY(-tower3->rotation);

		if (tower3->projectileFreqTime == 0.0f)
			CreateTowerProjectile(tower3->position, tower3->rotation);

		tower3->projectileFreqTime += deltaTimeSeconds;

		if (tower3->projectileFreqTime >= 100 * deltaTimeSeconds)
			tower3->projectileFreqTime = 0.0f;

		// if the enemy is not anymore in the tower's target
		float distance = glm::sqrt(glm::pow(tower3->position.x - tower3->targetedEnemy->position.x, 2) + glm::pow(tower3->position.z - tower3->targetedEnemy->position.z, 2));
		if (distance > towerCircleRadius) {
			tower3->targetedEnemy = nullptr;
		}
	}
	modelMatrix *= Transform3D::Scale(tower3->scale.x, tower3->scale.y, tower3->scale.z);
	tower3->modelMatrix = modelMatrix;
	RenderMesh(meshes[tower3->name], shaders["VertexNormal"], tower3->modelMatrix);
}

// render player's healthpoints as three spheres
// every sphere makes a rotation around the player, specific to the X, Y and Z axe
void Laborator5::RenderHealthPoints(float deltaTimeSeconds) {

	if (player->healthPoints == 3) {
		angularStepOY += deltaTimeSeconds * 100;

		modelMatrix = glm::mat4(1);
		modelMatrix *= Transform3D::Translate(player->position.x, player->position.y, player->position.z);
		modelMatrix *= Transform3D::RotateOY(player->rotation);
		modelMatrix *= Transform3D::Translate(-0.25f, 0.25f, 0.0f);
		modelMatrix *= Transform3D::Translate(0.25f, 0.0f, 0.0f);
		modelMatrix *= Transform3D::RotateOY(RADIANS(angularStepOY));
		modelMatrix *= Transform3D::Translate(-0.25f, 0.0f, 0.0f);
		modelMatrix *= Transform3D::Scale(0.1f, 0.1f, 0.1f);
		RenderMesh(meshes["sphere"], shaders["VertexNormal"], modelMatrix);
	}

	if (player->healthPoints >= 2) {
		angularStepOX += deltaTimeSeconds * 100;

		modelMatrix = glm::mat4(1);
		modelMatrix *= Transform3D::Translate(player->position.x, player->position.y, player->position.z);
		modelMatrix *= Transform3D::RotateOY(player->rotation);
		modelMatrix *= Transform3D::Translate(0.0f, -0.1f, 0.0f);
		modelMatrix *= Transform3D::Translate(0.0f, 0.35f, 0.0f);
		modelMatrix *= Transform3D::RotateOX(RADIANS(angularStepOX));
		modelMatrix *= Transform3D::Translate(0.0f, -0.35f, 0.0f);
		modelMatrix *= Transform3D::Scale(0.1f, 0.1f, 0.1f);
		RenderMesh(meshes["sphere"], shaders["VertexNormal"], modelMatrix);
	}

	if (player->healthPoints >= 1) {
		angularStepOZ += deltaTimeSeconds * 100;

		modelMatrix = glm::mat4(1);
		modelMatrix *= Transform3D::Translate(player->position.x, player->position.y, player->position.z);
		modelMatrix *= Transform3D::RotateOY(player->rotation);
		modelMatrix *= Transform3D::Translate(0.35f, 0.25f, 0.0f);
		modelMatrix *= Transform3D::Translate(-0.35f, 0.0f, 0.0f);
		modelMatrix *= Transform3D::RotateOZ(RADIANS(angularStepOZ));
		modelMatrix *= Transform3D::Translate(0.35f, 0.0f, 0.0f);
		modelMatrix *= Transform3D::Scale(0.1f, 0.1f, 0.1f);
		RenderMesh(meshes["sphere"], shaders["VertexNormal"], modelMatrix);
	}

}

// render player's weapons
void Laborator5::RenderWeapon() {
	if (renderPistol) {
		modelMatrix = glm::mat4(1);
		modelMatrix *= Transform3D::Translate(player->position.x, player->position.y, player->position.z);
		modelMatrix *= Transform3D::RotateOY(player->rotation + RADIANS(-90.0f));
		if (player->healthPoints != 0)
			modelMatrix *= Transform3D::Translate(0.12f, 0.3f, 0.15f);
		else {
			modelMatrix *= Transform3D::Translate(0.12f, 0.0f, 0.15f);
			modelMatrix *= Transform3D::RotateOX(RADIANS(90.0f));
		}
		modelMatrix *= Transform3D::Scale(0.08f, 0.08f, 0.08f);
		RenderMesh(meshes["pistol"], shaders["VertexNormal"], modelMatrix);
	}
	else if (renderGrenadeLauncher) {
		modelMatrix = glm::mat4(1);
		modelMatrix *= Transform3D::Translate(player->position.x, player->position.y, player->position.z);
		modelMatrix *= Transform3D::RotateOY(player->rotation + RADIANS(-90.0f));
		if (player->healthPoints != 0)
			modelMatrix *= Transform3D::Translate(0.05f, 0.3f, 0.15f);
		else {
			modelMatrix *= Transform3D::Translate(0.05f, 0.0f, 0.15f);
			modelMatrix *= Transform3D::RotateOX(RADIANS(90.0f));
		}
		modelMatrix *= Transform3D::Scale(0.0025f, 0.0025f, 0.0025f);
		RenderMesh(meshes["grenadeLauncher"], shaders["VertexNormal"], modelMatrix);
	}
}

void Laborator5::RenderProjectiles(std::vector<Projectile*> projectiles, float deltaTimeSeconds) {

	// render every projectile fired
	for (int i = 0; i < projectiles.size(); i++) {
		if ((projectiles.at(i))->healthPoints != 0) {
			modelMatrix = glm::mat4(1);
			modelMatrix *= Transform3D::Translate((projectiles.at(i))->position.x, (projectiles.at(i))->position.y, (projectiles.at(i))->position.z);
			modelMatrix *= Transform3D::RotateOY(-(projectiles.at(i))->rotation);
			modelMatrix *= Transform3D::Translate(0.12f, 0.3f, 0.15f);
			modelMatrix *= Transform3D::Scale((projectiles.at(i))->scale.x, (projectiles.at(i))->scale.y, (projectiles.at(i))->scale.z);

			(projectiles.at(i))->position.x += (projectiles.at(i))->direction.x * (projectiles.at(i))->speed * deltaTimeSeconds;

			// when player shoots with the grande launcher, use a trajectory based on the sinus ecuation
			// i use the forward camera for the angle
			if ((projectiles.at(i))->name.find("grenadeLauncherProjectile") != std::string::npos) {
				(projectiles.at(i))->time += deltaTimeSeconds;
				(projectiles.at(i))->position.y = 0.5f * glm::sin(glm::abs((projectiles.at(i))->direction.y) * (projectiles.at(i))->time);
			}
			(projectiles.at(i))->position.z += (projectiles.at(i))->direction.z * (projectiles.at(i))->speed * deltaTimeSeconds;
			(projectiles.at(i))->modelMatrix = modelMatrix;

			// when the projectile has passed the entire distance or reached the ground, stop rendering it
			float distance = glm::sqrt(glm::pow((projectiles.at(i))->initialPosition.x - (projectiles.at(i))->position.x, 2) + glm::pow((projectiles.at(i))->initialPosition.z - (projectiles.at(i))->position.z, 2));
			if (distance >= (projectiles.at(i))->distance || ((projectiles.at(i))->position.y + 0.3f) <= 0)
				(projectiles.at(i))->healthPoints = 0;
			RenderMesh(meshes[(projectiles.at(i))->name], shaders["VertexNormal"], modelMatrix);
		}
	}
}

// render the tower projectiles in the same way
void Laborator5::RenderTowerProjectiles(std::vector<Projectile*> projectiles, float deltaTimeSeconds) {
	for (int i = 0; i < projectiles.size(); i++) {
		if ((projectiles.at(i))->healthPoints != 0) {
			modelMatrix = glm::mat4(1);
			modelMatrix *= Transform3D::Translate((projectiles.at(i))->position.x, (projectiles.at(i))->position.y, (projectiles.at(i))->position.z);
			modelMatrix *= Transform3D::RotateOY(-(projectiles.at(i))->rotation);
			modelMatrix *= Transform3D::Scale((projectiles.at(i))->scale.x, (projectiles.at(i))->scale.y, (projectiles.at(i))->scale.z);

			(projectiles.at(i))->position.x += (projectiles.at(i))->direction.x * (projectiles.at(i))->speed * deltaTimeSeconds;
			(projectiles.at(i))->position.y -= (projectiles.at(i))->speed * deltaTimeSeconds / 4;
			(projectiles.at(i))->position.z += (projectiles.at(i))->direction.z * (projectiles.at(i))->speed * deltaTimeSeconds;
			(projectiles.at(i))->modelMatrix = modelMatrix;

			float distance = glm::sqrt(glm::pow((projectiles.at(i))->initialPosition.x - (projectiles.at(i))->position.x, 2) + glm::pow((projectiles.at(i))->initialPosition.z - (projectiles.at(i))->position.z, 2));
			if (distance >= (projectiles.at(i))->distance)
				(projectiles.at(i))->healthPoints = 0;

			RenderMesh(meshes[(projectiles.at(i))->name], shaders["VertexNormal"], modelMatrix);
		}
	}
}

// find the initial size of the 3D object
glm::vec3 Laborator5::GetObjectInitialSize(std::vector<glm::vec3> positions) {

	glm::vec3 minValues = { 9999.0f, 9999.0f, 9999.0f };
	glm::vec3 maxValues = { -9999.0f, -9999.0f, -9999.0f };

	for (int i = 0; i < positions.size(); i++) {

		// find min vertex value for each axe
		minValues.x = glm::min(positions[i].x, minValues.x);
		minValues.y = glm::min(positions[i].y, minValues.y);
		minValues.z = glm::min(positions[i].z, minValues.z);

		// find max vertex value for each axe
		maxValues.x = glm::max(positions[i].x, maxValues.x);
		maxValues.y = glm::max(positions[i].y, maxValues.y);
		maxValues.z = glm::max(positions[i].z, maxValues.z);
	}

	// return { width, height, depth }
	return{ maxValues.x - minValues.x, maxValues.y - minValues.y, maxValues.z - minValues.z };
}

// i use the sphere-sphere collision detection
bool Laborator5::DetectCollisionProjectileEnemy(Projectile *projectile, Enemy *enemy) {

	glm::vec3 projectileSphereCenter;
	if (projectile->name.find("towerProjectile") == std::string::npos)
		projectileSphereCenter = { projectile->position.x + 0.15f, ((projectile->position.y + 0.3f) + (projectile->position.y + 0.3f + projectile->size.y)) / 2.0f, \
			((projectile->position.z + 0.12f) + (projectile->position.z + 0.12f + projectile->size.z)) / 2.0f };
	else {
		projectileSphereCenter = { projectile->position.x, ((projectile->position.y) + (projectile->position.y + projectile->size.y)) / 2.0f, \
		((projectile->position.z) + (projectile->position.z + projectile->size.z)) / 2.0f };
	}

	glm::vec3 enemySphereCenter = { enemy->position.x, (enemy->position.y + (enemy->position.y + enemy->size.y)) / 2.0f, (enemy->position.z + (enemy->position.z + enemy->size.z)) / 2.0f };

	float projectileRadius = glm::sqrt(projectile->size.x / 2.0f * projectile->size.x / 2.0f + projectile->size.y / 2.0f * projectile->size.y / 2.0f + projectile->size.z / 2.0f * projectile->size.z / 2.0f);
	float enemyRadius = 0.5f * glm::sqrt(enemy->size.x / 2.0f * enemy->size.x / 2.0f + enemy->size.y / 2.0f * enemy->size.y / 2.0f + enemy->size.z / 2.0f * enemy->size.z / 2.0f);

	float distance = glm::sqrt(glm::pow(enemySphereCenter.x - projectileSphereCenter.x, 2) + glm::pow(enemySphereCenter.y - projectileSphereCenter.y, 2) + glm::pow(enemySphereCenter.z - projectileSphereCenter.z, 2));

	// if the distance between the sphere's center is less than the sum of their radii, then is a collision
	if (distance < (projectileRadius + enemyRadius))
		return true;

	return false;
}

void Laborator5::FrameEnd()
{
}

void Laborator5::RenderMesh(Mesh * mesh, Shader * shader, const glm::mat4 & modelMatrix)
{
	if (!mesh || !shader || !shader->program)
		return;

	// render an object using the specified shader and the specified position
	shader->Use();
	glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	mesh->Render();
}

// i created a new RenderMesh function for the Minimap
void Laborator5::myRenderMesh(Mesh * mesh, Shader * shader, const glm::mat4 & modelMatrix)
{
	if (!mesh || !shader || !shader->program)
		return;

	// render an object using the specified shader and the specified position
	shader->Use();
	glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(topCamera->GetViewMatrix()));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(minimapProjectionMatrix));
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	mesh->Render();
}

// Documentation for the input functions can be found in: "/Source/Core/Window/InputController.h" or
// https://github.com/UPB-Graphics/Framework-EGC/blob/master/Source/Core/Window/InputController.h

void Laborator5::OnInputUpdate(float deltaTime, int mods)
{
	// move the camera only if MOUSE_RIGHT button is pressed
	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT)) {
		float cameraSpeed = player->speed;

		if (window->KeyHold(GLFW_KEY_W)) {
			// translate the camera forward
			camera->MoveForward(cameraSpeed * deltaTime);

			// translate the top camera
			float dir = -glm::atan(camera->forward.z, camera->forward.x);
			topCamera->TranslateRight(cameraSpeed * deltaTime * glm::cos(dir));
			topCamera->MoveForward(cameraSpeed * deltaTime * glm::sin(dir));
		}

		if (window->KeyHold(GLFW_KEY_A)) {
			// translate the camera to the left
			camera->TranslateRight(-cameraSpeed * deltaTime);

			// translate the top camera
			float dir = -glm::atan(camera->forward.z, camera->forward.x);
			topCamera->TranslateRight(-cameraSpeed * deltaTime * glm::sin(dir));
			topCamera->MoveForward(cameraSpeed * deltaTime * glm::cos(dir));
		}

		if (window->KeyHold(GLFW_KEY_S)) {
			// translate the camera backwards
			camera->MoveForward(-cameraSpeed * deltaTime);

			// translate the top camera
			float dir = -glm::atan(camera->forward.z, camera->forward.x);
			topCamera->MoveForward(-cameraSpeed * deltaTime * glm::sin(dir));
			topCamera->TranslateRight(-cameraSpeed * deltaTime * glm::cos(dir));
		}

		if (window->KeyHold(GLFW_KEY_D)) {
			// translate the camera to the right
			camera->TranslateRight(cameraSpeed * deltaTime);

			// translate the top camera
			float dir = -glm::atan(camera->forward.z, camera->forward.x);
			topCamera->TranslateRight(cameraSpeed * deltaTime * glm::sin(dir));
			topCamera->MoveForward(-cameraSpeed * deltaTime * glm::cos(dir));
		}
	}

	// if the left mouse button is hold, fire continuously
	if (window->MouseHold(GLFW_MOUSE_BUTTON_LEFT)) {
		if (apparitionTime == 0.0f)
			CreateProjectile();

		apparitionTime += deltaTime;

		// every weapon has its time
		if (renderPistol)
			if (apparitionTime >= 50 * deltaTime)
				apparitionTime = 0.0f;

		if (zoom)
			if (apparitionTime >= 10 * deltaTime)
				apparitionTime = 0.0f;

		if (renderGrenadeLauncher)
			if (apparitionTime >= 100 * deltaTime)
				apparitionTime = 0.0f;
	}
}

void Laborator5::OnKeyPress(int key, int mods)
{
	//switch weapons when one of the keys is pressed

	if (key == GLFW_KEY_1) {
		zoom = false;
		renderPistol = true;
		renderGrenadeLauncher = false;
		projectionMatrix = glm::perspective(fov, window->props.aspectRatio, 0.01f, 200.0f);
	}

	if (key == GLFW_KEY_2) {
		zoom = true;
		renderPistol = false;
		renderGrenadeLauncher = false;

		// zoom in
		projectionMatrix = glm::perspective(fov / 6, window->props.aspectRatio, 0.01f, 200.0f);
	}

	if (key == GLFW_KEY_3) {
		zoom = false;
		renderPistol = false;
		renderGrenadeLauncher = true;
		projectionMatrix = glm::perspective(fov, window->props.aspectRatio, 0.01f, 200.0f);
	}
}

void Laborator5::OnKeyRelease(int key, int mods)
{
	// add key release event
}

void Laborator5::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event

	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT)) {
		float sensivityOX = 0.001f;
		float sensivityOY = 0.001f;

		// rotate the camera in Third-person mode around OX and OY using deltaX and deltaY
		// I use the sensitivity variables for setting up the rotation speed
		camera->RotateThirdPerson_OX(-deltaY * sensivityOX);
		camera->RotateThirdPerson_OY(-deltaX * sensivityOY);

	}
}

// create a projectile that has some properties like direction, position, power, speed, and size
// the projectiles are different for each weapon
void Laborator5::CreateProjectile() {
	if (renderPistol) {
		pistolProjectile = new Projectile();
		pistolProjectile->name = "pistolProjectile" + std::to_string(pistolProjectileId);
		pistolProjectile->scale = glm::vec3(0.05f);
		pistolProjectile->healthPoints = 1;
		pistolProjectile->speed = 4.0f;
		pistolProjectile->power = 2;
		pistolProjectile->distance = 10.0f;
		pistolProjectile->rotation = glm::atan(camera->forward.z, camera->forward.x);
		pistolProjectile->direction = { glm::cos(pistolProjectile->rotation), 0, glm::sin(pistolProjectile->rotation) };
		pistolProjectile->setPosition(player->position.x, player->position.y, player->position.z);
		pistolProjectile->initialPosition = player->position;
		pistolProjectileId++;
		Mesh* pistolProjectileMesh = new Mesh(pistolProjectile->name);
		pistolProjectileMesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
		meshes[pistolProjectileMesh->GetMeshID()] = pistolProjectileMesh;

		pistolProjectile->size = GetObjectInitialSize(pistolProjectileMesh->positions);
		pistolProjectile->size *= pistolProjectile->scale;
		pistolProjectiles.push_back(pistolProjectile);
	}
	else if (zoom) {
		rifleProjectile = new Projectile();
		rifleProjectile->name = "rifleProjectile" + std::to_string(rifleProjectileId);
		rifleProjectile->scale = glm::vec3(0.03f);
		rifleProjectile->healthPoints = 1;
		rifleProjectile->speed = 8.0f;
		rifleProjectile->power = 1;
		rifleProjectile->distance = 50.0f;
		rifleProjectile->rotation = glm::atan(camera->forward.z, camera->forward.x);
		rifleProjectile->direction = { glm::cos(rifleProjectile->rotation), 0, glm::sin(rifleProjectile->rotation) };
		rifleProjectile->setPosition(player->position.x, player->position.y, player->position.z);
		rifleProjectile->initialPosition = player->position;
		rifleProjectileId++;
		Mesh* rifleProjectileMesh = new Mesh(rifleProjectile->name);
		rifleProjectileMesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
		meshes[rifleProjectileMesh->GetMeshID()] = rifleProjectileMesh;

		rifleProjectile->size = GetObjectInitialSize(rifleProjectileMesh->positions);
		rifleProjectile->size *= rifleProjectile->scale;
		rifleProjectiles.push_back(rifleProjectile);
	}
	else if (renderGrenadeLauncher) {
		grenadeLauncherProjectile = new Projectile();
		grenadeLauncherProjectile->name = "grenadeLauncherProjectile" + std::to_string(grenadeLauncherProjectileId);
		grenadeLauncherProjectile->scale = glm::vec3(0.1f);
		grenadeLauncherProjectile->healthPoints = 1;
		grenadeLauncherProjectile->speed = 2.0f;
		grenadeLauncherProjectile->power = 4;
		grenadeLauncherProjectile->distance = 8.0f;
		grenadeLauncherProjectile->time = 0.0f;
		grenadeLauncherProjectile->rotation = glm::atan(camera->forward.z, camera->forward.x);
		grenadeLauncherProjectile->direction = { glm::cos(grenadeLauncherProjectile->rotation), glm::atan(camera->forward.z, camera->forward.y), glm::sin(grenadeLauncherProjectile->rotation) };
		grenadeLauncherProjectile->setPosition(player->position.x, player->position.y, player->position.z);
		grenadeLauncherProjectile->initialPosition = player->position;
		grenadeLauncherProjectileId++;
		Mesh* grenadeLauncherProjectileMesh = new Mesh(grenadeLauncherProjectile->name);
		grenadeLauncherProjectileMesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
		meshes[grenadeLauncherProjectileMesh->GetMeshID()] = grenadeLauncherProjectileMesh;

		grenadeLauncherProjectile->size = GetObjectInitialSize(grenadeLauncherProjectileMesh->positions);
		grenadeLauncherProjectile->size *= grenadeLauncherProjectile->scale;
		grenadeLauncherProjectiles.push_back(grenadeLauncherProjectile);
	}
}

// create a tower projectile
void Laborator5::CreateTowerProjectile(glm::vec3 position, float rotation) {
	towerProjectile = new Projectile();
	towerProjectile->name = "towerProjectile" + std::to_string(towerProjectileId);
	towerProjectile->scale = glm::vec3(0.1f);
	towerProjectile->healthPoints = 1;
	towerProjectile->speed = 4.0f;
	towerProjectile->power = 15;
	towerProjectile->rotation = rotation;
	towerProjectile->direction = { glm::cos(rotation), 0, glm::sin(rotation) };
	towerProjectileId++;
	Mesh* towerProjectileMesh = new Mesh(towerProjectile->name);
	towerProjectileMesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
	meshes[towerProjectileMesh->GetMeshID()] = towerProjectileMesh;

	towerProjectile->size = GetObjectInitialSize(towerProjectileMesh->positions);
	towerProjectile->size *= towerProjectile->scale;
	towerProjectile->setPosition(position.x, position.y + (tower1->size.y - 0.1f), position.z);
	towerProjectile->initialPosition = position;
	towerProjectile->distance = glm::sqrt(towerCircleRadius * towerCircleRadius + (tower1->size.y - 0.1f) * (tower1->size.y - 0.1f));
	towerProjectiles.push_back(towerProjectile);
}

void Laborator5::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// when left mouse button is pressed, shoot a projectile and wait some time until you can shoot again

	if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT)) {

		if (renderPistol)
			projectileOfsset = 0.85f;
		else if (zoom)
			projectileOfsset = 0.17f;
		else if (renderGrenadeLauncher)
			projectileOfsset = 1.7f;

		projectileCurrTime = (float)Engine::GetElapsedTime();
		if (projectileCurrTime >= projectilePreTime + projectileOfsset) {
			projectilePreTime = projectileCurrTime;
			CreateProjectile();
		}
	}
}

void Laborator5::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Laborator5::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Laborator5::OnWindowResize(int width, int height)
{
}
