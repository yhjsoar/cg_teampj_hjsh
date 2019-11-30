#include "cgmath.h"			// slee's simple math library
#include "cgut.h"			// slee's OpenGL utility
#include "trackball.h"		// virtual trackball
#include "cube.h"
#include "index.h"
#include "duck.h"
#include "minimap.h"
#include "background.h"
#include <math.h>
#include <string.h>
#include "irrKlang\irrKlang.h"


#pragma comment(lib, "irrKlang.lib")

#define GAME_SCENE 2
#define	DUCK_SCENE 1
#define MINIMAP 3
#define	STAGE_NUMBER 4
#define STAGE_COMPLETE 5
#define	FINISH_SCENE 6
 
//*******************************************************************
// forward declarations for freetype text
void text_init();
void render_text(std::string text, GLint x, GLint y, GLfloat scale, vec4 color);

//*******************************************************************
// global constants
static const char*	window_name = "duck";
static const char*	vert_shader_path = "../bin/shaders/trackball.vert";
static const char*	frag_shader_path = "../bin/shaders/trackball.frag";
static const char*	mp3_path = "../bin/duck_sound.mp3";
static const char*	mp3_back_path = "../bin/background_sound.mp3";

//*******************************************************************
// common structures
struct camera
{
	vec3	eye = vec3( 0, 150, 0 );
	vec3	at = vec3( 0, 0, 0 );
	vec3	up = vec3( 0, 0, 1 );
	mat4	view_matrix = mat4::look_at( eye, at, up );

	float	fovy = PI/4.0f; // must be in radian
	float	aspect_ratio;
	float	dnear = 1.0f;
	float	dfar = 1000.0f;
	mat4	projection_matrix;
};


//*******************************************************************
// window objects
GLFWwindow*	window = nullptr;
ivec2		window_size = ivec2( 1200, 900 );	// initial window size

//*******************************************************************
// OpenGL objects
GLuint	program			= 0;	// ID holder for GPU program
GLuint	vertex_buffer = 0;
GLuint	index_buffer = 0;
GLuint	vertex_char_buffer = 0;
GLuint	index_char_buffer = 0;
GLuint	vertex_char_moving_buffer[4] = { 0, 0, 0, 0 };
GLuint	index_char_moving_buffer[4] = { 0, 0, 0, 0 };
GLuint	vertex_sphere_buffer = 0;
GLuint	index_sphere_buffer = 0;

//*******************************************************************
// irrKlang objects
irrklang::ISoundEngine* engine;
irrklang::ISoundSource* mp3_src = nullptr;
irrklang::ISoundSource* mp3_back_src = nullptr;

//*******************************************************************
// global variables
int		frame = 0;				// index of rendering frames
bool	b_wireframe = false;	// this is the default
float	rotating_theta = 0.0f;
int		num_solid_color = 0;			// use circle's color?
bool	is_pause = false;
int		scene_number = DUCK_SCENE;

// time informations
float	last_time;
float	now_time = float(glfwGetTime());
float	stage_time[3] = { 30.0f, 50.0f, 100.0f };
float	stage_get_time[3] = { 0.0f, 0.0f, 0.0f };
float	stage_time_start;

// object informations
int		stage = 1;
float	cube_distance[3] = { 15.0f, 10, 7.5f };
int		map_size = 2;
float	char_size = 2.0f;
float	char_lobby_size = 6.0f;

// object variables
std::vector<std::vector<cube_t>> map;
duck_t	character = create_character(map_size, cube_distance[stage], char_size, stage);
duck_t	character_lobby = create_duck_lobby(char_lobby_size);
cube_t	lobby_cube = create_lobby(character_lobby.center, character_lobby.radius);
minimap_t minimap = create_minimap();
background_t background = create_background();

// map camera moving variables
float	stage_cam_duration_time = 2.0f;
float	stage_cam_start_time;
bool	stage_cam_moving;

