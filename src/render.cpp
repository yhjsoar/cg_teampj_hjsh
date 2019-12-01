#include "cgmath.h"			// slee's simple math library
#include "cgut.h"			// slee's OpenGL utility
#include "trackball.h"		// virtual trackball
#include "cube.h"
#include "index.h"
#include "duck.h"
#include <math.h>

#define GAME_SCENE 1


//*******************************************************************
// global constants
//static const char* window_name = "cgbase - trackball";
static const char* vert_shader_path = "../bin/shaders/trackball.vert";
static const char* frag_shader_path = "../bin/shaders/trackball.frag";

//*******************************************************************
// common structures
struct camera
{
	vec3	eye = vec3(100, 0, 0);
	vec3	at = vec3(0, 0, 0);
	vec3	up = vec3(0, 0, 1);
	mat4	view_matrix = mat4::look_at(eye, at, up);

	float	fovy = PI / 4.0f; // must be in radian
	float	aspect_ratio;
	float	dnear = 1.0f;
	float	dfar = 1000.0f;
	mat4	projection_matrix;
};


//*******************************************************************
// window objects
GLFWwindow* window = nullptr;
ivec2		window_size = ivec2(1200, 900);	// initial window size

//*******************************************************************
// OpenGL objects
GLuint	program = 0;	// ID holder for GPU program
GLuint	vertex_buffer = 0;
GLuint	index_buffer = 0;
GLuint	vertex_char_buffer = 0;
GLuint	index_char_buffer = 0;
GLuint	vertex_char_moving_buffer[4] = { 0, 0, 0, 0 };
GLuint	index_char_moving_buffer[4] = { 0, 0, 0, 0 };

//*******************************************************************
// global variables
				// index of rendering frames
bool	b_wireframe = false;	// this is the default
float	rotating_theta = 0.0f;
int		num_solid_color = 0;			// use circle's color?
bool	is_pause = false;
int		scene_number = GAME_SCENE;

// time informations
float	last_time;
float	now_time = float(glfwGetTime());

// object informations
int		stage = 1;
float	cube_distance[3] = { 15.0f, 10, 7.5f };
int		map_size = 2;
float	char_size = 2.0f;

// object variables
std::vector<std::vector<cube_t>> map;
duck_t	character = create_character(map_size, cube_distance[stage], char_size, stage);

// map rotating variables
int		command = 0;													// what rotation	
int		tick = 30;														// one rotation is split by tick
int		now_tick = 0;													// how much split tick done
int		double_rotate = 0;												// if tick time over, it will be more than one tick
float	tick_time = 0.006f;												// one split every tick time
float	start_time = 0.0f;												// time when rotation start
float	passed_time = 0.0f;												// passed time from start_time
bool	key_lock = false;												// is key locked (when rotation)
bool	rotate = false;													// is rotating

// character moving variables
bool	character_move_bool[4] = { false, false, false, false };		// left, right, up, down
bool	character_move_key_waiting[4] = { false, false, false, false };	// left, right, up, down
bool	moving = false;
float	moving_start_time;
int		indic = 0;

// gravity
bool	gravity_on = false;

vec3	char_center = character.center;

// indices
std::vector<uint> indices;
std::vector<uint> char_indices;
std::vector<uint> char_moving_indices[4];
struct { bool add = false, sub = false; operator bool() const { return add || sub; } } b; // flags of keys for smooth changes

//*************************************
// holder of vertices and indices of a unit circle
std::vector<vertex>	unit_cube_vertices;
std::vector<vertex>	duck_vertices;
std::vector<vertex> duck_moving_vertices[4];

//*******************************************************************
// scene objects
camera		cam;
trackball	tb;

//*******************************************************************
void update()
{
	// update projection matrix
	cam.aspect_ratio = window_size.x / float(window_size.y);
	cam.projection_matrix = mat4::perspective(cam.fovy, cam.aspect_ratio, cam.dnear, cam.dfar);

	GLint uloc;
	uloc = glGetUniformLocation(program, "view_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, cam.view_matrix);
	uloc = glGetUniformLocation(program, "projection_matrix");	if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, cam.projection_matrix);

	printf("asdf\n");
}

