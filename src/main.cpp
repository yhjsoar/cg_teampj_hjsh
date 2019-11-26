#include "cgmath.h"			// slee's simple math library
#include "cgut.h"			// slee's OpenGL utility
#include "trackball.h"		// virtual trackball
#include "cube.h"
#include <math.h>

//*******************************************************************
// global constants
static const char*	window_name = "cgbase - trackball";
static const char*	vert_shader_path = "../bin/shaders/trackball.vert";
static const char*	frag_shader_path = "../bin/shaders/trackball.frag";
//static const char*	mesh_vertex_path = "../bin/mesh/dragon.vertex.bin";
//static const char*	mesh_index_path	= "../bin/mesh/dragon.index.bin";
uint				NUM_TESS = 72;
uint				NUM_THET = 36;

//*******************************************************************
// common structures
struct camera
{
	vec3	eye = vec3( 100, 0, 0 );
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
ivec2		window_size = ivec2( 720, 480 );	// initial window size

//*******************************************************************
// OpenGL objects
GLuint	program			= 0;	// ID holder for GPU program
GLuint	vertex_buffer[2] = { 0, 0 };
GLuint	index_buffer[2] = { 0, 0 };

//GLuint	vertex_buffer_no_left = 0;
//GLuint	index_buffer_no_left = 0;

//*******************************************************************
// global variables
int		frame = 0;				// index of rendering frames
bool	b_wireframe = false;	// this is the default
float	rotating_theta = 0.0f;
int		num_solid_color = 0;			// use circle's color?
bool	shift_on = false;
bool	is_panning = false;
bool	is_pause = false;
float	first_time = 0.0f;
float	save_y = 1.0f;
float	save_x = 1.0f;
vec3	save_eye;
vec3	save_at;
vec3	save_u;
vec3	save_v;
vec3	save_n;
auto	map = std::move(create_map());
std::vector<uint> indices[2];
//std::vector<uint> indices_no_left;
//auto	circles = std::move(create_circles());
struct { bool add = false, sub = false; operator bool() const { return add || sub; } } b; // flags of keys for smooth changes

//*************************************
// holder of vertices and indices of a unit circle
std::vector<vertex>	unit_cube_vertices;
std::vector<vertex> unit_noleft_vertices;

//*******************************************************************
// scene objects
//mesh*		pMesh = nullptr;
camera		cam;
trackball	tb;

//*******************************************************************
void update()
{
	// update projection matrix
	cam.aspect_ratio = window_size.x/float(window_size.y);
	cam.projection_matrix = mat4::perspective( cam.fovy, cam.aspect_ratio, cam.dnear, cam.dfar );

	GLint uloc;
	uloc = glGetUniformLocation( program, "view_matrix" );			if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, cam.view_matrix );
	uloc = glGetUniformLocation( program, "projection_matrix" );	if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, cam.projection_matrix );
}

void render()
{
	// clear screen (with background color) and clear depth buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// notify GL that we use our own program and buffers
	glUseProgram( program );
	if (vertex_buffer[0])	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer[0]);
	if (index_buffer[0])	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer[0]);

	// bind vertex attributes to your shader program
	cg_bind_vertex_attributes( program );
	float t = float(glfwGetTime());

	int tmp = 0;
	for (auto& c : map) {
		if (tmp == 5) {
			glUseProgram(program);
			if (vertex_buffer[1])	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer[1]);
			if (index_buffer[1])	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer[1]);

			cg_bind_vertex_attributes(program);
		}
		c.update(t);
		// update per-circle uniforms
		GLint uloc;
		uloc = glGetUniformLocation(program, "solid_color");		if (uloc > -1) glUniform4fv(uloc, 1, c.color);	// pointer version
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, c.model_matrix);

		if (tmp >= 5) {
			glDrawElements(GL_TRIANGLES, indices[1].size(), GL_UNSIGNED_INT, nullptr);
		}
		else glDrawElements(GL_TRIANGLES, indices[0].size(), GL_UNSIGNED_INT, nullptr);
		tmp++;
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

