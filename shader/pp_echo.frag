
#version 150 core

uniform sampler2D in_texture;

in vec2 ex_texcoord;

out vec4 out_color;

void main(void) {

	out_color = texture(in_texture, ex_texcoord);
}