float	stage_cam_closer_duration_time = 1.0f;
float	stage_cam_closer_start_time;
bool	stage_cam_closer_moving;

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
bool	dash = false;

// gravity
bool	gravity_on = false;

// minimap
mat4	minimap_rotation_matrix = mat4{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

// lobby
float	cam_duration_time = 2.0f;
float	cam_start_time = float(glfwGetTime());
bool	cam_moving = true;
bool	to_next_scene = false;
float	to_next_time = 1.0f;
float	to_next_start = 0.0f;
float	cam_closer_duration_time = 1.0f;
float	cam_closer_start_time = float(glfwGetTime());
bool	cam_closer_moving = false;
float	text_start = float(glfwGetTime());

// stage scene
float	stage_start_time = float(glfwGetTime());

// complete scene
float	complete_start_time = float(glfwGetTime());


vec3	char_center = character.center;

// indices
std::vector<uint> indices;
std::vector<uint> char_indices;
std::vector<uint> char_moving_indices[4];
std::vector<uint> sphere_indices;
struct { bool add = false, sub = false; operator bool() const { return add || sub; } } b; // flags of keys for smooth changes

//*************************************
// holder of vertices and indices of a unit circle
std::vector<vertex>	unit_cube_vertices;
std::vector<vertex>	duck_vertices;
std::vector<vertex> duck_moving_vertices[4];
std::vector<vertex> sphere_vertices;

//*******************************************************************
// scene objects
camera		cam;
trackball	tb;

//*******************************************************************
void update()
{
	// update projection matrix
	cam.aspect_ratio = window_size.x/float(window_size.y);
	cam.projection_matrix = mat4::perspective( cam.fovy, cam.aspect_ratio, cam.dnear, cam.dfar );
	
	glUseProgram(program);

	GLint uloc;
	uloc = glGetUniformLocation( program, "view_matrix" );			if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, cam.view_matrix );
	uloc = glGetUniformLocation( program, "projection_matrix" );	if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, cam.projection_matrix );
}

