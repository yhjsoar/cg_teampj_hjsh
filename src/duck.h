#pragma once
#ifndef __DUCK_H__
#define __DUCK_H__

struct duck_t
{
	vec3	center = vec3(0);		// 2D position for translation
	float	radius = 100.0f;		// radius
	vec4	color;				// RGBA color in [0,1]
	float	save_z;
	bool	reversing;
	int		tick_time;
	mat4	rotation_matrix;
	mat4	model_matrix;		// modeling transformation
	

	// public functions
	void	update_rotate(int command, bool rotate, int tick, int rotate_time, bool isBacking, float cube_size);
	void	update_moving(bool move_up, bool move_down, bool move_left, bool move_right, float move_distance, std::vector<cube_t> maps);
	bool	update_gravity(std::vector<cube_t> maps, float gravity, float cube_size);
	bool	check_if_on_floor(std::vector<cube_t> maps, float cube_size);
};


inline duck_t create_character(int map_size, float distance, float char_size)
{
	float z = -(float)(map_size) / 2 * distance + char_size;
	float x = distance * (-0.5f + (float)(map_size-1));
	float y = distance * (-0.5f + (float)(map_size-1));
	mat4 rotation_matrix =
	{
		1, 0, 0, 0,
		0, cos(PI / 2), -sin(PI / 2), 0,
		0, sin(PI / 2), cos(PI / 2), 0,
		0, 0, 0, 1
	};
	duck_t duck = {vec3(x, y, z), char_size, vec4(1, 1, 0, 1.0f), z, false, 0, rotation_matrix};

	return duck;
}