void render()
{
	// clear screen (with background color) and clear depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// notify GL that we use our own program and buffers
	glUseProgram(program);
	if (scene_number == GAME_SCENE) {
		// bind vertex attributes to your shader program

		last_time = now_time;
		now_time = float(glfwGetTime());
		passed_time = now_time - start_time;

		if (key_lock) {
			if (tick_time * now_tick <= passed_time && tick_time * (now_tick + 1) >= passed_time) {
				now_tick++;
				double_rotate = 1;
				rotate = true;
			}
			else if ((now_tick + 1) * tick_time < passed_time) {
				for (int k = 2;; k++) {
					if ((now_tick + k) * tick_time >= passed_time) {
						if (now_tick + k > tick) {
							double_rotate = tick - now_tick;
							now_tick = tick;
						}
						else {
							now_tick += k;
							double_rotate = k;
						}
						rotate = true;
						break;
					}
				}
			}
		}

		if ((character_move_bool[0] || character_move_bool[1] || character_move_bool[2] || character_move_bool[3]) && !moving) {
			moving = true;
			moving_start_time = float(glfwGetTime());
		} if (!(character_move_bool[0] || character_move_bool[1] || character_move_bool[2] || character_move_bool[3])) {
			moving = false;
		}
		if ((character_move_bool[0] || character_move_bool[1] || character_move_bool[2] || character_move_bool[3])) {
			indic++;
			if (vertex_char_moving_buffer[indic / 10 % 4])	glBindBuffer(GL_ARRAY_BUFFER, vertex_char_moving_buffer[indic / 10 % 4]);
			if (index_char_moving_buffer[indic / 10 % 4])		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_char_moving_buffer[indic / 10 % 4]);
		}
		else {
			if (vertex_char_buffer)		glBindBuffer(GL_ARRAY_BUFFER, vertex_char_buffer);
			if (index_char_buffer)		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_char_buffer);

		}
		cg_bind_vertex_attributes(program);

		if (rotate) character.update_rotate(command, rotate, tick, double_rotate, false, cube_distance[stage - 1]);
		else {
			// gravity_on : true -> on floor / false -> not on floor
			if (gravity_on) {
				gravity_on = character.check_if_on_floor(map.at(stage - 1), cube_distance[stage - 1], stage);
				start_time = now_time;
			}
			if (!gravity_on) {
				gravity_on = character.update_gravity(map.at(stage - 1), passed_time / 2, cube_distance[stage - 1], stage);
			}
			else {
				character.update_moving(character_move_bool[2], character_move_bool[3], character_move_bool[0], character_move_bool[1], 10.0f * (now_time - last_time), map.at(stage - 1), stage, cube_distance[stage - 1]);
			}
		}
		char_center = character.center;
		GLint uloc;
		uloc = glGetUniformLocation(program, "b_solid_color");		if (uloc > -1) glUniform1i(uloc, false);
		uloc = glGetUniformLocation(program, "solid_color");		if (uloc > -1) glUniform4fv(uloc, 1, character.color);	// pointer version
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, character.model_matrix);

		glDrawElements(GL_TRIANGLES, char_indices.size(), GL_UNSIGNED_INT, nullptr);

		if (vertex_buffer)	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		if (index_buffer)	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);

		cg_bind_vertex_attributes(program);

		for (auto& c : map.at(stage - 1)) {
			if (rotate) {
				c.update(command, tick, double_rotate, char_center, char_size, cube_distance[stage - 1]);
			}
			else {
				c.update(command, tick, double_rotate, char_center, char_size, cube_distance[stage - 1]);
			}

			if (c.color.w == 0.0f) continue;
			// update per-circle uniforms
			GLint uloc;
			uloc = glGetUniformLocation(program, "b_solid_color");		if (uloc > -1) glUniform1i(uloc, true);	// pointer version
			uloc = glGetUniformLocation(program, "solid_color");		if (uloc > -1) glUniform4fv(uloc, 1, c.color);	// pointer version
			uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, c.model_matrix);

			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);

		}
		if (gravity_on && !rotate && character.isClear(map.at(stage - 1), stage)) {
			stage = stage == 3 ? 1 : stage + 1;
			character.set_location(stage, cube_distance[stage - 1], char_size);
		}
		rotate = false;
		if (key_lock) {
			if (now_tick == tick) {
				command = 0;
				key_lock = false;
			}
		}


	}


	// swap front and back buffers, and display to screen
	glfwSwapBuffers(window);
}

void reshape(GLFWwindow* window, int width, int height)
{
	// set current viewport in pixels (win_x, win_y, win_width, win_height)
	// viewport: the window area that are affected by rendering 
	window_size = ivec2(width, height);
	glViewport(0, 0, width, height);
}

void print_help()
{
	printf("[help]\n");
	printf("- press ESC or 'q' to terminate the program\n");
	printf("- press F1 or 'h' to see help\n");
	printf("- press 'w' to toggle wireframe\n");
	printf("- press Home to reset camera\n");
	printf("- press Pause to pause the simulation");
	printf("\n");
}

