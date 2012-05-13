//post-processing that does nothing!
#version 150 core

uniform mat4 perspective_matrix; //should be an orthographic projection


in vec4 in_position; //position referring to a pixel on the texture
in vec2 in_texcoord;

out vec2 ex_texcoord;

void main() {
	ex_texcoord = in_texcoord;
	gl_Position = perspective_matrix * in_position;
}