std::vector<vertex> create_cube_vertices() {
	std::vector<vertex> v;
	float x[] = { 0.5, 0.5, -0.5, -0.5 };
	float y[] = { -0.5, 0.5, 0.5, -0.5 };
	int i;
	//up
	for (i = 0; i < 4; i++) {
		v.push_back({
			vec3(x[i], y[i], 0.5),
			vec3(0, 0, 1),
			vec2(1, -1)
			});
	}
	//front
	float y2[] = { -0.5, 0.5, 0.5, -0.5 };
	float z2[] = { -0.5, -0.5, 0.5, 0.5 };
	for (i = 0; i < 4; i++) {
		v.push_back({
			vec3(0.5, y2[i], z2[i]),
			vec3(1, 0, 0),
			vec2(1, 1)
			});
	}
	//down
	for (i = 0; i < 4; i++) {
		v.push_back({
			vec3(z2[i], y2[i], -0.5),
			vec3(0, 0, -1),
			vec2(-1, 1)
			});
	}
	//back
	for (i = 0; i < 4; i++) {
		v.push_back({
			vec3(-0.5, y2[i], -z2[i]),
			vec3(-1, 0, 0),
			vec2(-1, -1)
			});
	}
	//left
	float x3[] = { 0.5, -0.5, -0.5, 0.5 };
	float z3[] = { -0.5, -0.5, 0.5, 0.5 };
	for (i = 0; i < 4; i++) {
		v.push_back({
			vec3(x3[i], 0.5, z3[i]),
			vec3(0, 1, 0),
			vec2(1, 0)
			});
	}
	//right
	for (i = 0; i < 4; i++) {
		v.push_back({
			vec3(-x3[i], -0.5, z3[i]),
			vec3(0, -1, 0),
			vec2(0, 1)
			});
	}
	return v;
}

std::vector<vertex> create_cube_vertices_no_left() {
	std::vector<vertex> v;
	float x[] = { 0.5, 0.5, -0.5, -0.5 };
	float y[] = { -0.5, 0.5, 0.5, -0.5 };
	int i;
	//up
	for (i = 0; i < 4; i++) {
		v.push_back({
			vec3(x[i], y[i], 0.5),
			vec3(0, 0, 1),
			vec2(1, -1)
			});
	}
	//front
	float y2[] = { -0.5, 0.5, 0.5, -0.5 };
	float z2[] = { -0.5, -0.5, 0.5, 0.5 };
	for (i = 0; i < 4; i++) {
		v.push_back({
			vec3(0.5, y2[i], z2[i]),
			vec3(1, 0, 0),
			vec2(1, 1)
			});
	}
	//down
	for (i = 0; i < 4; i++) {
		v.push_back({
			vec3(z2[i], y2[i], -0.5),
			vec3(0, 0, -1),
			vec2(-1, 1)
			});
	}
	//back
	for (i = 0; i < 4; i++) {
		v.push_back({
			vec3(-0.5, y2[i], -z2[i]),
			vec3(-1, 0, 0),
			vec2(-1, -1)
			});
	}
	//left
	float x3[] = { 0.5, -0.5, -0.5, 0.5 };
	float z3[] = { -0.5, -0.5, 0.5, 0.5 };

	//right
	for (i = 0; i < 4; i++) {
		v.push_back({
			vec3(-x3[i], -0.5, z3[i]),
			vec3(0, -1, 0),
			vec2(0, 1)
			});
	}
	return v;
}

void update_vertex_buffer(const std::vector<vertex>& vertices, int what_buffer) {
	// clear and create new buffers
	if (vertex_buffer[what_buffer])	glDeleteBuffers(1, &vertex_buffer[what_buffer]);	vertex_buffer[what_buffer] = 0;
	if (index_buffer[what_buffer])	glDeleteBuffers(1, &index_buffer[what_buffer]);	index_buffer[what_buffer] = 0;

	// check exceptions
	if (vertices.empty()) { printf("[error] vertices is empty.\n"); return; }

	// create buffers
	for (uint i = 0; i < 6; i++) {
		//printf("%f %f %f\n", vertices.at(4 * i).pos.x, vertices.at(4 * i).pos.y, vertices.at(4 * i).pos.z);
		indices[what_buffer].push_back(4 * i);
		indices[what_buffer].push_back(4 * i + 1);
		indices[what_buffer].push_back(4 * i + 2);

		indices[what_buffer].push_back(4 * i);
		indices[what_buffer].push_back(4 * i + 2);
		indices[what_buffer].push_back(4 * i + 3);
	}
	// generation of vertex buffer: use vertices as it is
	glGenBuffers(1, &vertex_buffer[what_buffer]);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer[what_buffer]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// geneation of index buffer
	glGenBuffers(1, &index_buffer[what_buffer]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer[what_buffer]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices[what_buffer].size(), &indices[what_buffer][0], GL_STATIC_DRAW);
}

