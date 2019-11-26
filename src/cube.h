#pragma once
#ifndef __CIRCLE_H__
#define __CIRCLE_H__

struct cube_t
{
	vec3	center = vec3(0);		// 2D position for translation
	float	radius = 100.0f;		// radius
	vec4	color;				// RGBA color in [0,1]
	mat4	model_matrix;		// modeling transformation

	// public functions
	void	update(float t);
};

inline std::vector<cube_t> create_map()
{
	std::vector<cube_t> map;
	cube_t c;
	float distance = 10.0f;
	vec3 cents[] = {vec3(distance, -distance, -distance), vec3(distance, 0, -distance), vec3(distance, distance, -distance),
					vec3(0, -distance, -distance), vec3(0, 0, -distance), vec3(0, distance, -distance),
					vec3(-distance, -distance, -distance), vec3(-distance, 0, -distance), vec3(-distance, distance, -distance),
					vec3(distance, -distance, 0), vec3(distance, 0, 0), vec3(distance, distance, 0),
					vec3(0, -distance, 0), vec3(0, 0, 0), vec3(0, distance, 0),
					vec3(-distance, -distance, 0), vec3(-distance, 0, 0), vec3(-distance, distance, 0),
					vec3(distance, -distance, distance), vec3(distance, 0, distance), vec3(distance, distance, distance),
					vec3(0, -distance, distance), vec3(0, 0, distance), vec3(0, distance, distance),
					vec3(-distance, -distance, distance), vec3(-distance, 0, distance), vec3(-distance, distance, distance) };
	for (int i = 0; i < 27; i++) {
		c = { cents[i], distance, vec4(1, 1, 1,0.7f) };
		map.emplace_back(c);
	}
	return map;
}

inline void cube_t::update(float t)
{
	// these transformations will be explained in later transformation lecture
	mat4 scale_matrix =
	{
		radius, 0, 0, 0,
		0, radius, 0, 0,
		0, 0, radius, 0,
		0, 0, 0, 1
	};

	mat4 rotation_matrix =
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	mat4 translate_matrix =
	{
		1, 0, 0, center.x,
		0, 1, 0, center.y,
		0, 0, 1, center.z,
		0, 0, 0, 1
	};

	model_matrix = translate_matrix * rotation_matrix * scale_matrix;
}

#endif
