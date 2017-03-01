#include "Laborator8.h"

#include <vector>
#include <string>
#include <iostream>

#include <Core/Engine.h>

#define BOUNCING_TIMES 5

using namespace std;

Laborator8::Laborator8()
{
}

Laborator8::~Laborator8()
{
}

void Laborator8::Init()
{
	{
		Mesh* mesh = new Mesh("sphere");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	ballPosition = glm::vec3(0, 1, 1);

	// Create a mesh box using custom data
	{

		// Create a new mesh from buffer data
		GenerateGrid();
		Mesh *grid = CreateMesh("grid", vertices, indices);
	}

	// Create a shader program for drawing face polygon with the color of the normal
	{
		Shader *shader = new Shader("ShaderLab8");
		shader->AddShader("Source/Laboratoare/Laborator8/Shaders/VertexShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Laborator8/Shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	//Light & material properties
	{
		lightPosition = glm::vec3(-1, 2, 0);
		lightDirection = glm::vec3(0, -1, 0);
		materialShininess = 30;
		materialKd = 0.5;
		materialKs = 0.5;
	}
}

void Laborator8::GenerateGrid() {

	for (int i = 0; i < gridSize; i++) {
		for (int j = 0; j < gridSize; j++) {
			vertices.push_back(VertexFormat(glm::vec3((float)(j - gridSize / 2) / 10, 0, (float)(i - gridSize / 2) / 10), glm::vec3(0, 0, 1)));
		}
	}

	for (int i = 0; i < gridSize - 1; i++) {
		for (int j = i * gridSize; j < (i + 1) * gridSize - 1; j++) {
			indices.push_back(gridSize + j);
			indices.push_back(gridSize + j + 1);
			indices.push_back(j + 1);
			indices.push_back(j + 1);
			indices.push_back(j);
			indices.push_back(gridSize + j);
		}
	}
}

void Laborator8::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0.502f, 0.502f, 0.502f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

void Laborator8::Update(float deltaTimeSeconds)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, ballPosition);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.32f));
		RenderSimpleMesh(meshes["sphere"], shaders["ShaderLab8"], modelMatrix, false, deltaTimeSeconds);
	}

	float aux_wavesNumber = wavesNumber;
	for (int i = 0; i < aux_wavesNumber; i++) {
		circle_radius.at(i) += change_speed * deltaTimeSeconds;
		desired_wave_length.at(i) += change_speed * 0.05f * deltaTimeSeconds;

		if (A.at(i) > 0.0f)
			A.at(i) -= change_speed * 0.05f * deltaTimeSeconds;
	}

	if (!showGridEdges) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	// Render the grid
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0.01f, 0));
		RenderSimpleMesh(meshes["grid"], shaders["ShaderLab8"], modelMatrix, true, deltaTimeSeconds, glm::vec3(0, 0, 1));
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Render the point light in the scene
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, lightPosition);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
		RenderSimpleMesh(meshes["sphere"], shaders["ShaderLab8"], modelMatrix, false, deltaTimeSeconds, glm::vec3(1, 1, 0));
	}

	for (int i = 0; i < drops.size(); i++) {
		if ((drops.at(i))->healthPoints != 0) {
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, (drops.at(i))->position);
			modelMatrix = glm::scale(modelMatrix, (drops.at(i))->scale);

			if (!(drops.at(i))->hitGround) {
				(drops.at(i))->speed += -(g * ((drops.at(i))->factor / 2) * deltaTimeSeconds) / 2;
				(drops.at(i))->position.y += (drops.at(i))->speed * deltaTimeSeconds;
			}
			else {
				(drops.at(i))->speed += (g * ((drops.at(i))->factor / 2) * deltaTimeSeconds) / 2;
				(drops.at(i))->position.y += (drops.at(i))->speed * deltaTimeSeconds;
			}

			if ((drops.at(i))->position.y <= 0.001f && !(drops.at(i))->hitGround) {
				(drops.at(i))->hitGround = true;
				(drops.at(i))->scale /= 2.0f;
				(drops.at(i))->factor = (float)(drops.at(i))->factor / 2;
				(drops.at(i))->newHeight = pow((drops.at(i))->speed, 2) / (2 * g * 1 / (drops.at(i))->factor);
				(drops.at(i))->speed = 0;

				A.push_back(0.2f * 2 / ((drops.at(i))->bouncing + 1));
				C.push_back(glm::vec2((drops.at(i))->position.x, (drops.at(i))->position.z));
				circle_radius.push_back(0.0f);
				desired_wave_length.push_back(0.5f * 2 / ((drops.at(i))->bouncing + 1));

				wavesNumber++;

				(drops.at(i))->bouncing++;

				if ((drops.at(i))->bouncing > BOUNCING_TIMES) {
					(drops.at(i))->healthPoints = 0;
				}
			}
			else if ((drops.at(i))->position.y >= (drops.at(i))->newHeight + 0.1 && (drops.at(i))->hitGround) {
				(drops.at(i))->hitGround = false;
				(drops.at(i))->speed = 0;
			}

			RenderSimpleMesh(meshes[(drops.at(i))->name], shaders["ShaderLab8"], modelMatrix, false, deltaTimeSeconds);
		}
	}
}

