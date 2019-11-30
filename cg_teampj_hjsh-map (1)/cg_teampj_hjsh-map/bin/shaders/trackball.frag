#version 330

// input from vertex shader
in vec2 tc;	// used for texture coordinate visualization
in vec3 norm;

// the only output variable
out vec4 fragColor;

// shader's global variables, called the uniform variables
uniform vec4 solid_color;
uniform bool b_solid_color;

void main()
{
	//fragColor = vec4(tc.xy, 0, 1);
	// fragColor = vec4(normalize(norm), 1.0);
	fragColor = b_solid_color ? solid_color : vec4(tc.xy, 0, 1);
}