void update_vertex_buffer(const std::vector<vertex>& vertices) {
	// clear and create new buffers
	if (vertex_buffer)	glDeleteBuffers(1, &vertex_buffer);	vertex_buffer = 0;
	if (index_buffer)	glDeleteBuffers(1, &index_buffer);	index_buffer = 0;

	// check exceptions
	if (vertices.empty()) { printf("[error] vertices is empty.\n"); return; }

	// create buffers
	for (uint i = 0; i < 6; i++) {
		indices.push_back(4 * i);
		indices.push_back(4 * i + 1);
		indices.push_back(4 * i + 2);

		indices.push_back(4 * i);
		indices.push_back(4 * i + 2);
		indices.push_back(4 * i + 3);
	}
	// generation of vertex buffer: use vertices as it is
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// geneation of index buffer
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);
}

void update_character_vertex_buffer(const std::vector<vertex>& vertices)
{
	// clear and create new buffers
	if (vertex_char_buffer)	glDeleteBuffers(1, &vertex_char_buffer);	vertex_char_buffer = 0;
	if (index_char_buffer)	glDeleteBuffers(1, &index_char_buffer);	index_char_buffer = 0;

	// check exceptions
	if (vertices.empty()) { printf("[error] vertices is empty.\n"); return; }

	// create buffers
	for (int k = 0; k < 11; k++) {
		char_indices.push_back(1 + k * 8);
		char_indices.push_back(4 + 8 * k);
		char_indices.push_back(5 + 8 * k);

		char_indices.push_back(0 + 8 * k);
		char_indices.push_back(4 + 8 * k);
		char_indices.push_back(1 + 8 * k);

		char_indices.push_back(0 + 8 * k);
		char_indices.push_back(6 + 8 * k);
		char_indices.push_back(4 + 8 * k);

		char_indices.push_back(2 + 8 * k);
		char_indices.push_back(6 + 8 * k);
		char_indices.push_back(0 + 8 * k);

		char_indices.push_back(2 + 8 * k);
		char_indices.push_back(7 + 8 * k);
		char_indices.push_back(6 + 8 * k);

		char_indices.push_back(3 + 8 * k);
		char_indices.push_back(7 + 8 * k);
		char_indices.push_back(2 + 8 * k);

		char_indices.push_back(3 + 8 * k);
		char_indices.push_back(5 + 8 * k);
		char_indices.push_back(7 + 8 * k);

		char_indices.push_back(1 + 8 * k);
		char_indices.push_back(5 + 8 * k);
		char_indices.push_back(3 + 8 * k);

		char_indices.push_back(5 + 8 * k);
		char_indices.push_back(6 + 8 * k);
		char_indices.push_back(7 + 8 * k);

		char_indices.push_back(4 + 8 * k);
		char_indices.push_back(6 + 8 * k);
		char_indices.push_back(5 + 8 * k);

		char_indices.push_back(3 + 8 * k);
		char_indices.push_back(0 + 8 * k);
		char_indices.push_back(1 + 8 * k);

		char_indices.push_back(2 + 8 * k);
		char_indices.push_back(0 + 8 * k);
		char_indices.push_back(3 + 8 * k);
	}
	// generation of vertex buffer: use vertices as it is
	glGenBuffers(1, &vertex_char_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_char_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// geneation of index buffer
	glGenBuffers(1, &index_char_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_char_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * char_indices.size(), &char_indices[0], GL_STATIC_DRAW);
}

void update_moving_character_vertex_buffer(const std::vector<vertex>& vertices, int i)
{
	// clear and create new buffers
	if (vertex_char_moving_buffer[i])	glDeleteBuffers(1, &vertex_char_moving_buffer[i]);	vertex_char_moving_buffer[i] = 0;
	if (index_char_moving_buffer[i])	glDeleteBuffers(1, &index_char_moving_buffer[i]);	index_char_moving_buffer[i] = 0;

	// check exceptions
	if (vertices.empty()) { printf("[error] vertices is empty.\n"); return; }

	// create buffers
	for (int k = 0; k < 11; k++) {
		char_moving_indices[i].push_back(1 + k * 8);
		char_moving_indices[i].push_back(4 + 8 * k);
		char_moving_indices[i].push_back(5 + 8 * k);

		char_moving_indices[i].push_back(0 + 8 * k);
		char_moving_indices[i].push_back(4 + 8 * k);
		char_moving_indices[i].push_back(1 + 8 * k);

		char_moving_indices[i].push_back(0 + 8 * k);
		char_moving_indices[i].push_back(6 + 8 * k);
		char_moving_indices[i].push_back(4 + 8 * k);

		char_moving_indices[i].push_back(2 + 8 * k);
		char_moving_indices[i].push_back(6 + 8 * k);
		char_moving_indices[i].push_back(0 + 8 * k);

		char_moving_indices[i].push_back(2 + 8 * k);
		char_moving_indices[i].push_back(7 + 8 * k);
		char_moving_indices[i].push_back(6 + 8 * k);

		char_moving_indices[i].push_back(3 + 8 * k);
		char_moving_indices[i].push_back(7 + 8 * k);
		char_moving_indices[i].push_back(2 + 8 * k);

		char_moving_indices[i].push_back(3 + 8 * k);
		char_moving_indices[i].push_back(5 + 8 * k);
		char_moving_indices[i].push_back(7 + 8 * k);

		char_moving_indices[i].push_back(1 + 8 * k);
		char_moving_indices[i].push_back(5 + 8 * k);
		char_moving_indices[i].push_back(3 + 8 * k);

		char_moving_indices[i].push_back(5 + 8 * k);
		char_moving_indices[i].push_back(6 + 8 * k);
		char_moving_indices[i].push_back(7 + 8 * k);

		char_moving_indices[i].push_back(4 + 8 * k);
		char_moving_indices[i].push_back(6 + 8 * k);
		char_moving_indices[i].push_back(5 + 8 * k);

		char_moving_indices[i].push_back(3 + 8 * k);
		char_moving_indices[i].push_back(0 + 8 * k);
		char_moving_indices[i].push_back(1 + 8 * k);

		char_moving_indices[i].push_back(2 + 8 * k);
		char_moving_indices[i].push_back(0 + 8 * k);
		char_moving_indices[i].push_back(3 + 8 * k);
	}
	// generation of vertex buffer: use vertices as it is
	glGenBuffers(1, &vertex_char_moving_buffer[i]);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_char_moving_buffer[i]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// geneation of index buffer
	glGenBuffers(1, &index_char_moving_buffer[i]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_char_moving_buffer[i]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * char_moving_indices[i].size(), &char_moving_indices[i][0], GL_STATIC_DRAW);
}


void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_UP) {
		if (action == GLFW_PRESS) {
			if (!character_move_bool[3] && !key_lock) {
				character_move_bool[2] = true;
			}
			if (character_move_bool[3] && !key_lock) {
				character_move_key_waiting[2] = true;
			}
		}
		else if (action == GLFW_RELEASE) {
			character_move_bool[2] = false;
			if (character_move_key_waiting[3]) {
				character_move_bool[3] = true;
				character_move_key_waiting[3] = false;
			}
		}
	}
	else if (key == GLFW_KEY_LEFT) {
		if (action == GLFW_PRESS) {
			if (!character_move_bool[1] && !key_lock) {
				character_move_bool[0] = true;
			}
			if (character_move_bool[1] && !key_lock) {
				character_move_key_waiting[0] = true;
			}
		}
		else if (action == GLFW_RELEASE) {
			character_move_bool[0] = false;
			if (character_move_key_waiting[1]) {
				character_move_bool[1] = true;
				character_move_key_waiting[1] = false;
			}
		}
	}
	else if (key == GLFW_KEY_RIGHT) {
		if (action == GLFW_PRESS) {
			if (!character_move_bool[0] && !key_lock) {
				character_move_bool[1] = true;
			}
			if (character_move_bool[0] && !key_lock) {
				character_move_key_waiting[1] = true;
			}
		}
		else if (action == GLFW_RELEASE) {
			character_move_bool[1] = false;
			if (character_move_key_waiting[0]) {
				character_move_bool[0] = true;
				character_move_key_waiting[0] = false;
			}
		}
	}
	else if (key == GLFW_KEY_DOWN) {
		if (action == GLFW_PRESS) {
			if (!character_move_bool[2] && !key_lock) {
				character_move_bool[3] = true;
			}
			if (character_move_bool[2] && !key_lock) {
				character_move_key_waiting[3] = true;
			}
		}
		else if (action == GLFW_RELEASE) {
			character_move_bool[3] = false;
			if (character_move_key_waiting[2]) {
				character_move_bool[2] = true;
				character_move_key_waiting[2] = false;
			}
		}
	}
	else if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)	glfwSetWindowShouldClose(window, GL_TRUE);
		else if (key == GLFW_KEY_H || key == GLFW_KEY_F1)	print_help();
		else if (key == GLFW_KEY_HOME) {
			cam = camera();
		}
		else if (key == GLFW_KEY_PAUSE) {
			is_pause = !is_pause;
		}
		else if (key == GLFW_KEY_O) {
			cam = camera();
		}
		else if (key == GLFW_KEY_S) {
			if (!key_lock) {
				key_lock = true;
				start_time = float(glfwGetTime());
				passed_time = 0.0f;
				now_tick = 0;
				command = 2;
			}
		}
		else if (key == GLFW_KEY_A) {
			if (!key_lock) {
				key_lock = true;
				start_time = float(glfwGetTime());
				passed_time = 0.0f;
				now_tick = 0;
				command = 3;
			}
		}
		else if (key == GLFW_KEY_D) {
			if (!key_lock) {
				key_lock = true;
				start_time = float(glfwGetTime());
				passed_time = 0.0f;
				now_tick = 0;
				command = 4;
			}
		}
		else if (key == GLFW_KEY_Y) {
			stage = stage == 3 ? 1 : stage + 1;
			character.set_location(stage, cube_distance[stage - 1], char_size);
		}
	}
}