void render()
{
	// clear screen (with background color) and clear depth buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	last_time = now_time;
	now_time = float(glfwGetTime());

	// notify GL that we use our own program and buffers
	glUseProgram( program );
	if (scene_number == GAME_SCENE) {
		// bind vertex attributes to your shader program
		if (stage_cam_moving) {
			float theta = (-stage_cam_start_time + now_time) / stage_cam_duration_time * 3.0f * PI / 2.0f;
			if (theta >= 3 * PI / 2) {
				theta = 3 * PI / 2;
				stage_cam_moving = false;
				stage_cam_closer_moving = true;
				stage_cam_closer_start_time = float(glfwGetTime());
			}
			float cam_y = cosf(theta) * 150.0f;
			float cam_x = sinf(theta) * 150.0f;
			cam.eye = vec3(-cam_x, cam_y, 0);
			cam.view_matrix = mat4::look_at(cam.eye, cam.at, cam.up);
			stage_time_start = float(glfwGetTime());
		}
		else if (stage_cam_closer_moving) {
			float pass = (now_time - stage_cam_closer_start_time) / stage_cam_closer_duration_time * 50.0f;
			if (pass >= 50.0f) {
				pass = 50.0f;
				stage_cam_closer_moving = false;
			}
			cam.eye = vec3(150.0f - pass, 0, 0);
			cam.view_matrix = mat4::look_at(cam.eye, cam.at, cam.up);
			stage_time_start = float(glfwGetTime());
		}

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
		}
		if (!(character_move_bool[0] || character_move_bool[1] || character_move_bool[2] || character_move_bool[3])) {
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

		if (rotate) {
			character.update_rotate(command, rotate, tick, double_rotate, false, cube_distance[stage - 1]);
		}
		else if (!key_lock) {
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

		vec3 start_location, finish_location, duck_location;
		duck_location = character.center;
		for (auto& c : map.at(stage - 1)) {
			if (c.type == 3) finish_location = c.center;
			else if (c.type == 2) start_location = c.center;
		}
		minimap.update(stage, cube_distance[stage - 1], start_location, finish_location, duck_location, minimap_rotation_matrix, tb.b_tracking);

		if (vertex_sphere_buffer)	glBindBuffer(GL_ARRAY_BUFFER, vertex_sphere_buffer);
		if (index_sphere_buffer)	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_sphere_buffer);
		cg_bind_vertex_attributes(program);

		uloc = glGetUniformLocation(program, "b_solid_color");		if (uloc > -1) glUniform1i(uloc, true);	// pointer version
		uloc = glGetUniformLocation(program, "solid_color");		if (uloc > -1) glUniform4fv(uloc, 1, minimap.start_color);	// pointer version
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, minimap.start_model_matrix);

		glDrawElements(GL_TRIANGLES, sphere_indices.size(), GL_UNSIGNED_INT, nullptr);

		uloc = glGetUniformLocation(program, "b_solid_color");		if (uloc > -1) glUniform1i(uloc, true);	// pointer version
		uloc = glGetUniformLocation(program, "solid_color");		if (uloc > -1) glUniform4fv(uloc, 1, minimap.finish_color);	// pointer version
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, minimap.finish_model_matrix);

		glDrawElements(GL_TRIANGLES, sphere_indices.size(), GL_UNSIGNED_INT, nullptr);

		uloc = glGetUniformLocation(program, "b_solid_color");		if (uloc > -1) glUniform1i(uloc, true);	// pointer version
		uloc = glGetUniformLocation(program, "solid_color");		if (uloc > -1) glUniform4fv(uloc, 1, minimap.duck_color);	// pointer version
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, minimap.duck_model_matrix);

		glDrawElements(GL_TRIANGLES, sphere_indices.size(), GL_UNSIGNED_INT, nullptr);

		if (vertex_buffer)	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		if (index_buffer)	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);

		cg_bind_vertex_attributes(program);

		uloc = glGetUniformLocation(program, "b_solid_color");		if (uloc > -1) glUniform1i(uloc, true);	// pointer version
		uloc = glGetUniformLocation(program, "solid_color");		if (uloc > -1) glUniform4fv(uloc, 1, minimap.color);	// pointer version
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, minimap.model_matrix);

		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);

		background.update(cam.eye);

		uloc = glGetUniformLocation(program, "solid_color");		if (uloc > -1) glUniform4fv(uloc, 1, background.color);	// pointer version
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, background.model_matrix);

		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);

		stage_get_time[stage - 1] = stage_time[stage - 1] - (now_time - stage_time_start);
		if (stage_get_time[stage - 1] >= stage_time[stage - 1]) stage_get_time[stage - 1] = stage_time[stage - 1];
		if (stage_get_time[stage - 1] <= 0) stage_get_time[stage - 1] = 0;
		std::string left_time = std::to_string(stage_get_time[stage-1]);
		render_text(left_time, 500, 100, 0.8f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
		if (stage_get_time[stage - 1] == 0) {
			scene_number = FINISH_SCENE;
		}

		if (stage_get_time[stage-1]!=0 && gravity_on && !rotate && character.isClear(map.at(stage - 1), stage)) {
			stage = stage == 3 ? 1 : stage + 1;
			scene_number = STAGE_COMPLETE;
			complete_start_time = float(glfwGetTime());
		}
		rotate = false;
		if (key_lock) {
			if (now_tick == tick) {
				command = 0;
				key_lock = false;
			}
		}

		
	}
	else if (scene_number == DUCK_SCENE) {
		if (cam_moving) {
			float theta = (-cam_start_time + now_time) / cam_duration_time * 3.0f * PI / 2.0f;
			if (theta >= 3 * PI / 2) {
				theta = 3 * PI / 2;
				cam_moving = false;
				cam_closer_moving = true;
				cam_closer_start_time = float(glfwGetTime());
			}
			float cam_y = cosf(theta) * 150.0f;
			float cam_x = sinf(theta) * 150.0f;
			cam.eye = vec3(-cam_x, cam_y, 0);
			cam.view_matrix = mat4::look_at(cam.eye, cam.at, cam.up);
		}
		else if (cam_closer_moving) {
			float pass = (now_time - cam_closer_start_time) / cam_closer_duration_time * 50.0f;
			if (pass >= 50.0f) {
				pass = 50.0f;
				cam_closer_moving = false;
				text_start = float(glfwGetTime());
			}
			cam.eye = vec3(150.0f - pass, 0, 0);
			cam.view_matrix = mat4::look_at(cam.eye, cam.at, cam.up);
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

		character_lobby.update_moving_lobby(character_move_bool[2], character_move_bool[3], character_move_bool[0], character_move_bool[1], 10.0f * (now_time - last_time), character_lobby.radius*4.0f);

		GLint uloc;
		uloc = glGetUniformLocation(program, "b_solid_color");		if (uloc > -1) glUniform1i(uloc, false);
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, character_lobby.model_matrix);

		glDrawElements(GL_TRIANGLES, char_indices.size(), GL_UNSIGNED_INT, nullptr);

		if (vertex_buffer)	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		if (index_buffer)	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
		cg_bind_vertex_attributes(program);

		uloc = glGetUniformLocation(program, "b_solid_color");		if (uloc > -1) glUniform1i(uloc, true);	// pointer version
		uloc = glGetUniformLocation(program, "solid_color");		if (uloc > -1) glUniform4fv(uloc, 1, lobby_cube.color);	// pointer version
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, lobby_cube.model_matrix);

		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);

		background.update(cam.eye);

		uloc = glGetUniformLocation(program, "solid_color");		if (uloc > -1) glUniform4fv(uloc, 1, background.color);	// pointer version
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, background.model_matrix);

		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);

		if (to_next_scene) {
			if (now_time - to_next_start >= to_next_time) {
				scene_number = STAGE_NUMBER;
				stage_start_time = float(glfwGetTime());
			}
		}

		if (!cam_moving && !cam_closer_moving) {
			float diff_time = now_time - text_start;
			if (diff_time >= 0.5f && diff_time <= 1.0f) {
				float opacity = (diff_time - 0.5f) / 0.5f;
				render_text("You can use Arrow Key to move your duck", 180, 100, 0.8f, vec4(1.0f, 1.0f, 1.0f, opacity));
			}
			else if (diff_time > 1.0f && diff_time <= 2.0f) {
				render_text("You can use Arrow Key to move your duck", 180, 100, 0.8f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
			} 
			else if (diff_time > 2.0f && diff_time <= 2.5f) {
				float opacity = (2.5f - diff_time) / 0.5f;
				render_text("You can use Arrow Key to move your duck", 180, 100, 0.8f, vec4(1.0f, 1.0f, 1.0f, opacity));
			} 
			else if (diff_time > 2.5f && diff_time <= 3.0f) {
				float opacity = (diff_time - 2.5f) / 0.5f;
				render_text("If you press space key, duck will cry", 200, 100, 0.8f, vec4(1.0f, 1.0f, 1.0f, opacity));
			}
			else if (!to_next_scene && diff_time > 3.0f && diff_time <= 3.8f) {
				render_text("If you press space key, duck will cry", 200, 100, 0.8f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
			} 
			else if (!to_next_scene && diff_time > 3.8f) {
				render_text("If you press space key, duck will cry", 200, 100, 0.8f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
				float opacity = sinf((diff_time - 3.8f) * PI);
				if (opacity < 0) opacity = -opacity;
				render_text("Press space key to start", 380, 150, 0.6f, vec4(1.0f, 1.0f, 1.0f, opacity));
			}
			if (to_next_scene && diff_time > 3.0f) {
				float opacity = (to_next_time-0.2f - now_time + to_next_start) / (to_next_time-0.2f);
				render_text("If you press space key, duck will cry", 200, 100, 0.8f, vec4(1.0f, 1.0f, 1.0f, opacity));
			}
		}
		
	}
	else if (scene_number == MINIMAP) {
		if (vertex_char_buffer)		glBindBuffer(GL_ARRAY_BUFFER, vertex_char_buffer);
		if (index_char_buffer)		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_char_buffer);
		cg_bind_vertex_attributes(program);
		character_lobby.update_moving_lobby(character_move_bool[2], character_move_bool[3], character_move_bool[0], character_move_bool[1], 10.0f * (now_time - last_time), character_lobby.radius * 4.0f);
		GLint uloc;
		uloc = glGetUniformLocation(program, "b_solid_color");		if (uloc > -1) glUniform1i(uloc, false);
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, character_lobby.model_matrix);
		glDrawElements(GL_TRIANGLES, char_indices.size(), GL_UNSIGNED_INT, nullptr);
	}
	else if (scene_number == STAGE_NUMBER) {
		GLuint uloc;

		if (vertex_buffer)	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		if (index_buffer)	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);

		cg_bind_vertex_attributes(program);

		background.update(cam.eye);

		uloc = glGetUniformLocation(program, "solid_color");		if (uloc > -1) glUniform4fv(uloc, 1, background.color);	// pointer version
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, background.model_matrix);

		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);

		float diff_time = now_time - stage_start_time;
		std::string str = "Stage 1";
		if (stage == 2) str[6] = '2';
		if (stage == 3) str[6] = '3';
		if (diff_time <= 0.5f) {}
		else if (diff_time <= 1.0f) {
			float opacity = diff_time / 1.0f;
			float size = diff_time / 1.0f * 2.0f;
			GLuint text_x = GLuint( (800.0f + 440.0f)/2 - (800.0f - 440.0f) / 2 * diff_time / 1.0f);
			GLuint text_y = GLuint( (415.0f + 480.0f)/2 + (480.0f - 415.0f) / 2 * diff_time / 1.0f);
			render_text(str, text_x, text_y, size, vec4(1.0f, 1.0f, 1.0f, diff_time));
		}
		else if (diff_time <= 1.7f) {
			render_text(str, 440, 480, 2.0f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
		}
		else if (diff_time < 2.2f) {
			float opacity = (2.2f - diff_time) / 0.5f;
			float size = (2.2f - diff_time) / 0.5f * 2.0f;
			GLuint text_x = GLuint((800.0f + 440.0f) / 2 + (800.0f - 440.0f) / 2 * (diff_time- 2.2f) / 0.5f);
			GLuint text_y = GLuint((415.0f + 480.0f) / 2 + (415.0f - 480.0f) / 2 * (diff_time - 2.2f) / 0.5f);
			render_text(str, text_x, text_y, size, vec4(1.0f, 1.0f, 1.0f, diff_time));
		}
		else if (diff_time >= 2.5f) {
			scene_number = GAME_SCENE;
			stage_cam_moving = true;
			stage_cam_start_time = float(glfwGetTime());
			character.set_location(stage, cube_distance[stage - 1], char_size);
			cam = camera();
		}
	}
	else if (scene_number == STAGE_COMPLETE) {
		GLuint uloc;

		if (vertex_buffer)	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		if (index_buffer)	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);

		cg_bind_vertex_attributes(program);

		background.update(cam.eye);

		uloc = glGetUniformLocation(program, "solid_color");		if (uloc > -1) glUniform4fv(uloc, 1, background.color);	// pointer version
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, background.model_matrix);

		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);

		float diff_time = now_time - complete_start_time;
		std::string str = "Complete";
		if (diff_time <= 0.5f) {}
		else if (diff_time <= 1.0f) {
			float opacity = diff_time / 1.0f;
			float size = diff_time / 1.0f * 2.0f;
			GLuint text_x = GLuint( (800.0f + 440.0f)/2 - (800.0f - 440.0f) / 2 * diff_time / 1.0f);
			GLuint text_y = GLuint( (415.0f + 480.0f)/2 + (480.0f - 415.0f) / 2 * diff_time / 1.0f);
			render_text(str, text_x, text_y, size, vec4(1.0f, 1.0f, 1.0f, diff_time));
		}
		else if (diff_time <= 1.7f) {
			render_text(str, 440, 480, 2.0f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
		}
		else if (diff_time < 2.2f) {
			float opacity = (2.2f - diff_time) / 0.5f;
			float size = (2.2f - diff_time) / 0.5f * 2.0f;
			GLuint text_x = GLuint((800.0f + 440.0f) / 2 + (800.0f - 440.0f) / 2 * (diff_time- 2.2f) / 0.5f);
			GLuint text_y = GLuint((415.0f + 480.0f) / 2 + (415.0f - 480.0f) / 2 * (diff_time - 2.2f) / 0.5f);
			render_text(str, text_x, text_y, size, vec4(1.0f, 1.0f, 1.0f, diff_time));
		}
		else if (diff_time >= 2.5f) {
			scene_number = STAGE_NUMBER;
			stage_start_time = float(glfwGetTime());
		}
	}
	else if (scene_number == FINISH_SCENE) {
		cam.eye = vec3(100, 0, 0);
		cam.view_matrix = mat4::look_at(cam.eye, cam.at, cam.up);
		if ((character_move_bool[0] || character_move_bool[1] || character_move_bool[2] || character_move_bool[3]) && !moving) {
			moving = true;
			moving_start_time = float(glfwGetTime());
		} if (!(character_move_bool[0] || character_move_bool[1] || character_move_bool[2] || character_move_bool[3])) {
			moving = false;
		}
		if ((character_move_bool[0] || character_move_bool[1] || character_move_bool[2] || character_move_bool[3])) {
			indic++;
			if (dash) {
				if (vertex_char_moving_buffer[indic / 5 % 4])	glBindBuffer(GL_ARRAY_BUFFER, vertex_char_moving_buffer[indic / 5 % 4]);
				if (index_char_moving_buffer[indic / 5 % 4])		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_char_moving_buffer[indic / 5 % 4]);
			}
			else {
				if (vertex_char_moving_buffer[indic / 10 % 4])	glBindBuffer(GL_ARRAY_BUFFER, vertex_char_moving_buffer[indic / 10 % 4]);
				if (index_char_moving_buffer[indic / 10 % 4])		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_char_moving_buffer[indic / 10 % 4]);
			}
			
		}
		else {
			if (vertex_char_buffer)		glBindBuffer(GL_ARRAY_BUFFER, vertex_char_buffer);
			if (index_char_buffer)		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_char_buffer);
		}
		cg_bind_vertex_attributes(program);
		if(dash) character_lobby.update_moving_lobby(character_move_bool[2], character_move_bool[3], character_move_bool[0], character_move_bool[1], 20.0f * (now_time - last_time), character_lobby.radius * 4.0f);
		else character_lobby.update_moving_lobby(character_move_bool[2], character_move_bool[3], character_move_bool[0], character_move_bool[1], 10.0f * (now_time - last_time), character_lobby.radius * 4.0f);
		

		GLint uloc;
		uloc = glGetUniformLocation(program, "b_solid_color");		if (uloc > -1) glUniform1i(uloc, false);
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, character_lobby.model_matrix);

		glDrawElements(GL_TRIANGLES, char_indices.size(), GL_UNSIGNED_INT, nullptr);

		if (vertex_buffer)	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		if (index_buffer)	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
		cg_bind_vertex_attributes(program);

		uloc = glGetUniformLocation(program, "b_solid_color");		if (uloc > -1) glUniform1i(uloc, true);	// pointer version
		uloc = glGetUniformLocation(program, "solid_color");		if (uloc > -1) glUniform4fv(uloc, 1, lobby_cube.color);	// pointer version
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, lobby_cube.model_matrix);

		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);

		background.update(cam.eye);

		uloc = glGetUniformLocation(program, "solid_color");		if (uloc > -1) glUniform4fv(uloc, 1, background.color);	// pointer version
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, background.model_matrix);

		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
	}
	// swap front and back buffers, and display to screen
	glfwSwapBuffers( window );
}

