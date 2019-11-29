#include "cgmath.h"			// slee's simple math library
#include "cgut.h"			// slee's OpenGL utility
#include "trackball.h"		// virtual trackball
#include "cube.h"
#include "index.h"
#include "duck.h"
#include <math.h>

/*
//*******************************************************************
// global constants
static const char*	window_name = "cgbase - trackball";
static const char*	vert_shader_path = "../bin/shaders/trackball.vert";
static const char*	frag_shader_path = "../bin/shaders/trackball.frag";

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
ivec2		window_size = ivec2( 1200, 900 );	// initial window size

//*******************************************************************
// OpenGL objects
GLuint	program			= 0;	// ID holder for GPU program
GLuint	vertex_buffer[63] = { 0,  };
GLuint	index_buffer[63] = { 0, };
GLuint	vertex_char_buffer = 0;
GLuint	index_char_buffer = 0;

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
int		command = 0;
float	start_time = 0.0f;
float	passed_time = 0.0f;
int		tick = 30;
int		now_tick = 0;
int		double_rotate = 0;
bool	rotate = false;
float	tick_time = 0.006f;
bool	key_lock = false;
float	cube_distance = 15.0f;
int		map_size = 2;
float	char_size = 2.0f;
bool	move_left = false;
bool	move_right = false;
bool	move_up = false;
bool	move_down = false;
//bool	cube_rotate_done = true;
bool	gravity_on = false;
float	last_time;
float	now_time = float(glfwGetTime());
auto	map = std::move(create_map(cube_distance));
duck_t	character = create_character(map_size, cube_distance, char_size);
vec3	char_center = character.center;
std::vector<uint> indices[63];
std::vector<uint> char_indices;
struct { bool add = false, sub = false; operator bool() const { return add || sub; } } b; // flags of keys for smooth changes

//*************************************
// holder of vertices and indices of a unit circle
std::vector<vertex>	unit_cube_vertices[63];
std::vector<vertex>	duck_vertices;

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
	if (vertex_buffer[33])	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer[33]);
	if (index_buffer[33])	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer[33]);

	// bind vertex attributes to your shader program
	cg_bind_vertex_attributes( program );
	last_time = now_time;
	now_time = float(glfwGetTime());
	passed_time = now_time - start_time;

	if (key_lock) {
		if (tick_time * now_tick <= passed_time && tick_time * (now_tick + 1) >= passed_time) {
			now_tick++;
			double_rotate = 1;
			rotate = true;
		}
		else if ((now_tick + 1)*tick_time < passed_time) {
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

	if (vertex_char_buffer)		glBindBuffer(GL_ARRAY_BUFFER, vertex_char_buffer);
	if (index_char_buffer)		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_char_buffer);
	cg_bind_vertex_attributes(program);
	if (rotate) character.update_rotate(command, rotate, tick, double_rotate, false, cube_distance);
	else {
		if (gravity_on) {
			gravity_on = character.check_if_on_floor(map, cube_distance);
			start_time = now_time;
		}
		if (!gravity_on) {
			key_lock = true;
			printf("passed_time: %f\n", passed_time);
			gravity_on = character.update_gravity(map, passed_time/3, cube_distance);
			if (gravity_on) key_lock = false;
		}
		else {
			character.update_moving(move_up, move_down, move_left, move_right, 10.0f * (now_time - last_time), map);
		}
	}
	char_center = character.center;
	GLint uloc;
	uloc = glGetUniformLocation(program, "b_solid_color");		if (uloc > -1) glUniform1i(uloc, false);
	uloc = glGetUniformLocation(program, "solid_color");		if (uloc > -1) glUniform4fv(uloc, 1, character.color);	// pointer version
	uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, character.model_matrix);

	glDrawElements(GL_TRIANGLES, char_indices.size(), GL_UNSIGNED_INT, nullptr);

	for (auto& c : map) {
		int block = c.block;
		//if (vertex_buffer[block])	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer[block]);
		//if (index_buffer[block])	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer[block]);

		if (vertex_buffer[62])	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer[62]);
		if (index_buffer[62])	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer[62]);

		cg_bind_vertex_attributes(program);
		
		if (rotate) {
			c.update(command, tick, rotate, double_rotate, char_center, char_size, cube_distance);
		}
		else {
			c.update(command, tick, rotate, double_rotate, char_center, char_size, cube_distance);
		}
		
		//if (c.type == 0) continue;
		if (c.color.w == 0.0f) continue;
		// update per-circle uniforms
		GLint uloc;
		uloc = glGetUniformLocation(program, "b_solid_color");		if (uloc > -1) glUniform1i(uloc, true);	// pointer version
		uloc = glGetUniformLocation(program, "solid_color");		if (uloc > -1) glUniform4fv(uloc, 1, c.color);	// pointer version
		uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, c.model_matrix);

		glDrawElements(GL_TRIANGLES, indices[block].size(), GL_UNSIGNED_INT, nullptr);

	}
	rotate = false;
	if (key_lock) {
		if (now_tick == tick) {
				command = 0;
				key_lock = false;
			//}
			
		}
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



void update_vertex_buffer(const std::vector<vertex>& vertices, int what_buffer) {
	// clear and create new buffers
	if (vertex_buffer[what_buffer])	glDeleteBuffers(1, &vertex_buffer[what_buffer]);	vertex_buffer[what_buffer] = 0;
	if (index_buffer[what_buffer])	glDeleteBuffers(1, &index_buffer[what_buffer]);	index_buffer[what_buffer] = 0;

	// check exceptions
	if (vertices.empty()) { printf("[error] vertices is empty.\n"); return; }

	// create buffers
	for (uint i = 0; i < 6; i++) {
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

void keyboard( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	if (key == GLFW_KEY_UP) {
		if (action == GLFW_PRESS) {
			if (!move_down && !key_lock) {
				move_up = true;
			}
		}
		else if (action == GLFW_RELEASE) {
			move_up = false;
		}
	}
	else if (key == GLFW_KEY_LEFT) {
		if (action == GLFW_PRESS) {
			if (!move_right && !key_lock) {
				move_left = true;
			}
		}
		else if (action == GLFW_RELEASE) {
			move_left = false;
		}
	}
	else if (key == GLFW_KEY_RIGHT) {
		if (action == GLFW_PRESS) {
			if (!move_left && !key_lock) {
				move_right = true;
			}
		}
		else if (action == GLFW_RELEASE) {
			move_right = false;
		}
	}
	else if (key == GLFW_KEY_DOWN) {
		if (action == GLFW_PRESS) {
			if (!move_up && !key_lock) {
				move_down = true;
			}
		}
		else if (action == GLFW_RELEASE) {
			move_down = false;
		}
	}
	else if(action==GLFW_PRESS)
	{
		if(key==GLFW_KEY_ESCAPE||key==GLFW_KEY_Q)	glfwSetWindowShouldClose( window, GL_TRUE );
		else if(key==GLFW_KEY_H||key==GLFW_KEY_F1)	print_help();
		else if (key == GLFW_KEY_HOME) {
			cam = camera();
		}
		else if (key == GLFW_KEY_PAUSE) {
			is_pause = !is_pause;
			first_time = float(glfwGetTime());
		}
		else if (key == GLFW_KEY_O) {
			cam = camera();
		}
		else if (key == GLFW_KEY_S) {
			if (!key_lock) {
				key_lock = true;
				///cube_rotate_done = false;
				start_time = float(glfwGetTime());
				passed_time = 0.0f;
				now_tick = 0;
				command = 2;
			}
		}
		else if (key == GLFW_KEY_A) {
			if (!key_lock) {
				key_lock = true;
				//cube_rotate_done = false;
				start_time = float(glfwGetTime());
				passed_time = 0.0f;
				now_tick = 0;
				command = 3;
			}
		}
		else if (key == GLFW_KEY_D) {
			if (!key_lock) {
				key_lock = true;
				//cube_rotate_done = false;
				start_time = float(glfwGetTime());
				passed_time = 0.0f;
				now_tick = 0;
				command = 4;
			}
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
	cam.view_matrix = tb.update(npos.x, npos.y);
	
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
	int isleft, isright, isup, isdown, isfront, isback, tmp;
	for (int i = 1; i < 64; i++) {
		tmp = i;
		isback = tmp % 2;
		tmp = tmp / 2;
		isfront = tmp % 2;
		tmp = tmp / 2;
		isdown = tmp % 2;
		tmp = tmp / 2;
		isup = tmp % 2;
		tmp = tmp / 2;
		isright = tmp % 2;
		isleft = tmp / 2;
		unit_cube_vertices[i-1] = std::move(create_cube_vertices(isleft, isright, isup, isdown, isfront, isback));
		update_vertex_buffer(unit_cube_vertices[i-1], i-1);
	}
	duck_vertices = std::move(create_character_vertices());
	update_character_vertex_buffer(duck_vertices);

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
*/

// dear imgui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <stdio.h>

// About Desktop OpenGL function loaders:
//  Modern desktop OpenGL doesn't have a standard portable header file to load OpenGL function pointers.
//  Helper libraries are often used for this purpose! Here we are supporting a few common ones (gl3w, glew, glad).
//  You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>    // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>    // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>  // Initialize with gladLoadGL()
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main(int, char**)
{
	// Setup window
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;

	// Decide GL+GLSL versions
#if __APPLE__
	// GL 3.2 + GLSL 150
	const char* glsl_version = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

	// Create window with graphics context
	GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
	if (window == NULL)
		return 1;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	// Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
	bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
	bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
	bool err = gladLoadGL() == 0;
#else
	bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
#endif
	if (err)
	{
		fprintf(stderr, "Failed to initialize OpenGL loader!\n");
		return 1;
	}

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'docs/FONTS.txt' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);

	// Our state
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		glfwPollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &show_another_window);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3("clear color", (float*)& clear_color); // Edit 3 floats representing a color

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		// 3. Show another simple window.
		if (show_another_window)
		{
			ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				show_another_window = false;
			ImGui::End();
		}

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
