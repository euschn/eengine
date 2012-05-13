// Vertex Shader – file "minimal.vert"

#version 150 core

uniform mat4 perspective_matrix;
uniform mat4 modelview_matrix;

in  vec3 in_position;
//in  vec3 in_normal;

out vec3 ex_normal;

void main(void)
{
	//ex_normal = (modelview_matrix * vec4(in_normal, 1)).xyz;
	gl_Position = perspective_matrix * modelview_matrix * vec4(in_position,1);
}
