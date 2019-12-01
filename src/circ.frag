#version 330

// inputs from vertex shader
in vec2 tc;	// used for texture coordinate visualization

// output of the fragment shader
out vec4 fragColor;

// shader's global variables, called the uniform variables
uniform int num_solid_color;
uniform vec4 solid_color;

void main()
{
	if(num_solid_color == 0){
		fragColor = vec4(tc.xy,0,1);
	}
	else if(num_solid_color == 1){
		fragColor = vec4(tc.xxx, 1);
	} else{
		fragColor = vec4(tc.yyy, 1);
	}
}