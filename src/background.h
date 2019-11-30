#pragma once
#ifndef __BACKGROUND_H__
#define __BACKGROUND_H__

struct background_t
{
	vec3 center;
	mat4 scale_matrix;
	vec4 color;
	mat4 model_matrix;
	float distance;

	void update(vec3 eye);
	void initial();
};

background_t create_background() {
	background_t back;
	back.distance = 200.0f;
	back.center = vec3(-200.0f, 0.0f, 0.0f);
	back.scale_matrix =
	{
		1, 0, 0, 0,
		0, 2000, 0, 0,
		0, 0, 2000, 0,
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
	back.color = vec4(0.3f, 0.3f, 0.8f, 1.0f);
	back.initial();
	return back;
}

inline void background_t::initial() {
	vec3 eye = vec3(0.0f, 150.0f, 0.0f);
	update(eye);
}

inline void background_t::update(vec3 eye) {
	vec3 direction = normalize(eye);
	direction.x = -direction.x;
	direction.y = -direction.y;
	direction.z = 0.0f;
	center = direction * distance;
	float cosx = direction.x;
	float sinx = direction.y;
	mat4 rotation_matrix =
	{
	    cosx, -sinx, 0, 0,
		sinx, cosx, 0, 0,
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