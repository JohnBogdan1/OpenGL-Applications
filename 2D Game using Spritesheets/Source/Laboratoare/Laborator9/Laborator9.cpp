#include "Laborator9.h"
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <Core/Engine.h>

using namespace std;

Laborator9::Laborator9()
{
}

Laborator9::~Laborator9()
{
}

void Laborator9::Init()
{
	const string textureLoc = "Source/Laboratoare/Laborator9/Textures/";

	player = new Player();
	player->setPosition(-4.0f, -2.8f);

	enemy = new Enemy();
	enemy->setPosition(4.0f, -2.8f);

	// Load textures
	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((textureLoc + "ssj3goku.gif").c_str(), GL_CLAMP_TO_BORDER);
		player->sheet_width = texture->GetWidth();
		player->sheet_height = texture->GetHeight();
		mapTextures["ssj3goku"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((textureLoc + "ssj1goku.png").c_str(), GL_CLAMP_TO_BORDER);
		enemy->sheet_width = texture->GetWidth();
		enemy->sheet_height = texture->GetHeight();
		mapTextures["ssj1goku"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((textureLoc + "arena.jpeg").c_str(), GL_CLAMP_TO_BORDER);
		mapTextures["arena"] = texture;
	}

	// Load quad for arena
	{
		Mesh* mesh = new Mesh("quad");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "quad.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	// Create a shader program for drawing face polygon with the color of the normal
	{
		Shader *shader = new Shader("ShaderLab9");
		shader->AddShader("Source/Laboratoare/Laborator9/Shaders/VertexShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Laborator9/Shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{
		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_file("Source/Laboratoare/Laborator9/ssj3goku_animations.xml");

		pugi::xml_node root = doc.first_child();

		RecursiveParsePlayer(root);
		player->animations.push_back(animation);
		//cout << player->animations.size() << endl;
		//cout << player->animations.at(4)->frames.size() << endl;

		for (int i = 0; i < player->animations.size(); i++) {
			player->animation_names[player->animations.at(i)->name] = i;
			for (int j = 0; j < player->animations.at(i)->frames.size(); j++) {

				int bottom_left = std::stoi(player->animations.at(i)->frames.at(j)->rectangle.at(0));
				int bottom_right = std::stoi(player->animations.at(i)->frames.at(j)->rectangle.at(2));
				int top_right = std::stoi(player->animations.at(i)->frames.at(j)->rectangle.at(3));
				int top_left = std::stoi(player->animations.at(i)->frames.at(j)->rectangle.at(1));
				vector<glm::vec2> textureCoords
				{
					glm::vec2((float)bottom_right / player->sheet_width, (float)top_left / player->sheet_height), // x + w, y
					glm::vec2((float)bottom_right / player->sheet_width, (float)top_right / player->sheet_height), // x + w, y + h
					glm::vec2((float)bottom_left / player->sheet_width, (float)top_right / player->sheet_height), // x, y + w
					glm::vec2((float)bottom_left / player->sheet_width, (float)top_left / player->sheet_height), // x, y
				};
				CreateMeshWithTextureCoords("player_" + player->animations.at(i)->name + std::to_string(j), textureCoords);
			}
		}
	}

	{
		// reset animation pointer
		animation = nullptr;

		// la fel si pentru enemy
		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_file("Source/Laboratoare/Laborator9/ssj1goku_animations.xml");

		pugi::xml_node root = doc.first_child();

		RecursiveParseEnemy(root);
		enemy->animations.push_back(animation);
		//cout << enemy->animations.size() << endl;
		//cout << enemy->animations.at(0)->frames.size() << endl;

		for (int i = 0; i < enemy->animations.size(); i++) {
			enemy->animation_names[enemy->animations.at(i)->name] = i;
			for (int j = 0; j < enemy->animations.at(i)->frames.size(); j++) {

				int bottom_left = std::stoi(enemy->animations.at(i)->frames.at(j)->rectangle.at(0));
				int bottom_right = std::stoi(enemy->animations.at(i)->frames.at(j)->rectangle.at(2));
				int top_right = std::stoi(enemy->animations.at(i)->frames.at(j)->rectangle.at(3));
				int top_left = std::stoi(enemy->animations.at(i)->frames.at(j)->rectangle.at(1));
				vector<glm::vec2> textureCoords
				{
					glm::vec2((float)bottom_right / enemy->sheet_width, (float)top_left / enemy->sheet_height), // x + w, y
					glm::vec2((float)bottom_right / enemy->sheet_width, (float)top_right / enemy->sheet_height), // x + w, y + h
					glm::vec2((float)bottom_left / enemy->sheet_width, (float)top_right / enemy->sheet_height), // x, y + w
					glm::vec2((float)bottom_left / enemy->sheet_width, (float)top_left / enemy->sheet_height), // x, y
				};
				CreateMeshWithTextureCoords("enemy_" + enemy->animations.at(i)->name + std::to_string(j), textureCoords);
			}
		}
	}
}

void split(const string &s, char delim, vector<string> &elems) {
	stringstream ss;
	ss.str(s);
	string item;
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}
}

vector<string> split(const string &s, char delim) {
	vector<string> elems;
	split(s, delim, elems);
	return elems;
}

void Laborator9::RecursiveParsePlayer(pugi::xml_node tools) {

	for (pugi::xml_node tool : tools.children()) {
		for (pugi::xml_node child : tool.children())
		{
			std::string pname = tool.name();

			if (pname == "sourceRect") {
				std::string str = child.value();
				str = str.substr(5, str.length() - 6);
				frame = new Frame();
				frame->rectangle = split(str, ',');
			}
			else if (pname == "animations") {
				if (animation != nullptr) {
					player->animations.push_back(animation);
				}
				animation = new Animation();
				animation->name = child.name();
			}
			else if (pname == "duration") {
				frame->duration = child.value();
			}
			else if (pname == "offset") {
				animation->frames.push_back(frame);
			}
			RecursiveParsePlayer(child);
		}
	}

	return;
}

void Laborator9::RecursiveParseEnemy(pugi::xml_node tools) {

	for (pugi::xml_node tool : tools.children()) {
		for (pugi::xml_node child : tool.children())
		{
			std::string pname = tool.name();

			if (pname == "sourceRect") {
				std::string str = child.value();
				str = str.substr(5, str.length() - 6);
				frame = new Frame();
				frame->rectangle = split(str, ',');
			}
			else if (pname == "animations") {
				if (animation != nullptr) {
					enemy->animations.push_back(animation);
				}
				animation = new Animation();
				animation->name = child.name();
			}
			else if (pname == "duration") {
				frame->duration = child.value();
			}
			else if (pname == "offset") {
				animation->frames.push_back(frame);
			}
			RecursiveParseEnemy(child);
		}
	}

	return;
}

bool Laborator9::DetectCollision(glm::vec2 player_position, glm::vec2 enemy_position) {

	// collision?
	if (player->orientation == "left" && enemy->orientation == "left") {
		if ((player_position.x - width) < enemy_position.x && player_position.x > (enemy_position.x - width) && \
			player_position.y < (enemy_position.y + height) && (player_position.y + height) > enemy_position.y) {
			if (player_position.x < enemy_position.x && enemy->hit) {
				return true;
			}
			else if (player_position.x > enemy_position.x && player->hit) {
				return true;
			}
		}
	}
	else if (player->orientation == "right" && enemy->orientation == "right") {
		if (player_position.x < (enemy_position.x + width) && (player_position.x + width) > enemy_position.x && \
			player_position.y < (enemy_position.y + height) && (player_position.y + height) > enemy_position.y) {
			if (player_position.x < enemy_position.x && player->hit) {
				return true;
			}
			else if (player_position.x > enemy_position.x && enemy->hit) {
				return true;
			}
		}
	}
	else if (player->orientation == "right" && enemy->orientation == "left") {
		if (player_position.x < enemy_position.x && (player_position.x + 2 * width) > enemy_position.x && \
			player_position.y < (enemy_position.y + height) && (player_position.y + height) > enemy_position.y) {
			if (player_position.x < (enemy_position.x - width)) {
				if (player->hit || enemy->hit)
					return true;
			}
		}
	}
	else if (player->orientation == "left" && enemy->orientation == "right") {
		if (player_position.x < (enemy_position.x + 2 * width) && player_position.x > enemy_position.x && \
			player_position.y < (enemy_position.y + height) && (player_position.y + height) > enemy_position.y) {
			if (enemy_position.x < (player_position.x - width)) {
				if (player->hit || enemy->hit)
					return true;
			}
		}
	}

	return false;
}

void Laborator9::CreateMeshWithTextureCoords(string name, vector<glm::vec2> textureCoords) {

	// Create a simple quad
	{
		vector<glm::vec3> vertices
		{
			glm::vec3(1.0f, 1.5f, 0.0f),	// Top Right
			glm::vec3(1.0f, 0.0f, 0.0f),	// Bottom Right
			glm::vec3(0.0f, 0.0f, 0.0f),	// Bottom Left
			glm::vec3(0.0f, 1.5f, 0.0f),	// Top Left
		};

		vector<glm::vec3> normals
		{
			glm::vec3(0, 1, 1),
			glm::vec3(1, 0, 1),
			glm::vec3(1, 0, 0),
			glm::vec3(0, 1, 0)
		};

		vector<unsigned short> indices =
		{
			0, 1, 3,
			1, 2, 3,
		};

		Mesh* mesh = new Mesh(name);
		mesh->InitFromData(vertices, normals, textureCoords, indices);
		meshes[mesh->GetMeshID()] = mesh;
	}
}

void Laborator9::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	viewSpace = ViewportSpace(0, 0, resolution.x, resolution.y);
	SetViewportArea(viewSpace, glm::vec3(0), true);
}

void Laborator9::SetViewportArea(const ViewportSpace & viewSpace, glm::vec3 colorColor, bool clear)
{
	glViewport(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

	glEnable(GL_SCISSOR_TEST);
	glScissor(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

	// Clears the color buffer (using the previously set color) and depth buffer
	glClearColor(colorColor.r, colorColor.g, colorColor.b, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_SCISSOR_TEST);

	GetSceneCamera()->SetOrthographic(-10, 10, -10, 10, 0.1f, 1200);
	GetSceneCamera()->SetRotation(glm::vec3(0, 0, 0));
	GetSceneCamera()->Update();
	GetCameraInput()->SetActive(false);
}

void Laborator9::Update(float deltaTimeSeconds)
{

	if (player->healthpoints == 0) {
		cout << "ENEMY WON THE FIGHT!!!" << endl;
		InputController::SetActive(false);
	}
	else if (enemy->healthpoints == 0) {
		cout << "PLAYER WON THE FIGHT!!!" << endl;
		InputController::SetActive(false);
	}

	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 1.6f, backgroundArenaZ));
		modelMatrix = glm::rotate(modelMatrix, RADIANS(180), glm::vec3(1, 0, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(10, 3 * 3.4f, 0));
		RenderSimpleMesh(meshes["quad"], shaders["ShaderLab9"], modelMatrix, mapTextures["arena"]);
	}

	// *********************PLAYER***********************
	if (player->default_state) {
		player->move_forward = false;
		player->move_backward = false;
		player->hit = false;
		player->blocking = false;
		player->falling = false;
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(player->position.x, player->position.y, fixedZ));
		if (player->orientation == "left")
			modelMatrix = glm::rotate(modelMatrix, RADIANS(180), glm::vec3(0, 1, 0)); // pentru mirrored
		modelMatrix = glm::scale(modelMatrix, glm::vec3(2));
		RenderSimpleMesh(meshes["player_" + player->animations.at(player->animation_names["default"])->name + to_string(0)], shaders["ShaderLab9"], modelMatrix, mapTextures["ssj3goku"]);
	}
	else if (player->move_forward) {
		glm::mat4 modelMatrix = glm::mat4(1);
		if (player->last_orientation == "left") {
			player->position.x -= 1.0f;
		}
		modelMatrix = glm::translate(modelMatrix, glm::vec3(player->position.x, player->position.y, fixedZ));
		//modelMatrix = glm::rotate(modelMatrix, RADIANS(180), glm::vec3(0, 1, 0)); // pentru mirrored
		modelMatrix = glm::scale(modelMatrix, glm::vec3(2));
		RenderSimpleMesh(meshes["player_" + player->animations.at(player->animation_names["walking"])->name + to_string(player->walking_forward_id)], shaders["ShaderLab9"], modelMatrix, mapTextures["ssj3goku"]);

		player->walking_forward_frames_id++;
		if (player->walking_forward_frames_id > 8 / factor) {
			player->walking_forward_id++;
			player->walking_forward_frames_id = 0;
		}
		if (player->walking_forward_id == 3) {
			player->walking_forward_id = 0;
			player->move_forward = false;
			player->default_state = true;
		}
		player->last_orientation = "right";
	}
	else if (player->move_backward) {
		glm::mat4 modelMatrix = glm::mat4(1);
		if (player->last_orientation == "right") {
			player->position.x += 1.0f;
		}
		modelMatrix = glm::translate(modelMatrix, glm::vec3(player->position.x, player->position.y, fixedZ));
		modelMatrix = glm::rotate(modelMatrix, RADIANS(180), glm::vec3(0, 1, 0)); // pentru mirrored
		modelMatrix = glm::scale(modelMatrix, glm::vec3(2));
		RenderSimpleMesh(meshes["player_" + player->animations.at(player->animation_names["walking"])->name + to_string(player->walking_backward_id)], shaders["ShaderLab9"], modelMatrix, mapTextures["ssj3goku"]);

		player->walking_backward_frames_id++;
		if (player->walking_backward_frames_id > 8 / factor) {
			player->walking_backward_id++;
			player->walking_backward_frames_id = 0;
		}
		if (player->walking_backward_id == 3) {
			player->walking_backward_id = 0;
			player->move_backward = false;
			player->default_state = true;
		}
		player->last_orientation = "left";
	}
	else if (player->hit) {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(player->position.x, player->position.y, fixedZ));
		if (player->orientation == "left")
			modelMatrix = glm::rotate(modelMatrix, RADIANS(180), glm::vec3(0, 1, 0)); // pentru mirrored
		modelMatrix = glm::scale(modelMatrix, glm::vec3(2));
		RenderSimpleMesh(meshes["player_" + player->animations.at(player->animation_names["hit"])->name + to_string(player->hit_id)], shaders["ShaderLab9"], modelMatrix, mapTextures["ssj3goku"]);

		player->hit_frames_id++;
		if (player->hit_frames_id > 6 / factor) {
			player->hit_id++;
			player->hit_frames_id = 0;
		}

		if (player->hit_id == 2) {
			player->hit_id = 0;
			player->hit = false;
			if (!player->falling)
				player->default_state = true;
			player->delay = 0;
			enemy->delay = 0;
		}
	}
	else if (player->blocking) {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(player->position.x, player->position.y, fixedZ));
		if (player->orientation == "left")
			modelMatrix = glm::rotate(modelMatrix, RADIANS(180), glm::vec3(0, 1, 0)); // pentru mirrored
		modelMatrix = glm::scale(modelMatrix, glm::vec3(2));
		RenderSimpleMesh(meshes["player_" + player->animations.at(player->animation_names["block"])->name + to_string(player->block_id)], shaders["ShaderLab9"], modelMatrix, mapTextures["ssj3goku"]);

		player->block_frames_id++;
		if (player->block_frames_id > 6 / factor) {
			player->block_id++;
			player->block_frames_id = 0;
		}

		if (player->block_id == 2) {
			player->block_id = 0;
			player->blocking = false;
			player->default_state = true;
		}
	}
	else if (player->falling) {
		// cat timp se afla in cadere, nu face altceva
		player->default_state = false;
		player->move_forward = false;
		player->move_backward = false;
		player->hit = false;
		player->blocking = false;

		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(player->position.x, player->position.y, fixedZ));
		if (player->orientation == "left")
			modelMatrix = glm::rotate(modelMatrix, RADIANS(180), glm::vec3(0, 1, 0)); // pentru mirrored
		modelMatrix = glm::scale(modelMatrix, glm::vec3(2));
		RenderSimpleMesh(meshes["player_" + player->animations.at(player->animation_names["falling"])->name + to_string(player->falling_id)], shaders["ShaderLab9"], modelMatrix, mapTextures["ssj3goku"]);

		player->falling_frames_id++;
		if (player->falling_frames_id > 12) {
			player->falling_id++;
			player->position.y -= abs(player->position.y) / 16;
			player->falling_frames_id = 0;
		}

		if (player->falling_id == 5) {
			player->falling_id = 0;
			player->falling = false;
			player->position.y = -2.8f;
			player->default_state = true;
		}
	}

	// *********************ENEMY***********************
	if (enemy->default_state) {
		enemy->move_forward = false;
		enemy->move_backward = false;
		enemy->hit = false;
		enemy->blocking = false;
		enemy->falling = false;

		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(enemy->position.x, enemy->position.y, fixedZ));
		if (enemy->orientation == "left")
			modelMatrix = glm::rotate(modelMatrix, RADIANS(180), glm::vec3(0, 1, 0)); // pentru mirrored
		modelMatrix = glm::scale(modelMatrix, glm::vec3(2));
		RenderSimpleMesh(meshes["enemy_" + enemy->animations.at(enemy->animation_names["default"])->name + to_string(0)], shaders["ShaderLab9"], modelMatrix, mapTextures["ssj1goku"]);
	}
	else if (enemy->move_forward) {
		glm::mat4 modelMatrix = glm::mat4(1);
		if (enemy->last_orientation == "right") {
			enemy->position.x += 1.0f;
		}
		modelMatrix = glm::translate(modelMatrix, glm::vec3(enemy->position.x, enemy->position.y, fixedZ));
		modelMatrix = glm::rotate(modelMatrix, RADIANS(180), glm::vec3(0, 1, 0)); // pentru mirrored
		modelMatrix = glm::scale(modelMatrix, glm::vec3(2));
		RenderSimpleMesh(meshes["enemy_" + enemy->animations.at(enemy->animation_names["walking"])->name + to_string(enemy->walking_forward_id)], shaders["ShaderLab9"], modelMatrix, mapTextures["ssj1goku"]);

		enemy->walking_forward_frames_id++;
		if (enemy->walking_forward_frames_id > 8 / factor) {
			enemy->walking_forward_id++;
			enemy->walking_forward_frames_id = 0;
		}
		if (enemy->walking_forward_id == 3) {
			enemy->walking_forward_id = 0;
			enemy->move_forward = false;
			enemy->default_state = true;
		}
		enemy->last_orientation = "left";
	}
	else if (enemy->move_backward) {
		glm::mat4 modelMatrix = glm::mat4(1);
		if (enemy->last_orientation == "left") {
			enemy->position.x -= 1.0f;
		}
		modelMatrix = glm::translate(modelMatrix, glm::vec3(enemy->position.x, enemy->position.y, fixedZ));
		//modelMatrix = glm::rotate(modelMatrix, RADIANS(180), glm::vec3(0, 1, 0)); // pentru mirrored
		modelMatrix = glm::scale(modelMatrix, glm::vec3(2));
		RenderSimpleMesh(meshes["enemy_" + enemy->animations.at(enemy->animation_names["walking"])->name + to_string(enemy->walking_backward_id)], shaders["ShaderLab9"], modelMatrix, mapTextures["ssj1goku"]);

		enemy->walking_backward_frames_id++;
		if (enemy->walking_backward_frames_id > 8 / factor) {
			enemy->walking_backward_id++;
			enemy->walking_backward_frames_id = 0;
		}
		if (enemy->walking_backward_id == 3) {
			enemy->walking_backward_id = 0;
			enemy->hit = false;
			enemy->default_state = true;
		}
		enemy->last_orientation = "right";
	}
	else if (enemy->hit) {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(enemy->position.x, enemy->position.y, fixedZ));
		if (enemy->orientation == "left")
			modelMatrix = glm::rotate(modelMatrix, RADIANS(180), glm::vec3(0, 1, 0)); // pentru mirrored
		modelMatrix = glm::scale(modelMatrix, glm::vec3(2));
		RenderSimpleMesh(meshes["enemy_" + enemy->animations.at(enemy->animation_names["hit"])->name + to_string(enemy->hit_id)], shaders["ShaderLab9"], modelMatrix, mapTextures["ssj1goku"]);

		enemy->hit_frames_id++;
		if (enemy->hit_frames_id > 6 / factor) {
			enemy->hit_id++;
			enemy->hit_frames_id = 0;
		}
		if (enemy->hit_id == 2) {
			enemy->hit_id = 0;
			enemy->hit = false;
			if (!enemy->falling)
				enemy->default_state = true;
			enemy->delay = 0;
			player->delay = 0;
		}
	}
	else if (enemy->blocking) {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(enemy->position.x, enemy->position.y, fixedZ));
		if (enemy->orientation == "left")
			modelMatrix = glm::rotate(modelMatrix, RADIANS(180), glm::vec3(0, 1, 0)); // pentru mirrored
		modelMatrix = glm::scale(modelMatrix, glm::vec3(2));
		RenderSimpleMesh(meshes["enemy_" + enemy->animations.at(enemy->animation_names["block"])->name + to_string(enemy->block_id)], shaders["ShaderLab9"], modelMatrix, mapTextures["ssj1goku"]);

		enemy->block_frames_id++;
		if (enemy->block_frames_id > 6 / factor) {
			enemy->block_id++;
			enemy->block_frames_id = 0;
		}
		if (enemy->block_id == 2) {
			enemy->block_id = 0;
			enemy->blocking = false;
			enemy->default_state = true;
		}
	}
	else if (enemy->falling) {

		// cat timp se afla in cadere, nu face altceva
		enemy->default_state = false;
		enemy->move_forward = false;
		enemy->move_backward = false;
		enemy->hit = false;
		enemy->blocking = false;

		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(enemy->position.x, enemy->position.y, fixedZ));
		if (enemy->orientation == "left")
			modelMatrix = glm::rotate(modelMatrix, RADIANS(180), glm::vec3(0, 1, 0)); // pentru mirrored
		modelMatrix = glm::scale(modelMatrix, glm::vec3(2));
		RenderSimpleMesh(meshes["enemy_" + enemy->animations.at(enemy->animation_names["falling"])->name + to_string(enemy->falling_id)], shaders["ShaderLab9"], modelMatrix, mapTextures["ssj1goku"]);

		enemy->falling_frames_id++;
		if (enemy->falling_frames_id > 12) {
			enemy->falling_id++;
			enemy->position.y -= abs(enemy->position.y) / 16;
			enemy->falling_frames_id = 0;
		}

		if (enemy->falling_id == 4) {
			enemy->falling_id = 0;
			enemy->falling = false;
			enemy->position.y = -2.8f;
			enemy->default_state = true;
		}
	}

	if (DetectCollision(player->position, enemy->position)) { // daca lovesc amandoi in acelasi timp, cad amandoi si pierd HP
		player->delay++;
		enemy->delay++;
		if (player->hit) {
			if (player->delay > 12 / factor && !enemy->blocking && !enemy->falling) {
				enemy->healthpoints -= player->power;
				cout << "Collision between Players. Player HIT!!!" << endl;
				cout << "Player HP: " << player->healthpoints << endl;
				cout << "Enemy HP: " << enemy->healthpoints << endl;
				cout << endl;
				player->delay = 0;
				enemy->falling = true;
				enemy->move_forward = false;
				enemy->move_backward = false;
				enemy->default_state = false;
			}
			else if (player->delay > 12 / factor && enemy->blocking && !enemy->falling) { // daca enemy a blocat si au aceeasi orientare
				if (player->orientation == enemy->orientation) {
					enemy->healthpoints -= player->power;
					cout << "Collision between Players. Player HIT!!!" << endl;
					cout << "Player HP: " << player->healthpoints << endl;
					cout << "Enemy HP: " << enemy->healthpoints << endl;
					cout << endl;
					player->delay = 0;
					enemy->falling = true;
					enemy->move_forward = false;
					enemy->move_backward = false;
					enemy->default_state = false;
				}
			}
		}

		if (enemy->hit) {
			if (enemy->delay > 12 / factor && !player->blocking && !player->falling) {
				player->healthpoints -= enemy->power;
				cout << "Collision between Players. Enemy HIT!!!" << endl;
				cout << "Player HP: " << player->healthpoints << endl;
				cout << "Enemy HP: " << enemy->healthpoints << endl;
				cout << endl;
				enemy->delay = 0;
				player->falling = true;
				player->move_forward = false;
				player->move_backward = false;
				player->default_state = false;
			}
			else if (enemy->delay > 12 / factor && player->blocking && !player->falling) { // daca player a blocat si au aceeasi orientare
				if (player->orientation == enemy->orientation) {
					player->healthpoints -= enemy->power;
					cout << "Collision between Players. Enemy HIT!!!" << endl;
					cout << "Player HP: " << player->healthpoints << endl;
					cout << "Enemy HP: " << enemy->healthpoints << endl;
					cout << endl;
					enemy->delay = 0;
					player->falling = true;
					player->move_forward = false;
					player->move_backward = false;
					player->default_state = false;
				}
			}
		}
	}
}

