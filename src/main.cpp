#include "cgmath.h"			// slee's simple math library
#include "cgut.h"			// slee's OpenGL utility
#include "circle.h"			// circle class definition

//*************************************
// global constants
static const char*	window_name = "cgcirc";
static const char*	vert_shader_path = "../bin/shaders/circ.vert";
static const char*	frag_shader_path = "../bin/shaders/circ.frag";
//static const uint	MIN_TESS = 3;		// minimum tessellation factor (down to a triangle)
//static const uint	MAX_TESS = 256;		// maximum tessellation factor (up to 64 triangles)
//uint				NUM_TESS = 4;// initial tessellation factor of the circle as a polygon
uint				latitude = 36;
uint				longitude =72;


//*************************************
// window objects
GLFWwindow*	window = nullptr;
ivec2		window_size = ivec2( 1024, 576 );	// initial window size

//*************************************
// OpenGL objects
GLuint	program			= 0;	// ID holder for GPU program
GLuint	vertex_buffer	= 0;	// ID holder for vertex buffer
GLuint	index_buffer	= 0;	// ID holder for index buffer

//*************************************
// global variables
int		frame = 0;						// index of rendering frames
float	t = 0.0f;						// current simulation parameter
bool	b_solid_color = false;			// use circle's color?
bool	b_index_buffer = true;			// use index buffering?
bool	b_wireframe = false;
int		toggles = 0;
ivec3	toggle = ivec3(1, 0, 0);
auto	circles = std::move(create_circles());
struct { bool yadd = false, ysub = false; operator bool() const { return yadd || ysub; } } yb;
//struct { bool add=false, sub=false; operator bool() const { return add||sub; } } b; // flags of keys for smooth changes

//*************************************
// holder of vertices and indices of a unit circle
std::vector<vertex>	unit_circle_vertices;	// host-side vertices

//*************************************
void update()
{
	// update global simulation parameter
	t = float(glfwGetTime())*0.4f;

	// tricky aspect correction matrix for non-square window
	float aspect = window_size.x/float(window_size.y);
	mat4 aspect_matrix = 
	{
		min(1/aspect,1.0f), 0, 0, 0,
		0, min(aspect,1.0f), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	mat4 view_projection_matrix = {
		0,1,0,0,
		0,0,1,0,
		-1,0,0,1,
		0,0,0,1
	};

	// update common uniform variables in vertex/fragment shaders
	GLint uloc;
	uloc = glGetUniformLocation( program, "b_solid_color" );	if(uloc>-1) glUniform1i( uloc, b_solid_color );
	uloc = glGetUniformLocation( program, "aspect_matrix" );	if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, aspect_matrix );
	uloc = glGetUniformLocation(program,"view_projection_matrix");
	glUniformMatrix4fv(uloc, 1, GL_TRUE, view_projection_matrix);
	// update vertex buffer by the pressed keys
	void update_tess(); // forward declaration
}

void render()
{
	// clear screen (with background color) and clear depth buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// notify GL that we use our own program and buffers
	glUseProgram( program );
	if(vertex_buffer)	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	if(index_buffer)	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);

	// bind vertex attributes to your shader program
	cg_bind_vertex_attributes( program );

	// render two circles: trigger shader program to process vertex data
	for( auto& c : circles )
	{
		// per-circle update
		c.update(t,yb.yadd,yb.ysub);
		
		// update per-circle uniforms
		GLint uloc;

		uloc = glGetUniformLocation(program, "toggle");	if (uloc > -1) glUniform3iv(uloc, 1, toggle);
		uloc = glGetUniformLocation( program, "solid_color" );		if(uloc>-1) glUniform4fv( uloc, 1, c.color );	// pointer version
		uloc = glGetUniformLocation( program, "model_matrix" );		if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, c.model_matrix );

		// per-circle draw calls
		if (b_index_buffer)	//glDrawElements( GL_TRIANGLES, NUM_TESS*6*(NUM_TESS-1), GL_UNSIGNED_INT, nullptr );
			glDrawElements(GL_TRIANGLES, longitude * 3 * 2 * (latitude - 1), GL_UNSIGNED_INT, nullptr);
		//else				glDrawArrays( GL_TRIANGLES, 0, NUM_TESS*2*18 ); // NUM_TESS = N
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
	printf( "- press 'd' to toggle between solid color and texture coordinates\n" );
	//printf( "- press '+/-' to increase/decrease tessellation factor (min=%d, max=%d)\n", MIN_TESS, MAX_TESS );
	printf( "- press 'w' to toggle wireframe\n" );
	printf( "\n" );
}