void Laborator8::FrameEnd()
{
	//DrawCoordinatSystem();
}

Mesh* Laborator8::CreateMesh(const char *name, const std::vector<VertexFormat> &vertices, const std::vector<unsigned short> &indices)
{
	unsigned int VAO = 0;
	// Create the VAO and bind it
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	// Create the VBO and bind it
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Send vertices data into the VBO buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
	// Create the IBO and bind it
	unsigned int IBO;
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	// Send indices data into the IBO buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// ========================================================================

	// set vertex position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);

	// set vertex normal attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(glm::vec3)));

	// set texture coordinate attribute
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3)));

	// set vertex color attribute
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(3 * sizeof(glm::vec3)));

	// ========================================================================

	// Unbind the VAO
	glBindVertexArray(0);
	// Check for OpenGL errors
	CheckOpenGLError();

	// Mesh information is saved into a Mesh object
	meshes[name] = new Mesh(name);
	meshes[name]->InitFromBuffer(VAO, static_cast<unsigned short>(indices.size()));
	meshes[name]->vertices = vertices;
	meshes[name]->indices = indices;
	return meshes[name];
}

void Laborator8::RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 & modelMatrix, bool wave, float deltaTime, const glm::vec3 &color)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	// Set shader uniforms for light & material properties
	// Set light position uniform
	int light_position = glGetUniformLocation(shader->program, "light_position");
	glUniform3f(light_position, lightPosition.x, lightPosition.y, lightPosition.z);

	int light_direction = glGetUniformLocation(shader->program, "light_direction");
	glUniform3f(light_direction, lightDirection.x, lightDirection.y, lightDirection.z);

	// Set eye position (camera position) uniform
	glm::vec3 eyePosition = GetSceneCamera()->transform->GetWorldPosition();
	int eye_position = glGetUniformLocation(shader->program, "eye_position");
	glUniform3f(eye_position, eyePosition.x, eyePosition.y, eyePosition.z);

	// Set material property uniforms (shininess, kd, ks, object color) 
	int material_shininess = glGetUniformLocation(shader->program, "material_shininess");
	glUniform1i(material_shininess, materialShininess);

	int material_kd = glGetUniformLocation(shader->program, "material_kd");
	glUniform1f(material_kd, materialKd);

	int material_ks = glGetUniformLocation(shader->program, "material_ks");
	glUniform1f(material_ks, materialKs);

	int object_color = glGetUniformLocation(shader->program, "object_color");
	glUniform3f(object_color, color.r, color.g, color.b);

	GLint key_f = glGetUniformLocation(shader->program, "key_f");
	glUniform1i(key_f, light_type);

	GLint ffang = glGetUniformLocation(shader->program, "angle");
	glUniform1f(ffang, angle);

	// send data to vertex shader
	if (wave) {

		GLint waves_nr_loc = glGetUniformLocation(shader->program, "waves_number");
		glUniform1i(waves_nr_loc, wavesNumber);

		GLint amplitude = glGetUniformLocation(shader->program, "A");
		glUniform1fv(amplitude, A.size(), reinterpret_cast<GLfloat *>(A.data()));

		// centrul undei
		GLint wave_center = glGetUniformLocation(shader->program, "C");
		glUniform2fv(wave_center, C.size(), reinterpret_cast<GLfloat *>(C.data()));

		GLint circle_radius_loc = glGetUniformLocation(shader->program, "circle_radius");
		glUniform1fv(circle_radius_loc, circle_radius.size(), reinterpret_cast<GLfloat *>(circle_radius.data()));

		GLint desired_wave_length_loc = glGetUniformLocation(shader->program, "desired_wave_length");
		glUniform1fv(desired_wave_length_loc, desired_wave_length.size(), reinterpret_cast<GLfloat *>(desired_wave_length.data()));

		GLint r_wave = glGetUniformLocation(shader->program, "wave");
		glUniform1i(r_wave, (GLint)wave);
	}

	GLint r_wave = glGetUniformLocation(shader->program, "wave");
	glUniform1i(r_wave, (GLint)wave);

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

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);
}

// Documentation for the input functions can be found in: "/Source/Core/Window/InputController.h" or
// https://github.com/UPB-Graphics/Framework-EGC/blob/master/Source/Core/Window/InputController.h

