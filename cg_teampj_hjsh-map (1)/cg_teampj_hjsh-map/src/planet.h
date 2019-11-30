#pragma once
#ifndef __CIRCLE_H__
#define __CIRCLE_H__

struct planet_t
{
	vec3	center = vec3(0);		// 2D position for translation
	float	radius = 100.0f;		// radius
	float	theta = 0.0f;			// rotation angle
	float	distance = 0.0f;
	float	last_theta = 0.0f;
	float	random_theta = 0.0f;
	float	distance_theta = 0.0f;
	float	random_distance = 0.0f;
	float	random_loc = 0.0f;
	vec4	color;				// RGBA color in [0,1]
	mat4	model_matrix;		// modeling transformation

	// public functions
	void	update(float t, float first_time, bool is_pause);
};

inline std::vector<planet_t> create_planet()
{
	std::vector<planet_t> planets;
	planet_t c;

	float theta_arr[9];
	float dist_arr[9];
	float loc_arr[9];

	int MAX_RAND = 20;

	std::srand(5323);
	for (int count = 0; count < 9; count++) {
		theta_arr[count] = (float)(std::rand() % MAX_RAND) / MAX_RAND + 1.0f;
		dist_arr[count] = (float)(std::rand() % MAX_RAND) / MAX_RAND + 0.5f;
		loc_arr[count] = (float)(std::rand() % MAX_RAND) / MAX_RAND * 2.0f;
	}

	float radius_arr[9] = { 10.f, 0.5f, 0.8f, 1.3f, 2.3f, 2.8f, 4.7f, 3.0f, 3.2f };
	float distance_arr[9] = { 0.0f, 12.0f, 15.0f, 19.0f, 24.0f, 34.0f, 48.0f, 60.0f, 70.0f };

	for (int i = 0; i < 9; i++) {
		c = { vec3(0, 0, 0), 1.2f*radius_arr[i], 0.0f, 1.2f*distance_arr[i], 0.0f, theta_arr[i], 0.0f, dist_arr[i], loc_arr[i], vec4(1.0f,0.5f,0.5f,1.0f) };
		planets.emplace_back(c);
	}

	return planets;
}

inline void planet_t::update(float t, float first_time, bool is_pause)
{
	if (!is_pause) {
		float time = t - first_time;
		theta = last_theta + time*random_theta;
		

		if (distance != 0) {
			distance_theta =  time * random_distance + random_loc;
			float ct = cos(distance_theta), st = sin(distance_theta);
			center.x = distance * ct;
			center.y = distance * st;
		}
	}
	else {
		last_theta = theta;
		random_loc = distance_theta;
	}
	float c = cos(theta), s = sin(theta);
	
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
		c, -s, 0, 0,
		s, c, 0, 0,
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
