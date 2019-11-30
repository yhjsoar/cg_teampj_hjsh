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

	void	update(int stage, float distance, vec3 start_location, vec3 finish_location, vec3 duck_location);
};

minimap_t create_minimap() {
	minimap_t minimap;
	minimap.radius = 10.0f;
	minimap.sphere_radius = 0.7f;
	minimap.center = vec3(0.0f, 35.0f, 35.0f);
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

	return minimap;
}

inline void minimap_t::update(int stage, float distance, vec3 start_location, vec3 finish_location, vec3 duck_location) {
	float cube_size = (float)stage * distance;
	float map_size = radius/2;
	float biyul = map_size / cube_size;

	start_loc = center + start_location * biyul;
	finish_loc = center + finish_location * biyul;
	duck_loc = center + duck_location * biyul;

	duck_loc.z += 1.5f;


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