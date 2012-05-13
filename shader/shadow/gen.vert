// deferred shading - shadowmap generation

#version 150 core

uniform mat4 perspective_matrix;
uniform mat4 modelview_matrix;

in  vec4 in_position;

out vec4 ex_position;

void main(void)
{

	gl_Position = perspective_matrix * modelview_matrix * in_position;
	ex_position = gl_Position;
}
