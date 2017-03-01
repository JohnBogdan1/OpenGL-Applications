#version 330

// TODO: get color value from vertex shader
in vec3 world_position;
in vec3 world_normal;

// Uniforms for light properties
uniform vec3 light_direction;
uniform vec3 light_position;
uniform vec3 eye_position;

uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;
uniform int key_f;
uniform float angle;

uniform vec3 object_color;

layout(location = 0) out vec4 out_color;

void main()
{
	vec3 L = normalize(light_position - world_position);
	vec3 V = normalize(eye_position - world_position);
	vec3 H = normalize(L + V);

	// TODO: define ambient light component
	float ambient_light = 0.25;

	// TODO: compute diffuse light component
	float diffuse_light = material_kd * max(dot(world_normal, L), 0);

	// TODO: compute specular light component
	float specular_light = 0;

	if (diffuse_light > 0)
	{
		specular_light = material_ks * pow(max(dot(world_normal, H), 0), material_shininess);
	}

	float light;
	vec3 color;

	// TODO: compute light
	float kc = 0.1;
	float kl = 0.1;
	float kq = 0.3;

	// TODO: write pixel out color
	if(key_f == 0) {
		float distance = distance(light_position, world_position);
		float atenuare = 1 / (pow(distance, 2) * kq + kc + kl * distance);
		float light = ambient_light + atenuare * (specular_light + diffuse_light);
		color = light * object_color;
		out_color = vec4(color,1);
	}

	else {
		float cut_off = radians(angle);
		float spot_light = dot(L, light_direction);
		float spot_light_limit = cos(cut_off);

		if(spot_light > spot_light_limit) {
			float linear_att = (spot_light - spot_light_limit) / (1 - spot_light_limit);
			float light_att_factor = pow(linear_att, 2);
			float light = ambient_light + light_att_factor * (specular_light + diffuse_light);
			color = light * object_color;
			out_color = vec4(color,1);
		} else {
			float light = ambient_light;
			out_color = vec4(light * object_color, 1);
		}
	}
}