void mouse(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		dvec2 pos; glfwGetCursorPos(window, &pos.x, &pos.y);
		vec2 npos = vec2(float(pos.x) / float(window_size.x - 1), float(pos.y) / float(window_size.y - 1));
		if (action == GLFW_PRESS) {
			tb.begin(cam.view_matrix, npos.x, npos.y, cam.at);
		}
		else if (action == GLFW_RELEASE) {
			tb.end();
		}
	}
}

void motion(GLFWwindow* window, double x, double y)
{
	if (!tb.is_tracking()) return;

	vec2 npos = vec2(float(x) / float(window_size.x - 1), float(y) / float(window_size.y - 1));
	cam.view_matrix = tb.update(npos.x, npos.y);

}

bool user_init()
{
	// log hotkeys
	print_help();

	// init GL states
	glLineWidth(1.0f);
	glClearColor(39 / 255.0f, 40 / 255.0f, 34 / 255.0f, 1.0f);	// set clear color
	glEnable(GL_CULL_FACE);								// turn on backface culling
	glEnable(GL_DEPTH_TEST);								// turn on depth tests
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	auto map1 = std::move(create_map(cube_distance[0], 1));
	auto map2 = std::move(create_map(cube_distance[1], 2));
	auto map3 = std::move(create_map(cube_distance[2], 3));
	map.emplace_back(map1);
	map.emplace_back(map2);
	map.emplace_back(map3);


	// load the mesh
	unit_cube_vertices = std::move(create_cube_vertices());
	update_vertex_buffer(unit_cube_vertices);
	duck_vertices = std::move(create_character_vertices());
	update_character_vertex_buffer(duck_vertices);
	duck_moving_vertices[0] = std::move(create_character_vertices_left_front());
	duck_moving_vertices[1] = std::move(create_character_vertices());
	duck_moving_vertices[2] = std::move(create_character_vertices_right_front());
	duck_moving_vertices[3] = std::move(create_character_vertices());
	for (int i = 0; i < 4; i++) {
		update_moving_character_vertex_buffer(duck_moving_vertices[i], i);
	}

	return true;
}

