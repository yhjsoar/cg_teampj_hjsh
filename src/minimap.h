#pragma once
#ifndef __MINIMAP_H__
#define __MINIMAP_H__

struct minimap_t {
	vec3	center;
	vec4	color;

	float	radius;
	float	sphere_radius;

	vec3	start_loc;
	vec3	finish_loc;
	vec3	duck_loc;

	vec4	start_color;
	vec4	finish_color;
	vec4	duck_color;

	mat4	rotation_matrix;
	mat4	model_matrix;
	mat4	start_model_matrix;
	mat4	finish_model_matrix;
	mat4	duck_model_matrix;
	mat4	save_rotation_matrix;
	mat4	saving_rotation_matrix;

	void	update(int stage, float distance, vec3 start_location, vec3 finish_location, vec3 duck_location, mat4 rotating_matrix, bool rotating, vec2 pos_diff, bool rotation_first);
};

minimap_t create_minimap() {
	minimap_t minimap;
	minimap.radius = 10.0f;
	minimap.sphere_radius = 0.7f;
	minimap.center = vec3(0.0f, 35.0f, 30.0f);
	minimap.start_color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	minimap.finish_color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	minimap.duck_color = vec4(1.0f, 1.0f, 0.0f, 1.0f);
	minimap.color = vec4(0.0, 0.0, 0.0, 0.5f);

	minimap.rotation_matrix = mat4{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	minimap.save_rotation_matrix = minimap.rotation_matrix;
	return minimap;
}

inline void minimap_t::update(int stage, float distance, vec3 start_location, vec3 finish_location, vec3 duck_location, mat4 rotating_matrix, bool rotating, vec2 pos_diff, bool rotation_first) {
	float cube_size = (float)stage * distance;
	float map_size = radius/2;
	float biyul = map_size / cube_size;

	start_loc = center + start_location * biyul;
	finish_loc = center + finish_location * biyul;
	duck_loc = center + duck_location * biyul;

	

	
	float angle_z = 1.8f * (pos_diff.x);
	float angle_y = 1.8f * (pos_diff.y);
		

	vec3 tmp = start_location * biyul;
	start_loc = vec3(tmp.x * cosf(-angle_y) - tmp.z * sinf(-angle_y), tmp.y, tmp.x * sinf(-angle_y) + tmp.z * cosf(-angle_y));
	tmp = start_loc;
	start_loc = center + vec3(tmp.x * cosf(angle_z) - tmp.y * sinf(angle_z), tmp.y * cosf(angle_z) + tmp.x * sinf(angle_z), tmp.z);

	tmp = finish_location * biyul;
	finish_loc = vec3(tmp.x * cosf(-angle_y) - tmp.z * sinf(-angle_y), tmp.y, tmp.x * sinf(-angle_y) + tmp.z * cosf(-angle_y));
	tmp = finish_loc;
	finish_loc = center + vec3(tmp.x * cosf(angle_z) - tmp.y * sinf(angle_z), tmp.y * cosf(angle_z) + tmp.x * sinf(angle_z), tmp.z);

	tmp = duck_location * biyul;
	tmp.z += 1.3f;
	duck_loc = vec3(tmp.x * cosf(-angle_y) - tmp.z * sinf(-angle_y), tmp.y, tmp.x * sinf(-angle_y) + tmp.z * cosf(-angle_y));
	tmp = duck_loc;
	duck_loc = center + vec3(tmp.x * cosf(angle_z) - tmp.y * sinf(angle_z), tmp.y * cosf(angle_z) + tmp.x * sinf(angle_z), tmp.z);

	mat4 rotation_matrix_z =
	{
		cosf(angle_z), -sinf(angle_z), 0, 0,
		sinf(angle_z), cosf(angle_z), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	mat4 rotation_matrix_y =
	{
		cosf(angle_y), 0, sinf(angle_y), 0,
		0, 1, 0, 0,
		-sinf(angle_y), 0, cosf(angle_y), 0,
		0, 0, 0, 1
	};

	//if (rotation_matrix) {
		//save_rotation_matrix = saving_rotation_matrix;
	//}
	//if (rotating) {
		//rotation_matrix = rotating_matrix * save_rotation_matrix;
		//saving_rotation_matrix = rotation_matrix;
	//}
	rotation_matrix = rotation_matrix_z * rotation_matrix_y;

	mat4 sphere_scale_matrix =
	{
		sphere_radius, 0, 0, 0,
		0, sphere_radius, 0, 0,
		0, 0, sphere_radius, 0,
		0, 0, 0, 1
	};

	mat4 scale_matrix =
	{
		radius, 0, 0, 0,
		0, radius, 0, 0,
		0, 0, radius, 0,
		0, 0, 0, 1
	};

	mat4 translate_matrix =
	{
		1, 0, 0, center.x,
		0, 1, 0, center.y,
		0, 0, 1, center.z,
		0, 0, 0, 1
	};

	mat4 finish_translate_matrix = {
		1, 0, 0, finish_loc.x,
		0, 1, 0, finish_loc.y,
		0, 0, 1, finish_loc.z,
		0, 0, 0, 1
	};

	mat4 duck_translate_matrix = {
		1, 0, 0, duck_loc.x,
		0, 1, 0, duck_loc.y,
		0, 0, 1, duck_loc.z,
		0, 0, 0, 1
	};

	mat4 start_translate_matrix = {
		1, 0, 0, start_loc.x,
		0, 1, 0, start_loc.y,
		0, 0, 1, start_loc.z,
		0, 0, 0, 1
	};


	model_matrix = translate_matrix * rotation_matrix * scale_matrix;
	start_model_matrix = start_translate_matrix * rotation_matrix * sphere_scale_matrix;
	finish_model_matrix = finish_translate_matrix * rotation_matrix * sphere_scale_matrix;
	duck_model_matrix = duck_translate_matrix * rotation_matrix * sphere_scale_matrix;
}


#endif