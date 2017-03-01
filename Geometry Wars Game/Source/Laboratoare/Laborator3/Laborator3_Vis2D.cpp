#include "Laborator3_Vis2D.h"

#include <vector>
#include <iostream>

#include <Core/Engine.h>
#include "Transform2D.h"

using namespace std;

Laborator3_Vis2D::Laborator3_Vis2D()
{
}

Laborator3_Vis2D::~Laborator3_Vis2D()
{
}

void Laborator3_Vis2D::Init()
{
	camera->SetPosition(glm::vec3(0, 0, 50));
	camera->SetRotation(glm::vec3(0, 0, 0));
	camera->Update();
	cameraInput->SetActive(false);

	// spatiul logic
	logicSpace.x = 0.0f;		// logic x
	logicSpace.y = 0.0f;		// logic y
	logicSpace.width = 4.0f;	// logic width
	logicSpace.height = 3.0f;	// logic height

	// inamicii apar initial la 2 secunde
	preTime = 0.0;
	offset = 2.0;
	extracter = 0.1;

	bossProjectilePreTime = 0.0;

	glm::vec3 corner = glm::vec3(0.0, 0.0, 0);
	length = 0.2f;

	// compute coordinates of square center
	float cx = corner.x + length / 2;
	float cy = corner.y + length / 2;

	killedEnemies = 0;
	bossYDirection = false;
	bossXDirection = false;

	bossInitialTime = true;
	bossShowedUp = true;

	// cand s-a terminat jocul vreau sa afisez un mesaj o singura data
	endGameState = true;

	generatedHPGift = false;
	generatedInvincibilityGift = false;

	// deschid fisierele de sunet si pornesc sunetul din background
	mciSendString((LPCWSTR)L"open background.mp3 type mpegvideo", NULL, 0, NULL);
	mciSendString((LPCWSTR)L"play background.mp3 repeat", NULL, 0, NULL);

	mciSendString((LPCWSTR)L"open fireshot.mp3 type mpegvideo", NULL, 0, NULL);
	mciSendString((LPCWSTR)L"open hit.mp3 type mpegvideo", NULL, 0, NULL);
	mciSendString((LPCWSTR)L"open explosion.mp3 type mpegvideo", NULL, 0, NULL);
	mciSendString((LPCWSTR)L"open extralife.mp3 type mpegvideo", NULL, 0, NULL);
	mciSendString((LPCWSTR)L"open invincibility.mp3 type mpegvideo", NULL, 0, NULL);

	player = new Player();
	hb = new HealthBar();
	bossEnemy = new Enemy();

	// setez proprietatile initiale ale obiectelor
	// am folosit acest chart pentru culori: http://www.rapidtables.com/web/color/RGB_Color.htm
	player->healthPoints = 3;
	player->speed = 2.0f;
	player->name = "player";
	// culoare lime
	player->color = glm::vec3(0, 1, 0);
	player->length = length;
	player->scale = { 1.0f, 1.0f };
	player->setPosition(logicSpace.width / 2 - length / 2, logicSpace.height / 2 - length / 2);

	bossEnemy->healthPoints = 100;
	bossEnemy->name = "bossEnemy";
	// culoare teal
	bossEnemy->color = glm::vec3(0, 0.502, 0.502);
	bossEnemy->length = length;
	bossEnemy->scale = { 5 * player->scale.x, 5 * player->scale.y };
	bossEnemy->setPosition(logicSpace.width / 2 - bossEnemy->length / 2, logicSpace.height + bossEnemy->scale.x * bossEnemy->length / 2);
	bossEnemy->speed = 0.5f * player->speed;
	Mesh* bossEnemyObj = bossEnemy->CreateObject(bossEnemy->name, corner, bossEnemy->length, bossEnemy->color);
	AddMeshToList(bossEnemyObj);

	// culoare cyan
	hb->color = glm::vec3(0, 1, 1);
	hb->name = "healthbar";
	hb->length = length;
	hb->scale = { (float)1 / 8 , (float)9 / 10 };

	Mesh* playerObj = player->CreateObject(player->name, corner, player->length, player->color);
	Mesh* hbObj = hb->CreateObject(hb->name, corner, hb->length = length, hb->color);
	AddMeshToList(hbObj);
	AddMeshToList(playerObj);
}

// 2D visualization matrix
glm::mat3 Laborator3_Vis2D::VisualizationTransf2D(const LogicSpace & logicSpace, const ViewportSpace & viewSpace)
{
	float sx, sy, tx, ty;
	sx = viewSpace.width / logicSpace.width;
	sy = viewSpace.height / logicSpace.height;
	tx = viewSpace.x - sx * logicSpace.x;
	ty = viewSpace.y - sy * logicSpace.y;

	return glm::transpose(glm::mat3(
		sx, 0.0f, tx,
		0.0f, sy, ty,
		0.0f, 0.0f, 1.0f));
}