std::vector<vertex> create_circle_vertices(  uint lon, uint lat )
{
	std::vector<vertex> v;
	for (uint k = 0; k <= lon; k++) {
		float tz = PI * 2.0f * k / float(lon);
		float cz = cos(tz);
		float sz = sin(tz);
		for (uint j = 0; j <= lat; j++) {
			float t = PI * j / float(lat);
			float c = cos(t);
			float s = sin(t);
			
			v.push_back({ vec3(s * cz,s * sz,c),vec3(s*cz,s*sz,c),vec2(tz/(2.0f*PI),1-t/PI) });
			//printf("%f %f %f\n", v[k * (lat + 1) + j].pos.x, v[k * (lat + 1) + j].pos.y, v[k * (lat+ 1) + j].pos.z);

		}
		printf("\n");
	}
	return v;
}

void update_vertex_buffer( const std::vector<vertex>& vertices,uint lon, uint lat)
{
	// clear and create new buffers
	if(vertex_buffer)	glDeleteBuffers( 1, &vertex_buffer );	vertex_buffer = 0;
	if(index_buffer)	glDeleteBuffers( 1, &index_buffer );	index_buffer = 0;

	// check exceptions
	if(vertices.empty()){ printf("[error] vertices is empty.\n"); return; }

	// create buffers
	if(b_index_buffer)
	{
		std::vector<uint> indices;
		for (uint k = 0; k < lon; k++) {
			indices.push_back(0);
			indices.push_back(1 + (lat + 1) * k);
			indices.push_back(1 + (lat + 1) * (k + 1));
			for (uint j = 1; j <= lat - 2; j++) {
				indices.push_back(j + (lat + 1) * (k + 1));
				indices.push_back(j + (lat + 1) * k);
				indices.push_back(j + 1 + (lat + 1) * (k + 1));

				indices.push_back(j + 1 + (lat + 1) * (k + 1));
				indices.push_back(j + (lat + 1) * k);
				indices.push_back(j + 1 + (lat + 1) * k);
			}
			indices.push_back(lat - 1 + (lat + 1) * (k + 1));
			indices.push_back(lat - 1 + (lat + 1) * k);
			indices.push_back(lat);
		}
		// generation of vertex buffer: use vertices as it is
		glGenBuffers( 1, &vertex_buffer );
		glBindBuffer( GL_ARRAY_BUFFER, vertex_buffer );
		glBufferData( GL_ARRAY_BUFFER, sizeof(vertex)*vertices.size(), &vertices[0], GL_STATIC_DRAW);

		// geneation of index buffer
		glGenBuffers( 1, &index_buffer );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, index_buffer );
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(uint)*indices.size(), &indices[0], GL_STATIC_DRAW );
	}
}

void update_tess()
{
	//uint n = NUM_TESS; if(b.add) n++; if(b.sub) n--;
	//if(n==NUM_TESS||n<MIN_TESS||n>MAX_TESS) return;
	
	

	unit_circle_vertices = create_circle_vertices(longitude,latitude);
	update_vertex_buffer( unit_circle_vertices, longitude,latitude);
	//printf( "> NUM_TESS = % -4d\r", NUM_TESS );
}