void keyboard( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) {
		if (action == GLFW_PRESS)	shift_on = true;
		if (action == GLFW_RELEASE)	shift_on = false;
	}
	else if (key == GLFW_KEY_LEFT_CONTROL) {
		if (action == GLFW_PRESS)	is_panning = true;
		if (action == GLFW_RELEASE)	is_panning = false;
	}
	else if(action==GLFW_PRESS)
	{
		if(key==GLFW_KEY_ESCAPE||key==GLFW_KEY_Q)	glfwSetWindowShouldClose( window, GL_TRUE );
		else if(key==GLFW_KEY_H||key==GLFW_KEY_F1)	print_help();
		else if (key == GLFW_KEY_HOME) {
			cam = camera();
		}
		else if (key == GLFW_KEY_W)
		{
			b_wireframe = !b_wireframe;
			glPolygonMode(GL_FRONT_AND_BACK, b_wireframe ? GL_LINE : GL_FILL);
			printf("> using %s mode\n", b_wireframe ? "wireframe" : "solid");
		}
		else if (key == GLFW_KEY_PAUSE) {
			is_pause = !is_pause;
			first_time = float(glfwGetTime());
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
			mat4 uvn = {
						cam.view_matrix[0], cam.view_matrix[1], cam.view_matrix[2],0,
						cam.view_matrix[4], cam.view_matrix[5], cam.view_matrix[6], 0,
						cam.view_matrix[8], cam.view_matrix[9], cam.view_matrix[10], 0,
						0, 0, 0, 1
			};
			mat4 eye_mat = uvn.inverse() * cam.view_matrix;
			cam.eye = { -eye_mat[3], -eye_mat[7], -eye_mat[11] };
			save_n = normalize(cam.eye - cam.at);
			save_u = { cam.view_matrix[0], cam.view_matrix[1], cam.view_matrix[2] };
			save_v = normalize(cross(save_n, save_u));
			cam.up = normalize(cross(save_n, save_u));
			save_eye = cam.eye;
			save_at = cam.at;
			tb.begin(cam.view_matrix, npos.x, npos.y, cam.at);
			save_y = (float)npos.y;
			save_x = (float)npos.x;
		}
		else if (action == GLFW_RELEASE) {
			tb.end();
			save_y = (float)npos.y;
			save_x = (float)npos.x;
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		dvec2 pos; glfwGetCursorPos(window, &pos.x, &pos.y);
		vec2 npos = vec2(float(pos.x) / float(window_size.x - 1), float(pos.y) / float(window_size.y - 1));
		if (action == GLFW_PRESS) {
			shift_on = true;
			mat4 uvn = {
						cam.view_matrix[0], cam.view_matrix[1], cam.view_matrix[2],0,
						cam.view_matrix[4], cam.view_matrix[5], cam.view_matrix[6], 0,
						cam.view_matrix[8], cam.view_matrix[9], cam.view_matrix[10], 0,
						0, 0, 0, 1
			};
			mat4 eye_mat = uvn.inverse() * cam.view_matrix;
			cam.eye = { -eye_mat[3], -eye_mat[7], -eye_mat[11] };
			save_n = normalize(cam.eye - cam.at);
			save_u = { cam.view_matrix[0], cam.view_matrix[1], cam.view_matrix[2] };
			save_v = normalize(cross(save_n, save_u));
			cam.up = normalize(cross(save_n, save_u));
			save_eye = cam.eye;
			save_at = cam.at;
			tb.begin(cam.view_matrix, npos.x, npos.y, cam.at);
			save_y = (float)npos.y;
			save_x = (float)npos.x;
		}
		else if (action == GLFW_RELEASE) {
			shift_on = false;
			tb.end();
			save_y = (float)npos.y;
			save_x = (float)npos.x;
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
		dvec2 pos; glfwGetCursorPos(window, &pos.x, &pos.y);
		vec2 npos = vec2(float(pos.x) / float(window_size.x - 1), float(pos.y) / float(window_size.y - 1));
		if (action == GLFW_PRESS) {
			is_panning = true;
			save_y = (float)npos.y;
			save_x = (float)npos.x;
			mat4 uvn = {
					cam.view_matrix[0], cam.view_matrix[1], cam.view_matrix[2],0,
					cam.view_matrix[4], cam.view_matrix[5], cam.view_matrix[6], 0,
					cam.view_matrix[8], cam.view_matrix[9], cam.view_matrix[10], 0,
					0, 0, 0, 1
			};
			mat4 eye_mat = uvn.inverse() * cam.view_matrix;
			cam.eye = { -eye_mat[3], -eye_mat[7], -eye_mat[11] };
			save_n = normalize(cam.eye - cam.at);
			save_u = { cam.view_matrix[0], cam.view_matrix[1], cam.view_matrix[2] };
			save_v = normalize(cross(save_n, save_u));
			cam.up = normalize(cross(save_n, save_u));

			save_eye = cam.eye;
			save_at = cam.at;
			tb.begin(cam.view_matrix, npos.x, npos.y, cam.at);
		}
		else if (action == GLFW_RELEASE) {
			is_panning = false;
			tb.end();
		}
	}
}

void motion( GLFWwindow* window, double x, double y )
{
	if(!tb.is_tracking()) return;
	if (shift_on) {
		float diff = 100.0f * (float(y) / float(window_size.y - 1) - save_y);
		vec3 next_eye = {	save_eye.x - diff * save_n.x,
							save_eye.y - diff * save_n.y,
							save_eye.z - diff * save_n.z
		};
		float distance = sqrtf(powf(next_eye.x - cam.at.x, 2) + powf(next_eye.y - cam.at.y, 2) + powf(next_eye.z - cam.at.z, 2));
		if (distance <= 2.0f || normalize(next_eye - cam.at) != normalize(save_eye - cam.at)) return;
		cam.eye = next_eye;
		cam.view_matrix = mat4::look_at(cam.eye, cam.at, cam.up);
	}
	else if (is_panning) {
		vec2 npos = vec2(float(x) / float(window_size.x - 1), float(y) / float(window_size.y - 1));
		float diff_x = - (save_x - npos.x);
		float diff_y = save_y - npos.y;
		cam.eye = { save_eye.x - 100.0f * save_v.x * diff_y - 100.0f * save_u.x * diff_x,
					save_eye.y - 100.0f * save_v.y * diff_y - 100.0f * save_u.y * diff_x,
					save_eye.z - 100.0f * save_v.z * diff_y - 100.0f * save_u.z * diff_x };
		cam.at = {	save_at.x - 100.0f * save_v.x * diff_y - 100.0f * save_u.x * diff_x,
					save_at.y - 100.0f * save_v.y * diff_y - 100.0f * save_u.y * diff_x,
					save_at.z - 100.0f * save_v.z * diff_y - 100.0f * save_u.z * diff_x };
		cam.view_matrix = mat4::look_at(cam.eye, cam.at, cam.up);
	}
	else {
		vec2 npos = vec2(float(x) / float(window_size.x - 1), float(y) / float(window_size.y - 1));
		cam.view_matrix = tb.update(npos.x, npos.y);
	}
	
}

bool user_init()
{
	// log hotkeys
	print_help();
	first_time = float(glfwGetTime());
	// init GL states
	glLineWidth(1.0f);
	glClearColor( 39/255.0f, 40/255.0f, 34/255.0f, 1.0f );	// set clear color
	glEnable( GL_CULL_FACE );								// turn on backface culling
	glEnable( GL_DEPTH_TEST );								// turn on depth tests
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// load the mesh
	unit_cube_vertices = std::move(create_cube_vertices());
	unit_noleft_vertices = std::move(create_cube_vertices_no_left());

	update_vertex_buffer(unit_cube_vertices, 0);
	update_vertex_buffer(unit_noleft_vertices, 1);
	//pMesh = cg_load_mesh( mesh_vertex_path, mesh_index_path );
	//if(pMesh==nullptr){ printf( "Unable to load mesh\n" ); return false; }

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
