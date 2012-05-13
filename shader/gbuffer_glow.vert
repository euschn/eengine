// deferred shading - gbuffer generation

#version 150 core

uniform mat4 perspective_matrix;
uniform mat4 modelview_matrix;
uniform mat4 normal_matrix;

in  vec3 in_position;
in  vec3 in_normal;
in  vec2 in_texcoord;

out vec4 ex_position;
out vec3 ex_normal;
out vec2 fragment_texcoord;

void main(void)
{
	fragment_texcoord = in_texcoord;

	ex_normal = (normal_matrix * vec4(in_normal, 0.0)).xyz;

	ex_position = modelview_matrix * vec4(in_position, 1);
	gl_Position = perspective_matrix * ex_position;
}