void reshape( GLFWwindow* window, int width, int height )
{
	// set current viewport in pixels (win_x, win_y, win_width, win_height)
	// viewport: the window area that are affected by rendering 
	window_size = ivec2(width,height);
	glViewport( 0, 0, width, height );
}

void print_help()
{
	printf( "[help]\n" );
	printf( "- press ESC or 'q' to terminate the program\n" );
	printf( "- press F1 or 'h' to see help\n" );
	printf( "- press 'w' to toggle wireframe\n");
	printf( "- press Home to reset camera\n" );
	printf( "- press Pause to pause the simulation");
	printf( "\n" );
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

void update_sphere_vertex_buffer(const std::vector<vertex>& vertices) {
	uint N = 72;
	uint M = 36;

	// clear and create new buffers
	if (vertex_sphere_buffer)	glDeleteBuffers(1, &vertex_sphere_buffer);	vertex_sphere_buffer = 0;
	if (index_sphere_buffer)	glDeleteBuffers(1, &index_sphere_buffer);	index_sphere_buffer = 0;

	// check exceptions
	if (vertices.empty()) { printf("[error] vertices is empty.\n"); return; }

	// create buffers
	for (uint k = 0; k < M; k++) {
		if (k == 0) {
			for (uint i = 1; i <= N; i++) {
				sphere_indices.push_back(0);
				sphere_indices.push_back(i);
				sphere_indices.push_back(i + 1);
			}
		}
		else if (k == M - 1) {
			for (uint i = 1; i <= N; i++) {
				sphere_indices.push_back((M - 2) * (N + 1) + i);
				sphere_indices.push_back((M - 1) * (N + 1) + 1);
				sphere_indices.push_back((M - 2) * (N + 1) + i + 1);
			}
		}
		else {
			for (uint i = 1; i < N + 1; i++) {
				sphere_indices.push_back((N + 1) * (k - 1) + i);
				sphere_indices.push_back((N + 1) * k + i);
				sphere_indices.push_back((N + 1) * k + i + 1);

				sphere_indices.push_back((N + 1) * (k - 1) + i);
				sphere_indices.push_back((N + 1) * k + i + 1);
				sphere_indices.push_back((N + 1) * (k - 1) + i + 1);
			}
		}
	}

	// generation of vertex buffer: use vertices as it is
	glGenBuffers(1, &vertex_sphere_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_sphere_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// geneation of index buffer
	glGenBuffers(1, &index_sphere_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_sphere_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * sphere_indices.size(), &sphere_indices[0], GL_STATIC_DRAW);
}

void keyboard( GLFWwindow* window, int key, int scancode, int action, int mods )
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
	else if (key == GLFW_KEY_LEFT_SHIFT) {
		if (action == GLFW_PRESS) {
			dash = true;
		} 
		else if (action == GLFW_RELEASE) {
			dash = false;
		}
	}
	else if(action==GLFW_PRESS)
	{
		if(key==GLFW_KEY_ESCAPE||key==GLFW_KEY_Q)	glfwSetWindowShouldClose( window, GL_TRUE );
		else if(key==GLFW_KEY_H||key==GLFW_KEY_F1)	print_help();
		else if (key == GLFW_KEY_HOME) {
			cam.eye = vec3(100, 0, 0);
			cam.view_matrix = mat4::look_at(cam.eye, cam.at, cam.up);
		}
		else if (key == GLFW_KEY_PAUSE) {
			is_pause = !is_pause;
		}
		else if (key == GLFW_KEY_O) {
			cam.eye = vec3(100, 0, 0);
			cam.view_matrix = mat4::look_at(cam.eye, cam.at, cam.up);
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
		else if (key == GLFW_KEY_SPACE) {
			engine->play2D(mp3_src, false);
			//engine->play2D(mp3_back_src, true);
			if (scene_number == DUCK_SCENE && !cam_closer_moving && !cam_moving) {
				to_next_scene = true;
				to_next_start = float(glfwGetTime());
			}
		} 
		else if (key == GLFW_KEY_Y) {
			stage_start_time = float(glfwGetTime());
		}
	}
}

void mouse( GLFWwindow* window, int button, int action, int mods )
{
	if(button==GLFW_MOUSE_BUTTON_LEFT)
	{
		dvec2 pos; glfwGetCursorPos(window,&pos.x,&pos.y);
		vec2 npos = vec2( float(pos.x)/float(window_size.x-1), float(pos.y)/float(window_size.y-1) );
		if (action == GLFW_PRESS) {
			tb.begin(cam.view_matrix, npos.x, npos.y, cam.at);
		}
		else if (action == GLFW_RELEASE) {
			tb.end();
		}
	}
}

void motion( GLFWwindow* window, double x, double y )
{
	if(!tb.is_tracking()) return;
	
	vec2 npos = vec2(float(x) / float(window_size.x - 1), float(y) / float(window_size.y - 1));
	//cam.view_matrix = tb.update(npos.x, npos.y);
	minimap_rotation_matrix = tb.update(npos.x, npos.y);
}

bool user_init()
{
	// log hotkeys
	print_help();

	// init GL states
	glLineWidth(1.0f);
	glClearColor( 39/255.0f, 40/255.0f, 34/255.0f, 1.0f );	// set clear color
	glEnable( GL_CULL_FACE );								// turn on backface culling
	glEnable( GL_DEPTH_TEST );								// turn on depth tests
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

	sphere_vertices = std::move(create_sphere_vertices());
	update_sphere_vertex_buffer(sphere_vertices);

	engine = irrklang::createIrrKlangDevice();
	if (!engine) return false;

	//add sound source from the sound file
	mp3_src = engine->addSoundSourceFromFile(mp3_path);

	mp3_src->setDefaultVolume(0.5f);

	mp3_back_src = engine->addSoundSourceFromFile(mp3_back_path);
	mp3_back_src -> setDefaultVolume(0.5f);

	engine->play2D(mp3_back_src, true);

	// setup freetype
	text_init();

	return true;
}

void user_finalize()
{
}

int main( int argc, char* argv[] )
{
	// initialization
	if(!glfwInit()){ printf( "[error] failed in glfwInit()\n" ); return 1; }

	// create window and initialize OpenGL extensions
	if(!(window = cg_create_window( window_name, window_size.x, window_size.y ))){ glfwTerminate(); return 1; }
	if(!cg_init_extensions( window )){ glfwTerminate(); return 1; }	// version and extensions
	// initializations and validations
	if(!(program=cg_create_program( vert_shader_path, frag_shader_path ))){ glfwTerminate(); return 1; }	// create and compile shaders/program
	if(!user_init()){ printf( "Failed to user_init()\n" ); glfwTerminate(); return 1; }					// user initialization
	// register event callbacks
	glfwSetWindowSizeCallback( window, reshape );	// callback for window resizing events
    glfwSetKeyCallback( window, keyboard );			// callback for keyboard events
	glfwSetMouseButtonCallback( window, mouse );	// callback for mouse click inputs
	glfwSetCursorPosCallback( window, motion );		// callback for mouse movement

	// enters rendering/event loop
	for( frame=0; !glfwWindowShouldClose(window); frame++ )
	{
		glfwPollEvents();	// polling and processing of events
		update();			// per-frame update
		render();			// per-frame render
	}

	// normal termination
	user_finalize();
	cg_destroy_window(window);

	return 0;
}