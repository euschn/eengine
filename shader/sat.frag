//SAT shader
#version 150 core

uniform sampler2D in_texture;
uniform float width;
uniform float stride; //pixel offset to go to the left
uniform vec2 direction; //decides whether we add up horizontal or vertical

in vec2 ex_texcoord;

out vec4 out_color;


void main(void) {
	float texel_step = 1.0 / width * stride;
	vec2 previous_texel = ex_texcoord - vec2(texel_step, texel_step)*direction;
	vec4 color = texture(in_texture, ex_texcoord);
	vec4 prev_color = texture(in_texture, previous_texel);
	color += prev_color;
	out_color = color;
}