// uniform 2D visualization matrix (same scale factor on x and y axes)
glm::mat3 Laborator3_Vis2D::VisualizationTransf2DUnif(const LogicSpace & logicSpace, const ViewportSpace & viewSpace)
{
	float sx, sy, tx, ty, smin;
	sx = viewSpace.width / logicSpace.width;
	sy = viewSpace.height / logicSpace.height;
	if (sx < sy)
		smin = sx;
	else
		smin = sy;
	tx = viewSpace.x - smin * logicSpace.x + (viewSpace.width - smin * logicSpace.width) / 2;
	ty = viewSpace.y - smin * logicSpace.y + (viewSpace.height - smin * logicSpace.height) / 2;

	return glm::transpose(glm::mat3(
		smin, 0.0f, tx,
		0.0f, smin, ty,
		0.0f, 0.0f, 1.0f));
}

void Laborator3_Vis2D::SetViewportArea(const ViewportSpace & viewSpace, glm::vec3 colorColor, bool clear)
{
	glViewport(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

	glEnable(GL_SCISSOR_TEST);
	glScissor(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

	// Clears the color buffer (using the previously set color) and depth buffer
	glClearColor(colorColor.r, colorColor.g, colorColor.b, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_SCISSOR_TEST);

	camera->SetOrthographic((float)viewSpace.x, (float)(viewSpace.x + viewSpace.width), (float)viewSpace.y, (float)(viewSpace.y + viewSpace.height), 0.1f, 400);
	camera->Update();
}

void Laborator3_Vis2D::FrameStart()
{
	// Clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();

	// Sets the screen area where to draw - the left half of the window
	viewSpace = ViewportSpace(0, 0, resolution.x, resolution.y);
	SetViewportArea(viewSpace, glm::vec3(0), true);
}

// generator de numere zecimale incluse intr-un interval (min, max), unde min si max sunt < 1
float Laborator3_Vis2D::RandomGenerator(float min, float max) {

	assert(max > min);

	// 0 < randomFloat < 1 
	double randomFloat = ((double)rand() / RAND_MAX);
	float range = max - min;

	return ((float)randomFloat * range) + min;
}

void Laborator3_Vis2D::Update(float deltaTimeSeconds)
{
	// daca nava sau boss enemy are 0 puncte de viata, s-a incheiat jocul
	// randez pe ecran statusul final al jocului
	// nu vor mai exista actualizari
	if (player->healthPoints == 0 || bossEnemy->healthPoints == 0) {

		// opreste orice interactiune externa
		InputController::SetActive(false);

		// inchid fisierele de sunet
		mciSendString((LPCWSTR)L"close boss.mp3", NULL, 0, NULL);
		mciSendString((LPCWSTR)L"close fireshot.mp3", NULL, 0, NULL);
		mciSendString((LPCWSTR)L"close hit.mp3", NULL, 0, NULL);
		mciSendString((LPCWSTR)L"close explosion.mp3", NULL, 0, NULL);
		mciSendString((LPCWSTR)L"close extralife.mp3", NULL, 0, NULL);
		mciSendString((LPCWSTR)L"close invincibility.mp3", NULL, 0, NULL);

		// rezum sunetul de background
		mciSendString((LPCWSTR)L"resume background.mp3", NULL, 0, NULL);

		// afisez mesajul final
		if (endGameState) {
			if (player->healthPoints == 0)
				cout << "Game Over! You died." << endl;
			else if (bossEnemy->healthPoints == 0)
				cout << "Game Over! You defeated the Boss Enemy." << endl;

			cout << "Your HighScore is: " << killedEnemies << endl;
			endGameState = false;
		}

		// animatia de final a ecranului
		SetViewportArea(viewSpace, glm::vec3(redColorStep, 0, 0), true);
		if (redColorStep < 2.0f)
			redColorStep += deltaTimeSeconds;

		// randez restul obiectelor ramase

		RenderMesh2D(meshes[player->name], player->modelMatrix, player->color);

		for (int i = 0; i < enemies.size(); i++) {
			if ((enemies.at(i))->healthPoints != 0) {
				RenderMesh2D(meshes[(enemies.at(i))->name], (enemies.at(i))->modelMatrix, (enemies.at(i))->color);
			}
		}

		for (int j = 0; j < projectiles.size(); j++) {
			if ((projectiles.at(j))->healthPoints != 0) {
				RenderMesh2D(meshes[(projectiles.at(j))->name], shaders["VertexColor"], (projectiles.at(j))->modelMatrix);
			}
		}

		RenderMesh2D(meshes[bossEnemy->name], shaders["VertexColor"], bossEnemy->modelMatrix);

		for (int j = 0; j < bossProjectiles.size(); j++) {
			if ((bossProjectiles.at(j))->healthPoints != 0) {
				RenderMesh2D(meshes[(bossProjectiles.at(j))->name], shaders["VertexColor"], (bossProjectiles.at(j))->modelMatrix);
			}
		}
	}
	else {

		glm::vec3 corner = glm::vec3(0.0, 0.0, 0);
		// Compute the 2D visualization matrix
		visMatrix = glm::mat3(1);
		visMatrix *= VisualizationTransf2DUnif(logicSpace, viewSpace);

		float cx = corner.x + length / 2;
		float cy = corner.y + length / 2;

		// am ales o varianta simpla de generare obiecte cadou
		// daca player-ul are o viata, se genereaza un obiect de invincibilitate al carui efect dureaza 2 secunde
		// nava se coloreaza in alb
		if (player->healthPoints == 1 && !generatedInvincibilityGift) {

			gift = new Gift();
			gift->setPosition(GeneratePointInEllipse().x + logicSpace.width / 2, GeneratePointInEllipse().y + logicSpace.height / 2);
			gift->healthPoints = 1;
			gift->color = glm::vec3(1, 0.647f, 0);
			gift->speed = 0;
			gift->length = length;
			gift->scale = { (float)1 / 4, (float)1 / 4 };
			gift->name = "invincibilityGift";
			gifts.push_back(gift);

			Mesh* giftObj = gift->CreateObject(gift->name, corner, gift->length, gift->color);
			AddMeshToList(giftObj);

			generatedInvincibilityGift = true;
		}
		// daca player-ul are 2 vieti, generez un obiect de bonus viata
		else if (player->healthPoints == 2 && !generatedHPGift) {

			gift = new Gift();
			gift->setPosition(GeneratePointInEllipse().x + logicSpace.width / 2, GeneratePointInEllipse().y + logicSpace.height / 2);
			gift->healthPoints = 1;
			gift->color = glm::vec3(0, 0.502f, 0);
			gift->speed = 0;
			gift->length = length;
			gift->scale = { (float)1 / 4, (float)1 / 4 };
			gift->name = "bonusHpGift";
			gifts.push_back(gift);

			Mesh* giftObj = gift->CreateObject(gift->name, corner, gift->length, gift->color);
			AddMeshToList(giftObj);

			generatedHPGift = true;
		}

		// dupa 2 secunde de invincibilitate, revine culoarea player-ului si nu mai este invincibil
		if ((Engine::GetElapsedTime() >= player->invincibilityElapsedTime) && player->isInvincible) {
			player->color = glm::vec3(0, 1, 0);
			generatedInvincibilityGift = false;
			player->isInvincible = false;
		}

		// randez obiectele cadou
		for (int k = 0; k < gifts.size(); k++) {

			// daca obiectul cadou nu fost consumat anterior
			if ((gifts.at(k))->healthPoints != 0) {

				// actualizez matricea de modelare
				modelMatrix = visMatrix * Transform2D::Translate((gifts.at(k))->positionX, (gifts.at(k))->positionY);
				modelMatrix *= Transform2D::Translate(cx, cy);
				modelMatrix *= Transform2D::Scale(gift->scale.x, gift->scale.y);
				modelMatrix *= Transform2D::Translate(-cx, -cy);

				// daca player-ul ia obiectul, acesta nu mai este randat in continuare
				if (DetectCollisionPlayerGift(player, gifts.at(k))) {
					if ((gifts.at(k))->name == "bonusHpGift") {
						generatedHPGift = false;
						player->healthPoints++;
						cout << "Well done! You increased your life." << endl;
						mciSendString((LPCWSTR)L"play extralife.mp3 from 0", NULL, 0, NULL);
					}
					else if ((gifts.at(k))->name == "invincibilityGift") {
						player->color = glm::vec3(1, 1, 1);
						player->invincibilityElapsedTime = (float)Engine::GetElapsedTime() + 2.0f;
						player->isInvincible = true;
						cout << "You've become immortal for 2 seconds!!!" << endl;
						mciSendString((LPCWSTR)L"play invincibility.mp3 from 0", NULL, 0, NULL);
					}
					(gifts.at(k))->healthPoints = 0;
					meshes.erase((gifts.at(k))->name);
				}

				// randez obiectul
				if ((gifts.at(k))->healthPoints != 0) {
					(gifts.at(k))->modelMatrix = modelMatrix;
					RenderMesh2D(meshes[(gifts.at(k))->name], shaders["VertexColor"], modelMatrix);
				}
			}
		}

		float centerX = player->positionX + length / 2;
		float centerY = player->positionY + length / 2;

		// calculez pozitia logica a mouse-ului
		LogicalMousePosition(window->GetCursorPosition().x, window->GetCursorPosition().y);

		// rotatia navei dupa mouse
		player->rotation = glm::atan(mouseY - centerY, mouseX - centerX);

		// randarea navei player-ului
		// actualizez matricea de modelare
		modelMatrix = visMatrix * Transform2D::Translate(player->positionX, player->positionY);
		modelMatrix *= Transform2D::Translate(cx, cy);
		modelMatrix *= Transform2D::Rotate(player->rotation);
		modelMatrix *= Transform2D::Translate(-cx, -cy);
		player->modelMatrix = modelMatrix;
		RenderMesh2D(meshes[player->name], modelMatrix, player->color);

		// generez inamici la un interval de timp pe circumferinta unei elipse
		// spatiul meu logic e un dreptunghi
		// dupa ce au murit 100 de inamici nu se mai genereaza altii
		currTime = Engine::GetElapsedTime();
		if (killedEnemies < 100) {

			if (currTime >= (preTime + offset)) {

				double circumferenceAngle = ((double)rand() / RAND_MAX) * 2 * glm::pi<float>();

				glm::vec2 position = { glm::cos(circumferenceAngle) * (logicSpace.width / 2 + length / 2) , glm::sin(circumferenceAngle) * (logicSpace.height / 2 + length / 2) };

				double probability = ((double)rand() / RAND_MAX) + 1;

				// viteza random a unui inamic mai mica ca viteza navei player-ului
				double randomSpeed = RandomGenerator(0.2f, 0.6f);

				// inamic cu proprietile respective
				enemy = new Enemy();
				enemy->setPosition(position.x + logicSpace.width / 2 - length / 2, position.y + logicSpace.height / 2 - length / 2);
				enemy->healthPoints = probability < 1.5f ? 1 : 2;
				enemy->color = enemy->healthPoints == 1 ? glm::vec3(0, 0, 1) : glm::vec3(1, 1, 0);
				enemy->speed = (float)randomSpeed * player->speed;
				enemy->length = length;
				enemy->scale = { 1.0f, 1.0f };
				enemy->enemyScaleStep = 0.0;
				enemy->name = "enemy" + to_string(enemyId);
				enemies.push_back(enemy);
				Mesh* enemyObj = enemy->CreateObject(enemy->name, corner, enemy->length, enemy->color);
				enemyId++;

				// in functie de probabilitate se schimba intervalul de timp in care se genereaza inamici
				// intervalul scade pana la 0.5 secunde
				double apparitionProbability = ((double)rand() / RAND_MAX);

				if (offset - 0.5f > 0.01f && apparitionProbability <= 0.5f)
					offset -= extracter;
				preTime = currTime;

				AddMeshToList(enemyObj);
			}
		}

		// cand au fost omorati 100 de inamici
		else if (killedEnemies == 100 && bossShowedUp) {
			bossShowedUp = false;
			mciSendString((LPCWSTR)L"pause background.mp3", NULL, 0, NULL);

			mciSendString((LPCWSTR)L"open boss.mp3 type mpegvideo", NULL, 0, NULL);
			mciSendString((LPCWSTR)L"play boss.mp3 repeat", NULL, 0, NULL);
		}

		// randez boss enemy dupa ce au fost omorati cel putin 100 de inamici
		if (killedEnemies >= 100) {

			if (bossEnemy->healthPoints != 0) {
				if (bossInitialTime) {
					bossProjectileCurrTime = Engine::GetElapsedTime();
					bossProjectilePreTime = Engine::GetElapsedTime();
					bossInitialTime = false;
				}
				else {
					bossProjectileCurrTime = Engine::GetElapsedTime();
				}

				float bossCx = corner.x + bossEnemy->length / 2;
				float bossCy = corner.y + bossEnemy->length / 2;

				// boss-ul se roteste spre player mereu si trage cu proiectile spre acesta
				bossEnemy->rotation = glm::atan(player->positionY - bossEnemy->positionY, player->positionX - bossEnemy->positionX);

				// actualizez matricea de modelare
				modelMatrix = visMatrix * Transform2D::Translate(bossEnemy->positionX, bossEnemy->positionY);
				modelMatrix *= Transform2D::Translate(bossCx, bossCy);
				modelMatrix *= Transform2D::Rotate(bossEnemy->rotation);
				modelMatrix *= Transform2D::Translate(-bossCx, -bossCy);
				modelMatrix *= Transform2D::Translate(bossCx, bossCy);
				modelMatrix *= Transform2D::Scale(bossEnemy->scale.x, bossEnemy->scale.y);
				modelMatrix *= Transform2D::Translate(-bossCx, -bossCy);

				// generez proiectilele
				if (bossProjectileCurrTime - bossProjectilePreTime >= 0.8) {
					projectile = new Projectile();
					projectile->scale = { (float)1 / 4, (float)1 / 4 };
					projectile->setPosition(bossEnemy->positionX, bossEnemy->positionY);
					projectile->rotation = glm::atan(player->positionY - projectile->positionY, player->positionX - projectile->positionX);
					projectile->name = "bossProjectile" + to_string(bossProjectileId);
					projectile->color = glm::vec3(1, 0, 1);
					projectile->speed = player->speed;
					projectile->healthPoints = 1;
					projectile->length = length;
					bossProjectiles.push_back(projectile);
					Mesh* projectileObj = projectile->CreateObject(projectile->name, glm::vec3(0.0, 0.0, 0), projectile->length, projectile->color);
					bossProjectileId++;
					AddMeshToList(projectileObj);

					// sunet atunci cand boss enemy trage cu un proiectil
					mciSendString((LPCWSTR)L"play fireshot.mp3 from 0", NULL, 0, NULL);

					bossProjectilePreTime = bossProjectileCurrTime;
				}

				// coliziune intre player si boss enemy
				// nava player-ului moare si se porneste un sunet de explozie
				if (DetectCollisionPlayerEnemy(player, bossEnemy)) {
					mciSendString((LPCWSTR)L"play explosion.mp3 from 0", NULL, 0, NULL);
					player->healthPoints = 0;
					bossEnemy->healthPoints--;
				}
				else {
					// altfel daca este coliziune intre un proiectil si o nava, se scade din healthpoints si se pornesc sunetele respective
					// daca player-ul este invincibil viata acestuia este neafectata
					// daca player-ul este lovit de un proiectil al boss-ului, se scade din viata cu o unitate
					for (int j = 0; j < bossProjectiles.size(); j++) {

						if (bossEnemy->healthPoints != 0 && (bossProjectiles.at(j))->healthPoints != 0) {
							if (DetectCollisionProjectileEnemy(bossProjectiles.at(j), player)) {
								mciSendString((LPCWSTR)L"play hit.mp3 from 0", NULL, 0, NULL);
								if (!player->isInvincible)
									player->healthPoints--;
								(bossProjectiles.at(j))->healthPoints = 0;
							}
						}
					}

					for (int j = 0; j < projectiles.size(); j++) {

						if (bossEnemy->healthPoints != 0 && (projectiles.at(j))->healthPoints != 0) {
							if (DetectCollisionProjectileEnemy(projectiles.at(j), bossEnemy)) {
								mciSendString((LPCWSTR)L"play hit.mp3 from 0", NULL, 0, NULL);
								bossEnemy->healthPoints--;
								(projectiles.at(j))->healthPoints = 0;
							}
						}
					}
				}

				// se actualizeaza pozitia boss-ului
				// acesta vine initial din partea de sus a ecranului pana ajunge la o anumita pozitie
				// cand a ajuns la acea pozitie, se misca doar stanga si dreapta
				if (bossEnemy->positionY > (logicSpace.height - bossEnemy->length / 2 * bossEnemy->scale.x - bossEnemy->length / 2)) {
					bossEnemy->positionY -= bossEnemy->speed * deltaTimeSeconds;
					bossYDirection = true;
				}
				else
					bossYDirection = false;

				if (!bossYDirection) {

					if (!bossXDirection) {
						bossEnemy->positionX += bossEnemy->speed * deltaTimeSeconds;
					}
					else
						bossEnemy->positionX -= bossEnemy->speed * deltaTimeSeconds;

					if (bossEnemy->positionX < bossEnemy->length / 2 * bossEnemy->scale.x - bossEnemy->length / 2)
						bossXDirection = false;
					else if (bossEnemy->positionX > logicSpace.width - bossEnemy->length / 2 * bossEnemy->scale.x - bossEnemy->length / 2)
						bossXDirection = true;
				}

				// randez boss-ul daca nu este inca mort
				if (bossEnemy->healthPoints != 0) {
					bossEnemy->modelMatrix = modelMatrix;
					RenderMesh2D(meshes[bossEnemy->name], modelMatrix, bossEnemy->color);
				}
			}
			// randez proiectilele boss-ului
			RenderProjectile(bossProjectiles, deltaTimeSeconds);
		}

		for (int i = 0; i < enemies.size(); i++) {

			// daca inamicul nu e mort ii actualizez matricea de modelare
			if ((enemies.at(i))->healthPoints != 0) {
				(enemies.at(i))->rotation = glm::atan(player->positionY - (enemies.at(i))->positionY, player->positionX - (enemies.at(i))->positionX);
				modelMatrix = visMatrix * Transform2D::Translate((enemies.at(i))->positionX, (enemies.at(i))->positionY);
				modelMatrix *= Transform2D::Translate(cx, cy);
				modelMatrix *= Transform2D::Rotate((enemies.at(i))->rotation);
				modelMatrix *= Transform2D::Translate(-cx, -cy);
				if ((enemies.at(i))->healthPoints == 1 && (enemies.at(i))->color == glm::vec3(1, 0, 0)) {
					modelMatrix *= Transform2D::Translate(cx, cy);

					// incrementez pasul de scalare la fiecare frame
					if ((enemies.at(i))->enemyScaleStep < 0.25f)
						(enemies.at(i))->enemyScaleStep += deltaTimeSeconds;

					// actualizez enemy length la fiecare frame pentru a realiza o coliziune cat mai realistica
					(enemies.at(i))->length = length * ((0.5f - 2 * (enemies.at(i))->enemyScaleStep) + (enemies.at(i))->scale.x);
					modelMatrix *= Transform2D::Scale((0.5f - 2 * (enemies.at(i))->enemyScaleStep) + (enemies.at(i))->scale.x, (0.5f - 2 * (enemies.at(i))->enemyScaleStep) + (enemies.at(i))->scale.y);
					modelMatrix *= Transform2D::Translate(-cx, -cy);
				}

				// coliziune intre player si enemy
				if (DetectCollisionPlayerEnemy(player, enemies.at(i))) {
					mciSendString((LPCWSTR)L"play explosion.mp3 from 0", NULL, 0, NULL);
					if (!player->isInvincible)
						player->healthPoints--;
					(enemies.at(i))->healthPoints = 0;
					meshes.erase((enemies.at(i))->name);
				}
				else {
					// actualizez pozitia inamicului
					// acesta se deplaseaza cu viteza constanta spre player cu ajutorul functiilor cos si sin care ofera orientarea
					(enemies.at(i))->positionX += glm::cos((enemies.at(i))->rotation) * (enemies.at(i))->speed * deltaTimeSeconds;
					(enemies.at(i))->positionY += glm::sin((enemies.at(i))->rotation) * (enemies.at(i))->speed * deltaTimeSeconds;

					if ((enemies.at(i))->healthPoints != 0) {
						for (int j = 0; j < projectiles.size(); j++) {

							if ((enemies.at(i))->healthPoints != 0 && (projectiles.at(j))->healthPoints != 0) {
								if (DetectCollisionProjectileEnemy(projectiles.at(j), enemies.at(i))) {
									// in urma coliziunii dintre proiectil si enemy
									// daca inamicul are 2 vieti, se scaleaza la jumatate, se schimba culoarea si se dubleaza viteza
									if ((enemies.at(i))->healthPoints == 2) {
										mciSendString((LPCWSTR)L"play hit.mp3 from 0", NULL, 0, NULL);
										(enemies.at(i))->healthPoints--;
										(enemies.at(i))->color = glm::vec3(1, 0, 0);
										(enemies.at(i))->scale = { (float)1 / 2, (float)1 / 2 };
										(enemies.at(i))->speed *= 2;
									}
									else {
										// altfel, daca inamicul are o viata, in urma coliziunii acesta moare
										mciSendString((LPCWSTR)L"play explosion.mp3 from 0", NULL, 0, NULL);
										(enemies.at(i))->healthPoints = 0;
										meshes.erase((enemies.at(i))->name);
										killedEnemies++;
										cout << "Score: " << killedEnemies << endl;
									}

									// proiectilul acesta nu va mai fi randat
									(projectiles.at(j))->healthPoints = 0;
								}
							}
						}
					}
					// daca inamicul nu e mort
					if ((enemies.at(i))->healthPoints != 0) {
						(enemies.at(i))->modelMatrix = modelMatrix;
						RenderMesh2D(meshes[(enemies.at(i))->name], modelMatrix, (enemies.at(i))->color);
					}
				}
			}
		}

		// randez proiectilele navei si barele de viata
		RenderProjectile(projectiles, deltaTimeSeconds);
		RenderHealthPoints();
	}
}

// functie care genereaza un punct random in interiorul unei elipse cu ajutorul a doua numere random subunitare
glm::vec2 Laborator3_Vis2D::GeneratePointInEllipse() {
	double randomNumber1 = ((double)rand() / (RAND_MAX));
	double randomNumber2 = ((double)rand() / (RAND_MAX));

	// interschimb numerele in caz ca primul e mai mare
	if (randomNumber2 < randomNumber1) {
		double aux = randomNumber2;
		randomNumber2 = randomNumber1;
		randomNumber1 = aux;
	}
	glm::vec2 position = { randomNumber2 * (logicSpace.width - length / 2) / 2 * glm::cos(2 * glm::pi<float>() * randomNumber1 / randomNumber2), \
		randomNumber2 * (logicSpace.height - length / 2) / 2 * glm::sin(2 * glm::pi<float>() * randomNumber1 / randomNumber2) };

	return position;
}

void Laborator3_Vis2D::RenderHealthPoints() {
	for (int i = 0; i < player->healthPoints; i++) {
		// barele de viata sunt uniform distantate
		modelMatrix = visMatrix * Transform2D::Translate(logicSpace.width - (0.25f + (float)(player->healthPoints - i - 1) / 18), logicSpace.height - 0.35f);
		modelMatrix *= Transform2D::Scale(hb->scale.x, hb->scale.y);
		RenderMesh2D(meshes[hb->name], shaders["VertexColor"], modelMatrix);
	}
}

void Laborator3_Vis2D::RenderProjectile(std::vector<Projectile*> projectiles, float deltaTimeSeconds) {

	glm::vec3 corner = glm::vec3(0.0, 0.0, 0);

	for (int i = 0; i < projectiles.size(); i++) {
		if ((projectiles.at(i))->healthPoints != 0) {
			float cx = corner.x + (length / 2);
			float cy = corner.y + (length / 2);

			// actualizez matricea de modelare
			modelMatrix = visMatrix * Transform2D::Translate((projectiles.at(i))->positionX, (projectiles.at(i))->positionY);
			modelMatrix *= Transform2D::Translate(cx, cy);
			modelMatrix *= Transform2D::Rotate((projectiles.at(i))->rotation);
			modelMatrix *= Transform2D::Translate(-cx, -cy);
			modelMatrix *= Transform2D::Translate(cx, cy);
			modelMatrix *= Transform2D::Scale((projectiles.at(i))->scale.x, (projectiles.at(i))->scale.y);
			modelMatrix *= Transform2D::Translate(-cx, -cy);

			// fiecare proiectil se deplaseaza spre pozitia mouse-ului
			(projectiles.at(i))->positionX += glm::cos((projectiles.at(i))->rotation) * (projectiles.at(i))->speed * deltaTimeSeconds;
			(projectiles.at(i))->positionY += glm::sin((projectiles.at(i))->rotation) * (projectiles.at(i))->speed * deltaTimeSeconds;
			(projectiles.at(i))->modelMatrix = modelMatrix;
			RenderMesh2D(meshes[(projectiles.at(i))->name], shaders["VertexColor"], modelMatrix);
		}
	}
}

// coliziunea dintre player si enemy
// folosesc coliziunea cerc-cerc
bool Laborator3_Vis2D::DetectCollisionPlayerEnemy(Player *player, Enemy *enemy) {

	float playerRadius = player->length / 2;
	float enemyRadius;
	glm::vec2 offsetPosition;

	if (enemy->name == "bossEnemy") {
		enemyRadius = enemy->scale.x * enemy->length / 2;
		offsetPosition = { enemy->length / 2 - enemyRadius, enemy->length / 2 - enemyRadius };
	}
	else {
		enemyRadius = enemy->length / 2;
		offsetPosition = { length / 2 - enemyRadius, length / 2 - enemyRadius };
	}

	glm::vec2 distance = { (player->positionX - (enemy->positionX + offsetPosition.x)) + (playerRadius - enemyRadius), \
		(player->positionY - (enemy->positionY + offsetPosition.y)) + (playerRadius - enemyRadius) };
	float rootDistance = glm::pow(distance.x, 2) + glm::pow(distance.y, 2);

	// daca suma patratelor distantelor dintre cele doua cercuri e mai mica ca patratul sumei razelor
	if (rootDistance < glm::pow(playerRadius + enemyRadius, 2))
		return true;
	return false;
}

// coliziunea dintre proiectil si enemy(boss enemy)
bool Laborator3_Vis2D::DetectCollisionProjectileEnemy(Projectile *projectile, GameObject *enemy) {

	// in caz ca proiectilul este dreptunghi, aleg latura mai mica(optional)
	float projectileLength = projectile->length * projectile->scale.x;
	float projectileWidth = projectile->length * projectile->scale.y;
	float projectileMinDiameter = projectileLength < projectileWidth ? projectileLength : projectileWidth;
	float projectileRadius = (float)glm::sqrt(2 * glm::pow(projectileMinDiameter, 2)) / 2;
	float enemyRadius;
	glm::vec2 offsetPosition;

	if (enemy->name == "bossEnemy") {
		enemyRadius = enemy->scale.x * enemy->length / 2;
		offsetPosition = { enemy->length / 2 - enemyRadius, enemy->length / 2 - enemyRadius };
	}
	else {
		enemyRadius = enemy->length / 2;
		offsetPosition = { length / 2 - enemyRadius, length / 2 - enemyRadius };
	}

	// proiectilul are aceeasi pozitie (x, y), chiar daca este scalat
	// de aceea trebuie sa ma folosesc de o pozitie virtuala pentru a realiza coliziunea corect
	float projectilePositionX = projectile->positionX + (projectile->length - projectile->length * projectile->scale.x) / 2;
	float projectilePositionY = projectile->positionY + (projectile->length - projectile->length * projectile->scale.y) / 2;
	glm::vec2 distance = { (projectilePositionX + (projectileLength - projectileWidth)) - (enemy->positionX + offsetPosition.x) + (projectileRadius - enemyRadius), \
		projectilePositionY - (enemy->positionY + offsetPosition.y) + (projectileRadius - enemyRadius) };
	float rootDistance = glm::pow(distance.x, 2) + glm::pow(distance.y, 2);

	// daca suma patratelor distantelor dintre cele doua cercuri e mai mica ca patratul sumei razelor
	if (rootDistance < glm::pow(projectileRadius + enemyRadius, 2))
		return true;
	return false;
}

// coliziunea dintre player si obiectul cadou
bool Laborator3_Vis2D::DetectCollisionPlayerGift(Player *player, Gift *gift) {
	float playerRadius = player->length / 2;
	float giftRadius;
	glm::vec2 offsetPosition;

	giftRadius = gift->scale.x * gift->length / 2;
	offsetPosition = { gift->length / 2 - giftRadius, gift->length / 2 - giftRadius };

	glm::vec2 distance = { (player->positionX - (gift->positionX + offsetPosition.x)) + (playerRadius - giftRadius), \
		(player->positionY - (gift->positionY + offsetPosition.y)) + (playerRadius - giftRadius) };
	float rootDistance = glm::pow(distance.x, 2) + glm::pow(distance.y, 2);

	if (rootDistance < glm::pow(playerRadius + giftRadius, 2))
		return true;
	return false;
}

void Laborator3_Vis2D::FrameEnd()
{

}

void Laborator3_Vis2D::OnInputUpdate(float deltaTime, int mods)
{
	// nava se misca in orice directie continuu ca urmare a apasarii tastelor W, A, S, D

	if (window->KeyHold(GLFW_KEY_W)) {
		player->positionY += player->speed * deltaTime;
	}

	if (window->KeyHold(GLFW_KEY_A)) {
		player->positionX -= player->speed * deltaTime;
	}

	if (window->KeyHold(GLFW_KEY_S)) {
		player->positionY -= player->speed * deltaTime;
	}

	if (window->KeyHold(GLFW_KEY_D)) {
		player->positionX += player->speed * deltaTime;
	}
}

void Laborator3_Vis2D::OnKeyPress(int key, int mods)
{

}

void Laborator3_Vis2D::OnKeyRelease(int key, int mods)
{

}

// pozitia logica a mouse-lui in spatiul logic definit
void Laborator3_Vis2D::LogicalMousePosition(int mouseX, int mouseY) {
	float sx, sy, tx, ty, smin;
	sx = viewSpace.width / logicSpace.width;
	sy = viewSpace.height / logicSpace.height;
	if (sx < sy)
		smin = sx;
	else
		smin = sy;
	tx = viewSpace.x - smin * logicSpace.x + (viewSpace.width - smin * logicSpace.width) / 2;
	ty = viewSpace.y - smin * logicSpace.y + (viewSpace.height - smin * logicSpace.height) / 2;

	this->mouseX = (mouseX - tx) / smin;
	this->mouseY = (viewSpace.height - mouseY - ty) / smin;
}

void Laborator3_Vis2D::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// cand se misca cursorul, calculez pozitia logica a mouse-ului
	LogicalMousePosition(mouseX, mouseY);
}

void Laborator3_Vis2D::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	float sx, sy, tx, ty, smin;
	sx = viewSpace.width / logicSpace.width;
	sy = viewSpace.height / logicSpace.height;
	if (sx < sy)
		smin = sx;
	else
		smin = sy;
	tx = viewSpace.x - smin * logicSpace.x + (viewSpace.width - smin * logicSpace.width) / 2;
	ty = viewSpace.y - smin * logicSpace.y + (viewSpace.height - smin * logicSpace.height) / 2;

	float mousePressedX = (mouseX - tx) / smin;
	float mousePressedY = (viewSpace.height - mouseY - ty) / smin;

	// cand apas click-dreapta generez un proiectil
	if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT)) {
		mciSendString((LPCWSTR)L"play fireshot.mp3 from 0", NULL, 0, NULL);

		projectile = new Projectile();
		projectile->scale = { (float)1 / 6, (float)1 / 6 };
		projectile->setPosition(player->positionX, player->positionY);
		projectile->rotation = glm::atan(mousePressedY - projectile->positionY - length / 2, mousePressedX - projectile->positionX - length / 2);
		projectile->name = "projectile" + to_string(projectileId);
		projectile->color = glm::vec3(1, 0, 1);
		projectile->speed = 2 * player->speed;
		projectile->healthPoints = 1;
		projectile->length = length;
		projectiles.push_back(projectile);
		Mesh* projectileObj = projectile->CreateObject(projectile->name, glm::vec3(0.0, 0.0, 0), projectile->length, projectile->color);
		projectileId++;
		AddMeshToList(projectileObj);
	}
}

void Laborator3_Vis2D::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{

}

void Laborator3_Vis2D::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}