void Laborator9::FrameEnd()
{
}

void Laborator9::RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 & modelMatrix, Texture2D* texture1, Texture2D* texture2)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	// Bind model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Bind view matrix
	glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Bind projection matrix
	glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	if (texture1)
	{
		// activate texture location 0
		glActiveTexture(GL_TEXTURE0);
		// Bind the texture1 ID
		glBindTexture(GL_TEXTURE_2D, texture1->GetTextureID());
		// Send texture uniform value
		glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);
	}

	if (texture2)
	{
		// activate texture location 1
		glActiveTexture(GL_TEXTURE1);
		// Bind the texture2 ID
		glBindTexture(GL_TEXTURE_2D, texture2->GetTextureID());
		// Send texture uniform value
		glUniform1i(glGetUniformLocation(shader->program, "texture_2"), 1);
	}

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);
}

// Documentation for the input functions can be found in: "/Source/Core/Window/InputController.h" or
// https://github.com/UPB-Graphics/Framework-EGC/blob/master/Source/Core/Window/InputController.h

void Laborator9::OnInputUpdate(float deltaTime, int mods)
{
	float speed = 6;
	factor = 2;

	if (!player->falling) {
		if (window->KeyHold(GLFW_KEY_A)) {
			player->default_state = false;
			player->move_forward = false;
			player->move_backward = true;
			player->hit = false;
			player->blocking = false;
			player->orientation = "left";
			player->position.x -= speed * deltaTime;
		}

		if (window->KeyHold(GLFW_KEY_D)) {
			player->default_state = false;
			player->move_forward = true;
			player->move_backward = false;
			player->hit = false;
			player->blocking = false;
			player->orientation = "right";
			player->position.x += speed * deltaTime;
		}

		if (window->KeyHold(GLFW_KEY_Q)) {
			player->default_state = false;
			player->move_forward = false;
			player->move_backward = false;
			player->hit = true;
			player->blocking = false;
			player->orientation = player->last_orientation;
		}

		if (window->KeyHold(GLFW_KEY_W)) {
			player->default_state = false;
			player->move_forward = false;
			player->move_backward = false;
			player->hit = false;
			player->blocking = true;
			player->orientation = player->last_orientation;
		}
	}

	if (!enemy->falling) {
		if (window->KeyHold(GLFW_KEY_LEFT)) {
			enemy->default_state = false;
			enemy->move_forward = true;
			enemy->move_backward = false;
			enemy->hit = false;
			enemy->blocking = false;
			enemy->orientation = "left";
			enemy->position.x -= speed * deltaTime;
		}

		if (window->KeyHold(GLFW_KEY_RIGHT)) {
			enemy->default_state = false;
			enemy->move_forward = false;
			enemy->move_backward = true;
			enemy->hit = false;
			enemy->blocking = false;
			enemy->orientation = "right";
			enemy->position.x += speed * deltaTime;
		}

		if (window->KeyHold(GLFW_KEY_J)) {
			enemy->default_state = false;
			enemy->move_forward = false;
			enemy->move_backward = false;
			enemy->hit = true;
			enemy->blocking = false;
			enemy->orientation = enemy->last_orientation;
		}

		if (window->KeyHold(GLFW_KEY_K)) {
			enemy->default_state = false;
			enemy->move_forward = false;
			enemy->move_backward = false;
			enemy->hit = false;
			enemy->blocking = true;
			enemy->orientation = enemy->last_orientation;
		}
	}
}


