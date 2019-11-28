#version 330

// inputs from vertex shader
in vec2 tc;	// used for texture coordinate visualization

// output of the fragment shader
out vec4 fragColor;

// shader's global variables, called the uniform variables
uniform ivec3 toggle;
uniform bool b_x;
uniform bool b_solid_color;
uniform vec4 solid_color;

void main()
{
	//fragColor = b_solid_color ? solid_color : vec4(tc.xy,0,1);
	fragColor = vec4(tc.xy,0,1);
	fragColor = toggle.x * vec4(tc.xy,0,1) + toggle.y * vec4(tc.xxx,1) + toggle.z * vec4(tc.yyy,1);
}