void keyboard( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	if(action==GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)	glfwSetWindowShouldClose(window, GL_TRUE);
		else if (key == GLFW_KEY_H || key == GLFW_KEY_F1)	print_help();
		//else if(key==GLFW_KEY_KP_ADD||(key==GLFW_KEY_EQUAL&&(mods&GLFW_MOD_SHIFT)))	b.add = true;
		//else if(key==GLFW_KEY_KP_SUBTRACT||key==GLFW_KEY_MINUS) b.sub = true;
		else if (key == GLFW_KEY_W)
		{
			b_wireframe = !b_wireframe;
			glPolygonMode(GL_FRONT_AND_BACK, b_wireframe ? GL_LINE : GL_FILL);
			printf("> using %s mode\n", b_wireframe ? "wireframe" : "solid");
		}
		else if (key == GLFW_KEY_D)
		{
			toggles += 1;
			if (toggles % 3 == 0) {
				toggle.x = 1;
				toggle.y = 0;
				toggle.z = 0;
			}
			else if (toggles % 3 == 1) {
				toggle.x = 0;
				toggle.y = 1;
				toggle.z = 0;
			}
			else {
				toggle.x = 0;
				toggle.y = 0;
				toggle.z = 1;
			}
			printf("> using %s\n", b_solid_color ? "solid color" : "texture coordinates as color");
		}
		else if (key == GLFW_KEY_RIGHT) {
			yb.yadd = true;
		}
		else if (key == GLFW_KEY_LEFT)	yb.ysub = true;
		//else if (key == GLFW_KEY_UP)	
	}
	else if (action == GLFW_RELEASE)
	{
		if (key == GLFW_KEY_RIGHT)	yb.yadd = false;
		else if (key == GLFW_KEY_LEFT) yb.ysub = false;
	}
	/*else if(action==GLFW_RELEASE)
	{
		if(key==GLFW_KEY_KP_ADD||(key==GLFW_KEY_EQUAL&&(mods&GLFW_MOD_SHIFT)))	b.add = false;
		else if(key==GLFW_KEY_KP_SUBTRACT||key==GLFW_KEY_MINUS) b.sub = false;
	}*/
}

void mouse( GLFWwindow* window, int button, int action, int mods )
{
	if(button==GLFW_MOUSE_BUTTON_LEFT&&action==GLFW_PRESS )
	{
		dvec2 pos; glfwGetCursorPos(window,&pos.x,&pos.y);
		printf( "> Left mouse button pressed at (%d, %d)\n", int(pos.x), int(pos.y) );
	}
}

void motion( GLFWwindow* window, double x, double y )
{
}

bool user_init()
{
	// log hotkeys
	print_help();

	// init GL states
	glLineWidth( 1.0f );
	glClearColor( 39/255.0f, 40/255.0f, 34/255.0f, 1.0f );	// set clear color
	glEnable( GL_CULL_FACE );								// turn on backface culling
	glEnable( GL_DEPTH_TEST );								// turn on depth tests
	
	// define the position of four corner vertices
	unit_circle_vertices = std::move(create_circle_vertices( longitude,latitude ));

	// create vertex buffer; called again when index buffering mode is toggled
	update_vertex_buffer( unit_circle_vertices, longitude, latitude);

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
	if(!cg_init_extensions( window )){ glfwTerminate(); return 1; }	// init OpenGL extensions

	// initializations and validations of GLSL program
	if(!(program=cg_create_program( vert_shader_path, frag_shader_path ))){ glfwTerminate(); return 1; }	// create and compile shaders/program
	if(!user_init()){ printf( "Failed to user_init()\n" ); glfwTerminate(); return 1; }					// user initialization

	// register event callbacks
	glfwSetWindowSizeCallback( window, reshape );	// callback for window resizing events
    glfwSetKeyCallback( window, keyboard );			// callback for keyboard events
	glfwSetMouseButtonCallback( window, mouse );	// callback for mouse click inputs
	glfwSetCursorPosCallback( window, motion );		// callback for mouse movements

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