void user_finalize()
{
}
/*
int main(int argc, char* argv[])
{
	// initialization
	if (!glfwInit()) { printf("[error] failed in glfwInit()\n"); return 1; }

	// create window and initialize OpenGL extensions
	if (!(window = cg_create_window(window_name, window_size.x, window_size.y))) { glfwTerminate(); return 1; }
	if (!cg_init_extensions(window)) { glfwTerminate(); return 1; }	// version and extensions
	// initializations and validations
	if (!(program = cg_create_program(vert_shader_path, frag_shader_path))) { glfwTerminate(); return 1; }	// create and compile shaders/program
	if (!user_init()) { printf("Failed to user_init()\n"); glfwTerminate(); return 1; }					// user initialization
	// register event callbacks
	glfwSetWindowSizeCallback(window, reshape);	// callback for window resizing events
	glfwSetKeyCallback(window, keyboard);			// callback for keyboard events
	glfwSetMouseButtonCallback(window, mouse);	// callback for mouse click inputs
	glfwSetCursorPosCallback(window, motion);		// callback for mouse movement

	// enters rendering/event loop
	for (frame = 0; !glfwWindowShouldClose(window); frame++)
	{
		glfwPollEvents();	// polling and processing of events
		update();			// per-frame update
		render();			// per-frame render
	}

	// normal termination
	user_finalize();
	cg_destroy_window(window);

	return 0;
}*/