#version 330

#define PI 3.1415926535

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;
layout(location = 3) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

uniform int wave;
uniform int waves_number;
uniform vec2 C[600];
uniform float A[600];
uniform float circle_radius[600];
uniform float desired_wave_length[600];

//uniform float w;
//uniform float phi;
//uniform float t;

// Output values to fragment shader
out vec3 world_position;
out vec3 world_normal;

void main()
{

	vec2 vertex_position = vec2(v_position.x, v_position.z);

	float y1 = v_position.y;

	float distance_by_center = 0.0f;

	// cele doua derivate
	float b = 0.0f, t = 0.0f;

	if (wave == 1) {
		int i;

		for(i = 0; i < waves_number; i++) {
			distance_by_center = distance(vertex_position, C[i]);

			float wave_length = distance_by_center - circle_radius[i];

			if (wave_length >= 0 && wave_length <= desired_wave_length[i]) {

				y1 += A[i] * sin(((wave_length * PI) / desired_wave_length[i]));

				vec2 D = normalize(vertex_position - C[i]);

				b += A[i] * D.x * cos((wave_length * PI) / desired_wave_length[i]);
				t += A[i] * D.y * cos((wave_length * PI) / desired_wave_length[i]);
			}
		}
	}

	// compute world space vertex position and normal
	// send world position and world normal to Fragment Shader

	// "y1" applied only for the grid
	if (wave == 1) {
		gl_Position = Projection * View * Model * vec4(vec3(v_position.x, y1, v_position.z), 1.0);
		world_position = (Model * vec4(vec3(vertex_position.x, y1, vertex_position.y),1)).xyz;
		world_normal = normalize(vec3( -b , 1 , -t ));
	} else {
		gl_Position = Projection * View * Model * vec4(v_position, 1.0);
		world_position = (Model * vec4(v_position,1)).xyz;
		world_normal = normalize(mat3(Model) * v_normal);
	}
}
