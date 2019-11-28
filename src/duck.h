#pragma once
#ifndef __DUCK_H__
#define __DUCK_H__

struct duck_t
{
	vec3	center = vec3(0);		// 2D position for translation
	float	radius = 100.0f;		// radius
	vec4	color;				// RGBA color in [0,1]
	mat4	model_matrix;		// modeling transformation

	// public functions
	void	update(float t, int state);
};


inline duck_t create_character()
{
	duck_t duck = {vec3(0, 0, 0), 4.0f, vec4(1, 1, 0, 1.0f)};
	return duck;
}

inline void duck_t::update(float t, int state)
{
	// these transformations will be explained in later transformation lecture

	mat4 rotation_matrix =
	{
		1, 0, 0, 0,
		0, cos(PI / 2), -sin(PI / 2), 0,
		0, sin(PI / 2), cos(PI / 2), 0,
		0, 0, 0, 1
	};

	if (state == 1) {
		//front
		mat4 rotation_matrix_z = {
			cos(PI / 2), -sin(PI / 2), 0, 0,
			sin(PI / 2), cos(PI / 2), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};
		rotation_matrix = rotation_matrix_z * rotation_matrix;
	}
	else if (state == 2) {
		//left
	}
	else if (state == 3) {
		//righ
		mat4 rotation_matrix_z = {
			cos(PI), -sin(PI), 0, 0,
			sin(PI), cos(PI), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};
		rotation_matrix = rotation_matrix_z * rotation_matrix;
	}
	else if (state == 4) {
		//back
		mat4 rotation_matrix_z = {
			cos(-PI / 2), -sin(-PI / 2), 0, 0,
			sin(-PI / 2), cos(-PI / 2), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};
		rotation_matrix = rotation_matrix_z * rotation_matrix;
	}
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

	model_matrix = translate_matrix * rotation_matrix * scale_matrix;
}


#endif