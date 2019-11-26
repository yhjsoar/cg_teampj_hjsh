#pragma once
#ifndef __CIRCLE_H__
#define __CIRCLE_H__

struct circle_t
{
	vec2	center=vec2(0);		// 2D position for translation
	float	radius=1.0f;		// radius
	float	theta=0.0f;			// rotation angle
	vec4	color;				// RGBA color in [0,1]
	mat4	model_matrix;		// modeling transformation

	// public functions
	void	update( float t ,bool right,bool left);	
};

inline std::vector<circle_t> create_circles()
{
	std::vector<circle_t> circles;
	circle_t c;
	
	c = {vec2(0,0),0.5f,0.0f,vec4(1.0f,0.5f,0.5f,1.0f)};
	circles.emplace_back(c);


	return circles;
}

inline void circle_t::update( float t ,bool right,bool left)
{
	//radius	= 0.35f+cos(t)*0.1f;		// simple animation
	theta	= t;
	float c=0	, s=1;

	// these transformations will be explained in later transformation lecture
	mat4 scale_matrix =
	{
		radius, 0, 0, 0,
		0, radius, 0, 0,
		0, 0, radius, 0,
		0, 0, 0, 1
	};
	float dist = 2 * PI * radius * t / 360;
	if (right) {
		center.y += dist;
		c = -cos(theta);
		s = -sin(theta);
	}
	else if (left) {
		center.y -= dist;
		c = cos(theta);
		s = sin(theta);
	}
	mat4 rotation_matrix =
	{
		1, 0, 0, 0,
		0, c, -s, 0,
		0, s, c, 0,
		0, 0, 0, 1
	};
	mat4 translate_matrix =
	{
		1, 0, 0, center.x,
		0, 1, 0, center.y,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	model_matrix = translate_matrix*rotation_matrix*scale_matrix;
	//model_matrix =translate_matrix*rotation_matrix;
}

#endif