void Laborator8::OnInputUpdate(float deltaTime, int mods)
{
	float speed = 2;

	if (!window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		glm::vec3 up = glm::vec3(0, 1, 0);
		glm::vec3 right = GetSceneCamera()->transform->GetLocalOXVector();
		glm::vec3 forward = GetSceneCamera()->transform->GetLocalOZVector();
		forward = glm::normalize(glm::vec3(forward.x, 0, forward.z));

		// Control light position using on W, A, S, D, E, Q
		if (window->KeyHold(GLFW_KEY_W)) lightPosition -= forward * deltaTime * speed;
		if (window->KeyHold(GLFW_KEY_A)) lightPosition -= right * deltaTime * speed;
		if (window->KeyHold(GLFW_KEY_S)) lightPosition += forward * deltaTime * speed;
		if (window->KeyHold(GLFW_KEY_D)) lightPosition += right * deltaTime * speed;
		if (window->KeyHold(GLFW_KEY_E)) lightPosition += up * deltaTime * speed;
		if (window->KeyHold(GLFW_KEY_Q)) lightPosition -= up * deltaTime * speed;
		if (window->KeyHold(GLFW_KEY_P)) lightDirection.y += deltaTime * speed;
		if (window->KeyHold(GLFW_KEY_L)) lightDirection.y -= deltaTime * speed;

		if (window->KeyHold(GLFW_KEY_UP)) ballPosition -= forward * deltaTime * speed;
		if (window->KeyHold(GLFW_KEY_LEFT)) ballPosition -= right * deltaTime * speed;
		if (window->KeyHold(GLFW_KEY_DOWN)) ballPosition += forward * deltaTime * speed;
		if (window->KeyHold(GLFW_KEY_RIGHT)) ballPosition += right * deltaTime * speed;

		if (window->KeyHold(GLFW_KEY_I)) {
			lightDirection.x += deltaTime * speed;
			lightDirection.z += deltaTime * speed;
		}
		if (window->KeyHold(GLFW_KEY_J)) {
			lightDirection.x -= deltaTime * speed;
			lightDirection.z -= deltaTime * speed;
		}
		if (window->KeyHold(GLFW_KEY_O)) angle += 4 * deltaTime * speed;
		if (window->KeyHold(GLFW_KEY_K)) angle -= 4 * deltaTime * speed;

		if (window->KeyHold(GLFW_KEY_1)) {
			if (change_speed < 8.0f)
				change_speed += deltaTime;
		}
		if (window->KeyHold(GLFW_KEY_2)) {
			if (change_speed > 0.1f)
				change_speed -= deltaTime;
		}
	}
}

void Laborator8::OnKeyPress(int key, int mods)
{
	// add key press event

	if (key == GLFW_KEY_B) {
		bonus = !bonus;
	}

	if (key == GLFW_KEY_SPACE) {
		// create a drop
		if (!bonus) {
			drop = new Drop();
			drop->name = "drop" + std::to_string(dropId);
			drop->setPosition(ballPosition.x, ballPosition.y - 0.05f, ballPosition.z);
			drop->hitGround = false;
			drop->speed = 0.0f;
			drop->bouncing = 0;
			drop->scale = { 0.16f, 0.16f, 0.16f };
			drop->healthPoints = 1;

			Mesh* dropMesh = new Mesh(drop->name);
			dropMesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
			meshes[dropMesh->GetMeshID()] = dropMesh;

			drops.push_back(drop);
			dropId++;
		}
		else {
			for (int i = -8; i < 8; i += 4) {
				drop = new Drop();
				drop->name = "drop" + std::to_string(dropId);
				double probability = ((double)rand() / RAND_MAX);
				float offset;
				if (probability < 0.5) {
					offset = (float)i / 2;
					drop->setPosition(ballPosition.x + offset, ballPosition.y - 0.05f, ballPosition.z + offset);
				}
				else {
					offset = -(float)i / 2;
					drop->setPosition(ballPosition.x + offset, ballPosition.y - 0.05f, ballPosition.z - offset);
				}
				drop->hitGround = false;
				drop->speed = 0.0f;
				drop->bouncing = 0;
				drop->scale = { 0.16f, 0.16f, 0.16f };
				drop->healthPoints = 1;

				Mesh* dropMesh = new Mesh(drop->name);
				dropMesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
				meshes[dropMesh->GetMeshID()] = dropMesh;

				drops.push_back(drop);
				dropId++;
			}
		}
	}

	if (key == GLFW_KEY_G) {
		switch (showGridEdges)
		{
		case false:
			showGridEdges = true;
			break;
		default:
			showGridEdges = false;
			break;
		}
	}
}

void Laborator8::OnKeyRelease(int key, int mods)
{
	// add key release event
	if (key == GLFW_KEY_F) {
		light_type = !light_type;
	}
}

void Laborator8::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event
}

void Laborator8::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
}

void Laborator8::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Laborator8::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Laborator8::OnWindowResize(int width, int height)
{
}
