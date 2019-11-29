#pragma once
#ifndef __CIRCLE_H__
#define __CIRCLE_H__

struct cube_t
{
	vec3	center = vec3(0);		// 2D position for translation
	float	radius = 100.0f;		// radius
	vec4	color;				// RGBA color in [0,1]
	int		type;
	float	save_z;
	bool	reversing;
	int		tick_time;

	mat4	rotation_matrix;
	mat4	model_matrix;		// modeling transformation

	// public functions
	void	update(int command, int tick, int rotate_time, vec3 char_center, float char_size, float block_size);
};


inline std::vector<cube_t> create_map(float distance, int stage)
{
	printf("making map: %d\n", stage);
	int map_size = stage * 2;
	cube_t c;
	float x, y, z;
	vec4 color;
	int i, j, k;
	mat4 rotation_matrix =
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	std::vector<cube_t> map;

	if (stage == 1) {
		int arr_map[2][2][2] = { {{2, 1}, {0, 0}}, {{0, 1}, {0, 3}} };
		for (i = 0; i < map_size; i++) {
			for (j = 0; j < map_size; j++) {
				for (k = 0; k < map_size; k++) {
					z = distance * ((float)stage - 0.5f - (float)i);
					x = distance * (-(float)stage + 0.5f + (float)j);
					y = distance * (-(float)stage + 0.5f + (float)k);
					switch (arr_map[i][j][k]) {
					case 0:
						color = { 0, 0, 0, 1.0f };
						break;
					case 1:
						color = { 1, 1, 1, 1.0f };
						break;
					case 2:
						color = { 1, 0, 0, 1.0f };
						break;
					case 3:
						color = { 0, 0, 1, 1.0f };
					}
					c = { vec3(x, y, z), distance, color, arr_map[i][j][k], z, false, 0, rotation_matrix };
					map.emplace_back(c);
				}
			}
		}
	}
	else if (stage == 2) {
		int arr_map[4][4][4] = { { {2, 1, 1, 0}, {0, 0, 0, 0}, {1, 0, 1, 0}, {1, 1, 1, 1} },
								{ {0, 0, 1, 1}, {0, 1, 1, 0}, {1, 0, 1, 0}, {0, 0, 0, 0}},
								{{1, 0, 0, 0}, {1, 0, 0, 0}, {1, 0, 0, 0}, {1, 1, 1, 0}},
								{{1, 1, 1, 1}, {0, 1, 0, 1}, {0, 1, 1, 1}, {0, 0, 0, 3}} };
		for (i = 0; i < map_size; i++) {
			for (j = 0; j < map_size; j++) {
				for (k = 0; k < map_size; k++) {
					z = distance * ((float)stage - 0.5f - (float)i);
					x = distance * (-(float)stage + 0.5f + (float)j);
					y = distance * (-(float)stage + 0.5f + (float)k);
					switch (arr_map[i][j][k]) {
					case 0:
						color = { 0, 0, 0, 1.0f };
						break;
					case 1:
						color = { 1, 1, 1, 1.0f };
						break;
					case 2:
						color = { 1, 0, 0, 1.0f };
						break;
					case 3:
						color = { 0, 0, 1, 1.0f };
					}
					c = { vec3(x, y, z), distance, color, arr_map[i][j][k], z, false, 0, rotation_matrix };
					map.emplace_back(c);
				}
			}
		}
	} 
	else if (stage == 3) {
		int arr_map[6][6][6] = { {{2, 1, 1, 1, 1, 0}, {0, 0, 0, 0, 1, 1}, {1, 1, 1, 1, 0, 1}, {1, 0, 0, 1, 0, 0}, {0, 1, 1, 1, 0, 0}, {0, 0, 0, 0, 0, 0}},
								{{0, 0, 0, 0, 0, 1}, {0, 1, 1, 1, 0, 0}, {0, 0, 1, 0, 1, 1}, {0, 0, 0, 0, 0, 1}, {0, 1, 0, 0, 0, 1}, {0, 0, 0, 1, 1, 1}},
								{{1, 1, 1, 1, 1, 1}, {1, 0, 0, 0, 1, 0}, {1, 1, 1, 0, 1, 0}, {0, 0, 0, 0, 1, 0}, {0, 1, 0, 0, 0, 0}, {1, 1, 1, 0, 0, 0}},
								{{0, 0, 0, 0, 0, 0}, {0, 1, 1, 1, 0, 1}, {0, 0, 0, 0, 0, 1}, {1, 1, 1, 1, 0, 1}, {0, 1, 0, 1, 0, 1}, {0, 0, 0, 1, 1, 1}},
								{{0, 0, 1, 1, 0, 0}, {0, 1, 0, 1, 1, 1}, {0, 0, 0, 0, 1, 0}, {0, 0, 0, 0, 1, 0}, {0, 1, 0, 0, 1, 0}, {0, 1, 0, 0, 0, 0}},
								{{0, 0, 1, 0, 0, 0}, {0, 1, 0, 0, 0, 0}, {1, 1, 0, 0, 0, 0}, {1, 0, 0, 0, 0, 0}, {1, 0, 0, 0, 1, 0}, {1, 1, 1, 1, 1, 3}} };
		for (i = 0; i < map_size; i++) {
			for (j = 0; j < map_size; j++) {
				for (k = 0; k < map_size; k++) {
					z = distance * ((float)stage - 0.5f - (float)i);
					x = distance * (-(float)stage + 0.5f + (float)j);
					y = distance * (-(float)stage + 0.5f + (float)k);
					switch (arr_map[i][j][k]) {
					case 0:
						color = { 0, 0, 0, 1.0f };
						break;
					case 1:
						color = { 1, 1, 1, 1.0f };
						break;
					case 2:
						color = { 1, 0, 0, 1.0f };
						break;
					case 3:
						color = { 0, 0, 1, 1.0f };
					}
					c = { vec3(x, y, z), distance, color, arr_map[i][j][k], z, false, 0, rotation_matrix };
					map.emplace_back(c);
				}
			}
		}
	}
	
	return map;
}

inline void cube_t::update(int command, int tick, int rotate_time, vec3 char_center, float char_size, float block_size)
{
	// these transformations will be explained in later transformation lecture
	if (command == 2) {
		//down
		if (!reversing) {
			save_z = -2 * center.z / (float)tick;
			tick_time = 0;
			reversing = true;
		}
		center.z += save_z * (float)rotate_time;
		tick_time += rotate_time;
		if (tick_time == tick) reversing = false;
	}
	else if (command == 3) {
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
	else if (command == 4) {
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
	if (center.x - block_size / 2 >= char_center.x + char_size) {
		color.w = 0.0f;
	}
	else if (center.x + block_size/2 >= char_center.x - char_size) {
		color.w = 0.7f;
	}
	else {
		color.w = 1.0f;
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
