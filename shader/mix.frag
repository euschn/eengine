//postprocessing that blends two textures
#version 150 core

uniform sampler2D in_texture;
uniform sampler2D in_texture2;

in vec2 ex_texcoord;

out vec4 out_color;


void main(void) {
	vec4 color1 = texture(in_texture, ex_texcoord);
	vec4 color2 = texture(in_texture2, ex_texcoord);

	out_color = vec4(color1.xyz + color2.xyz ,1);
}