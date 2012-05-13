// Vertex Shader – file "pointlight.vert"

#version 150 core

uniform mat4 perspective_matrix;
uniform mat4 modelview_matrix;
uniform mat4 light_matrix;

in  vec3 in_position;

out vec4 vs_viewspace_position;

void main(void)
{
	gl_Position = perspective_matrix * modelview_matrix * vec4(in_position,1);
	vs_viewspace_position = gl_Position;
}
