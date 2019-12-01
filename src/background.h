#pragma once
#ifndef __BACKGROUND_H__
#define __BACKGROUND_H__

struct background_t
{
	vec3 center = vec3(0.0f, 0.0f, 0.0f);
	mat4 scale_matrix;
	vec4 color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	mat4 model_matrix;
	float distance = 0.0f;

	void update();
	void initial();
};

background_t create_background() {
	background_t back;
	back.distance = 200.0f;
	back.center = vec3(-200.0f, 35.0f, 30.0f);
	back.scale_matrix =
	{
		1, 0, 0, 0,
		0, 27, 0, 0,
		0, 0, 27, 0,
		0, 0, 0, 1
	};
	mat4 translate_matrix =
	{
		1, 0, 0, back.center.x,
		0, 1, 0, back.center.y,
		0, 0, 1, back.center.z,
		0, 0, 0, 1
	};
	mat4 rotation_matrix =
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	back.model_matrix = translate_matrix * rotation_matrix * back.scale_matrix;
	back.initial();
	return back;
}

inline void background_t::initial() {
}

inline void background_t::update() {
	center = vec3(-50.0f, 61.0f, 45.0f);
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