void Laborator9::OnKeyPress(int key, int mods)
{
	// add key press event
	factor = 1;

	if (!player->falling) {
		if (key == GLFW_KEY_D) {
			player->default_state = false;
			player->move_forward = true;
			player->move_backward = false;
			player->hit = false;
			player->blocking = false;
			player->orientation = "right";
			player->position.x += 0.25f;
		}
		if (key == GLFW_KEY_A) {
			player->default_state = false;
			player->move_forward = false;
			player->move_backward = true;
			player->hit = false;
			player->blocking = false;
			player->orientation = "left";
			player->position.x -= 0.25f;
		}

		if (key == GLFW_KEY_Q) {
			player->default_state = false;
			player->move_forward = false;
			player->move_backward = false;
			player->hit = true;
			player->blocking = false;
			player->orientation = player->last_orientation;
		}

		if (key == GLFW_KEY_W) {
			player->default_state = false;
			player->move_forward = false;
			player->move_backward = false;
			player->hit = false;
			player->blocking = true;
			player->orientation = player->last_orientation;
		}
	}

	if (!enemy->falling) {
		if (key == GLFW_KEY_LEFT) {
			enemy->default_state = false;
			enemy->move_forward = true;
			enemy->move_backward = false;
			enemy->hit = false;
			enemy->blocking = false;
			enemy->orientation = "left";
			enemy->position.x -= 0.25f;
		}
		if (key == GLFW_KEY_RIGHT) {
			enemy->default_state = false;
			enemy->move_forward = false;
			enemy->move_backward = true;
			enemy->hit = false;
			enemy->blocking = false;
			enemy->orientation = "right";
			enemy->position.x += 0.25f;
		}

		if (key == GLFW_KEY_J) {
			enemy->default_state = false;
			enemy->move_forward = false;
			enemy->move_backward = false;
			enemy->hit = true;
			enemy->blocking = false;
			enemy->orientation = enemy->last_orientation;
		}

		if (key == GLFW_KEY_K) {
			enemy->default_state = false;
			enemy->move_forward = false;
			enemy->move_backward = false;
			enemy->hit = false;
			enemy->blocking = true;
			enemy->orientation = enemy->last_orientation;
		}
	}
}

void Laborator9::OnKeyRelease(int key, int mods)
{
	// add key release event
}

void Laborator9::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event
}

void Laborator9::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
}

void Laborator9::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Laborator9::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Laborator9::OnWindowResize(int width, int height)
{
}