inline void duck_t::update_rotate(int command, bool rotate, int tick, int rotate_time, bool isBacking, float cube_size)
{
	// these transformations will be explained in later transformation lecture
	float next_x = center.x;
	float next_y = center.y;
	if (command == 1 && rotate) {
		//up
		if(!isBacking) {
			float dist = sqrt(center.x * center.x + center.z * center.z);
			float theta = atan2(center.z, center.x);
			float after_theta = theta + PI / 2 / (float)tick * (float)rotate_time;
			center.x = dist * cos(after_theta);
			center.z = dist * sin(after_theta);
		}
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
		if (!reversing) {
			float floor = center.z - radius + cube_size;
			float after_center = -floor +radius;
			float diff = - center.z + after_center;
			printf("original z: %f\n", center.z);
			save_z = diff / (float)tick;
			//save_z = -2 * center.z / (float)tick;
			tick_time = 0;
			reversing = true;
		}
		center.z += save_z * (float)rotate_time;
		tick_time += rotate_time;
		if (tick_time == tick) reversing = false;
		/*if (!isBacking) {
			float dist = sqrt(center.x * center.x + center.z * center.z);
			float theta = atan2(center.z, center.x);
			float after_theta = theta - PI / 2 / (float)tick * (float)rotate_time;
			center.x = dist * cos(after_theta);
			center.z = dist * sin(after_theta);`````````````````````
		}
		float thet = PI / 2 / (float)tick * (float)rotate_time;
		float cos_y = cos(thet), sin_y = sin(thet);*/
		/*mat4 rotation_matrix_y =
		{
			cos_y, 0, sin_y, 0,
			0, 1, 0, 0,
			-sin_y, 0, cos_y, 0,
			0, 0, 0, 1
		};
		rotation_matrix = rotation_matrix_y * rotation_matrix;*/
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

inline void duck_t::update_moving(bool move_up, bool move_down, bool move_left, bool move_right, float move_distance, std::vector<cube_t> maps) {
	
	float next_x = center.x;
	float next_y = center.y;

	if (move_up && move_left) {
		rotation_matrix =
		{
			1, 0, 0, 0,
			0, cos(PI / 2), -sin(PI / 2), 0,
			0, sin(PI / 2), cos(PI / 2), 0,
			0, 0, 0, 1
		};
		mat4 rotation_matrix_z = {
			cos(-PI / 4), -sin(-PI / 4), 0, 0,
			sin(-PI / 4), cos(-PI / 4), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};
		rotation_matrix = rotation_matrix_z * rotation_matrix;
		next_x -= move_distance / sqrtf(2);
		next_y -= move_distance / sqrtf(2);
	}
	else if (move_up && move_right) {
		rotation_matrix =
		{
			1, 0, 0, 0,
			0, cos(PI / 2), -sin(PI / 2), 0,
			0, sin(PI / 2), cos(PI / 2), 0,
			0, 0, 0, 1
		};
		mat4 rotation_matrix_z = {
			cos(-3 * PI / 4), -sin(-3 * PI / 4), 0, 0,
			sin(-3 * PI / 4), cos(-3 * PI / 4), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};
		rotation_matrix = rotation_matrix_z * rotation_matrix;
		next_x -= move_distance / sqrtf(2);
		next_y += move_distance / sqrtf(2);
	}
	else if (move_up) {
		rotation_matrix =
		{
			1, 0, 0, 0,
			0, cos(PI / 2), -sin(PI / 2), 0,
			0, sin(PI / 2), cos(PI / 2), 0,
			0, 0, 0, 1
		};
		mat4 rotation_matrix_z = {
			cos(-PI / 2), -sin(-PI / 2), 0, 0,
			sin(-PI / 2), cos(-PI / 2), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};
		rotation_matrix = rotation_matrix_z * rotation_matrix;
		next_x -= move_distance;
	}
	else if (move_down && move_left) {
		rotation_matrix =
		{
			1, 0, 0, 0,
			0, cos(PI / 2), -sin(PI / 2), 0,
			0, sin(PI / 2), cos(PI / 2), 0,
			0, 0, 0, 1
		};
		mat4 rotation_matrix_z = {
			cos(PI / 4), -sin(PI / 4), 0, 0,
			sin(PI / 4), cos(PI / 4), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};
		rotation_matrix = rotation_matrix_z * rotation_matrix;
		next_x += move_distance / sqrtf(2);
		next_y -= move_distance / sqrtf(2);
	}
	else if (move_down && move_right) {
		rotation_matrix =
		{
			1, 0, 0, 0,
			0, cos(PI / 2), -sin(PI / 2), 0,
			0, sin(PI / 2), cos(PI / 2), 0,
			0, 0, 0, 1
		};
		mat4 rotation_matrix_z = {
			cos(3 * PI / 4), -sin(3 * PI / 4), 0, 0,
			sin(3 * PI / 4), cos(3 * PI / 4), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};
		rotation_matrix = rotation_matrix_z * rotation_matrix;
		next_x += move_distance / sqrtf(2);
		next_y += move_distance / sqrtf(2);
	}
	else if (move_down) {
		rotation_matrix =
		{
			1, 0, 0, 0,
			0, cos(PI / 2), -sin(PI / 2), 0,
			0, sin(PI / 2), cos(PI / 2), 0,
			0, 0, 0, 1
		};
		mat4 rotation_matrix_z = {
			cos(PI / 2), -sin(PI / 2), 0, 0,
			sin(PI / 2), cos(PI / 2), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};
		rotation_matrix = rotation_matrix_z * rotation_matrix;
		next_x += move_distance;
	}
	else if (move_left) {
		rotation_matrix =
		{
			1, 0, 0, 0,
			0, cos(PI / 2), -sin(PI / 2), 0,
			0, sin(PI / 2), cos(PI / 2), 0,
			0, 0, 0, 1
		};
		next_y -= move_distance;
	}
	else if (move_right) {
		rotation_matrix =
		{
			1, 0, 0, 0,
			0, cos(PI / 2), -sin(PI / 2), 0,
			0, sin(PI / 2), cos(PI / 2), 0,
			0, 0, 0, 1
		};
		mat4 rotation_matrix_z = {
			cos(PI), -sin(PI), 0, 0,
			sin(PI), cos(PI), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};
		rotation_matrix = rotation_matrix_z * rotation_matrix;
		next_y += move_distance;
	}
	
	if (next_x != center.x || next_y != center.y) {
		printf("moving");
		for (auto& c : maps) {
			if (c.type == 0) {
				if (c.center.z + c.radius / 2 <= center.z - radius || c.center.z - c.radius / 2 >= center.z + radius) {
					continue;
				}
				if (c.center.x + c.radius / 2 <= next_x - radius || c.center.x - c.radius / 2 >= next_x + radius) {
					continue;
				}
				if (c.center.y + c.radius / 2 <= next_y - radius || c.center.y - c.radius / 2 >= next_y + radius) {
					continue;
				}
				next_x = center.x;
				next_y = center.y;
				break;
			}
		}
	}
	center.x = next_x;
	center.y = next_y;

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

inline bool duck_t::update_gravity(std::vector<cube_t> maps, float gravity, float cube_size) {
	printf("start gravity - %f\n", center.z);
	center.z = center.z - gravity;
	printf("after gravity - %f\n", center.z);
	bool result = check_if_on_floor(maps, cube_size);
	printf("after check - %f\n", center.z);

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

	return result;
}

inline bool duck_t::check_if_on_floor(std::vector<cube_t> maps, float cube_size) {
	if (center.z + radius < -cube_size * 0) {
		if (center.z - radius == -cube_size * 1) return true;
		if (center.z - radius < -cube_size * 1) {
			center.z = -cube_size * 1 + radius;
			return true;
		}
		return false;
	}
	for (auto& c : maps) {
		if (c.type == 1 || c.type == 2) continue;
		if (c.center.x + c.radius / 2 <= center.x - radius || c.center.x - c.radius / 2 >= center.x + radius)	continue;
		if (c.center.y + c.radius / 2 <= center.y - radius || c.center.y - c.radius / 2 >= center.y + radius)	continue;
		if (c.center.z - c.radius / 2 >= center.z + radius) continue;
		//if (c.center.z + c.radius / 2 <= center.z - radius || c.center.z - c.radius / 2 >= center.z + radius)	continue;
		if (c.center.z + c.radius / 2 >= center.z - radius) {
			printf("c.center.z + c.radius / 2 : %f\n", c.center.z + c.radius);
			printf("center.z - radius: %f\n", center.z - radius);
			center.z = c.center.z + c.radius / 2 + radius;
			return true;
		}
	}
	return false;
}
#endif