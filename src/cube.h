#pragma once
#ifndef __CIRCLE_H__
#define __CIRCLE_H__

struct cube_t
{
	vec3	center = vec3(0);		// 2D position for translation
	float	radius = 100.0f;		// radius
	vec4	color;				// RGBA color in [0,1]
	int		block;

	mat4	rotation_matrix;
	mat4	model_matrix;		// modeling transformation

	// public functions
	void	update(float t, int command, int tick, bool rotate, int rotate_time);
};


inline std::vector<cube_t> create_map()
{
	int arr_map[6][6][6] = {	{{2, 1, 1, 1, 1, 0}, {0, 0, 0, 0, 1, 1}, {1, 1, 1, 1, 0, 1}, {1, 0, 0, 1, 0, 0}, {0, 1, 1, 1, 0, 0}, {0, 0, 0, 0, 0, 0}},
								{{0, 0, 0, 0, 0, 1}, {0, 1, 1, 1, 0, 0}, {0, 0, 1, 0, 1, 1}, {0, 0, 0, 0, 0, 1}, {0, 1, 0, 0, 0, 1}, {0, 0, 0, 1, 1, 1}},
								{{1, 1, 1, 1, 1, 1}, {1, 0, 0, 0, 1, 0}, {1, 1, 1, 0, 1, 0}, {0, 0, 0, 0, 1, 0}, {0, 1, 0, 0, 0, 0}, {1, 1, 1, 0, 0, 0}},
								{{0, 0, 0, 0, 0, 0}, {0, 1, 1, 1, 0, 1}, {0, 0, 0, 0, 0, 1}, {1, 1, 1, 1, 0, 1}, {0, 1, 0, 1, 0, 1}, {0, 0, 0, 1, 1, 1}},
								{{0, 0, 1, 1, 0, 0}, {0, 1, 0, 1, 1, 1}, {0, 0, 0, 0, 1, 0}, {0, 0, 0, 0, 1, 0}, {0, 1, 0, 0, 1, 0}, {0, 1, 0, 0, 0, 0}},
								{{0, 0, 1, 0, 0, 0}, {0, 1, 0, 0, 0, 0}, {1, 1, 0, 0, 0, 0}, {1, 0, 0, 0, 0, 0}, {1, 0, 0, 0, 1, 0}, {1, 1, 1, 1, 1, 2}} };
	std::vector<cube_t> map;
	cube_t c;
	float distance = 7.5f;
	float x, y, z;
	int isleft, isright, isup, isdown, isfront, isback;
	vec4 color;
	int block;
	mat4 rotation_matrix =
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 6; j++) {
			for (int k = 0; k < 6; k++) {
				z = distance * (2.5f - (float)i);
				x = distance * (-2.5f + (float)j);
				y = distance * (-2.5f + (float)k);
				if (arr_map[i][j][k] == 2) color = { 1, 0, 0, 1.0f };
				else if (arr_map[i][j][k] == 1) color = { 1, 1, 1, 1.0f };
				else color = { 0, 0, 0, 1.0f };
				if (k == 0 || arr_map[i][j][k] == arr_map[i][j][k - 1])	isleft = 1;
				else isleft = 0;
				if (k == 5 || arr_map[i][j][k] != arr_map[i][j][k + 1]) isright = 1;
				else isright = 0;
				if (i == 0 || arr_map[i][j][k] != arr_map[i - 1][j][k]) isup = 1;
				else isup = 0;
				if (i == 5 || arr_map[i][j][k] != arr_map[i + 1][j][k]) isdown = 1;
				else isdown = 0;
				if (j == 5 || arr_map[i][j][k] != arr_map[i][j + 1][k]) isfront = 1;
				else isfront = 0;
				if (j == 0 || arr_map[i][j][k] != arr_map[i][j - 1][k]) isback = 1;
				else isback = 0;
				block = isback + isfront * 2 + isdown * 4 + isup * 8 + isright * 16 + isleft * 32;
				if (arr_map[i][j][k] == 2) {
					if (i == 0) {
						block = 1 + 2 + 4 + 8 + 0 + 32;
					}
					else {
						block = 1 + 2 + 4 + 8 + 16 + 0;
					}
				}
				block--;
				c = { vec3(x, y, z), distance, color, block, rotation_matrix};
				map.emplace_back(c);
			}
		}
	}
	return map;
}

inline void cube_t::update(float t, int command, int tick, bool rotate, int rotate_time)
{
	// these transformations will be explained in later transformation lecture
	if (command == 1 && rotate) {
		//up
		float dist = sqrt(center.x * center.x + center.z * center.z);
		float theta = atan2(center.z, center.x);
		float after_theta = theta + PI / 2 / (float)tick * (float)rotate_time;
		center.x = dist * cos(after_theta);
		center.z = dist * sin(after_theta);
		float thet = -PI / 2 / (float)tick * (float)rotate_time;
		float cos_y = cos(thet), sin_y = sin(thet);
		mat4 rotation_matrix_y =
		{
			cos_y, 0, sin_y, 0,
			0, 1, 0, 0,
			-sin_y, 0, cos_y, 0,
			0, 0, 0, 1
		};
		rotation_matrix = rotation_matrix_y * rotation_matrix;
	}
	else if (command == 2 && rotate) {
		//down
		float dist = sqrt(center.x * center.x + center.z * center.z);
		float theta = atan2(center.z, center.x);
		float after_theta = theta - PI / 2 / (float)tick * (float)rotate_time;
		center.x = dist * cos(after_theta);
		center.z = dist * sin(after_theta);
		float thet = PI / 2 / (float)tick * (float)rotate_time;
		float cos_y = cos(thet), sin_y = sin(thet);
		mat4 rotation_matrix_y =
		{
			cos_y, 0, sin_y, 0,
			0, 1, 0, 0,
			-sin_y, 0, cos_y, 0,
			0, 0, 0, 1
		};
		rotation_matrix = rotation_matrix_y * rotation_matrix;
	}
	else if (command == 3 && rotate) {
		//left
		float dist = sqrt(center.x * center.x + center.y * center.y);
		float theta = atan2(center.y, center.x);
		float after_theta = theta - PI / 2 / (float)tick * (float)rotate_time;
		center.x = dist * cos(after_theta);
		center.y = dist * sin(after_theta);
		float thet = -PI / 2 / (float)tick * (float)rotate_time;
		float cos_z = cos(thet), sin_z = sin(thet);
		mat4 rotation_matrix_z =
		{
			cos_z, -sin_z, 0, 0,
			sin_z, cos_z, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};
		rotation_matrix = rotation_matrix_z * rotation_matrix;
	}
	else if (command == 4 && rotate) {
		//right
		float dist = sqrt(center.x * center.x + center.y * center.y);
		float theta = atan2(center.y, center.x);
		float after_theta = theta + PI / 2 / (float)tick * (float)rotate_time;
		center.x = dist * cos(after_theta);
		center.y = dist * sin(after_theta);
		float thet = PI / 2 / (float)tick * (float)rotate_time;
		float cos_z = cos(thet), sin_z = sin(thet);
		mat4 rotation_matrix_z =
		{
			cos_z, -sin_z, 0, 0,
			sin_z, cos_z, 0, 0,
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
