//post-processing that does nothing!
#version 150 core

uniform sampler2D in_texture;

in vec2 ex_texcoord;

out vec4 out_color;

//the color we want to bloom
const vec3 bloom_color = vec3(1, 1, 1);
//maximum distance to bloom color;
const float maximum_color_distance = 0.7;

void main(void) {
	vec4 texel_color = texture(in_texture, ex_texcoord);
	float color_dist = distance(bloom_color, texel_color.xyz);

	//only paint colors that are close to bloom color
	if (color_dist < maximum_color_distance || texel_color.w < 1) {
		out_color = texel_color;
	}
	else {
		discard;
	}
}