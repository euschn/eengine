//simple raytracing
#version 150 core


in vec4 in_position; //position referring to a pixel on the texture
in vec2 in_texcoord;

out vec2 ex_texcoord;

void main() {
	ex_texcoord = in_texcoord;
	gl_Position = in_position;
}
