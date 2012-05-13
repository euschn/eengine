// Vertex Shader – file "minimal.vert"

#version 150 core

uniform mat4 perspective_matrix;
uniform mat4 modelview_matrix;

in  vec3 in_position;
in  vec3 in_normal;
in  vec2 in_texcoord;

out  vec3 ex_normal;
out  vec2 ex_texcoord;

void main(void)
{
	ex_normal = (modelview_matrix * vec4(in_normal, 0)).xyz;
	ex_texcoord = in_texcoord;

	gl_Position = perspective_matrix * modelview_matrix * vec4(in_position,1);
}
